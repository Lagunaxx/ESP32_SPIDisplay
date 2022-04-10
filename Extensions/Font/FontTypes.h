/*
 * FontTypes.h
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: producer
 */

#ifndef DEVICE_DISPLAY_GRAPHICS_FONT_FONTTYPES_H_
	#define DEVICE_DISPLAY_GRAPHICS_FONT_FONTTYPES_H_

#define FONT_TYPE_NOTINITIALIZED 0 // default for not initialized
#define FONT_TYPE_BUILTIN	1	// Built-in fonts (numbers from 1 to 9 in setTextFont-function of Fonts object)
#define FONT_TYPE_FREEFONT	2	// Freefonts
#define FONT_TYPE_TRUETYPE	4	// TrueType fonts
#define FONT_TYPE_VLW		8	// VLW fonts


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
