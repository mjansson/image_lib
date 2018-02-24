/* image.c  -  Image library  -  Public Domain  -  2018 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform image loading library in C11 for projects
 * based on our foundation library.
 *
 * The latest source code maintained by Rampant Pixels is always available at
 *
 * https://github.com/rampantpixels/image_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>

#include "image.h"

static image_config_t _image_config;
static bool _image_initialized;

static void
image_initialize_config(const image_config_t config) {
	FOUNDATION_UNUSED(config);
}

int
image_module_initialize(const image_config_t config) {
	if (_image_initialized)
		return 0;

	image_initialize_config(config);

	log_debugf(HASH_IMAGE, STRING_CONST("Initializing image module"));

	_image_initialized = true;

	return 0;
}

bool
image_module_is_initialized(void) {
	return _image_initialized;
}

void
image_module_finalize(void) {
	if (!_image_initialized)
		return;

	log_debug(HASH_IMAGE, STRING_CONST("Terminating image module"));
}

image_config_t
image_module_config(void) {
	return _image_config;
}
