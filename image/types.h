/* types.h  -  Image library  -  Public Domain  -  2018 Mattias Jansson / Rampant Pixels
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
 * This library is put in the public domain; you can redistribute it and/or modify it without any
 * restrictions.
 *
 */

#pragma once

/*! \file types.h
    Image data types */

#include <foundation/platform.h>
#include <foundation/types.h>

#include <image/build.h>

#if defined(IMAGE_COMPILE) && IMAGE_COMPILE
#ifdef __cplusplus
#define IMAGE_EXTERN extern "C"
#define IMAGE_API extern "C"
#else
#define IMAGE_EXTERN extern
#define IMAGE_API extern
#endif
#else
#ifdef __cplusplus
#define IMAGE_EXTERN extern "C"
#define IMAGE_API extern "C"
#else
#define IMAGE_EXTERN extern
#define IMAGE_API extern
#endif
#endif

typedef enum image_datatype_t {
	IMAGE_DATATYPE_UNSIGNED_INT = 0,
	IMAGE_DATATYPE_INT,
	IMAGE_DATATYPE_FLOAT,

	IMAGE_NUM_DATATYPE
} image_datatype_t;

typedef enum image_colorspace_t {
	IMAGE_COLORSPACE_LINEAR = 0,
	IMAGE_COLORSPACE_sRGB,

	IMAGE_NUM_COLORSPACE
} image_colorspace_t;

typedef enum image_channel_t {
	IMAGE_CHANNEL_RED = 0,
	IMAGE_CHANNEL_GREEN,
	IMAGE_CHANNEL_BLUE,
	IMAGE_CHANNEL_ALPHA,
	IMAGE_CHANNEL_DEPTH,

	IMAGE_NUM_CHANNELS = 16
} image_channel_t;

typedef enum image_compression_t {
	IMAGE_COMPRESSION_NONE = 0,
	IMAGE_COMPRESSION_BC1,
	IMAGE_COMPRESSION_BC2,
	IMAGE_COMPRESSION_BC3,
	IMAGE_COMPRESSION_PVRTC_2BPP,
	IMAGE_COMPRESSION_PVRTC_4BPP,
	IMAGE_COMPRESSION_PVRTC2_2BPP,
	IMAGE_COMPRESSION_PVRTC2_4BPP,
	IMAGE_COMPRESSION_ETC1,
	IMAGE_COMPRESSION_ETC2,

	IMAGE_NUM_COMPRESSION
} image_compression_t;

typedef struct image_config_t image_config_t;
typedef struct image_t image_t;
typedef struct image_pixelformat_t image_pixelformat_t;
typedef struct image_channel_format_t image_channel_format_t;

typedef int (*image_load_fn)(image_t*, stream_t*);

struct image_config_t {
	image_load_fn loader;
};

struct image_channel_format_t {
	image_datatype_t data_type;
	unsigned int bits_per_pixel;
	unsigned int offset;
};

struct image_pixelformat_t {
	image_compression_t compression;
	image_colorspace_t colorspace;
	bool premultiplied_alpha;
	unsigned int bits_per_pixel;
	unsigned int num_channels;
	image_channel_format_t channel[IMAGE_NUM_CHANNELS];
};

struct image_t {
	image_pixelformat_t format;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int levels;
	unsigned char* data;
};
