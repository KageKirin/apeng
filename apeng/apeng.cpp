#include "apeng.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <png.h>	//! MUST point to apng-patched libpng/png.h

#include <memory>

///////////////////////////////////////////////////////////////////////////////
//! make sure we have proper support for APNG through patched libpng
//! rationale: yes, there are other ways to load APNG even with the unpatched libpng, but what gives

#ifndef PNG_APNG_SUPPORTED
#error "libPNG missing support for APNG. Make sure you use the PATCHED version"
#endif	// PNG_APNG_SUPPORTED

///////////////////////////////////////////////////////////////////////////////
//! globals

enum struct APENG_ERROR : unsigned int
{
	no_error = 0,
	file_invalid,
	data_invalid,
};

///////////////////////////////////////////////////////////////////////////////
//! writer

//--- save API

//! apeng_save_frames_file_blob
//! saves all frames from large buffer frame_blob
//!  frame_blob must be deleted by user using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_file_blob(FILE*		  file,
																   const uint8_t* frames_blob,
																   unsigned int   frames_blob_size,
																   unsigned int   width,
																   unsigned int   height,
																   unsigned int   colortype,
																   unsigned int   rowbytes,
																   unsigned int   frames)
{
	assert(frames_blob);
	assert(frames_blob_size % (height * rowbytes) == 0);
	assert(frames_blob_size / (height * rowbytes * frames) == 1);

	uint8_t** frames_array = (uint8_t**)malloc(frames * sizeof(uint8_t*));
	for (unsigned int i = 0; i < frames; ++i)
	{
		frames_array[i] = (uint8_t*)&frames_blob[i * height * rowbytes];
	}

	unsigned int err = apeng_save_frames_file(file, (const uint8_t**)frames_array, frames, width, height, colortype, rowbytes);

	free(frames_array);

	return err;
}


//! apeng_save_frames_file_nt
//! saves all frames from nullptr-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_file_nt(
  FILE* file, const uint8_t** frames_array, unsigned int width, unsigned int height, unsigned int colortype, unsigned int rowbytes)
{
	assert(frames_array);
	unsigned int frames = 0;

	const uint8_t* cur_frames = *frames_array;
	while (cur_frames != nullptr)
	{
		++frames;
	}

	return apeng_save_frames_file(file, frames_array, frames, width, height, colortype, rowbytes);
}

//! apeng_save_frames_file
//! saves all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_file(FILE*			  file,
															  const uint8_t** frames_array,
															  unsigned int	frames,
															  unsigned int	width,
															  unsigned int	height,
															  unsigned int	colortype,
															  unsigned int	rowbytes)
{
	assert(file);
	assert(frames_array);


	// void save_png(unsigned char* p_frame, unsigned int w, unsigned int h, unsigned int d, unsigned int t, unsigned
	// int frames)

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	assert(png_ptr);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);

	if (png_ptr != nullptr && info_ptr != nullptr && setjmp(png_jmpbuf(png_ptr)) == 0)
	{
		png_init_io(png_ptr, file);
		png_set_compression_level(png_ptr, 9);
		unsigned int channels = rowbytes / width;
		unsigned int bitdepth = 8; //TODO: compute from channels
		png_set_IHDR(png_ptr, info_ptr, width, height, bitdepth, colortype, 0, 0, 0);

#ifdef PNG_APNG_SUPPORTED
		png_set_acTL(png_ptr, info_ptr, frames, 0);
// png_set_first_frame_is_hidden(png_ptr, info_ptr, 1);

#endif	// PNG_APNG_SUPPORTED

		png_write_info(png_ptr, info_ptr);

		png_bytepp rows = (png_bytepp)malloc(height * sizeof(png_bytep));
		assert(rows);

		for (unsigned int frameIdx = 0; frameIdx < frames; ++frameIdx)
		{
			for (unsigned int rowIdx = 0; rowIdx < height; ++rowIdx)
			{
				rows[rowIdx] = (png_bytep)(frames_array[frameIdx] + (rowIdx * rowbytes));
			}

#ifdef PNG_APNG_SUPPORTED
			png_write_frame_head(png_ptr, info_ptr, nullptr, width, height, 0, 0, 12, 100, PNG_DISPOSE_OP_NONE, PNG_BLEND_OP_SOURCE);
#endif	// PNG_APNG_SUPPORTED

			png_write_image(png_ptr, rows);

#ifdef PNG_APNG_SUPPORTED
			png_write_frame_tail(png_ptr, info_ptr);
#endif	// PNG_APNG_SUPPORTED
		}

		free(rows);

		png_write_end(png_ptr, info_ptr);
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return (unsigned int)APENG_ERROR::no_error;
}


//! apeng_save_frames_blob
//! saves all frames from large buffer frame_blob
//!  frame_blob must be deleted by user using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_blob(const char*	filename,
															  const uint8_t* frames_blob,
															  unsigned int   frames_blob_size,
															  unsigned int   width,
															  unsigned int   height,
															  unsigned int   colortype,
															  unsigned int   rowbytes,
															  unsigned int   frames)
{
	std::shared_ptr<FILE> file(fopen(filename, "wb"), fclose);
	assert(file);
	return apeng_save_frames_file_blob(file.get(), frames_blob, frames_blob_size, width, height, colortype, rowbytes, frames);
}


//! apeng_save_frames_nt
//! saves all frames from nullptr-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_nt(
  const char* filename, const uint8_t** frames_array, unsigned int width, unsigned int height, unsigned int colortype, unsigned int rowbytes)
{
	std::shared_ptr<FILE> file(fopen(filename, "wb"), fclose);
	assert(file);
	return apeng_save_frames_file_nt(file.get(), frames_array, width, height, colortype, rowbytes);
}

//! apeng_save_frames
//! saves all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames(const char*	 filename,
														 const uint8_t** frames_array,
														 unsigned int	frames,
														 unsigned int	width,
														 unsigned int	height,
														 unsigned int	colortype,
														 unsigned int	rowbytes)
{
	std::shared_ptr<FILE> file(fopen(filename, "wb"), fclose);
	assert(file);
	return apeng_save_frames_file(file.get(), frames_array, frames, width, height, colortype, rowbytes);
}

///////////////////////////////////////////////////////////////////////////////
//! loader

//! apeng_load_frames_file_blob
//! loads all frames into large buffer frame_blob
//!  frame_blob must be deleted by user using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_file_blob(FILE*		 file,
																   uint8_t**	 frames_blob,
																   unsigned int* frames_blob_size,
																   unsigned int* width,
																   unsigned int* height,
																   unsigned int* channels,
																   unsigned int* rowbytes,
																   unsigned int* frames)
{
	uint8_t**	temp_frames_array;
	unsigned int err = apeng_load_frames_file(file, &temp_frames_array, frames, width, height, channels, rowbytes);

	*frames_blob = (uint8_t*)malloc((*frames) * (*height) * (*rowbytes) * sizeof(uint8_t));

	for (unsigned int i = 0; i < *frames; ++i)
	{
		memcpy(&frames_blob[i * (*height) * (*rowbytes)], temp_frames_array[i], (*height) * (*rowbytes));
		free(temp_frames_array[i]);
	}

	free(temp_frames_array);

	return err;
}


//! apeng_load_frames_file_nt
//! loads all frames into nullptr-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_file_nt(
  FILE* file, uint8_t*** frames_array, unsigned int* width, unsigned int* height, unsigned int* channels, unsigned int* rowbytes)
{
	uint8_t**	temp_frames_array;
	unsigned int frames;
	unsigned int err = apeng_load_frames_file(file, &temp_frames_array, &frames, width, height, channels, rowbytes);

	*frames_array = (uint8_t**)malloc((frames + 1) * sizeof(uint8_t*));
	memset(*frames_array, 0, frames + 1);
	memcpy(*frames_array, temp_frames_array, frames);
	free(temp_frames_array);

	return err;
}

//! apeng_load_frames_file
//! loads all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_file(FILE*			file,
															  uint8_t***	frames_array,
															  unsigned int* frames,
															  unsigned int* width,
															  unsigned int* height,
															  unsigned int* channels,
															  unsigned int* rowbytes)
{
	assert(file);
	assert(frames_array);
	assert(frames);
	assert(width);
	assert(height);
	assert(channels);
	assert(rowbytes);

	unsigned char sig[8];
	if (!(fread(sig, 1, 8, file) == 8 && png_sig_cmp(sig, 0, 8) == 0))
	{
		return (unsigned int)APENG_ERROR::data_invalid;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	assert(png_ptr);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);


	if (png_ptr != nullptr && info_ptr != nullptr && setjmp(png_jmpbuf(png_ptr)) == 0)
	{
		png_init_io(png_ptr, file);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		png_set_expand(png_ptr);
		png_set_strip_16(png_ptr);
		png_set_gray_to_rgb(png_ptr);
		png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
		png_set_bgr(png_ptr);
		(void)png_set_interlace_handling(png_ptr);
		png_read_update_info(png_ptr, info_ptr);

		*width				   = png_get_image_width(png_ptr, info_ptr);
		*height				   = png_get_image_height(png_ptr, info_ptr);
		*channels			   = png_get_channels(png_ptr, info_ptr);
		*rowbytes			   = png_get_rowbytes(png_ptr, info_ptr);
		unsigned int framesize = (*height) * (*rowbytes);

		png_bytepp rows = (png_bytepp)malloc((*height) * sizeof(png_bytep));
		assert(rows);

		*frames		   = 1;
		png_uint_32 w0 = *width;
		png_uint_32 h0 = *height;

#ifdef PNG_APNG_SUPPORTED
		png_uint_32 plays = 0;
		png_uint_32 x0	= 0;
		png_uint_32 y0	= 0;

		unsigned short delay_num = 1;
		unsigned short delay_den = 10;
		unsigned char  displayOp = 0;
		unsigned char  blendOp   = 0;

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL))
		{
			png_get_acTL(png_ptr, info_ptr, frames, &plays);
		}
#endif

		*frames_array = (uint8_t**)malloc(*frames * sizeof(uint8_t*));

		for (unsigned int frameIdx = 0; frameIdx < *frames; ++frameIdx)
		{
			(*frames_array)[frameIdx] = (uint8_t*)malloc(framesize);

			for (unsigned rowIdx = 0; rowIdx < *height; ++rowIdx)
			{
				rows[rowIdx] = (*frames_array)[frameIdx] + (rowIdx * (*rowbytes));
			}

#ifdef PNG_APNG_SUPPORTED
			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL))
			{
				png_read_frame_head(png_ptr, info_ptr);
				png_get_next_frame_fcTL(png_ptr, info_ptr, &w0, &h0, &x0, &y0, &delay_num, &delay_den, &displayOp, &blendOp);
			}
#endif

			png_read_image(png_ptr, rows);
		}

		png_read_end(png_ptr, info_ptr);
		free(rows);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	return (unsigned int)APENG_ERROR::no_error;
}


//! apeng_load_frames_blob
//! loads all frames into large buffer frame_blob
//!  frame_blob must be deleted by user using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_blob(const char*   filename,
															  uint8_t**		frames_blob,
															  unsigned int* frames_blob_size,
															  unsigned int* width,
															  unsigned int* height,
															  unsigned int* channels,
															  unsigned int* rowbytes,
															  unsigned int* frames)
{
	std::shared_ptr<FILE> file(fopen(filename, "rb"), fclose);
	assert(file);
	return apeng_load_frames_file_blob(file.get(), frames_blob, frames_blob_size, width, height, channels, rowbytes, frames);
}


//! apeng_load_frames_nt
//! loads all frames into nullptr-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_nt(
  const char* filename, uint8_t*** frames_array, unsigned int* width, unsigned int* height, unsigned int* channels, unsigned int* rowbytes)
{
	std::shared_ptr<FILE> file(fopen(filename, "rb"), fclose);
	assert(file);
	return apeng_load_frames_file_nt(file.get(), frames_array, width, height, channels, rowbytes);
}

//! apeng_load_frames
//! loads all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames(const char*   filename,
														 uint8_t***	frames_array,
														 unsigned int* frames,
														 unsigned int* width,
														 unsigned int* height,
														 unsigned int* channels,
														 unsigned int* rowbytes)
{
	std::shared_ptr<FILE> file(fopen(filename, "rb"), fclose);
	assert(file);
	return apeng_load_frames_file(file.get(), frames_array, frames, width, height, channels, rowbytes);
}


///////////////////////////////////////////////////////////////////////////////
/// C++

#ifdef __cplusplus

APENG_DLLIMPORT unsigned int APENG_API apeng::load_frames(FILE*			file,
														  uint8_t**		frames_blob,
														  unsigned int* frames_blob_size,
														  unsigned int* width,
														  unsigned int* height,
														  unsigned int* channels,
														  unsigned int* rowbytes,
														  unsigned int* frames)
{
	return ::apeng_load_frames_file_blob(file, frames_blob, frames_blob_size, width, height, channels, rowbytes, frames);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::load_frames(
  FILE* file, uint8_t*** frames_array, unsigned int* width, unsigned int* height, unsigned int* channels, unsigned int* rowbytes)
{
	return ::apeng_load_frames_file_nt(file, frames_array, width, height, channels, rowbytes);
}

APENG_DLLIMPORT unsigned int APENG_API apeng::load_frames(FILE*			file,
														  uint8_t***	frames_array,
														  unsigned int* frames,
														  unsigned int* width,
														  unsigned int* height,
														  unsigned int* channels,
														  unsigned int* rowbytes)
{
	return ::apeng_load_frames_file(file, frames_array, frames, width, height, channels, rowbytes);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::load_frames(const char*   filename,
														  uint8_t**		frames_blob,
														  unsigned int* frames_blob_size,
														  unsigned int* width,
														  unsigned int* height,
														  unsigned int* channels,
														  unsigned int* rowbytes,
														  unsigned int* frames)
{
	return ::apeng_load_frames_blob(filename, frames_blob, frames_blob_size, width, height, channels, rowbytes, frames);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::load_frames(
  const char* filename, uint8_t*** frames_array, unsigned int* width, unsigned int* height, unsigned int* channels, unsigned int* rowbytes)
{
	return ::apeng_load_frames_nt(filename, frames_array, width, height, channels, rowbytes);
}

APENG_DLLIMPORT unsigned int APENG_API apeng::load_frames(const char*   filename,
														  uint8_t***	frames_array,
														  unsigned int* frames,
														  unsigned int* width,
														  unsigned int* height,
														  unsigned int* channels,
														  unsigned int* rowbytes)
{
	return ::apeng_load_frames(filename, frames_array, frames, width, height, channels, rowbytes);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::save_frames(FILE*			 file,
														  const uint8_t* frames_blob,
														  unsigned int   frames_blob_size,
														  unsigned int   width,
														  unsigned int   height,
														  unsigned int   colortype,
														  unsigned int   rowbytes,
														  unsigned int   frames)
{
	return ::apeng_save_frames_file_blob(file, frames_blob, frames_blob_size, width, height, colortype, rowbytes, frames);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::save_frames(
  FILE* file, const uint8_t** frames_array, unsigned int width, unsigned int height, unsigned int colortype, unsigned int rowbytes)
{
	return ::apeng_save_frames_file_nt(file, frames_array, width, height, colortype, rowbytes);
}

APENG_DLLIMPORT unsigned int APENG_API apeng::save_frames(FILE*			  file,
														  const uint8_t** frames_array,
														  unsigned int	frames,
														  unsigned int	width,
														  unsigned int	height,
														  unsigned int	colortype,
														  unsigned int	rowbytes)
{
	return ::apeng_save_frames_file(file, frames_array, frames, width, height, colortype, rowbytes);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::save_frames(const char*	filename,
														  const uint8_t* frames_blob,
														  unsigned int   frames_blob_size,
														  unsigned int   width,
														  unsigned int   height,
														  unsigned int   colortype,
														  unsigned int   rowbytes,
														  unsigned int   frames)
{
	return ::apeng_save_frames_blob(filename, frames_blob, frames_blob_size, width, height, colortype, rowbytes, frames);
}


APENG_DLLIMPORT unsigned int APENG_API apeng::save_frames(
  const char* filename, const uint8_t** frames_array, unsigned int width, unsigned int height, unsigned int colortype, unsigned int rowbytes)
{
	return ::apeng_save_frames_nt(filename, frames_array, width, height, colortype, rowbytes);
}

APENG_DLLIMPORT unsigned int APENG_API apeng::save_frames(const char*	 filename,
														  const uint8_t** frames_array,
														  unsigned int	frames,
														  unsigned int	width,
														  unsigned int	height,
														  unsigned int	colortype,
														  unsigned int	rowbytes)
{
	return ::apeng_save_frames(filename, frames_array, frames, width, height, colortype, rowbytes);
}

#endif	//__cplusplus

///////////////////////////////////////////////////////////////////////////////
/// EOF
