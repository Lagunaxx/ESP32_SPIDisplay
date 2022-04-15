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
	TBwidth = 0;
	TBheight = 0;
	lineSpace = 0;
	buffer = new(char);
	foregroundColor = (t_color_r8g8b8){0,0,0,0};
	backgroundColor = (t_color_r8g8b8){0,0,0,0};
	fillBackground = false;
	fitText = false;
	moveWholeWord = false;
	drawSymbolPart = false;
	skipFirstSpace = false;
	bufLength = 0;
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
** Function name:           setPosition
** Description :            sets position for TextBlock in pixels
** Args:
**		setx - value for X-coordinate
**		sety - value for Y-coordinate
***************************************************************************************/

void TextBlock::setPosition(t_DispCoords setx, t_DispCoords sety){
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

void TextBlock::setSize(t_DispCoords setwidth, t_DispCoords setheight){
	TBwidth = setwidth;
	TBheight = setheight;
}

/***************************************************************************************
** Function name:           setLineSpace
** Description :            sets size of space between text lines in TextBlock in pixels
** Args:
**		size - size of space
***************************************************************************************/

void TextBlock::setLineSpace(t_DispCoords size){
	lineSpace = size;
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
	rvalue.width = TBwidth;
	rvalue.height = TBheight;
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
	bufLength = strlen(string);
	if(buffer != 0) free((void *)buffer);
	buffer = (char *)malloc(sizeof(const char)*(bufLength));

	for (uint16_t i=0; i<bufLength; i++) {
		memcpy((void *)(buffer+i),string+i,sizeof(const char));
	}
	memset((void *)(buffer+bufLength),0,1);
}

/***************************************************************************************
** Function name:			Draw
** Description :			Draw TextBlock content
** Args:
** 				posx - from what symbol start to print (horizontal scrolling, enabled when
** 						text prints into one line)
** 				posy - from what line start to print (vertical scrolling)
***************************************************************************************/
void TextBlock::Draw(){
	this->Draw(0);
}
void TextBlock::Draw(t_DispCoords posx, t_DispCoords posy){
	char* line;
	t_DispCoords lettersFit = 0;
	t_DispCoords cnt = 0;
	uint8_t lettersPrinted = 0,
			lineNumber = 0;

	// Calculate simbols that fits in line

	while(lettersPrinted < bufLength) {
		if ((lineNumber - posy) >= textHeightFit()) break;
		lettersFit = textWidthFit(lettersPrinted);
		if (skipFirstSpace) {
			cnt = 0;
			while ((*(buffer + cnt + lettersPrinted) == ' ') && (cnt < lettersFit)) {
				cnt++;
			}
			if (cnt != 0) {
				lettersPrinted += cnt;
				continue;
			}
		}

		if (moveWholeWord){
			cnt = lettersFit; if((cnt + lettersPrinted) < bufLength) cnt++;
			if ((bufLength - lettersPrinted - cnt) != 0 ){
				while ((*(buffer + cnt + lettersPrinted - 1) != ' ') && (cnt > 0)) {
					cnt--;
				}
				if (cnt > 0) lettersFit = cnt;
			}
		}

		line = (char *) malloc(sizeof(char)*(lettersFit + 1));

		if(line){
			memset(line, 0, lettersFit + 1);
			memcpy((void *)line,(buffer + lettersPrinted), lettersFit);

//			if (lineNumber >= textHeightFit()) {if(line)free(line); break;}

			if ((y + ((lineNumber - posy) * fontHeight()) < Graph->height()) && (y + ((lineNumber - posy) * fontHeight()) > 0))
				if(lineNumber>=posy)drawString(line, x, y + ((lineNumber - posy) * (fontHeight() + ( ((lineNumber - posy) == 0) ? 0 : lineSpace ) ) ) );
			free(line);
			lineNumber++;
			lettersPrinted+=lettersFit;
		}
	}


}

/***************************************************************************************
** Function name:           textWidthFit
** Description:             Return amount of symbols fits in the width
** Args:
** 				skip - start position in buffer
***************************************************************************************/
t_DispCoords TextBlock::textWidthFit(t_DispCoords skip){
	  t_DispCoords str_width = 0;
	  uint16_t uniCode  = 0;
	  uint8_t scount = 0; // counting symbols
	  uint32_t position = 0;
	#ifdef SMOOTH_FONT
	  if(fontLoaded)
	  {
		while (*(buffer + skip + position))
		{
		  uniCode = decodeUTF8(*buffer + skip + position); position++;
		  if (uniCode)
		  {
			if (uniCode == 0x20) { str_width += gFont.spaceWidth * textsize; if (position > 0) str_width += symbolSpace;}
			else
			{
			  uint16_t gNum = 0;
			  bool found = getUnicodeIndex(uniCode, &gNum);
			  if (found)
			  {
				if(str_width == 0 && gdX[gNum] < 0) {str_width -= gdX[gNum] * textsize;if (position > 0) str_width += symbolSpace;}
				if (*(buffer + position) || isDigits) {str_width += gxAdvance[gNum] * textsize;if (position > 0) str_width += symbolSpace;}
				else {str_width += (gdX[gNum] + gWidth[gNum]) * textsize;if (position > 0) str_width += symbolSpace;}
			  }
			  else {str_width += (gFont.spaceWidth + 1) * textsize;if (position > 0) str_width += symbolSpace;}
			}
		  }
		  if ( str_width < TBwidth) scount++; else break;
		}
		isDigits = false;
		return scount;
	  }
	#endif

	  if (textfont>1 && textfont<9)
	  {
		char *widthtable = (char *)pgm_read_dword( &(fontdata[textfont].widthtbl ) ) - 32; //subtract the 32 outside the loop

		while (*(buffer + skip + position))
		{
		  uniCode = *(buffer + skip + position); position++;
		  if (uniCode > 31 && uniCode < 128){
			  str_width += pgm_read_byte( widthtable + uniCode) * textsize; // Normally we need to subtract 32 from uniCode
			  if (position > 0) str_width += symbolSpace;
		  }
		  else {
			  str_width += pgm_read_byte( widthtable + 32) * textsize; // Set illegal character = space width
			  if (position > 0) str_width += symbolSpace;
		  }
		  if ( str_width < TBwidth) scount++; else break;
		}

	  }
	  else
	  {

	#ifdef LOAD_GFXFF
		if(gfxFont) // New font
		{

		  while (*(buffer + skip + position))
		  {
			uniCode = decodeUTF8(*buffer + skip + position); position++;
			if ((uniCode >= pgm_read_word(&gfxFont->first)) && (uniCode <= pgm_read_word(&gfxFont->last )))
			{
			  uniCode -= pgm_read_word(&gfxFont->first);
			  GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[uniCode]);
			  // If this is not the  last character or is a digit then use xAdvance
			  if (*(buffer + skip + position)  || isDigits) { str_width += pgm_read_byte(&glyph->xAdvance) * textsize;;if (position > 0) str_width += symbolSpace;}
			  // Else use the offset plus TBwidth since this can be bigger than xAdvance
			  else {str_width += ((int8_t)pgm_read_byte(&glyph->xOffset) + pgm_read_byte(&glyph->width)) * textsize;;if (position > 0) str_width += symbolSpace;}
			}
			if ( str_width < TBwidth) scount++; else break;
		  }
		}
		else
	#endif
		{
	#ifdef LOAD_GLCD
		  while (*(buffer + skip + position)) { position++; str_width += 6 * textsize;if (position > 0) str_width += symbolSpace; if ( str_width < TBwidth) scount++; else break;}
	#endif
		}
	  }
	  isDigits = false;
	  return scount;
}

/***************************************************************************************
** Function name:           textHeightFit
** Description:             Return amount of lines fits in the TBheight
***************************************************************************************/
t_DispCoords TextBlock::textHeightFit() {
	t_DispCoords lines, fh;
	fh = fontHeight();
	lines = (TBheight / ( fh + lineSpace ));
	if ((lines * (fh + lineSpace) + fh) <= TBheight) lines++;
	return lines;
}

/***************************************************************************************
** Function name:			setMoveWholeWord
** Description :			if true, then try to move whole word on new line
** Args:
**			move - value to set
***************************************************************************************/
void TextBlock::setMoveWholeWord(bool move) {
	moveWholeWord = move;
}

/***************************************************************************************
** Function name:			setSkipFistSpace
** Description :			if true, then skip all ' '(space)-chars at line beginning
** Args:
**			skip - value to set
***************************************************************************************/
void TextBlock::setSkipFistSpace(bool skip) {
	skipFirstSpace = skip;
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
