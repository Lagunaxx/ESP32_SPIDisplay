/*
 * Graphics.h
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_GRAPHICS_H_
#define COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_GRAPHICS_H_

#include "ESP32_SPIDisplay.h"
//#include "../ESP32_SPIDisplay.h"

namespace Device {
	namespace Display {
		namespace Graphics {


			struct t_color_r5g6b5{
				union{
					struct{
						int b:5;
						int g:6;
						int r:5;
					};
						uint16_t rgb;
				};
				int null : 16;
			};
			struct t_color_r6g6b6{
				int b:6;
				int g:6;
				int r:6;
				int null:14;
			};
			struct t_color_r8g8b8{
				int r:8;
				int g:8;
				int b:8;
				int null:8;
			};

			struct t_color_r8g8b8a8{
				byte r;
				byte g;
				byte b;
				byte alpha;
			};


			class Graphics: public Screen {
			public:
				Graphics();
				virtual ~Graphics();

			void	fillScreen(uint32_t color),
					drawRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint32_t color),
					drawRoundRect(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, T_DispCoords radius, uint32_t color),
					fillRoundRect(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, T_DispCoords radius, uint32_t color),

					drawCircle(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint32_t color),
					drawCircleHelper(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint8_t cornername, uint32_t color),
					fillCircle(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint32_t color),
					fillCircleHelper(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint8_t cornername, int32_t delta, uint32_t color),

					drawEllipse(T_DispCoords x0, T_DispCoords y0, T_DispCoords rx, T_DispCoords ry, uint16_t color),
					fillEllipse(T_DispCoords x0, T_DispCoords y0, T_DispCoords rx, T_DispCoords ry, uint16_t color),

					drawTriangle(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, T_DispCoords x2, T_DispCoords y2, uint32_t color),
					fillTriangle(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, T_DispCoords x2, T_DispCoords y2, uint32_t color),

					drawBitmap(T_DispCoords x, T_DispCoords y, const uint8_t *bitmap, T_DispCoords w, T_DispCoords h, uint16_t color),
					drawXBitmap(T_DispCoords x, T_DispCoords y, const uint8_t *bitmap, T_DispCoords w, T_DispCoords h, uint16_t color),
					drawXBitmap(T_DispCoords x, T_DispCoords y, const uint8_t *bitmap, T_DispCoords w, T_DispCoords h, uint16_t fgcolor, uint16_t bgcolor),
					setBitmapColor(uint16_t fgcolor, uint16_t bgcolor), // For 1bpp sprites
					drawImageBuffer(T_DispCoords x, T_DispCoords y, void * buffer, T_DispCoords widh, T_DispCoords height),
					drawImageBufferAlpha(T_DispCoords x, T_DispCoords y, void * buffer, uint8_t* alpha, T_DispCoords widh, T_DispCoords height),
					drawImageBufferAlpha(T_DispCoords x, T_DispCoords y, t_color_r5g6b5 color, uint8_t* alpha, T_DispCoords widh, T_DispCoords height);

			// Functions to work with colors
			t_color_r5g6b5* InitColorR5G6B5();
			t_color_r6g6b6* InitColorR6G6B6();
			t_color_r8g8b8* InitColorR8G8B8();
			bool InitColor(t_color_r5g6b5 **dst),
			 InitColor(t_color_r6g6b6 **dst),
			 InitColor(t_color_r8g8b8 **dst);
			void ResetColor(t_color_r5g6b5 *dst),
			 ResetColor(t_color_r6g6b6 *dst),
			 ResetColor(t_color_r8g8b8 *dst),
			 color_R8G8B8A8toR8G8B8(t_color_r8g8b8 *dst, t_color_r8g8b8a8 *src),	//Converts 32bit-color(r8,g8,b8,alpha8) into 24bit-color(r8,g8,b8)
			 color_R8G8B8toR6G6B6(t_color_r6g6b6 *dst, t_color_r8g8b8 *src),		//Converts 24bit-color(r8,g8,b8) into 18bit-color(r6,g6,b6)
			 color_R8G8B8toR5G6B5(t_color_r5g6b5 *dst, t_color_r8g8b8 *src);		//Converts 24bit-color(r8,g8,b8) into 16bit-color(r5,g6,b5)
			bool color_R6G6B6touint32(uint32_t *dst, t_color_r6g6b6 *src),			//Converts 18bit-color(r6,g6,b6) into uint32_t
			 color_R5G6B5touint32(uint32_t *dst, t_color_r5g6b5 *src),				//Converts 16bit-color(r5,g6,b5) into uint32_t
			 color_R6G6B6touint16(uint16_t *dst, t_color_r6g6b6 *src),				//Converts 18bit-color(r6,g6,b6) into uint16_t
			 color_R5G6B5touint16(uint16_t *dst, t_color_r5g6b5 *src);				//Converts 16bit-color(r5,g6,b5) into uint16_t


			  void _bitmap_fg(uint32_t c);
			  void _bitmap_bg(uint32_t c);
			  T_DispCoords __xpivot();
			  T_DispCoords __ypivot();


			int16_t getPivotX(void);
			int16_t getPivotY(void);
			void setPivot(T_DispCoords x, T_DispCoords y);

			uint32_t bitmap_fg, bitmap_bg;
			T_DispCoords _xpivot;   // x pivot point coordinate
			T_DispCoords _ypivot;   // y pivot point coordinate


			};

			  extern Graphics* Graph;
			  bool init(uint32_t color = TFT_BLACK);
			  bool init();
			  bool remove();
// */
		} /* namespace Graphics */
	} /* namespace Display */
} /* namespace Device */

#endif /* COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_GRAPHICS_H_ */
