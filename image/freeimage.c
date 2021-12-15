/* freeimage.c  -  Image library  -  Public Domain  -  2018 Mattias Jansson
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

#include "ext/FreeImage.h"

extern void
image_freeimage_initialize(void);

extern void
image_freeimage_finalize(void);

static object_t library_freeimage;

typedef void(DLL_CALLCONV* FreeImage_Initialise_t)(BOOL);
typedef void(DLL_CALLCONV* FreeImage_DeInitialise_t)(void);
typedef FREE_IMAGE_FORMAT(DLL_CALLCONV* FreeImage_GetFileTypeFromHandle_t)(FreeImageIO*, fi_handle, int);
typedef FIBITMAP*(DLL_CALLCONV* FreeImage_LoadFromHandle_t)(FREE_IMAGE_FORMAT, FreeImageIO*, fi_handle, int);
typedef void(DLL_CALLCONV* FreeImage_Unload_t)(FIBITMAP*);

typedef unsigned(DLL_CALLCONV* FreeImage_GetWidth_t)(FIBITMAP*);
typedef unsigned(DLL_CALLCONV* FreeImage_GetHeight_t)(FIBITMAP*);
typedef unsigned(DLL_CALLCONV* FreeImage_GetPitch_t)(FIBITMAP*);
typedef unsigned(DLL_CALLCONV* FreeImage_GetBPP_t)(FIBITMAP*);
typedef FREE_IMAGE_TYPE(DLL_CALLCONV* FreeImage_GetImageType_t)(FIBITMAP*);
typedef FREE_IMAGE_COLOR_TYPE(DLL_CALLCONV* FreeImage_GetColorType_t)(FIBITMAP*);
typedef BYTE*(DLL_CALLCONV* FreeImage_GetBits_t)(FIBITMAP*);

static FreeImage_Initialise_t FreeImage_Initialise_Fn;
static FreeImage_DeInitialise_t FreeImage_DeInitialise_Fn;
static FreeImage_GetFileTypeFromHandle_t FreeImage_GetFileTypeFromHandle_Fn;
static FreeImage_LoadFromHandle_t FreeImage_LoadFromHandle_Fn;
static FreeImage_Unload_t FreeImage_Unload_Fn;

static FreeImage_GetWidth_t FreeImage_GetWidth_Fn;
static FreeImage_GetHeight_t FreeImage_GetHeight_Fn;
static FreeImage_GetPitch_t FreeImage_GetPitch_Fn;
static FreeImage_GetBPP_t FreeImage_GetBPP_Fn;
static FreeImage_GetImageType_t FreeImage_GetImageType_Fn;
static FreeImage_GetColorType_t FreeImage_GetColorType_Fn;
static FreeImage_GetBits_t FreeImage_GetBits_Fn;

void
image_freeimage_initialize(void) {
	if (!library_freeimage)
		library_freeimage = library_load(STRING_CONST("FreeImage"));
	if (!library_freeimage)
		library_freeimage = library_load(STRING_CONST("freeimage"));

	if (library_freeimage) {
		FreeImage_Initialise_Fn =
		    (FreeImage_Initialise_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_Initialise"));
		FreeImage_DeInitialise_Fn =
		    (FreeImage_DeInitialise_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_DeInitialise"));
		FreeImage_GetFileTypeFromHandle_Fn = (FreeImage_GetFileTypeFromHandle_t)library_symbol(
		    library_freeimage, STRING_CONST("FreeImage_GetFileTypeFromHandle"));
		FreeImage_LoadFromHandle_Fn =
		    (FreeImage_LoadFromHandle_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_LoadFromHandle"));
		FreeImage_Unload_Fn = (FreeImage_Unload_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_Unload"));
		FreeImage_GetWidth_Fn =
		    (FreeImage_GetWidth_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetWidth"));
		FreeImage_GetHeight_Fn =
		    (FreeImage_GetHeight_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetHeight"));
		FreeImage_GetPitch_Fn =
		    (FreeImage_GetPitch_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetPitch"));
		FreeImage_GetBPP_Fn = (FreeImage_GetBPP_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetBPP"));
		FreeImage_GetPitch_Fn =
		    (FreeImage_GetPitch_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetPitch"));
		FreeImage_GetImageType_Fn =
		    (FreeImage_GetImageType_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetImageType"));
		FreeImage_GetColorType_Fn =
		    (FreeImage_GetColorType_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetColorType"));
		FreeImage_GetBits_Fn =
		    (FreeImage_GetBits_t)library_symbol(library_freeimage, STRING_CONST("FreeImage_GetBits"));
	} else {
		FreeImage_Initialise_Fn = 0;
		FreeImage_DeInitialise_Fn = 0;
		log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED, STRING_CONST("Failed to load FreeImage library"));
		return;
	}

	if (!FreeImage_Initialise_Fn || !FreeImage_DeInitialise_Fn || !FreeImage_GetFileTypeFromHandle_Fn ||
	    !FreeImage_LoadFromHandle_Fn || !FreeImage_Unload_Fn || !FreeImage_GetWidth_Fn || !FreeImage_GetHeight_Fn ||
	    !FreeImage_GetPitch_Fn || !FreeImage_GetBPP_Fn || !FreeImage_GetImageType_Fn || !FreeImage_GetColorType_Fn ||
	    !FreeImage_GetBits_Fn) {
		FreeImage_Initialise_Fn = 0;
		FreeImage_DeInitialise_Fn = 0;
		log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED, STRING_CONST("Failed to find symbold is loaded FreeImage library"));
	}

	if (FreeImage_Initialise_Fn)
		FreeImage_Initialise_Fn(0);
}

void
image_freeimage_finalize(void) {
	if (FreeImage_DeInitialise_Fn)
		FreeImage_DeInitialise_Fn();
	if (library_freeimage)
		library_release(library_freeimage);
	library_freeimage = 0;
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

bool
image_freeimage_load(image_t* image, stream_t* stream) {
	if (!FreeImage_Initialise_Fn)
		return false;

	FreeImageIO io = {image_freeimage_read, image_freeimage_write, image_freeimage_seek, image_freeimage_tell};

	size_t begin_pos = stream_tell(stream);
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle_Fn(&io, (fi_handle)stream, 0);
	if (fif == FIF_UNKNOWN) {
		log_info(HASH_IMAGE, STRING_CONST("FreeImage failed to get file type from stream"));
		return false;
	}

	stream_seek(stream, (ssize_t)begin_pos, STREAM_SEEK_BEGIN);
	FIBITMAP* bitmap = FreeImage_LoadFromHandle_Fn(fif, &io, (fi_handle)stream, 0);
	if (!bitmap) {
		log_info(HASH_IMAGE, STRING_CONST("FreeImage failed to load image from stream"));
		return false;
	}

	image_pixelformat_t pixelformat;
	memset(&pixelformat, 0, sizeof(pixelformat));

	int err = -1;
	unsigned int width = FreeImage_GetWidth_Fn(bitmap);
	unsigned int height = FreeImage_GetHeight_Fn(bitmap);
	unsigned int pitch = FreeImage_GetPitch_Fn(bitmap);
	FREE_IMAGE_TYPE image_type = FreeImage_GetImageType_Fn(bitmap);
	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType_Fn(bitmap);

	log_infof(HASH_IMAGE, STRING_CONST("Loaded image: %.*s (type %d, color %d)"), STRING_FORMAT(stream->path),
	          (int)image_type, (int)color_type);

	if ((color_type != FIC_RGB) && (color_type != FIC_RGBALPHA) && (color_type != FIC_CMYK)) {
		log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED, STRING_CONST("Unsupported FreeImage color type: %u"),
		          (unsigned int)color_type);
		goto cleanup;
	}

	pixelformat.colorspace = IMAGE_COLORSPACE_LINEAR;
	pixelformat.compression = IMAGE_COMPRESSION_NONE;
	pixelformat.premultiplied_alpha = false;

	image_datatype_t data_type = IMAGE_DATATYPE_UNSIGNED_INT;
	unsigned int bits_per_channel = 0;
	unsigned int source_bpp = 0;

	if (image_type == FIT_BITMAP) {
		source_bpp = FreeImage_GetBPP_Fn(bitmap);
		if ((color_type != FIC_RGB) && (color_type != FIC_RGBALPHA)) {
			log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED, STRING_CONST("Unsupported FreeImage color type: %u"),
			          (unsigned int)image_type);
			goto cleanup;
		}
		if ((source_bpp != 24) && (source_bpp != 32)) {
			log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED, STRING_CONST("Unsupported FreeImage bitdepth: %u"), source_bpp);
			goto cleanup;
		}

		if (color_type == FIC_RGBALPHA)
			pixelformat.channels_count = 4;
		else
			pixelformat.channels_count = 3;
		bits_per_channel = 8;
	} else {
		if ((image_type != FIT_RGB16) && (image_type != FIT_RGBA16) && (image_type != FIT_RGBF) &&
		    (image_type != FIT_RGBAF)) {
			log_warnf(HASH_IMAGE, WARNING_UNSUPPORTED, STRING_CONST("Unsupported FreeImage image type: %u"),
			          (unsigned int)image_type);
			goto cleanup;
		}

		if (image_type == FIT_RGBA16) {
			bits_per_channel = 16;
			pixelformat.channels_count = 4;
		} else if (image_type == FIT_RGBAF) {
			bits_per_channel = 32;
			pixelformat.channels_count = 4;
			data_type = IMAGE_DATATYPE_FLOAT;
		} else if (image_type == FIT_RGB16) {
			bits_per_channel = 16;
			pixelformat.channels_count = 3;
		} else if (image_type == FIT_RGBF) {
			bits_per_channel = 32;
			pixelformat.channels_count = 3;
			data_type = IMAGE_DATATYPE_FLOAT;
		}
		source_bpp = bits_per_channel * pixelformat.channels_count;
	}

	pixelformat.bits_per_pixel = bits_per_channel * pixelformat.channels_count;

	if (data_type == IMAGE_DATATYPE_FLOAT)
		pixelformat.colorspace = IMAGE_COLORSPACE_LINEAR;
	else
		pixelformat.colorspace = IMAGE_COLORSPACE_sRGB;

	pixelformat.channel[IMAGE_CHANNEL_RED].bits_per_pixel = bits_per_channel;
	pixelformat.channel[IMAGE_CHANNEL_RED].data_type = data_type;
	pixelformat.channel[IMAGE_CHANNEL_RED].offset = 0;
	pixelformat.channel[IMAGE_CHANNEL_GREEN].bits_per_pixel = bits_per_channel;
	pixelformat.channel[IMAGE_CHANNEL_GREEN].data_type = data_type;
	pixelformat.channel[IMAGE_CHANNEL_GREEN].offset = bits_per_channel;
	pixelformat.channel[IMAGE_CHANNEL_BLUE].bits_per_pixel = bits_per_channel;
	pixelformat.channel[IMAGE_CHANNEL_BLUE].data_type = data_type;
	pixelformat.channel[IMAGE_CHANNEL_BLUE].offset = bits_per_channel * 2;
	if (pixelformat.channels_count == 4) {
		pixelformat.channel[IMAGE_CHANNEL_ALPHA].bits_per_pixel = bits_per_channel;
		pixelformat.channel[IMAGE_CHANNEL_ALPHA].data_type = data_type;
		pixelformat.channel[IMAGE_CHANNEL_ALPHA].offset = bits_per_channel * 3;
	}

	image_allocate_storage(image, &pixelformat, width, height, 1, 1);

	// FreeImage loads images with bottom-left corner at start of buffer,
	// but we store images with top-left corner at start of buffer
	unsigned int source_bytes_per_pixel = source_bpp / 8;
	if (image_type == FIT_BITMAP) {
		if (color_type == FIC_RGB) {
			const RGBTRIPLE* line = (const RGBTRIPLE*)FreeImage_GetBits_Fn(bitmap);
			const RGBTRIPLE* source = (const RGBTRIPLE*)pointer_offset_const(line, pitch * (height - 1));
			uint8_t* dest = (void*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				line = source;
				for (unsigned int x = 0; x < width; ++x) {
					*dest++ = source->rgbtRed;
					*dest++ = source->rgbtGreen;
					*dest++ = source->rgbtBlue;
					source = pointer_offset_const(source, source_bytes_per_pixel);
				}
				source = (const RGBTRIPLE*)pointer_offset_const(line, -(int)pitch);
			}
			err = 0;
		} else if (color_type == FIC_RGBALPHA) {
			const RGBQUAD* line = (const RGBQUAD*)FreeImage_GetBits_Fn(bitmap);
			const RGBQUAD* source = (const RGBQUAD*)pointer_offset_const(line, pitch * (height - 1));
			uint8_t* dest = (void*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				line = source;
				for (unsigned int x = 0; x < width; ++x) {
					*dest++ = source->rgbRed;
					*dest++ = source->rgbGreen;
					*dest++ = source->rgbBlue;
					*dest++ = source->rgbReserved;
					source = pointer_offset_const(source, source_bytes_per_pixel);
				}
				source = (const RGBQUAD*)pointer_offset_const(line, -(int)pitch);
			}
			err = 0;
		}
	} else {
		if (image_type == FIT_RGB16) {
			const FIRGB16* line = (const FIRGB16*)FreeImage_GetBits_Fn(bitmap);
			const FIRGB16* source = (const FIRGB16*)pointer_offset_const(line, pitch * (height - 1));
			uint16_t* dest = (void*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				line = source;
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
				}
				source = (const FIRGB16*)pointer_offset_const(line, -(int)pitch);
			}
			err = 0;
		} else if (image_type == FIT_RGBA16) {
			const FIRGBA16* line = (const FIRGBA16*)FreeImage_GetBits_Fn(bitmap);
			const FIRGBA16* source = (const FIRGBA16*)pointer_offset_const(line, pitch * (height - 1));
			uint16_t* dest = (void*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				line = source;
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
					*dest++ = source->alpha;
				}
				source = (const FIRGBA16*)pointer_offset_const(line, -(int)pitch);
			}
			err = 0;
		} else if (image_type == FIT_RGBF) {
			const FIRGBF* line = (const FIRGBF*)FreeImage_GetBits_Fn(bitmap);
			const FIRGBF* source = (const FIRGBF*)pointer_offset_const(line, pitch * (height - 1));
			float32_t* dest = (void*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				line = source;
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
				}
				source = (const FIRGBF*)pointer_offset_const(line, -(int)pitch);
				line = source;
			}
			err = 0;
		} else if (image_type == FIT_RGBAF) {
			const FIRGBAF* line = (const FIRGBAF*)FreeImage_GetBits_Fn(bitmap);
			const FIRGBAF* source = (const FIRGBAF*)pointer_offset_const(line, pitch * (height - 1));
			float32_t* dest = (void*)image->data;
			for (unsigned int y = 0; y < height; ++y) {
				line = source;
				for (unsigned int x = 0; x < width; ++x, ++source) {
					*dest++ = source->red;
					*dest++ = source->green;
					*dest++ = source->blue;
					*dest++ = source->alpha;
				}
				source = (const FIRGBAF*)pointer_offset_const(line, -(int)pitch);
			}
			err = 0;
		}
	}

cleanup:
	FreeImage_Unload_Fn(bitmap);

	return !err;
}
