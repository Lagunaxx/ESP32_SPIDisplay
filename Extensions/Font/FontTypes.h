/*
 * FontTypes.h
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: producer
 */

#ifndef DEVICE_DISPLAY_GRAPHICS_FONT_FONTTYPES_H_
	#define DEVICE_DISPLAY_GRAPHICS_FONT_FONTTYPES_H_

	namespace Device{
		namespace Display{
			namespace Graphics{

				typedef struct
				{
					uint16_t gCount;     // Total number of characters
					uint16_t yAdvance;   // Line advance
					uint16_t spaceWidth; // Width of a space character
					int16_t  ascent;     // Height of top of 'd' above baseline, other characters may be taller
					int16_t  descent;    // Offset to bottom of 'p', other characters may have a larger descent
					uint16_t maxAscent;  // Maximum ascent found in font
					uint16_t maxDescent; // Maximum descent found in font
				} fontMetrics;

				typedef struct {
					const uint8_t *chartbl;
					const uint8_t *widthtbl;
					uint8_t height;
					uint8_t baseline;
				} fontinfo;


			}
		}
	}

#endif /* DEVICE_DISPLAY_GRAPHICS_FONT_FONTTYPES_H_ */
