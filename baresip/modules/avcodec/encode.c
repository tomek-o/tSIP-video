/**
 * @file avcodec/encode.c  Video codecs using FFmpeg libavcodec -- encoder
 *
 * Copyright (C) 2010 - 2013 Creytiv.com
 */
#include <re.h>
#include <stdint.h>
#include <rem.h>
#include <baresip.h>
#ifdef __BORLANDC__
#pragma warn -8056	// disable "Integer arithmetic overflow" warning
#endif
#include <libavcodec/avcodec.h>
#ifdef __BORLANDC__
#pragma warn .8056
#endif
#include <libavutil/opt.h>
#include "h26x.h"
#include "avcodec.h"


#define DEBUG_MODULE "avcodec"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


enum {
	DEFAULT_GOP_SIZE =   10,
};


struct picsz {
	enum h263_fmt fmt;  /**< Picture size */
	uint8_t mpi;        /**< Minimum Picture Interval (1-32) */
};


struct videnc_state {
	const AVCodec *codec;
	AVCodecContext *ctx;
	AVFrame *pict;
	struct mbuf *mb;
	size_t sz_max; /* todo: figure out proper buffer size */
	int64_t pts;
	struct mbuf *mb_frag;
	struct videnc_param encprm;
	struct vidsz encsize;
	enum AVCodecID codec_id;

	union {
		struct {
			struct picsz picszv[8];
			uint32_t picszn;
		} h263;

		struct {
			uint32_t packetization_mode;
			uint32_t profile_idc;
			uint32_t profile_iop;
			uint32_t level_idc;
			uint32_t max_fs;
			uint32_t max_smbps;
		} h264;
	} u;
};


static void destructor(void *arg)
{
	struct videnc_state *st = arg;

	mem_deref(st->mb);
	mem_deref(st->mb_frag);

	if (st->ctx) {
		if (st->ctx->codec)
			avcodec_close(st->ctx);
		av_free(st->ctx);
	}

	if (st->pict)
		av_free(st->pict);
}


static enum h263_fmt h263_fmt(const struct pl *name)
{
	if (0 == pl_strcasecmp(name, "sqcif")) return H263_FMT_SQCIF;
	if (0 == pl_strcasecmp(name, "qcif"))  return H263_FMT_QCIF;
	if (0 == pl_strcasecmp(name, "cif"))   return H263_FMT_CIF;
	if (0 == pl_strcasecmp(name, "cif4"))  return H263_FMT_4CIF;
	if (0 == pl_strcasecmp(name, "cif16")) return H263_FMT_16CIF;
	return H263_FMT_OTHER;
}


static int decode_sdpparam_h263(struct videnc_state *st, const struct pl *name,
				const struct pl *val)
{
	enum h263_fmt fmt = h263_fmt(name);
	const int mpi = pl_u32(val);

	if (fmt == H263_FMT_OTHER) {
		DEBUG_NOTICE("h263: unknown param '%r'\n", name);
		return 0;
	}
	if (mpi < 1 || mpi > 32) {
		DEBUG_NOTICE("h263: %r: MPI out of range %d\n", name, mpi);
		return 0;
	}

	if (st->u.h263.picszn >= ARRAY_SIZE(st->u.h263.picszv)) {
		DEBUG_NOTICE("h263: picszv overflow: %r\n", name);
		return 0;
	}

	st->u.h263.picszv[st->u.h263.picszn].fmt = fmt;
	st->u.h263.picszv[st->u.h263.picszn].mpi = mpi;

	++st->u.h263.picszn;

	return 0;
}


static int init_encoder(struct videnc_state *st, const char *name)
{
	/*
	 * Special handling of H.264 encoder
	 */
	if (st->codec_id == AV_CODEC_ID_H264 && avcodec_h264enc) {

		st->codec = avcodec_h264enc;

		DEBUG_INFO("avcodec: h264 encoder activated\n");

		return 0;
	}

	if (0 == str_casecmp(name, "h265")) {

		st->codec = avcodec_h265enc;

		DEBUG_INFO("avcodec: h265 encoder activated\n");

		return 0;
	}

	st->codec = avcodec_find_encoder(st->codec_id);
	if (!st->codec)
		return ENOENT;

	return 0;
}


static int open_encoder(struct videnc_state *st,
			const struct videnc_param *prm,
			const struct vidsz *size)
{
	int err = 0;

	if (st->ctx) {
		if (st->ctx->codec)
			avcodec_close(st->ctx);
		av_free(st->ctx);
	}

	if (st->pict)
		av_free(st->pict);

	st->ctx = avcodec_alloc_context3(st->codec);

	st->pict = av_frame_alloc(); //avcodec_alloc_frame();

	if (!st->ctx || !st->pict) {
		err = ENOMEM;
		goto out;
	}

	st->ctx->bit_rate  = prm->bitrate;
	st->ctx->width     = size->w;
	st->ctx->height    = size->h;
	st->ctx->gop_size  = DEFAULT_GOP_SIZE;
	st->ctx->pix_fmt   = AV_PIX_FMT_YUV420P;
	st->ctx->time_base.num = 1;
	st->ctx->time_base.den = prm->fps;
	if (0 == str_cmp(st->codec->name, "libx264")) {

		av_opt_set(st->ctx->priv_data, "profile", "baseline", 0);
		av_opt_set(st->ctx->priv_data, "preset", "ultrafast", 0);
		av_opt_set(st->ctx->priv_data, "tune", "zerolatency", 0);

		if (st->u.h264.packetization_mode == 0) {
			av_opt_set_int(st->ctx->priv_data,
				       "slice-max-size", prm->pktsize, 0);
		}
	}

	/* params to avoid ffmpeg/x264 default preset error */
	if (st->codec_id == AV_CODEC_ID_H264) {
		if (0 == str_cmp(st->codec->name, "h264_vaapi")) {
			av_opt_set(st->ctx->priv_data, "profile",
				   "constrained_baseline", 0);
		}
		else {
			av_opt_set(st->ctx->priv_data, "profile",
				   "baseline", 0);
		}
		st->ctx->me_range = 16;
		st->ctx->qmin = 10;
		st->ctx->qmax = 51;
		st->ctx->max_qdiff = 4;
		if (st->codec == avcodec_find_encoder_by_name("nvenc_h264") ||
		    st->codec == avcodec_find_encoder_by_name("h264_nvenc")) {

			err = av_opt_set(st->ctx->priv_data,
					 "preset", "llhp", 0);
			if (err < 0) {
				DEBUG_INFO("avcodec: h264 nvenc setting preset "
				      "\"llhp\" failed; error: %u\n", err);
			}
			else {
				DEBUG_INFO("avcodec: h264 nvenc preset "
				      "\"llhp\" selected\n");
			}
			err = av_opt_set_int(st->ctx->priv_data,
					     "2pass", 1, 0);
			if (err < 0) {
				DEBUG_INFO("avcodec: h264 nvenc option "
				      "\"2pass\" failed; error: %u\n", err);
			}
			else {
				DEBUG_INFO("avcodec: h264 nvenc option "
				      "\"2pass\" selected\n");
			}
		}
	}

	if (0 == str_cmp(st->codec->name, "libx265")) {

		av_opt_set(st->ctx->priv_data, "profile", "main444-8", 0);
		av_opt_set(st->ctx->priv_data, "preset", "ultrafast", 0);
		av_opt_set(st->ctx->priv_data, "tune", "zerolatency", 0);
	}
	if (avcodec_open2(st->ctx, st->codec, NULL) < 0) {
		err = ENOENT;
		goto out;
	}

 out:
	if (err) {
		if (st->ctx) {
			if (st->ctx->codec)
				avcodec_close(st->ctx);
			av_free(st->ctx);
			st->ctx = NULL;
		}

		if (st->pict) {
			av_free(st->pict);
			st->pict = NULL;
		}
	}
	else
		st->encsize = *size;

	return err;
}


int decode_sdpparam_h264(struct videnc_state *st, const struct pl *name,
			 const struct pl *val)
{
	if (0 == pl_strcasecmp(name, "packetization-mode")) {
		st->u.h264.packetization_mode = pl_u32(val);

		if (st->u.h264.packetization_mode != 0 &&
		    st->u.h264.packetization_mode != 1 ) {
			DEBUG_WARNING("illegal packetization-mode %u\n",
				      st->u.h264.packetization_mode);
			return EPROTO;
		}
	}
	else if (0 == pl_strcasecmp(name, "profile-level-id")) {
		struct pl prof = *val;
		if (prof.l != 6) {
			DEBUG_WARNING("invalid profile-level-id (%r)\n", val);
			return EPROTO;
		}

		prof.l = 2;
		st->u.h264.profile_idc = pl_x32(&prof); prof.p += 2;
		st->u.h264.profile_iop = pl_x32(&prof); prof.p += 2;
		st->u.h264.level_idc   = pl_x32(&prof);
	}
	else if (0 == pl_strcasecmp(name, "max-fs")) {
		st->u.h264.max_fs = pl_u32(val);
	}
	else if (0 == pl_strcasecmp(name, "max-smbps")) {
		st->u.h264.max_smbps = pl_u32(val);
	}

	return 0;
}


static void param_handler(const struct pl *name, const struct pl *val,
			  void *arg)
{
	struct videnc_state *st = arg;

	if (st->codec_id == AV_CODEC_ID_H263)
		(void)decode_sdpparam_h263(st, name, val);
	else if (st->codec_id == AV_CODEC_ID_H264)
		(void)decode_sdpparam_h264(st, name, val);
}


static int general_packetize(struct mbuf *mb, size_t pktsize,
			     videnc_packet_h *pkth, void *arg)
{
	int err = 0;

	/* Assemble frame into smaller packets */
	while (!err) {
		size_t sz, left = mbuf_get_left(mb);
		bool last = (left < pktsize);
		if (!left)
			break;

		sz = last ? left : pktsize;

		err = pkth(last, NULL, 0, mbuf_buf(mb), sz, arg);

		mbuf_advance(mb, sz);
	}

	return err;
}


static int h263_packetize(struct videnc_state *st, struct mbuf *mb,
			  videnc_packet_h *pkth, void *arg)
{
	struct h263_strm h263_strm;
	struct h263_hdr h263_hdr;
	size_t pos;
	int err;

	/* Decode bit-stream header, used by packetizer */
	err = h263_strm_decode(&h263_strm, mb);
	if (err)
		return err;

	h263_hdr_copy_strm(&h263_hdr, &h263_strm);

	st->mb_frag->pos = st->mb_frag->end = 0;
	err = h263_hdr_encode(&h263_hdr, st->mb_frag);
	pos = st->mb_frag->pos;

	/* Assemble frame into smaller packets */
	while (!err) {
		size_t sz, left = mbuf_get_left(mb);
		bool last = (left < st->encprm.pktsize);
		if (!left)
			break;

		sz = last ? left : st->encprm.pktsize;

		st->mb_frag->pos = st->mb_frag->end = pos;
		err = mbuf_write_mem(st->mb_frag, mbuf_buf(mb), sz);
		if (err)
			break;

		st->mb_frag->pos = 0;

		err = pkth(last, NULL, 0, mbuf_buf(st->mb_frag),
			   mbuf_get_left(st->mb_frag), arg);

		mbuf_advance(mb, sz);
	}

	return err;
}


int encode_update(struct videnc_state **vesp, const struct vidcodec *vc,
		  struct videnc_param *prm, const char *fmtp)
{
	struct videnc_state *st;
	int err = 0;

	if (!vesp || !vc || !prm)
		return EINVAL;

	if (*vesp)
		return 0;

	st = mem_zalloc(sizeof(*st), destructor);
	if (!st)
		return ENOMEM;

	st->encprm = *prm;

	st->codec_id = avcodec_resolve_codecid(vc->name);
	if (st->codec_id == AV_CODEC_ID_NONE) {
		DEBUG_WARNING("avcodec: unknown encoder (%s)\n", vc->name);
		err = EINVAL;
		goto out;
	}

	st->mb  = mbuf_alloc(AV_INPUT_BUFFER_MIN_SIZE * 20);
	st->mb_frag = mbuf_alloc(1024);
	if (!st->mb || !st->mb_frag) {
		err = ENOMEM;
		goto out;
	}

	st->sz_max = st->mb->size;

	err = init_encoder(st, vc->name);
	if (err) {
		DEBUG_WARNING("avcodec: %s: could not init encoder\n", vc->name);
		goto out;
	}

	if (str_isset(fmtp)) {
		struct pl sdp_fmtp;

		pl_set_str(&sdp_fmtp, fmtp);

		fmt_param_apply(&sdp_fmtp, param_handler, st);
	}

	re_printf("video encoder %s: %d fps, %d bit/s, pktsize=%u\n",
		  vc->name, prm->fps, prm->bitrate, prm->pktsize);

 out:
	if (err)
		mem_deref(st);
	else
		*vesp = st;

	return err;
}


int encode(struct videnc_state *st, bool update, const struct vidframe *frame,
	   videnc_packet_h *pkth, void *arg)
{
	int i, err, ret;

	if (!st || !frame || !pkth)
		return EINVAL;

	if (!st->ctx || !vidsz_cmp(&st->encsize, &frame->size)) {

		err = open_encoder(st, &st->encprm, &frame->size);
		if (err) {
			DEBUG_WARNING("open_encoder: %m\n", err);
			return err;
		}
	}

	for (i=0; i<4; i++) {
		st->pict->data[i]     = frame->data[i];
		st->pict->linesize[i] = frame->linesize[i];
	}
	st->pict->pts = st->pts++;
	if (update) {
		re_printf("avcodec encoder picture update\n");
		st->pict->key_frame = 1;
#ifdef FF_I_TYPE
		st->pict->pict_type = FF_I_TYPE;  /* Infra Frame */
#else
		st->pict->pict_type = AV_PICTURE_TYPE_I;
#endif
	}
	else {
		st->pict->key_frame = 0;
		st->pict->pict_type = 0;
	}

	mbuf_rewind(st->mb);

	do {
		AVPacket *avpkt = av_packet_alloc();
		int got_packet = 0;

		avpkt->data = st->mb->buf;
		avpkt->size = (int)st->mb->size;

		ret = avcodec_encode_video2(st->ctx, avpkt,
						st->pict, &got_packet);
		if (ret < 0) {
			av_packet_free(&avpkt);
			return EBADMSG;
		}
		if (!got_packet) {
			av_packet_free(&avpkt);
			return 0;
		}

		mbuf_set_end(st->mb, avpkt->size);
		av_packet_free(&avpkt);
	} while (0);


	switch (st->codec_id) {

	case AV_CODEC_ID_H263:
		err = h263_packetize(st, st->mb, pkth, arg);
		break;

	case AV_CODEC_ID_H264:
		err = h264_packetize(st->mb, st->encprm.pktsize, pkth, arg);
		break;

	case AV_CODEC_ID_MPEG4:
		err = general_packetize(st->mb, st->encprm.pktsize, pkth, arg);
		break;

	default:
		err = EPROTO;
		break;
	}

	return err;
}
