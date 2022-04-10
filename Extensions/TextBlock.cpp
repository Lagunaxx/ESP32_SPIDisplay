/*
 * TextBlock.cpp
 *
 *  Created on: 9 апр. 2022 г.
 *      Author: producer
 */

#include "TextBlock.h"

namespace Device {
//namespace Memory {
namespace Display {
namespace Graphics {

TextBlock::TextBlock() {
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	lineSpace = 0;
	symbolSpace = 0;
	buffer = new(c_Buffer);
	fonttype = 0;
	bifont = 0;
	Gfont = 0;
	foregroundColor = 0;
	backgroundColor = 0;
	fillBackground = false;
	fitText = false;
	moveWholeWord = false;
	drawSymbolPart = false;
	fontsize = 1;
}

TextBlock::~TextBlock() {
	if(buffer!=0)delete(buffer);
}

TextBlock::TextBlock(const TextBlock &other) {
	// TODO Auto-generated constructor stub

}

TextBlock::TextBlock(TextBlock &&other) {
	// TODO Auto-generated constructor stub

}

/***************************************************************************************
** Function name:           setFont
** Description :            sets font for text inside TextBlock
** Args:
** 		Font - uint8_t type (from 1 to 9) for built in, or GFXfont-value
** 				for freefont or truetype
***************************************************************************************/

void TextBlock::setFont(uint8_t Font) {
	fonttype = FONT_TYPE_BUILTIN;
	bifont = Font;
}

void TextBlock::setFont(GFXfont Font) {
#warning "Initialize and remove GFXfont youself. TextBlock::setFont(GFXfont) do not initialize and do not delete it! Comment or remove this line to remove warning!"
	/* Gfont mast be initialized out of class TextBlock,
	 * also it mast be deleted out of class TextBlock.
	 * In this class only stores address for initialized GFXfont
	 */
	fonttype = FONT_TYPE_FREEFONT;
	Gfont = Font;
}

/***************************************************************************************
** Function name:           setPosition
** Description :            sets position for TextBlock in pixels
** Args:
**		setx - value for X-coordinate
**		sety - value for Y-coordinate
***************************************************************************************/

void TextBlock::setPosition(T_DispCoords setx, T_DispCoords sety){
	x = setx;
	y = sety;
}

/***************************************************************************************
** Function name:           setSize
** Description :            sets size of TextBlock in pixels
** Args:
**		setwidth - width of TextBlock
**		setheight - height of TextBlock
***************************************************************************************/

void TextBlock::setSize(T_DispCoords setwidth, T_DispCoords setheight){
	width = setwidth;
	height = setheight;
}

/***************************************************************************************
** Function name:           setLineSpace
** Description :            sets size of space between text lines in TextBlock in pixels
** Args:
**		size - size of space
***************************************************************************************/

void TextBlock::setLineSpace(T_DispCoords size){
	lineSpace = size;
}

/***************************************************************************************
** Function name:           setSymbolSpace
** Description :            sets size of space between symbols in TextBlock in pixels
** Args:
**		size - size of space
***************************************************************************************/

void TextBlock::setSymbolSpace(T_DispCoords size){
	symbolSpace = size;
}

/***************************************************************************************
** Function name:           setFGColor
** Description :            sets color for text inside TextBlock
** Args:
**		color - RGB(R8G8B8)-value of color for text
***************************************************************************************/

void TextBlock::setFGColor(t_color_r8g8b8 color){
 foregroundColor = color;
}

/***************************************************************************************
** Function name:           setBGColor
** Description :            sets background color inside TextBlock if fill-background
** 							switched on
** Args:
**		color - RGB(R8G8B8)-value of background color
***************************************************************************************/

void TextBlock::setBGColor(t_color_r8g8b8 color){
	backgroundColor = color;
}

/***************************************************************************************
** Function name:			setTextBlock
** Description :			set most params for TextBlock
** Args:
**		Font - uint8_t type (from 1 to 9) for built in, or GFXfont-value for freefont or truetype
**		setx - value for X-coordinate
**		sety - value for Y-coordinate
**		setwidth - width of TextBlock
**		setheight - height of TextBlock
**		FGcolor - RGB(R8G8B8)-value of color for text
**		BGcolor - RGB(R8G8B8)-value of background color
** 		Font - uint8_t type (from 1 to 9) for built in font
**		linespace - size of space between lines
**		symbolspace - size of space between symbols
***************************************************************************************/
void TextBlock::setTextBlock(T_DispCoords setx, T_DispCoords sety, T_DispCoords setwidth, T_DispCoords setheight,
		t_color_r8g8b8 FGcolor, t_color_r8g8b8 BGcolor,
		uint8_t Font, T_DispCoords linespace, T_DispCoords symbolspace){
	fonttype = FONT_TYPE_BUILTIN;
	bifont = Font;
	x = setx;
	y = sety;
	width = setwidth;
	height = setheight;
	lineSpace = linespace;
	symbolSpace = symbolspace;
	foregroundColor = FGcolor;
	backgroundColor = BGcolor;
}

/***************************************************************************************
** Function name:			setTextBlock
** Description :			set most params for TextBlock
** Args:
**		Font - uint8_t type (from 1 to 9) for built in, or GFXfont-value for freefont or truetype
**		setx - value for X-coordinate
**		sety - value for Y-coordinate
**		setwidth - width of TextBlock
**		setheight - height of TextBlock
**		FGcolor - RGB(R8G8B8)-value of color for text
**		BGcolor - RGB(R8G8B8)-value of background color
** 		Font - GFXfont-value for freefont or truetype
**		linespace - size of space between lines
**		symbolspace - size of space between symbols
***************************************************************************************/
void TextBlock::setTextBlock(T_DispCoords setx, T_DispCoords sety, T_DispCoords setwidth, T_DispCoords setheight,
		t_color_r8g8b8 FGcolor, t_color_r8g8b8 BGcolor,
		GFXfont Font, T_DispCoords linespace, T_DispCoords symbolspace){
	fonttype = FONT_TYPE_FREEFONT;
	Gfont = Font;
	x = setx;
	y = sety;
	width = setwidth;
	height = setheight;
	lineSpace = linespace;
	symbolSpace = symbolspace;
	foregroundColor = FGcolor;
	backgroundColor = BGcolor;
}

/***************************************************************************************
** Function name:			getPosition
** Description :			returns position of TextBlock
***************************************************************************************/
t_Coordinate2D TextBlock::getPosition(){
	t_Coordinate2D rvalue;
	rvalue.x = x;
	rvalue.y = y;
	return rvalue;
}

/***************************************************************************************
** Function name:			getSize
** Description :			returns size of TextBlock
***************************************************************************************/
t_Size2D TextBlock::getSize(){
	t_Size2D rvalue;
	rvalue.width = width;
	rvalue.height = height;
	return rvalue;
}

/***************************************************************************************
** Function name:			setFitText
** Description :			set fit text in width of block
** Args:
**			fit - true if text mast fit to width
***************************************************************************************/
void TextBlock::setFitText(bool fit){
	fitText = fit;
}

/***************************************************************************************
** Function name:			setFillBackground
** Description :			set fill background when drawing text
** Args:
**			fill - true to fill background, false - not fill
***************************************************************************************/
void TextBlock::setFillBackground(bool fill){
	fillBackground = fill;
}

void TextBlock::setText(const char *string){
	uint16_t len = strlen(string);
	if(buffer != 0) free(buffer);
	buffer = malloc(sizeof(const char)*(len+1));

	for (uint16_t i=0; i<len; i++) {
		memcpy(buffer+i,string+i,sizeof(const char));
	}
}

/***************************************************************************************
** Function name:			Draw
** Description :			Draw TextBlock content
***************************************************************************************/
void TextBlock::Draw(){
	char* line;
	T_DispCoords lettersFit = 0;
	if (fonttype == FONT_TYPE_NOTINITIALIZED) return; // exit if no info about font
	if ((fonttype = FONT_TYPE_BUILTIN) & ((bifont < 1) | (bifont > 9))) return; // exit if wrong font for built-in fonts
	if ((fonttype == FONT_TYPE_FREEFONT) & (Gfont == 0)) return; // exit if font not initialized for freefont
	if ((fonttype == FONT_TYPE_VLW) & (VLWFont == "")) return; // exit if font not initialized for vlw-font

	// Calculate simbols that fits in line
	if (fonttype = FONT_TYPE_BUILTIN) lettersFit = width / (5 * fontsize + symbolSpace);
	if (fonttype = FONT_TYPE_FREEFONT)
}
/***************************************************************************************
** Function name:			name
** Description :			desc
** Args:
**			arg1 - value
***************************************************************************************/



} /* namespace Graphics */
} /* namespace Display */
//} /* namespace Memory */
} /* namespace Device */
