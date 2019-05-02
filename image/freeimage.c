/* freeimage.c  -  Image library  -  Public Domain  -  2018 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>

#include "image.h"

#include "ext/FreeImage.h"

void
image_freeimage_initialize(void);

void
image_freeimage_finalize(void);

static object_t _library_freeimage;

typedef void(DLL_CALLCONV* FreeImage_Initialise_Fn)(BOOL);
typedef void(DLL_CALLCONV* FreeImage_DeInitialise_Fn)(void);
typedef FREE_IMAGE_FORMAT(DLL_CALLCONV* FreeImage_GetFileTypeFromHandle_Fn)(FreeImageIO*, fi_handle,
                                                                            int);
typedef FIBITMAP*(DLL_CALLCONV* FreeImage_LoadFromHandle_Fn)(FREE_IMAGE_FORMAT, FreeImageIO*,
                                                             fi_handle, int);
typedef void(DLL_CALLCONV* FreeImage_Unload_Fn)(FIBITMAP*);

static FreeImage_Initialise_Fn _FreeImage_Initialise;
static FreeImage_DeInitialise_Fn _FreeImage_DeInitialise;
static FreeImage_GetFileTypeFromHandle_Fn _FreeImage_GetFileTypeFromHandle;
static FreeImage_LoadFromHandle_Fn _FreeImage_LoadFromHandle;
static FreeImage_Unload_Fn _FreeImage_Unload;

void
image_freeimage_initialize(void) {
	if (!_library_freeimage)
		_library_freeimage = library_load(STRING_CONST("FreeImage"));
	if (!_library_freeimage)
		_library_freeimage = library_load(STRING_CONST("freeimage"));

	if (_library_freeimage) {
		_FreeImage_Initialise = (FreeImage_Initialise_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_Initialise"));
		_FreeImage_DeInitialise = (FreeImage_DeInitialise_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_DeInitialise"));
		_FreeImage_GetFileTypeFromHandle = (FreeImage_GetFileTypeFromHandle_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_GetFileTypeFromHandle"));
		_FreeImage_LoadFromHandle = (FreeImage_LoadFromHandle_Fn)library_symbol(
		    _library_freeimage, STRING_CONST("FreeImage_LoadFromHandle"));
		_FreeImage_Unload = (FreeImage_Unload_Fn)library_symbol(_library_freeimage,
		                                                        STRING_CONST("FreeImage_Unload"));
	} else {
		_FreeImage_Initialise = 0;
	}

	if (!_FreeImage_Initialise || !_FreeImage_DeInitialise || !_FreeImage_GetFileTypeFromHandle ||
	    !_FreeImage_LoadFromHandle || !_FreeImage_Unload) {
		_FreeImage_Initialise = 0;
		_FreeImage_DeInitialise = 0;
		_FreeImage_GetFileTypeFromHandle = 0;
		_FreeImage_LoadFromHandle = 0;
		_FreeImage_Unload = 0;
	}

	if (_FreeImage_Initialise)
		_FreeImage_Initialise(0);
}

void
image_freeimage_finalize(void) {
	if (_FreeImage_DeInitialise)
		_FreeImage_DeInitialise();
	if (_library_freeimage)
		library_release(_library_freeimage);
	_library_freeimage = 0;
}

static unsigned DLL_CALLCONV
image_freeimage_read(void* buffer, unsigned size, unsigned count, fi_handle handle) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return 0;
	return (unsigned)(stream_read(stream, buffer, size * count) / size);
}

static unsigned DLL_CALLCONV
image_freeimage_write(void* buffer, unsigned size, unsigned count, fi_handle handle) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return 0;
	return (unsigned)(stream_write(stream, buffer, size * count) / size);
}

static int DLL_CALLCONV
image_freeimage_seek(fi_handle handle, long offset, int origin) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return 0;

	stream_seek_mode_t mode = STREAM_SEEK_CURRENT;
	if (origin == SEEK_END)
		mode = STREAM_SEEK_END;
	else if (origin == SEEK_SET)
		mode = STREAM_SEEK_BEGIN;
	stream_seek(stream, offset, mode);

	return 1;
}

static long DLL_CALLCONV
image_freeimage_tell(fi_handle handle) {
	stream_t* stream = (stream_t*)handle;
	if (!stream)
		return -1;

	return (long)stream_tell(stream);
}

int
image_freeimage_load(image_t* image, stream_t* stream) {
	if (!_FreeImage_LoadFromHandle)
		return -1;

	FreeImageIO io = {image_freeimage_read, image_freeimage_write, image_freeimage_seek,
	                  image_freeimage_tell};

	FREE_IMAGE_FORMAT fif = _FreeImage_GetFileTypeFromHandle(&io, (fi_handle)stream, 0);
	if (fif == FIF_UNKNOWN)
		return -1;

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	FIBITMAP* bitmap = _FreeImage_LoadFromHandle(fif, &io, (fi_handle)stream, 0);
	if (!bitmap)
		return -1;

	FOUNDATION_UNUSED(image);

	_FreeImage_Unload(bitmap);

	return -1;
}
