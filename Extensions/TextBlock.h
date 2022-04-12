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
		setFitText(bool fit),
		setFillBackground(bool fill),
		setMoveWholeWord(bool move),
		setSkipFistSpace(bool skip),
		setPosition(t_DispCoords x, t_DispCoords y),
		setSize(t_DispCoords width, t_DispCoords height),
		setLineSpace(t_DispCoords size),
		setFGColor(t_color_r8g8b8 color),
		setBGColor(t_color_r8g8b8 color);

	t_Coordinate2D getPosition();
	t_Size2D getSize();

	void setText(const char *string);

	void Draw();

protected:
	const char * buffer;

private:
	t_DispCoords x, y, TBwidth, TBheight,	// set position and size of text block
		lineSpace;			// set size of space between lines  in pixels
	t_color_r8g8b8 foregroundColor, backgroundColor; // store colors of background and symbol
	bool fillBackground,	// fill background with backgroundColor (true) or not fill(false)
		fitText,			// Fit text to width (may be do not fit in block in height)
		moveWholeWord,		// If part of word do not fit, then try to move whole word on new line
		drawSymbolPart,		// draw symbol if it do not fits partialy
		skipFirstSpace;		// skip fist ' '(space)-symbols in line beginning
	size_t bufLength; // Text length

	t_DispCoords	textWidthFit(t_DispCoords skip = 0),
					textHeightFit();

};


} /* namespace Graphics */
} /* namespace Display */
//} /* namespace Memory */
} /* namespace Device */

#endif /* COMPONENTS_ESP32_SPIDISPLAY_EXTENSIONS_TEXTBLOCK_H_ */
