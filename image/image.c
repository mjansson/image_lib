/* image.c  -  Image library  -  Public Domain  -  2018 Mattias Jansson
 *
 * This library provides a cross-platform image loading library in C11 for projects
 * based on our foundation library.
 *
 * The latest source code maintained by Mattias Jansson is always available at
 *
 * https://github.com/mjansson/image_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any
 * restrictions.
 *
 */

#include <foundation/foundation.h>

#include "image.h"
#include "freeimage.h"

static image_config_t _image_config;
static bool _image_initialized;

void
image_freeimage_initialize(void);

void
image_freeimage_finalize(void);

static void
image_initialize_config(const image_config_t config) {
	_image_config = config;
}

int
image_module_initialize(const image_config_t config) {
	if (_image_initialized)
		return 0;

	image_initialize_config(config);

	log_debug(HASH_IMAGE, STRING_CONST("Initializing image module"));

	image_freeimage_initialize();

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

	image_freeimage_finalize();
}

image_config_t
image_module_config(void) {
	return _image_config;
}

void
image_initialize(image_t* image) {
	memset(image, 0, sizeof(image_t));
}

void
image_finalize(image_t* image) {
	if (image && image->data)
		memory_deallocate(image->data);
}

image_t*
image_allocate(void) {
	image_t* image = memory_allocate(HASH_IMAGE, sizeof(image_t), 0, MEMORY_PERSISTENT);
	image_initialize(image);
	return image;
}

void
image_deallocate(image_t* image) {
	image_finalize(image);
	memory_deallocate(image);
}

void
image_allocate_storage(image_t* image, const image_pixelformat_t* pixelformat, unsigned int width, unsigned int height,
                       unsigned int depth, unsigned int levels) {
	if (image->data)
		memory_deallocate(image->data);

	size_t data_size = image_buffer_size(pixelformat, width, height, depth, levels);

	memcpy(&image->format, pixelformat, sizeof(image_pixelformat_t));
	image->width = width;
	image->height = height;
	image->depth = depth;
	image->levels = levels;
	image->data = memory_allocate(HASH_IMAGE, data_size, 0, MEMORY_PERSISTENT);

	if ((image->format.compression >= IMAGE_COMPRESSION_PVRTC_2BPP) &&
	    (image->format.compression <= IMAGE_COMPRESSION_PVRTC2_4BPP)) {
		if (width < 8)
			image->width = 8;
		if (height < 8)
			image->height = 8;
	}
}

unsigned int
image_width(const image_t* image, unsigned int level) {
	if (!level)
		return image->width;
	unsigned int width = image->width >> level;
	if ((image->format.compression >= IMAGE_COMPRESSION_PVRTC_2BPP) &&
	    (image->format.compression <= IMAGE_COMPRESSION_PVRTC2_4BPP) && (width < 8))
		width = 8;
	return !width ? 1 : width;
}

unsigned int
image_height(const image_t* image, unsigned int level) {
	if (!level)
		return image->height;
	unsigned int height = image->height >> level;
	if ((image->format.compression >= IMAGE_COMPRESSION_PVRTC_2BPP) &&
	    (image->format.compression <= IMAGE_COMPRESSION_PVRTC2_4BPP) && (height < 8))
		height = 8;
	return !height ? 1 : height;
}

unsigned int
image_depth(const image_t* image, unsigned int level) {
	if (!level)
		return image->depth;
	unsigned int depth = image->depth >> level;
	return !depth ? 1 : depth;
}

size_t
image_buffer_size(const image_pixelformat_t* pixelformat, unsigned int width, unsigned int height, unsigned int depth,
                  unsigned int level_count) {
	size_t total_size = 0;

	while (level_count) {
		if (!width)
			width = 1;
		if (!height)
			height = 1;
		if (!depth)
			depth = 1;

		unsigned int level_width = width;
		unsigned int level_height = height;

		if ((pixelformat->compression >= IMAGE_COMPRESSION_PVRTC_2BPP) &&
		    (pixelformat->compression <= IMAGE_COMPRESSION_PVRTC2_4BPP)) {
			if (width < 8)
				level_width = 8;
			if (height < 8)
				level_height = 8;
		}

		size_t level_size = pixelformat->bits_per_pixel * level_width * level_height * depth;

		total_size += level_size;
		if ((width == 1) && (height == 1) && (depth == 1))
			break;

		width >>= 1;
		height >>= 1;
		depth >>= 1;
		--level_count;
	}

	return total_size;
}

bool
image_load(image_t* image, stream_t* stream) {
	if (_image_config.loader) {
		if (_image_config.loader(image, stream))
			return true;
	}
	if (image_freeimage_load(image, stream))
		return true;
	return false;
}

bool
image_convert_channels(image_t* image, image_datatype_t data_type, unsigned int bitdepth) {
	bool need_convert = false;
	for (unsigned int ich = 0; ich < IMAGE_CHANNEL_COUNT; ++ich) {
		if (!image->format.channel[ich].bits_per_pixel)
			continue;
		if ((image->format.channel[ich].bits_per_pixel != bitdepth) ||
		    (image->format.channel[ich].data_type != data_type)) {
			need_convert = true;
			break;
		}
	}
	if (!need_convert)
		return true;

	// TODO: Implement

	return false;
}
