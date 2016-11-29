//! APENG
//! simple APNG wrapper
//! requires apng-patched libpng

#ifndef _APENG_H_INC_
#define _APENG_H_INC_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
#define APENG_API __cdecl
#ifdef APENG_BUILD_DLL
#define APENG_DLLIMPORT __declspec(dllexport)
#elif APENG_USE_DLL
#define APENG_DLLIMPORT __declspec(dllimport)
#endif
#else
#define APENG_API
#endif

#ifndef APENG_DLLIMPORT
#define APENG_DLLIMPORT
#endif


#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus


// low-level C-API

//--- load API

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
																   unsigned int* frames);


//! apeng_load_frames_file_nt
//! loads all frames into null-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_file_nt(
  FILE* file, uint8_t*** frames_array, unsigned int* width, unsigned int* height, unsigned int* channels, unsigned int* rowbytes);

//! apeng_load_frames_file
//! loads all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_file(FILE*			file,
															  uint8_t***	frames_array,
															  unsigned int* frames,
															  unsigned int* width,
															  unsigned int* height,
															  unsigned int* channels,
															  unsigned int* rowbytes);


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
															  unsigned int* frames);


//! apeng_load_frames_nt
//! loads all frames into null-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames_nt(const char*   filename,
															uint8_t***	frames_array,
															unsigned int* width,
															unsigned int* height,
															unsigned int* channels,
															unsigned int* rowbytes);

//! apeng_load_frames
//! loads all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_load_frames(const char*   filename,
														 uint8_t***	frames_array,
														 unsigned int* frames,
														 unsigned int* width,
														 unsigned int* height,
														 unsigned int* channels,
														 unsigned int* rowbytes);


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
																   unsigned int   frames);


//! apeng_save_frames_file_nt
//! saves all frames from null-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_file_nt(
  FILE* file, const uint8_t** frames_array, unsigned int width, unsigned int height, unsigned int colortype, unsigned int rowbytes);

//! apeng_save_frames_file
//! saves all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_file(FILE*			  file,
															  const uint8_t** frames_array,
															  unsigned int	frames,
															  unsigned int	width,
															  unsigned int	height,
															  unsigned int	colortype,
															  unsigned int	rowbytes);


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
															  unsigned int   frames);


//! apeng_save_frames_nt
//! saves all frames from null-terminated array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames_nt(const char*		filename,
															const uint8_t** frames_array,
															unsigned int	width,
															unsigned int	height,
															unsigned int	colortype,
															unsigned int	rowbytes);

//! apeng_save_frames
//! saves all frames array of buffers
//! all buffers and the returned array must be deleted using free()
APENG_DLLIMPORT unsigned int APENG_API apeng_save_frames(const char*	 filename,
														 const uint8_t** frames_array,
														 unsigned int	frames,
														 unsigned int	width,
														 unsigned int	height,
														 unsigned int	colortype,
														 unsigned int	rowbytes);


#ifdef __cplusplus
}
#endif	//__cplusplus


#ifdef __cplusplus
// higher-level C++ API
namespace apeng
{
	//! load_frames
	//! loads all frames into large buffer frame_blob
	//!  frame_blob must be deleted by user using free()
	APENG_DLLIMPORT unsigned int APENG_API load_frames(FILE*		 file,
													   uint8_t**	 frames_blob,
													   unsigned int* frames_blob_size,
													   unsigned int* width,
													   unsigned int* height,
													   unsigned int* channels,
													   unsigned int* rowbytes,
													   unsigned int* frames);


	//! load_frames
	//! loads all frames into null-terminated array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API load_frames(
	  FILE* file, uint8_t*** frames_array, unsigned int* width, unsigned int* height, unsigned int* channels, unsigned int* rowbytes);

	//! load_frames
	//! loads all frames array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API load_frames(FILE*		 file,
													   uint8_t***	frames_array,
													   unsigned int* frames,
													   unsigned int* width,
													   unsigned int* height,
													   unsigned int* channels,
													   unsigned int* rowbytes);


	//! load_frames
	//! loads all frames into large buffer frame_blob
	//!  frame_blob must be deleted by user using free()
	APENG_DLLIMPORT unsigned int APENG_API load_frames(const char*   filename,
													   uint8_t**	 frames_blob,
													   unsigned int* frames_blob_size,
													   unsigned int* width,
													   unsigned int* height,
													   unsigned int* channels,
													   unsigned int* rowbytes,
													   unsigned int* frames);


	//! load_frames
	//! loads all frames into null-terminated array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API load_frames(const char*   filename,
													   uint8_t***	frames_array,
													   unsigned int* width,
													   unsigned int* height,
													   unsigned int* channels,
													   unsigned int* rowbytes);

	//! load_frames
	//! loads all frames array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API load_frames(const char*   filename,
													   uint8_t***	frames_array,
													   unsigned int* frames,
													   unsigned int* width,
													   unsigned int* height,
													   unsigned int* channels,
													   unsigned int* rowbytes);


	//--- save API

	//! save_frames
	//! saves all frames from large buffer frame_blob
	//!  frame_blob must be deleted by user using free()
	APENG_DLLIMPORT unsigned int APENG_API save_frames(FILE*		  file,
													   const uint8_t* frames_blob,
													   unsigned int   frames_blob_size,
													   unsigned int   width,
													   unsigned int   height,
													   unsigned int   colortype,
													   unsigned int   rowbytes,
													   unsigned int   frames);


	//! save_frames
	//! saves all frames from null-terminated array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API save_frames(
	  FILE* file, const uint8_t** frames_array, unsigned int width, unsigned int height, unsigned int colortype, unsigned int rowbytes);

	//! save_frames
	//! saves all frames array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API save_frames(FILE*		   file,
													   const uint8_t** frames_array,
													   unsigned int	frames,
													   unsigned int	width,
													   unsigned int	height,
													   unsigned int	colortype,
													   unsigned int	rowbytes);


	//! save_frames
	//! saves all frames from large buffer frame_blob
	//!  frame_blob must be deleted by user using free()
	APENG_DLLIMPORT unsigned int APENG_API save_frames(const char*	filename,
													   const uint8_t* frames_blob,
													   unsigned int   frames_blob_size,
													   unsigned int   width,
													   unsigned int   height,
													   unsigned int   colortype,
													   unsigned int   rowbytes,
													   unsigned int   frames);


	//! save_frames
	//! saves all frames from null-terminated array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API save_frames(const char*	 filename,
													   const uint8_t** frames_array,
													   unsigned int	width,
													   unsigned int	height,
													   unsigned int	colortype,
													   unsigned int	rowbytes);

	//! save_frames
	//! saves all frames array of buffers
	//! all buffers and the returned array must be deleted using free()
	APENG_DLLIMPORT unsigned int APENG_API save_frames(const char*	 filename,
													   const uint8_t** frames_array,
													   unsigned int	frames,
													   unsigned int	width,
													   unsigned int	height,
													   unsigned int	colortype,
													   unsigned int	rowbytes);
}	// namespace apeng
#endif	//__cplusplus


#endif	// !_APENG_H_INC_