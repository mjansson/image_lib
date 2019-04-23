/* image.h  -  Image library  -  Public Domain  -  2018 Mattias Jansson / Rampant Pixels
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

/*! \file image.h
    Image library */

#include <image/types.h>
#include <image/hashstrings.h>

/*! Initialize image functionality. Must be called prior to any other image
module API calls.
\param config Image configuration
\return       0 if initialization successful, <0 if error */
IMAGE_API int
image_module_initialize(const image_config_t config);

/*! Terminate all image functionality. Must match a call to #image_module_initialize.
After this function has been called no image module API calls can be made until
another call to #image_module_initialize. */
IMAGE_API void
image_module_finalize(void);

/*! Query if image module is initialized properly
\return true if initialized, false if not */
IMAGE_API bool
image_module_is_initialized(void);

/*! Query image config
\return Image module config */
IMAGE_API image_config_t
image_module_config(void);

/*! Query image module build version
\return Version of image module */
IMAGE_API version_t
image_module_version(void);

image_t*
image_allocate(const image_pixelformat_t* pixelformat, image_colorspace_t colorspace, unsigned int width,
               unsigned int height, unsigned int depth, unsigned int miplevels);

void
image_deallocate(image_t* image);

void*
image_buffer(image_t* image, unsigned int miplevel);

unsigned int
image_buffer_size(image_t* image, unsigned int level, unsigned int num_levels);

unsigned int
image_raw_buffer_size(const image_pixelformat_t* pixelformat, unsigned int width, unsigned int height,
                      unsigned int depth, unsigned int levels);

image_t*
image_load_raw(stream_t* stream);

void
image_save_raw(stream_t* stream, image_t* image);

uint64_t
image_io_raw_size(image_t* image);

unsigned int
image_width(const image_t* image, unsigned int level);

unsigned int
image_height(const image_t* image, unsigned int level);

unsigned int
image_depth(const image_t* image, unsigned int level);
