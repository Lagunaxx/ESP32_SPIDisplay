/*
 * Graphics.h
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_GRAPHICS_H_
#define COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_GRAPHICS_H_

#include "../ESP32_SPIDisplay.h"

namespace Device {
	namespace Display {
		namespace Graphics {

			class Graphics: private Screen {
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
					drawImageBuffer(T_DispCoords x, T_DispCoords y, void * buffer, T_DispCoords widh, T_DispCoords height);
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

			  static Graphics* Graph;
			  bool init(uint32_t color = TFT_BLACK);
			  //bool init();
			  bool remove();

		} /* namespace Graphics */
	} /* namespace Display */
} /* namespace Device */

#endif /* COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_GRAPHICS_H_ */
