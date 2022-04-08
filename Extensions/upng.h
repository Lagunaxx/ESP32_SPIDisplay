/*
auPNG -- derived from LodePNG version 20100808

Copyright (c) 2005-2010 Lode Vandevenne
Copyright (c) 2010 Sean Middleditch
Copyright (c) 2019 Helco

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.

		2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		distribution.
*/

#if !defined(UPNG_H)
#define UPNG_H

#include "ESP32_SPIDisplay.h"

#ifdef GRAPH_PNG

#include <Arduino.h>
#include <FS.h>
#include "SPIFFS.h"
#include <Types.h>
#include "pebble.h"
#include "Graphics.h"

//using namespace Device::Display;



			typedef enum upng_error {
				UPNG_EOK			= 0, /* success (no error) */
				UPNG_ENOMEM			= 1, /* memory allocation failed */
				UPNG_ENOTFOUND		= 2, /* resource not found (file missing) */
				UPNG_ENOTPNG		= 3, /* image data does not have a PNG header */
				UPNG_EMALFORMED		= 4, /* image data is not a valid PNG image */
				UPNG_EUNSUPPORTED	= 5, /* critical PNG chunk type is not supported */
				UPNG_EUNINTERLACED	= 6, /* image interlacing is not supported */
				UPNG_EUNFORMAT		= 7, /* image color format is not supported */
				UPNG_EPARAM			= 8, /* invalid parameter to method call */
				UPNG_EREAD          = 9  /* read callback did not return all data */
			} upng_error;

namespace Device {
	namespace Display {
		namespace uPNG {

			using Device::Display::Graphics::t_color_r5g6b5;
			using Device::Display::Graphics::t_color_r6g6b6;
			using Device::Display::Graphics::t_color_r8g8b8;
			using Device::Display::Graphics::t_color_r8g8b8a8;
			using Device::Display::Graphics::Graph;

			typedef enum upng_format {
				UPNG_BADFORMAT,
				UPNG_INDEXED1,
				UPNG_INDEXED2,
				UPNG_INDEXED4,
				UPNG_INDEXED8,
				UPNG_RGB8,
				UPNG_RGB16,
				UPNG_RGBA8,
				UPNG_RGBA16,
				UPNG_LUMINANCE1,
				UPNG_LUMINANCE2,
				UPNG_LUMINANCE4,
				UPNG_LUMINANCE8,
				UPNG_LUMINANCE_ALPHA1,
				UPNG_LUMINANCE_ALPHA2,
				UPNG_LUMINANCE_ALPHA4,
				UPNG_LUMINANCE_ALPHA8
			} upng_format;

			typedef struct upng_t upng_t;

			typedef struct __attribute__((__packed__)) rgb {
			  unsigned char r;
			  unsigned char g;
			  unsigned char b;
			} rgb;

			typedef void (*upng_source_free_cb)(void* user);
			typedef unsigned long (*upng_source_read_cb)(void* user, unsigned long offset, void* buffer, unsigned long size);
			typedef struct upng_source
			{
				void* user;
				unsigned long size;
				upng_source_free_cb free;
				upng_source_read_cb read;
			} upng_source;

			class c_uPNG{
			public:
				void DrawFile(char *filename, T_DispCoords x, T_DispCoords y); // Open and draw file
//			private:
				#ifndef UPNG_USE_STDIO
				upng_t*		upng_new_from_file	 (char* path);
				#endif
				upng_t*		upng_new_from_bytes	 (unsigned char* source_buffer, unsigned long source_size, unsigned char**buffer);
				upng_t*     upng_new_from_source (upng_source source);
				void		upng_free			 (upng_t* upng);

				upng_error	upng_header			 (upng_t* upng);
				upng_error	upng_decode			 (upng_t* upng); // backwards-compatibility, shortcut for upng_decode_frame(0)
				upng_error  upng_decode_frame    (upng_t* upng, int frame_index);

				upng_error	upng_get_error		 (upng_t* upng);
				unsigned	upng_get_error_line	 (upng_t* upng);

				unsigned	upng_get_width		 (upng_t* upng);
				unsigned	upng_get_height		 (upng_t* upng);
				int	        upng_get_x_offset	 (upng_t* upng);
				int	        upng_get_y_offset	 (upng_t* upng);
				unsigned	upng_get_bpp		 (upng_t* upng);
				unsigned	upng_get_bitdepth	 (upng_t* upng);
				unsigned	upng_get_components	 (upng_t* upng);
				upng_format	upng_get_format		 (upng_t* upng);
				unsigned    upng_get_frames      (upng_t* upng);
				unsigned    upng_get_plays       (upng_t* upng); // 0 means unlimited plays

				//returns count of entries in palette
				int         upng_get_palette(upng_t* upng, rgb **palette);

				unsigned char*	upng_get_buffer		(upng_t* upng);
				unsigned				upng_get_size		(upng_t* upng);

				//returns keyword and text_out matching keyword
				char* upng_get_text(upng_t* upng, char** text_out, unsigned int index);
				int         upng_get_alpha(upng_t* upng, uint8_t **alpha);

				void upng_GetPixel(void* pixel, upng_t* upng, int x, int y); //Get pixel info from buffer

				uint16_t* colorBuffer_R8G8B8toR5G6B5(upng_t* upng);
				uint8_t* colorBuffer_A8R8G8B8toA8(upng_t* upng);
				upng_t* upng_new(void);

				void post_process_scanlines(upng_t *upng, unsigned char *out, unsigned char *in, upng_t *info_png);
			};

			  extern c_uPNG* uPNG;
			  bool init();
			  bool remove();

		}
	}
}

#endif /*#ifdef GRAPH_PNG*/

#endif /*defined(UPNG_H)*/

