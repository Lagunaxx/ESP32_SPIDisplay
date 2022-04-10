/*
 * TextBlock.h
 *
 *  Created on: 9 апр. 2022 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ESP32_SPIDISPLAY_EXTENSIONS_TEXTBLOCK_H_
#define COMPONENTS_ESP32_SPIDISPLAY_EXTENSIONS_TEXTBLOCK_H_

#include "ESP32_SPIDisplay.h"



#include "cBuffer.h"
#include "Font.h"

namespace Device {
//namespace Memory {
namespace Display {
namespace Graphics {

using namespace Device::Display;
using Device::Memory::c_Buffer;
//using Graphics::t_color_r8g8b8;
//using Graphics::GFXfont;

class TextBlock: public Font {
public:
	TextBlock();
	virtual ~TextBlock();
	TextBlock(const TextBlock &other);
	TextBlock(TextBlock &&other);

	void
		setFont(uint8_t Font),
		setFont(GFXfont Font);

	void
		setFitText(bool fit),
		setFillBackground(bool fill),
		setPosition(T_DispCoords x, T_DispCoords y),
		setSize(T_DispCoords width, T_DispCoords height),
		setLineSpace(T_DispCoords size),
		setSymbolSpace(T_DispCoords size),
		setFGColor(t_color_r8g8b8 color),
		setBGColor(t_color_r8g8b8 color),
		setTextBlock(T_DispCoords x, T_DispCoords y, T_DispCoords width, T_DispCoords height,
				t_color_r8g8b8 FGcolor, t_color_r8g8b8 BGcolor,
				uint8_t Font, T_DispCoords linespace, T_DispCoords symbolspace),
		setTextBlock(T_DispCoords x, T_DispCoords y, T_DispCoords width, T_DispCoords height,
				t_color_r8g8b8 FGcolor, t_color_r8g8b8 BGcolor,
				GFXfont Font, T_DispCoords linespace, T_DispCoords symbolspace);

	t_Coordinate2D getPosition();
	t_Size2D getSize();

	void setText(const char *string);

	void Draw();


private:
	T_DispCoords x, y, width, height,	// set position and size of text block
		lineSpace, symbolSpace;			// set size of space between lines and between symbols in pixels
//	c_Buffer * buffer;	// store text to 2D buffer
	void * buffer;
	uint16_t fonttype;	// Look for FONT_TYPE_* in FontTypes.h
	uint8_t bifont;		// store font name depend on fonttype (uint8_t or GFXfont)
	GFXfont * Gfont;	//
	String VLWFont;		//
	uint8_t fontsize;	// scale of font.
	t_color_r8g8b8 foregroundColor, backgroundColor; // store colors of background and symbol
	bool fillBackground,	// fill background with backgroundColor (true) or not fill(false)
		fitText,			// Fit text to width (may be do not fit in block in height)
		moveWholeWord,		// If part of word do not fit, then try to move whole word on new line
		drawSymbolPart;		// draw symbol if it do not fits partialy
};


} /* namespace Graphics */
} /* namespace Display */
//} /* namespace Memory */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SPIDISPLAY_EXTENSIONS_TEXTBLOCK_H_ */
