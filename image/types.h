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
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#pragma once

/*! \file types.h
    Image data types */

#include <foundation/platform.h>
#include <foundation/types.h>

#include <image/build.h>

#if defined(IMAGE_COMPILE) && IMAGE_COMPILE
#  ifdef __cplusplus
#  define IMAGE_EXTERN extern "C"
#  define IMAGE_API extern "C"
#  else
#  define IMAGE_EXTERN extern
#  define IMAGE_API extern
#  endif
#else
#  ifdef __cplusplus
#  define IMAGE_EXTERN extern "C"
#  define IMAGE_API extern "C"
#  else
#  define IMAGE_EXTERN extern
#  define IMAGE_API extern
#  endif
#endif

typedef struct image_config_t      image_config_t;

struct image_config_t {
	size_t _unused;
};
