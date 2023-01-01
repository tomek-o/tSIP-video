/**
 * @file vidcodec.c Video Codec
 *
 * Copyright (C) 2010 Creytiv.com
 */

#include <re.h>
#include <baresip.h>


static struct list vidcodecl;


/**
 * Register a Video Codec
 *
 * @param vc Video Codec
 */
void vidcodec_register(struct vidcodec *vc)
{
	if (!vc)
		return;

	list_append(&vidcodecl, &vc->le, vc);

	if (vc->variant) {
		(void)re_printf("vidcodec: %s, variant: %s\n", vc->name, vc->variant);
	} else {
		(void)re_printf("vidcodec: %s\n", vc->name);
	}
}


/**
 * Unregister a Video Codec
 *
 * @param vc Video Codec
 */
void vidcodec_unregister(struct vidcodec *vc)
{
	if (!vc)
		return;

	list_unlink(&vc->le);
}


/**
 * Find a Video Codec by name
 *
 * @param name    Name of the Video Codec to find
 * @param variant Codec Variant
 *
 * @return Matching Video Codec if found, otherwise NULL
 */
const struct vidcodec *vidcodec_find(const char *name, const char *variant)
{
	struct le *le;

	for (le=vidcodecl.head; le; le=le->next) {

		struct vidcodec *vc = le->data;

		if (name && 0 != str_casecmp(name, vc->name))
			continue;

		if (variant && 0 != str_casecmp(variant, vc->variant))
			continue;

		return vc;
	}

	return NULL;
}

/**
 * Find a Video Encoder by name
 *
 * @param name      Name of the Video Encoder to find
 *
 * @return Matching Video Encoder if found, otherwise NULL
 */
const struct vidcodec *vidcodec_find_encoder(const char *name)
{
	struct le *le;

	for (le=list_head(&vidcodecl); le; le=le->next) {

		struct vidcodec *vc = le->data;

		if (name && 0 != str_casecmp(name, vc->name))
			continue;

		if (vc->ench)
			return vc;
	}

	return NULL;
}


/**
 * Find a Video Decoder by name
 *
 * @param name      Name of the Video Decoder to find
 *
 * @return Matching Video Decoder if found, otherwise NULL
 */
const struct vidcodec *vidcodec_find_decoder(const char *name)
{
	struct le *le;

	for (le=list_head(&vidcodecl); le; le=le->next) {

		struct vidcodec *vc = le->data;

		if (name && 0 != str_casecmp(name, vc->name))
			continue;

		if (vc->dech)
			return vc;
	}

	return NULL;
}


/**
 * Get the list of Video Codecs
 *
 * @return List of Video Codecs
 */
struct list *vidcodec_list(void)
{
	return &vidcodecl;
}
