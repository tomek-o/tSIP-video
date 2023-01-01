/**
 * @file avcodec.c  Video codecs using FFmpeg libavcodec
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re.h>
#include <stdint.h>
#include <rem.h>
#include <baresip.h>
#include <libavcodec/avcodec.h>
#include "h26x.h"
#include "avcodec.h"


#define DEBUG_MODULE "avcodec"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


int avcodec_resolve_codecid(const char *s)
{
	if (0 == str_casecmp(s, "H263"))
		return AV_CODEC_ID_H263;
	else if (0 == str_casecmp(s, "H264"))
		return AV_CODEC_ID_H264;
	else if (0 == str_casecmp(s, "MP4V-ES"))
		return AV_CODEC_ID_MPEG4;
	else
		return AV_CODEC_ID_NONE;
}


static uint32_t packetization_mode(const char *fmtp)
{
	struct pl pl, mode;

	if (!fmtp)
		return 0;

	pl_set_str(&pl, fmtp);

	if (fmt_param_get(&pl, "packetization-mode", &mode))
		return pl_u32(&mode);

	return 0;
}


static int h264_fmtp_enc(struct mbuf *mb, const struct sdp_format *fmt,
			 bool offer, void *arg)
{
	struct vidcodec *vc = arg;
	const uint8_t profile_idc = 0x42; /* baseline profile */
	const uint8_t profile_iop = 0x80;
	(void)offer;

	if (!mb || !fmt || !vc)
		return 0;

	return mbuf_printf(mb, "a=fmtp:%s"
			   " packetization-mode=0"
			   ";profile-level-id=%02x%02x%02x"
			   "\r\n",
			   fmt->id, profile_idc, profile_iop, h264_level_idc);
}


static bool h264_fmtp_cmp(const char *fmtp1, const char *fmtp2, void *data)
{
	(void)data;

	return packetization_mode(fmtp1) == packetization_mode(fmtp2);
}


static int h263_fmtp_enc(struct mbuf *mb, const struct sdp_format *fmt,
			 bool offer, void *arg)
{
	(void)offer;
	(void)arg;

	if (!mb || !fmt)
		return 0;

	return mbuf_printf(mb, "a=fmtp:%s CIF=1;CIF4=1\r\n", fmt->id);
}


static int mpg4_fmtp_enc(struct mbuf *mb, const struct sdp_format *fmt,
			 bool offer, void *arg)
{
	(void)offer;
	(void)arg;

	if (!mb || !fmt)
		return 0;

	return mbuf_printf(mb, "a=fmtp:%s profile-level-id=3\r\n", fmt->id);
}

/*
struct vidcodec {
	struct le le;
	const char *pt;
	const char *name;
	const char *variant;
	const char *fmtp;
	videnc_update_h *encupdh;
	videnc_encode_h *ench;
	viddec_update_h *decupdh;
	viddec_decode_h *dech;
	sdp_fmtp_enc_h *fmtp_ench;
	sdp_fmtp_cmp_h *fmtp_cmph;
};
*/

static struct vidcodec h264 = {
#if defined(__BORLANDC__)
	LE_INIT,
	NULL,
	"H264",
	"packetization-mode=0",
	NULL,
	encode_update,
	encode,
	decode_update,
	decode_h264,
	h264_fmtp_enc,
	h264_fmtp_cmp,
#else
	.name      = "H264",
	.variant   = "packetization-mode=0",
	.encupdh   = encode_update,
	.ench      = encode,
	.decupdh   = decode_update,
	.dech      = decode_h264,
	.fmtp_ench = h264_fmtp_enc,
	.fmtp_cmph = h264_fmtp_cmp,
#endif
};

static struct vidcodec h263 = {
#if defined(__BORLANDC__)
	LE_INIT,		/* le */
	"34",			/* pt */
	"H263",			/* name */
	NULL,			/* variant */
	NULL,			/* fmtp */
	encode_update,	/* encupdh */
	encode,			/* ench */
	decode_update,	/* decupdh */
	decode_h263,	/* dech */
	h263_fmtp_enc,	/* fmtp_ench */
	NULL			/* fmtp_cmph */
#else
	.pt        = "34",
	.name      = "H263",
	.encupdh   = encode_update,
	.ench      = encode,
	.decupdh   = decode_update,
	.dech      = decode_h263,
	.fmtp_ench = h263_fmtp_enc,
#endif
};

static struct vidcodec mpg4 = {
#if defined(__BORLANDC__)
	LE_INIT,		/* le */
	NULL,			/* pt */
	"MP4V-ES",		/* name */
	NULL,			/* variant */
	NULL,			/* fmtp */
	encode_update,	/* encupdh */
	encode,			/* ench */
	decode_update,	/* decupdh */
	decode_mpeg4,	/* dech */
	mpg4_fmtp_enc,	/* fmtp_ench */
	NULL			/* fmtp_cmph */
#else
	.name      = "MP4V-ES",
	.encupdh   = encode_update,
	.ench      = encode,
	.decupdh   = decode_update,
	.dech      = decode_mpeg4,
	.fmtp_ench = mpg4_fmtp_enc,
#endif
};


static int module_init(void)
{
	avcodec_register_all();

	if (avcodec_find_decoder(AV_CODEC_ID_H264))
		vidcodec_register(&h264);

	if (avcodec_find_decoder(AV_CODEC_ID_H263))
		vidcodec_register(&h263);

	if (avcodec_find_decoder(AV_CODEC_ID_MPEG4))
		vidcodec_register(&mpg4);

	return 0;
}


static int module_close(void)
{
	vidcodec_unregister(&mpg4);
	vidcodec_unregister(&h263);
	vidcodec_unregister(&h264);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(avcodec) = {
	"avcodec",
	"codec",
	module_init,
	module_close
};
