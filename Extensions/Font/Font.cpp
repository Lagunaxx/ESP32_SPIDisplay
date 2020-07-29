 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the Screen class and is associated with anti-aliased font functions
 

////////////////////////////////////////////////////////////////////////////////////////
// New anti-aliased (smoothed) font functions added below
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a new font vlw file
*************************************************************************************x*/
#include "Font.h"

namespace Device{
	namespace Display{
		namespace Graphics{

	Font::Font(){
		gFont = { 0, 0, 0, 0, 0, 0, 0 };
		gUnicode = NULL;  //UTF-16 code, the codes are searched so do not need to be sequential
		gHeight = NULL;   //cheight
		gWidth = NULL;    //cwidth
		gxAdvance = NULL; //setWidth
		gdY = NULL;       //topExtent
		gdX = NULL;       //leftExtent
		gBitmap = NULL;   //file pointer to greyscale bitmap
		fontLoaded = false; // Flags when a anti-aliased font is loaded
		fontFile = new fs::File();
		textwrapX=true;
		textwrapY=false;
		textbgcolor=TFT_BLACK;
		textdatum=TL_DATUM;
		textfont=1;
		textcolor=TFT_WHITE;
		decoderBuffer=0;
		gfxFont=0;
		textsize=1;
		_cp437    = true;
		_utf8     = true;
		  isDigits   = false;   // No bounding box adjustment
		  textwrapX  = true;    // Wrap text at end of line when using print stream
		  textwrapY  = false;   // Wrap text at bottom of screen when using print stream
		  textdatum = TL_DATUM; // Top Left text alignment is default
		  padX = 0;             // No padding
		  glyph_bb=0;
		  glyph_ab=0;

		  ImageBuffer=new c_TextBuffer();//Device::Memory::c_Buffer();//

#ifdef LOAD_GLCD
  fontsloaded  = 0x0002; // Bit 1 set
#endif

#ifdef LOAD_FONT2
  fontsloaded |= 0x0004; // Bit 2 set
#endif

#ifdef LOAD_FONT4
  fontsloaded |= 0x0010; // Bit 4 set
#endif

#ifdef LOAD_FONT6
  fontsloaded |= 0x0040; // Bit 6 set
#endif

#ifdef LOAD_FONT7
  fontsloaded |= 0x0080; // Bit 7 set
#endif

#ifdef LOAD_FONT8
  fontsloaded |= 0x0100; // Bit 8 set
#endif

#ifdef LOAD_FONT8N
  fontsloaded |= 0x0200; // Bit 9 set
#endif

#ifdef SMOOTH_FONT
  fontsloaded |= 0x8000; // Bit 15 set
#endif
	}
	Font::~Font(){

	}
		void Font::loadFont(String fontName, fs::FS &ffs)
		{
		  fontFS = ffs;
		  loadFont(fontName, false);
		}
		/***************************************************************************************
		** Function name:           loadFont
		** Description:             loads parameters from a new font vlw file
		*************************************************************************************x*/
		void Font::loadFont(String fontName, bool flash)
		{
		  /*
			The vlw font format does not appear to be documented anywhere, so some reverse
			engineering has been applied!

			Header of vlw file comprises 6 uint32_t parameters (24 bytes total):
			  1. The gCount (number of character glyphs)
			  2. A version number (0xB = 11 for the one I am using)
			  3. The font size (in points, not pixels)
			  4. Deprecated mboxY parameter (typically set to 0)
			  5. Ascent in pixels from baseline to top of "d"
			  6. Descent in pixels from baseline to bottom of "p"

			Next are gCount sets of values for each glyph, each set comprises 7 int32t parameters (28 bytes):
			  1. Glyph Unicode stored as a 32 bit value
			  2. Height of bitmap bounding box
			  3. Width of bitmap bounding box
			  4. gxAdvance for cursor (setWidth in Processing)
			  5. dY = distance from cursor baseline to top of glyph bitmap (signed value +ve = up)
			  6. dX = distance from cursor to left side of glyph bitmap (signed value -ve = left)
			  7. padding value, typically 0

			The bitmaps start next at 24 + (28 * gCount) bytes from the start of the file.
			Each pixel is 1 byte, an 8 bit Alpha value which represents the transparency from
			0xFF foreground colour, 0x00 background. The sketch uses a linear interpolation
			between the foreground and background RGB component colours. e.g.
				pixelRed = ((fgRed * alpha) + (bgRed * (255 - alpha))/255
			To gain a performance advantage fixed point arithmetic is used with rounding and
			division by 256 (shift right 8 bits is faster).

			After the bitmaps is:
			   1 byte for font name string length (excludes null)
			   a zero terminated character string giving the font name
			   1 byte for Postscript name string length
			   a zero/one terminated character string giving the font name
			   last byte is 0 for non-anti-aliased and 1 for anti-aliased (smoothed)

			Then the font name seen by Java when it's created
			Then the postscript name of the font
			Then a boolean to tell if smoothing is on or not.

			Glyph bitmap example is:
			// Cursor coordinate positions for this and next character are marked by 'C'
			// C<------- gxAdvance ------->C  gxAdvance is how far to move cursor for next glyph cursor position
			// |                           |
			// |                           |   ascent is top of "d", descent is bottom of "p"
			// +-- gdX --+             ascent
			// |         +-- gWidth--+     |   gdX is offset to left edge of glyph bitmap
			// |   +     x@.........@x  +  |   gdX may be negative e.g. italic "y" tail extending to left of
			// |   |     @@.........@@  |  |   cursor position, plot top left corner of bitmap at (cursorX + gdX)
			// |   |     @@.........@@ gdY |   gWidth and gHeight are glyph bitmap dimensions
			// |   |     .@@@.....@@@@  |  |
			// | gHeight ....@@@@@..@@  +  +    <-- baseline
			// |   |     ...........@@     |
			// |   |     ...........@@     |   gdY is the offset to the top edge of the bitmap
			// |   |     .@@.......@@. descent plot top edge of bitmap at (cursorY + yAdvance - gdY)
			// |   +     x..@@@@@@@..x     |   x marks the corner pixels of the bitmap
			// |                           |
			// +---------------------------+   yAdvance is y delta for the next line, font size or (ascent + descent)
			//                                  some fonts can overlay in y direction so may need a user adjust value

		  */

		  spiffs = flash;

		  if(spiffs) fontFS = SPIFFS;

		  unloadFont();

		  // Avoid a crash on the ESP32 if the file does not exist
		  if (fontFS.exists("/" + fontName + ".vlw") == false) {
			Serial.println("Font file " + fontName + " not found!");
			return;
		  }

		  *fontFile = fontFS.open( "/" + fontName + ".vlw", "r");

		  if(!fontFile) return;

		  fontFile->seek(0, fs::SeekSet);

		  gFont.gCount   = (uint16_t)readInt32(); // glyph count in file
									 readInt32(); // vlw encoder version - discard
		  gFont.yAdvance = (uint16_t)readInt32(); // Font size in points, not pixels
									 readInt32(); // discard
		  gFont.ascent   = (uint16_t)readInt32(); // top of "d"
		  gFont.descent  = (uint16_t)readInt32(); // bottom of "p"

		  // These next gFont values might be updated when the Metrics are fetched
		  gFont.maxAscent  = gFont.ascent;   // Determined from metrics
		  gFont.maxDescent = gFont.descent;  // Determined from metrics
		  gFont.yAdvance   = gFont.ascent + gFont.descent;
		  gFont.spaceWidth = gFont.yAdvance / 4;  // Guess at space width

		  fontLoaded = true;

		  // Fetch the metrics for each glyph
		  loadMetrics(gFont.gCount);

		  //fontFile.close();
		}


		/***************************************************************************************
		** Function name:           loadMetrics
		** Description:             Get the metrics for each glyph and store in RAM
		*************************************************************************************x*/
		//#define SHOW_ASCENT_DESCENT
		void Font::loadMetrics(uint16_t gCount)
		{
		  uint32_t headerPtr = 24;
		  uint32_t bitmapPtr = 24 + gCount * 28;

		#if defined (ESP32) && defined (CONFIG_SPIRAM_SUPPORT)
		  if ( psramFound() )
		  {
			gUnicode  = (uint16_t*)ps_malloc( gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
			gHeight   =  (uint8_t*)ps_malloc( gCount );    // Height of glyph
			gWidth    =  (uint8_t*)ps_malloc( gCount );    // Width of glyph
			gxAdvance =  (uint8_t*)ps_malloc( gCount );    // xAdvance - to move x cursor
			gdY       =  (int16_t*)ps_malloc( gCount * 2); // offset from bitmap top edge from lowest point in any character
			gdX       =   (int8_t*)ps_malloc( gCount );    // offset for bitmap left edge relative to cursor X
			gBitmap   = (uint32_t*)ps_malloc( gCount * 4); // seek pointer to glyph bitmap in the file
		  }
		  else
		#endif
		  {
			gUnicode  = (uint16_t*)malloc( gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
			gHeight   =  (uint8_t*)malloc( gCount );    // Height of glyph
			gWidth    =  (uint8_t*)malloc( gCount );    // Width of glyph
			gxAdvance =  (uint8_t*)malloc( gCount );    // xAdvance - to move x cursor
			gdY       =  (int16_t*)malloc( gCount * 2); // offset from bitmap top edge from lowest point in any character
			gdX       =   (int8_t*)malloc( gCount );    // offset for bitmap left edge relative to cursor X
			gBitmap   = (uint32_t*)malloc( gCount * 4); // seek pointer to glyph bitmap in the file
		  }

		#ifdef SHOW_ASCENT_DESCENT
		  Serial.print("ascent  = "); Serial.println(gFont.ascent);
		  Serial.print("descent = "); Serial.println(gFont.descent);
		#endif

		  uint16_t gNum = 0;
		  fontFile->seek(headerPtr, fs::SeekSet);
		  while (gNum < gCount)
		  {
			gUnicode[gNum]  = (uint16_t)readInt32(); // Unicode code point value
			gHeight[gNum]   =  (uint8_t)readInt32(); // Height of glyph
			gWidth[gNum]    =  (uint8_t)readInt32(); // Width of glyph
			gxAdvance[gNum] =  (uint8_t)readInt32(); // xAdvance - to move x cursor
			gdY[gNum]       =  (int16_t)readInt32(); // y delta from baseline
			gdX[gNum]       =   (int8_t)readInt32(); // x delta from cursor
			readInt32(); // ignored

			//Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gHeight  = "); Serial.println(gHeight[gNum]);
			//Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gWidth  = "); Serial.println(gWidth[gNum]);
			//Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gxAdvance  = "); Serial.println(gxAdvance[gNum]);
			//Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gdY  = "); Serial.println(gdY[gNum]);

			// Different glyph sets have different ascent values not always based on "d", so we could get
			// the maximum glyph ascent by checking all characters. BUT this method can generate bad values
			// for non-existant glyphs, so we will reply on processing for the value and disable this code for now...
			/*
			if (gdY[gNum] > gFont.maxAscent)
			{
			  // Try to avoid UTF coding values and characters that tend to give duff values
			  if (((gUnicode[gNum] > 0x20) && (gUnicode[gNum] < 0x7F)) || (gUnicode[gNum] > 0xA0))
			  {
				gFont.maxAscent   = gdY[gNum];
		#ifdef SHOW_ASCENT_DESCENT
				Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", maxAscent  = "); Serial.println(gFont.maxAscent);
		#endif
			  }
			}
			*/

			// Different glyph sets have different descent values not always based on "p", so get maximum glyph descent
			if (((int16_t)gHeight[gNum] - (int16_t)gdY[gNum]) > gFont.maxDescent)
			{
			  // Avoid UTF coding values and characters that tend to give duff values
			  if (((gUnicode[gNum] > 0x20) && (gUnicode[gNum] < 0xA0) && (gUnicode[gNum] != 0x7F)) || (gUnicode[gNum] > 0xFF))
			  {
				gFont.maxDescent   = gHeight[gNum] - gdY[gNum];
		#ifdef SHOW_ASCENT_DESCENT
				Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", maxDescent = "); Serial.println(gHeight[gNum] - gdY[gNum]);
		#endif
			  }
			}

			gBitmap[gNum] = bitmapPtr;

			headerPtr += 28;

			bitmapPtr += gWidth[gNum] * gHeight[gNum];

			gNum++;
			yield();
		  }

		  gFont.yAdvance = gFont.maxAscent + gFont.maxDescent;

		  gFont.spaceWidth = (gFont.ascent + gFont.descent) * 2/7;  // Guess at space width
		}


		/***************************************************************************************
		** Function name:           deleteMetrics
		** Description:             Delete the old glyph metrics and free up the memory
		*************************************************************************************x*/
		void Font::unloadFont( void )
		{
		  if (gUnicode)
		  {
			free(gUnicode);
			gUnicode = NULL;
		  }

		  if (gHeight)
		  {
			free(gHeight);
			gHeight = NULL;
		  }

		  if (gWidth)
		  {
			free(gWidth);
			gWidth = NULL;
		  }

		  if (gxAdvance)
		  {
			free(gxAdvance);
			gxAdvance = NULL;
		  }

		  if (gdY)
		  {
			free(gdY);
			gdY = NULL;
		  }

		  if (gdX)
		  {
			free(gdX);
			gdX = NULL;
		  }

		  if (gBitmap)
		  {
			free(gBitmap);
			gBitmap = NULL;
		  }

		  if(fontFile) fontFile->close();
		  fontLoaded = false;
		}


		/***************************************************************************************
		** Function name:           decodeUTF8
		** Description:             Line buffer UTF-8 decoder with fall-back to extended ASCII
		*************************************************************************************x*/
		/* Function moved to Screen.cpp
		#define DECODE_UTF8
		uint16_t Font::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
		{
		  byte c = buf[(*index)++];
		  //Serial.print("Byte from string = 0x"); Serial.println(c, HEX);

		#ifdef DECODE_UTF8
		  // 7 bit Unicode
		  if ((c & 0x80) == 0x00) return c;

		  // 11 bit Unicode
		  if (((c & 0xE0) == 0xC0) && (remaining > 1))
			return ((c & 0x1F)<<6) | (buf[(*index)++]&0x3F);

		  // 16 bit Unicode
		  if (((c & 0xF0) == 0xE0) && (remaining > 2))
		  {
			c = ((c & 0x0F)<<12) | ((buf[(*index)++]&0x3F)<<6);
			return  c | ((buf[(*index)++]&0x3F));
		  }

		  // 21 bit Unicode not supported so fall-back to extended ASCII
		  // if ((c & 0xF8) == 0xF0) return c;
		#endif

		  return c; // fall-back to extended ASCII
		}
		*/

		/***************************************************************************************
		** Function name:           decodeUTF8
		** Description:             Serial UTF-8 decoder with fall-back to extended ASCII
		*************************************************************************************x*/
		/* Function moved to Screen.cpp
		uint16_t Font::decodeUTF8(uint8_t c)
		{

		#ifdef DECODE_UTF8

		  // 7 bit Unicode
		  if ((c & 0x80) == 0x00) {
			decoderState = 0;
			return (uint16_t)c;
		  }

		  if (decoderState == 0)
		  {
			// 11 bit Unicode
			if ((c & 0xE0) == 0xC0)
			{
			  decoderBuffer = ((c & 0x1F)<<6);
			  decoderState = 1;
			  return 0;
			}

			// 16 bit Unicode
			if ((c & 0xF0) == 0xE0)
			{
			  decoderBuffer = ((c & 0x0F)<<12);
			  decoderState = 2;
			  return 0;
			}
			// 21 bit Unicode not supported so fall-back to extended ASCII
			if ((c & 0xF8) == 0xF0) return (uint16_t)c;
		  }
		  else
		  {
			if (decoderState == 2)
			{
			  decoderBuffer |= ((c & 0x3F)<<6);
			  decoderState--;
			  return 0;
			}
			else
			{
			  decoderBuffer |= (c & 0x3F);
			  decoderState = 0;
			  return decoderBuffer;
			}
		  }
		#endif

		  decoderState = 0;
		  return (uint16_t)c; // fall-back to extended ASCII
		}
		*/



		/***************************************************************************************
		** Function name:           alphaBlend
		** Description:             Blend foreground and background and return new colour
		*************************************************************************************x*/
		uint16_t Font::alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc)
		{
		  // For speed use fixed point maths and rounding to permit a power of 2 division
		  uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
		  uint16_t fgG = ((fgc >>  4) & 0x7E) + 1;
		  uint16_t fgB = ((fgc <<  1) & 0x3E) + 1;

		  uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
		  uint16_t bgG = ((bgc >>  4) & 0x7E) + 1;
		  uint16_t bgB = ((bgc <<  1) & 0x3E) + 1;

		  // Shift right 1 to drop rounding bit and shift right 8 to divide by 256
		  uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
		  uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
		  uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

		  // Combine RGB565 colours into 16 bits
		  //return ((r&0x18) << 11) | ((g&0x30) << 5) | ((b&0x18) << 0); // 2 bit greyscale
		  //return ((r&0x1E) << 11) | ((g&0x3C) << 5) | ((b&0x1E) << 0); // 4 bit greyscale
		  return (r << 11) | (g << 5) | (b << 0);
		}


		/***************************************************************************************
		** Function name:           readInt32
		** Description:             Get a 32 bit integer from the font file
		*************************************************************************************x*/
		uint32_t Font::readInt32(void)
		{
		  uint32_t val = 0;
		  val |= fontFile->read() << 24;
		  val |= fontFile->read() << 16;
		  val |= fontFile->read() << 8;
		  val |= fontFile->read();
		  return val;
		}


		/***************************************************************************************
		** Function name:           getUnicodeIndex
		** Description:             Get the font file index of a Unicode character
		*************************************************************************************x*/
		bool Font::getUnicodeIndex(uint16_t unicode, uint16_t *index)
		{
		  for (uint16_t i = 0; i < gFont.gCount; i++)
		  {
			if (gUnicode[i] == unicode)
			{
			  *index = i;
			  return true;
			}
		  }
		  return false;
		}


		/***************************************************************************************
		** Function name:           drawGlyph
		** Description:             Write a character to the TFT cursor position
		*************************************************************************************x*/
		// Expects file to be open
		void Font::drawGlyph(uint16_t code)//, c_Cursor &cursor)
		{
			_CoordsType cursor_x = ImageBuffer->X();
			_CoordsType cursor_y = ImageBuffer->Y();

		  if (code < 0x21)
		  {
			if (code == 0x20) {
			  cursor_x += gFont.spaceWidth;
			  ImageBuffer->X(cursor_x);
			  return;
			}

			if (code == '\n') {
			  ImageBuffer->X(0);
			  cursor_y += gFont.yAdvance;
			  if (cursor_y >= __height()) cursor_y = 0;
			  ImageBuffer->Set(cursor_x,cursor_y);
			  return;
			}
		  }

		  uint16_t gNum = 0;
		  bool found = getUnicodeIndex(code, &gNum);

		  uint16_t fg = _textcolor();
		  uint16_t bg = _textbgcolor();

		  if (found)
		  {

			if (textwrapX && (cursor_x + gWidth[gNum] + gdX[gNum] > Device::Display::Display->width()))
			{
			  cursor_y += gFont.yAdvance;
			  cursor_x = 0;
			}
			if (textwrapY && ((cursor_y + gFont.yAdvance) >= Device::Display::Display->height())) cursor_y = 0;
			if (cursor_x == 0) cursor_x -= gdX[gNum];

			fontFile->seek(gBitmap[gNum], fs::SeekSet); // This is taking >30ms for a significant position shift

			uint8_t pbuffer[gWidth[gNum]];

			_CoordsType xs = 0;
			uint32_t dl = 0;

			_CoordsType cy = cursor_y + gFont.maxAscent - gdY[gNum];
			_CoordsType cx = cursor_x + gdX[gNum];
			ImageBuffer->init(2,sizeof(uint16_t),cx,cy);

//			startWrite(); // Avoid slow ESP32 transaction overhead for every pixel

			for (int y = 0; y < gHeight[gNum]; y++)
			{
			  if (spiffs)
			  {
				fontFile->read(pbuffer, gWidth[gNum]);
				//Serial.println("SPIFFS");
			  }
			  else
			  {
//				endWrite();    // Release SPI for SD card transaction
				fontFile->read(pbuffer, gWidth[gNum]);
//				startWrite();  // Re-start SPI for TFT transaction
				//Serial.println("Not SPIFFS");
			  }

			  /*
			   * ToDo: Modify pbuffer ( it contains 1 byte-per-pixel color,
			   *  need to make function to reproduce this kind of byte-printing for Graphics driver)
			   */

			  for (int x = 0; x < gWidth[gNum]; x++)
			  {
				uint8_t pixel = pbuffer[x]; //<//
				if (pixel)
				{
/*
					if (pixel != 0xFF)
				  {
					if (dl) {
					  if (dl==1) drawPixel(xs, y + cy, fg);
					  else drawFastHLine( xs, y + cy, dl, fg);
					  dl = 0;
					}
					drawPixel(x + cx, y + cy, alphaBlend(pixel, fg, bg));
				  }
				  else
				  {
					if (dl==0) xs = x + cx;
					dl++;
				  }// */

				}
				else
				{
//				  if (dl) { drawFastHLine( xs, y + cy, dl, fg); dl = 0; }
				}
			  }
//			  if (dl) { drawFastHLine( xs, y + cy, dl, fg); dl = 0; }
			}// */

			cursor_x += gxAdvance[gNum];
//			endWrite();
		  }
		  else
		  {
			// Not a Unicode in font so draw a rectangle and move on cursor
//			drawRect(cursor_x, cursor_y + gFont.maxAscent - gFont.ascent, gFont.spaceWidth, gFont.ascent, fg);
			cursor_x += gFont.spaceWidth + 1;
		  }
		  ImageBuffer->Set(cursor_x, cursor_y);
		}

		/***************************************************************************************
		** Function name:           showFont
		** Description:             Page through all characters in font, td ms between screens
		*************************************************************************************x*/
		void Font::showFont(uint32_t td, Screen &display)
		{
/*		  if(!fontLoaded) return;

		  if(!fontFile)
		  {
			fontLoaded = false;
			return;
		  }

		  int16_t cursorX = Device::Display::Driver->width(); // Force start of new page to initialise cursor
		  int16_t cursorY = Device::Display::Driver->height();// for the first character
		  uint32_t timeDelay = 0;    // No delay before first page
		  c_Cursor *cursor=Cursor();

		  fillScreen(_textbgcolor());
		  //fillScreen(textbgcolor);

		  for (uint16_t i = 0; i < gFont.gCount; i++)
		  {
			// Check if this will need a new screen
			if (cursorX + gdX[i] + gWidth[i] >= Device::Display::Driver->width())  {
			  cursorX = -gdX[i];

			  cursorY += gFont.yAdvance;
			  if (cursorY + gFont.maxAscent + gFont.descent >= Device::Display::Driver->height()) {
				cursorX = -gdX[i];
				cursorY = 0;
				delay(timeDelay);
				timeDelay = td;
				fillScreen(_textbgcolor());
			  }
			}

			ImageBuffer->Set(cursorX, cursorY);
			drawGlyph(gUnicode[i], *cursor);
			cursorX += gxAdvance[i];
			//cursorX +=  printToSprite( cursorX, cursorY, i );
			yield();
		  }

		  delay(timeDelay);
		  fillScreen(_textbgcolor());
		  //fillScreen(textbgcolor);
		  //fontFile.close();

*/		}


		/**************************************************************************
		** Function name:           setAttribute
		** Description:             Sets a control parameter of an attribute
		**************************************************************************/
		void Font::setAttribute(uint8_t attr_id, uint8_t param) {
			switch (attr_id) {
					break;
				case 1:
					_cp437 = param;
					break;
				case 2:
					_utf8  = param;
					break;
				//case 3: // TBD future feature control
				//    _tbd = param;
				//    break;
			}
		}


		/**************************************************************************
		** Function name:           getAttribute
		** Description:             Get value of an attribute (control parameter)
		**************************************************************************/
		uint8_t Font::getAttribute(uint8_t attr_id) {
			switch (attr_id) {
				case 1: // ON/OFF control of full CP437 character set
					return _cp437;
					break;
				case 2: // ON/OFF control of UTF-8 decoding
					return _utf8;
					break;
				//case 3: // TBD future feature control
				//    return _tbd;
				//    break;
			}

			return false;
		}

		/***************************************************************************************
		** Function name:           setTextSize
		** Description:             Set the text size multiplier
		***************************************************************************************/
		void Font::setTextSize(uint8_t s)
		{
		  if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
		  textsize = (s > 0) ? s : 1; // Don't allow font size 0
		}


		/***************************************************************************************
		** Function name:           setTextColor
		** Description:             Set the font foreground colour (background is transparent)
		***************************************************************************************/
		void Font::setTextColor(uint16_t c)
		{
		  // For 'transparent' background, we'll set the bg
		  // to the same as fg instead of using a flag
		  textcolor = textbgcolor = c;
		}


		/***************************************************************************************
		** Function name:           setTextColor
		** Description:             Set the font foreground and background colour
		***************************************************************************************/
		void Font::setTextColor(uint16_t c, uint16_t b)
		{
		  textcolor   = c;
		  textbgcolor = b;
		}

		/***************************************************************************************
		** Function name:           setTextWrap
		** Description:             Define if text should wrap at end of line
		***************************************************************************************/
		void Font::setTextWrap(boolean wrapX, boolean wrapY)
		{
		  textwrapX = wrapX;
		  textwrapY = wrapY;
		}


		/***************************************************************************************
		** Function name:           setTextDatum
		** Description:             Set the text position reference datum
		***************************************************************************************/
		void Font::setTextDatum(uint8_t d)
		{
		  textdatum = d;
		}


		/***************************************************************************************
		** Function name:           setTextPadding
		** Description:             Define padding width (aids erasing old text and numbers)
		***************************************************************************************/
		void Font::setTextPadding(T_DispCoords x_width)
		{
		  padX = x_width;
		}

		/***************************************************************************************
		** Function name:           textWidth
		** Description:             Return the width in pixels of a string in a given font
		***************************************************************************************/
		T_DispCoords Font::textWidth(const String& string)
		{
		  int16_t len = string.length() + 2;
		  char buffer[len];
		  string.toCharArray(buffer, len);
		  return textWidth(buffer, textfont);
		}

		T_DispCoords Font::textWidth(const String& string, uint8_t font)
		{
		  int16_t len = string.length() + 2;
		  char buffer[len];
		  string.toCharArray(buffer, len);
		  return textWidth(buffer, font);
		}

		T_DispCoords Font::textWidth(const char *string)
		{
		  return textWidth(string, textfont);
		}

		T_DispCoords Font::textWidth(const char *string, uint8_t font)
		{
			T_DispCoords str_width = 0;
		  uint16_t uniCode  = 0;

		#ifdef SMOOTH_FONT
		  if(fontLoaded)
		  {
			while (*string)
			{
			  uniCode = decodeUTF8(*string++);
			  if (uniCode)
			  {
				if (uniCode == 0x20) str_width += gFont.spaceWidth;
				else
				{
				  uint16_t gNum = 0;
				  bool found = getUnicodeIndex(uniCode, &gNum);
				  if (found)
				  {
					if(str_width == 0 && gdX[gNum] < 0) str_width -= gdX[gNum];
					if (*string || isDigits) str_width += gxAdvance[gNum];
					else str_width += (gdX[gNum] + gWidth[gNum]);
				  }
				  else str_width += gFont.spaceWidth + 1;
				}
			  }
			}
			isDigits = false;
			return str_width;
		  }
		#endif

		  if (font>1 && font<9)
		  {
			char *widthtable = (char *)pgm_read_dword( &(fontdata[font].widthtbl ) ) - 32; //subtract the 32 outside the loop

			while (*string)
			{
			  uniCode = *(string++);
			  if (uniCode > 31 && uniCode < 128)
			  str_width += pgm_read_byte( widthtable + uniCode); // Normally we need to subtract 32 from uniCode
			  else str_width += pgm_read_byte( widthtable + 32); // Set illegal character = space width
			}

		  }
		  else
		  {

		#ifdef LOAD_GFXFF
			if(gfxFont) // New font
			{
			  while (*string)
			  {
				uniCode = decodeUTF8(*string++);
				if ((uniCode >= pgm_read_word(&gfxFont->first)) && (uniCode <= pgm_read_word(&gfxFont->last )))
				{
				  uniCode -= pgm_read_word(&gfxFont->first);
				  GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[uniCode]);
				  // If this is not the  last character or is a digit then use xAdvance
				  if (*string  || isDigits) str_width += pgm_read_byte(&glyph->xAdvance);
				  // Else use the offset plus width since this can be bigger than xAdvance
				  else str_width += ((int8_t)pgm_read_byte(&glyph->xOffset) + pgm_read_byte(&glyph->width));
				}
			  }
			}
			else
		#endif
			{
		#ifdef LOAD_GLCD
			  while (*string++) str_width += 6;
		#endif
			}
		  }
		  isDigits = false;
		  return str_width * textsize;
		}


		/***************************************************************************************
		** Function name:           fontsLoaded
		** Description:             return an encoded 16 bit value showing the fonts loaded
		***************************************************************************************/
		// Returns a value showing which fonts are loaded (bit N set =  Font N loaded)

		uint16_t Font::fontsLoaded(void)
		{
		  return fontsloaded;
		}


		/***************************************************************************************
		** Function name:           fontHeight
		** Description:             return the height of a font (yAdvance for free fonts)
		***************************************************************************************/
		T_DispCoords Font::fontHeight(int16_t font)
		{
		#ifdef SMOOTH_FONT
		  if(fontLoaded) return gFont.yAdvance;
		#endif

		#ifdef LOAD_GFXFF
		  if (font==1)
		  {
			if(gfxFont) // New font
			{
			  return pgm_read_byte(&gfxFont->yAdvance) * textsize;
			}
		  }
		#endif
		  return pgm_read_byte( &fontdata[font].height ) * textsize;
		}

		T_DispCoords Font::fontHeight(void)
		{
		  return fontHeight(textfont);
		}
		/***************************************************************************************
		** Function name:           drawString (with or without user defined font)
		** Description :            draw string with padding if it is defined
		***************************************************************************************/
		// Without font number, uses font set by setTextFont()
		T_DispCoords Font::drawString(const String& string, T_DispCoords poX, T_DispCoords poY)
		{
		  int16_t len = string.length() + 2;
		  char buffer[len];
		  string.toCharArray(buffer, len);
		  return drawString(buffer, poX, poY, textfont);
		}
		// With font number
		T_DispCoords Font::drawString(const String& string, T_DispCoords poX, T_DispCoords poY, uint8_t font)
		{
		  int16_t len = string.length() + 2;
		  char buffer[len];
		  string.toCharArray(buffer, len);
		  return drawString(buffer, poX, poY, font);
		}

		// Without font number, uses font set by setTextFont()
		T_DispCoords Font::drawString(const char *string, T_DispCoords poX, T_DispCoords poY)
		{
		  return drawString(string, poX, poY, textfont);
		}

		// With font number. Note: font number is over-ridden if a smooth font is loaded
		T_DispCoords Font::drawString(const char *string, T_DispCoords poX, T_DispCoords poY, uint8_t font)
		{
		  int16_t sumX = 0;
		  uint8_t padding = 1, baseline = 0;
		  uint16_t cwidth = textWidth(string, font); // Find the pixel width of the string in the font
		  uint16_t cheight = 8 * textsize;

		#ifdef LOAD_GFXFF
		  #ifdef SMOOTH_FONT
			bool freeFont = (font == 1 && gfxFont && !fontLoaded);
		  #else
			bool freeFont = (font == 1 && gfxFont);
		  #endif

		  if (freeFont) {
			cheight = glyph_ab * textsize;
			poY += cheight; // Adjust for baseline datum of free fonts
			baseline = cheight;
			padding =101; // Different padding method used for Free Fonts

			// We need to make an adjustment for the bottom of the string (eg 'y' character)
			if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
			  cheight += glyph_bb * textsize;
			}
		  }
		#endif


		  // If it is not font 1 (GLCD or free font) get the baseline and pixel height of the font
		#ifdef SMOOTH_FONT
		  if(fontLoaded) {
			baseline = gFont.maxAscent;
			cheight  = fontHeight();
		  }
		  else
		#endif
		  if (font!=1) {
			baseline = pgm_read_byte( &fontdata[font].baseline ) * textsize;
			cheight = fontHeight(font);
		  }

		  if (textdatum || padX)
		  {

			switch(textdatum) {
			  case TC_DATUM:
				poX -= cwidth/2;
				padding += 1;
				break;
			  case TR_DATUM:
				poX -= cwidth;
				padding += 2;
				break;
			  case ML_DATUM:
				poY -= cheight/2;
				//padding += 0;
				break;
			  case MC_DATUM:
				poX -= cwidth/2;
				poY -= cheight/2;
				padding += 1;
				break;
			  case MR_DATUM:
				poX -= cwidth;
				poY -= cheight/2;
				padding += 2;
				break;
			  case BL_DATUM:
				poY -= cheight;
				//padding += 0;
				break;
			  case BC_DATUM:
				poX -= cwidth/2;
				poY -= cheight;
				padding += 1;
				break;
			  case BR_DATUM:
				poX -= cwidth;
				poY -= cheight;
				padding += 2;
				break;
			  case L_BASELINE:
				poY -= baseline;
				//padding += 0;
				break;
			  case C_BASELINE:
				poX -= cwidth/2;
				poY -= baseline;
				padding += 1;
				break;
			  case R_BASELINE:
				poX -= cwidth;
				poY -= baseline;
				padding += 2;
				break;
			}
			// Check coordinates are OK, adjust if not
			if (poX < 0) poX = 0;
//			if (poX+cwidth > Device::Display::Driver->width())   poX = Device::Display::Driver->width() - cwidth;
			if (poY < 0) poY = 0;
//			if (poY+cheight-baseline> Device::Display::Driver->height()) poY = Device::Display::Driver->height() - cheight;
		  }


		  int8_t xo = 0;
		#ifdef LOAD_GFXFF
		  if (freeFont && (textcolor!=textbgcolor))
			{
			  cheight = (glyph_ab + glyph_bb) * textsize;
			  // Get the offset for the first character only to allow for negative offsets
			  uint16_t c2 = 0;
			  uint16_t len = strlen(string);
			  uint16_t n = 0;

			  while (n < len && c2 == 0) c2 = decodeUTF8((uint8_t*)string, &n, len - n);

			  if((c2 >= pgm_read_word(&gfxFont->first)) && (c2 <= pgm_read_word(&gfxFont->last) ))
			  {
				c2 -= pgm_read_word(&gfxFont->first);
				GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
				xo = pgm_read_byte(&glyph->xOffset) * textsize;
				// Adjust for negative xOffset
				if (xo > 0) xo = 0;
				else cwidth -= xo;
				// Add 1 pixel of padding all round
				//cheight +=2;
				//fillRect(poX+xo-1, poY - 1 - glyph_ab * textsize, cwidth+2, cheight, textbgcolor);
//				fillRect(poX+xo, poY - glyph_ab * textsize, cwidth, cheight, textbgcolor); //ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
			  }
			  padding -=100;
			}
		#endif

		  uint16_t len = strlen(string);
		  uint16_t n = 0;

		#ifdef SMOOTH_FONT
		  if(fontLoaded)
		  {
//			if (textcolor!=textbgcolor) fillRect(poX, poY, cwidth, cheight, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
			//drawLine(poX - 5, poY, poX + 5, poY, TFT_GREEN);
			//drawLine(poX, poY - 5, poX, poY + 5, TFT_GREEN);
			//fontFile = SPIFFS.open( _gFontFilename, "r");
			if(!fontFile) return 0;

//			ImageBuffer->Set(poX, poY);

			while (n < len)
			{
			  uint16_t uniCode = decodeUTF8((uint8_t*)string, &n, len - n);
//			  drawGlyph(uniCode, *cursor); //ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
			}
			sumX += cwidth;
			//fontFile.close();
		  }
		  else
		#endif
		  {
			while (n < len)
			{
			  uint16_t uniCode = decodeUTF8((uint8_t*)string, &n, len - n);
			  sumX += drawChar(uniCode, poX+sumX, poY, font);
			}
		  }

		//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// Switch on debugging for the padding areas
		//#define PADDING_DEBUG

		#ifndef PADDING_DEBUG
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		  if((padX>cwidth) && (textcolor!=textbgcolor))
		  {
			int16_t padXc = poX+cwidth+xo;
		#ifdef LOAD_GFXFF
			if (freeFont)
			{
			  poX +=xo; // Adjust for negative offset start character
			  poY -= glyph_ab * textsize;
			  sumX += poX;
			}
		#endif
			switch(padding) {
			  case 1:
//				fillRect(padXc,poY,padX-cwidth,cheight, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				break;
			  case 2:
//				fillRect(padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				padXc = (padX-cwidth)>>1;
				if (padXc>poX) padXc = poX;
//				fillRect(poX - padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				break;
			  case 3:
				if (padXc>padX) padXc = padX;
//				fillRect(poX + cwidth - padXc,poY,padXc-cwidth,cheight, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				break;
			}
		  }


		#else

		//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// This is debug code to show text (green box) and blanked (white box) areas
		// It shows that the padding areas are being correctly sized and positioned

		  if((padX>sumX) && (textcolor!=textbgcolor))
		  {
			int16_t padXc = poX+sumX; // Maximum left side padding
		#ifdef LOAD_GFXFF
			if ((font == 1) && (gfxFont)) poY -= glyph_ab;
		#endif
			drawRect(poX,poY,sumX,cheight, TFT_GREEN);
			switch(padding) {
			  case 1:
				drawRect(padXc,poY,padX-sumX,cheight, TFT_WHITE);
				break;
			  case 2:
				drawRect(padXc,poY,(padX-sumX)>>1, cheight, TFT_WHITE);
				padXc = (padX-sumX)>>1;
				if (padXc>poX) padXc = poX;
				drawRect(poX - padXc,poY,(padX-sumX)>>1,cheight, TFT_WHITE);
				break;
			  case 3:
				if (padXc>padX) padXc = padX;
				drawRect(poX + sumX - padXc,poY,padXc-sumX,cheight, TFT_WHITE);
				break;
			}
		  }
		#endif
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		return sumX;
		}


		/***************************************************************************************
		** Function name:           drawCentreString (deprecated, use setTextDatum())
		** Descriptions:            draw string centred on dX
		***************************************************************************************/
		T_DispCoords Font::drawCentreString(const String& string, T_DispCoords dX, T_DispCoords poY, uint8_t font)
		{
		  int16_t len = string.length() + 2;
		  char buffer[len];
		  string.toCharArray(buffer, len);
		  return drawCentreString(buffer, dX, poY, font);
		}

		T_DispCoords Font::drawCentreString(const char *string, T_DispCoords dX, T_DispCoords poY, uint8_t font)
		{
		  uint8_t tempdatum = textdatum;
		  int32_t sumX = 0;
		  textdatum = TC_DATUM;
		  sumX = drawString(string, dX, poY, font);
		  textdatum = tempdatum;
		  return sumX;
		}


		/***************************************************************************************
		** Function name:           drawRightString (deprecated, use setTextDatum())
		** Descriptions:            draw string right justified to dX
		***************************************************************************************/
		T_DispCoords Font::drawRightString(const String& string, T_DispCoords dX, T_DispCoords poY, uint8_t font)
		{
		  int16_t len = string.length() + 2;
		  char buffer[len];
		  string.toCharArray(buffer, len);
		  return drawRightString(buffer, dX, poY, font);
		}

		T_DispCoords Font::drawRightString(const char *string, T_DispCoords dX, T_DispCoords poY, uint8_t font)
		{
		  uint8_t tempdatum = textdatum;
		  int16_t sumX = 0;
		  textdatum = TR_DATUM;
		  sumX = drawString(string, dX, poY, font);
		  textdatum = tempdatum;
		  return sumX;
		}


		/***************************************************************************************
		** Function name:           drawNumber
		** Description:             draw a long integer
		***************************************************************************************/
		T_DispCoords Font::drawNumber(long long_num, T_DispCoords poX, T_DispCoords poY)
		{
		  isDigits = true; // Eliminate jiggle in monospaced fonts
		  char str[12];
		  ltoa(long_num, str, 10);
		  return drawString(str, poX, poY, textfont);
		}

		T_DispCoords Font::drawNumber(long long_num, T_DispCoords poX, T_DispCoords poY, uint8_t font)
		{
		  isDigits = true; // Eliminate jiggle in monospaced fonts
		  char str[12];
		  ltoa(long_num, str, 10);
		  return drawString(str, poX, poY, font);
		}


		/***************************************************************************************
		** Function name:           drawFloat
		** Descriptions:            drawFloat, prints 7 non zero digits maximum
		***************************************************************************************/
		// Assemble and print a string, this permits alignment relative to a datum
		// looks complicated but much more compact and actually faster than using print class
		T_DispCoords Font::drawFloat(float floatNumber, uint8_t dp, T_DispCoords poX, T_DispCoords poY)
		{
		  return drawFloat(floatNumber, dp, poX, poY, textfont);
		}

		T_DispCoords Font::drawFloat(float floatNumber, uint8_t dp, T_DispCoords poX, T_DispCoords poY, uint8_t font)
		{
		  isDigits = true;
		  char str[14];               // Array to contain decimal string
		  uint8_t ptr = 0;            // Initialise pointer for array
		  int8_t  digits = 1;         // Count the digits to avoid array overflow
		  float rounding = 0.5;       // Round up down delta

		  if (dp > 7) dp = 7; // Limit the size of decimal portion

		  // Adjust the rounding value
		  for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

		  if (floatNumber < -rounding)    // add sign, avoid adding - sign to 0.0!
		  {
			str[ptr++] = '-'; // Negative number
			str[ptr] = 0; // Put a null in the array as a precaution
			digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
			floatNumber = -floatNumber; // Make positive
		  }

		  floatNumber += rounding; // Round up or down

		  // For error put ... in string and return (all Screen library fonts contain . character)
		  if (floatNumber >= 2147483647) {
			strcpy(str, "...");
			return drawString(str, poX, poY, font);
		  }
		  // No chance of overflow from here on

		  // Get integer part
		  uint32_t temp = (uint32_t)floatNumber;

		  // Put integer part into array
		  ltoa(temp, str + ptr, 10);

		  // Find out where the null is to get the digit count loaded
		  while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
		  digits += ptr;                  // Count the digits

		  str[ptr++] = '.'; // Add decimal point
		  str[ptr] = '0';   // Add a dummy zero
		  str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

		  // Get the decimal portion
		  floatNumber = floatNumber - temp;

		  // Get decimal digits one by one and put in array
		  // Limit digit count so we don't get a false sense of resolution
		  uint8_t i = 0;
		  while ((i < dp) && (digits < 9)) // while (i < dp) for no limit but array size must be increased
		  {
			i++;
			floatNumber *= 10;       // for the next decimal
			temp = floatNumber;      // get the decimal
			ltoa(temp, str + ptr, 10);
			ptr++; digits++;         // Increment pointer and digits count
			floatNumber -= temp;     // Remove that digit
		  }

		  // Finally we can plot the string and return pixel length
		  return drawString(str, poX, poY, font);
		}


		/***************************************************************************************
		** Function name:           setFreeFont
		** Descriptions:            Sets the GFX free font to use
		***************************************************************************************/

		#ifdef LOAD_GFXFF

		void Font::setFreeFont(const GFXfont *f)
		{
		  if (f == nullptr) // Fix issue #400 (ESP32 crash)
		  {
			setTextFont(1); // Use GLCD font
			return;
		  }

		  textfont = 1;
		  gfxFont = (GFXfont *)f;

		  glyph_ab = 0;
		  glyph_bb = 0;
		  uint16_t numChars = pgm_read_word(&gfxFont->last) - pgm_read_word(&gfxFont->first);

		  // Find the biggest above and below baseline offsets
		  for (uint8_t c = 0; c < numChars; c++)
		  {
			GFXglyph *glyph1  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
			int8_t ab = -pgm_read_byte(&glyph1->yOffset);
			if (ab > glyph_ab) glyph_ab = ab;
			int8_t bb = pgm_read_byte(&glyph1->height) - ab;
			if (bb > glyph_bb) glyph_bb = bb;
		  }
		}


		/***************************************************************************************
		** Function name:           setTextFont
		** Description:             Set the font for the print stream
		***************************************************************************************/
		void Font::setTextFont(uint8_t f)
		{
		  textfont = (f > 0) ? f : 1; // Don't allow font 0
		  gfxFont = NULL;
		}

		#else


		/***************************************************************************************
		** Function name:           setFreeFont
		** Descriptions:            Sets the GFX free font to use
		***************************************************************************************/

		// Alternative to setTextFont() so we don't need two different named functions
		void Font::setFreeFont(uint8_t font)
		{
		  setTextFont(font);
		}


		/***************************************************************************************
		** Function name:           setTextFont
		** Description:             Set the font for the print stream
		***************************************************************************************/
		void Font::setTextFont(uint8_t f)
		{
		  textfont = (f > 0) ? f : 1; // Don't allow font 0
		}

		#endif

		  uint32_t Font::_textcolor(){
			  return (unsigned int)textcolor;
		  }
		  uint32_t Font::_textbgcolor(){
			  return (unsigned int)textbgcolor;
		  }
		  uint8_t Font::_textsize(){
			  return (uint8_t)textsize;
		  }
			/***************************************************************************************
			** Function name:           decodeUTF8
			** Description:             Serial UTF-8 decoder with fall-back to extended ASCII
			*************************************************************************************x*/
			#define DECODE_UTF8 // Test only, comment out to stop decoding
			uint16_t Font::decodeUTF8(uint8_t c)
			{
			#ifdef DECODE_UTF8
			  // 7 bit Unicode Code Point
			  if ((c & 0x80) == 0x00) {
				decoderState = 0;
				return (uint16_t)c;
			  }

			  if (decoderState == 0)
			  {
				// 11 bit Unicode Code Point
				if ((c & 0xE0) == 0xC0)
				{
				  decoderBuffer = ((c & 0x1F)<<6);
				  decoderState = 1;
				  return 0;
				}

				// 16 bit Unicode Code Point
				if ((c & 0xF0) == 0xE0)
				{
				  decoderBuffer = ((c & 0x0F)<<12);
				  decoderState = 2;
				  return 0;
				}
				// 21 bit Unicode  Code Point not supported so fall-back to extended ASCII
				if ((c & 0xF8) == 0xF0) return (uint16_t)c;
			  }
			  else
			  {
				if (decoderState == 2)
				{
				  decoderBuffer |= ((c & 0x3F)<<6);
				  decoderState--;
				  return 0;
				}
				else
				{
				  decoderBuffer |= (c & 0x3F);
				  decoderState = 0;
				  return decoderBuffer;
				}
			  }

			  decoderState = 0;
			#endif

			  return (uint16_t)c; // fall-back to extended ASCII
			}


			/***************************************************************************************
			** Function name:           decodeUTF8
			** Description:             Line buffer UTF-8 decoder with fall-back to extended ASCII
			*************************************************************************************x*/
			uint16_t Font::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
			{
			  uint16_t c = buf[(*index)++];
			  //Serial.print("Byte from string = 0x"); Serial.println(c, HEX);

			#ifdef DECODE_UTF8
			  // 7 bit Unicode
			  if ((c & 0x80) == 0x00) return c;

			  // 11 bit Unicode
			  if (((c & 0xE0) == 0xC0) && (remaining > 1))
				return ((c & 0x1F)<<6) | (buf[(*index)++]&0x3F);

			  // 16 bit Unicode
			  if (((c & 0xF0) == 0xE0) && (remaining > 2))
			  {
				c = ((c & 0x0F)<<12) | ((buf[(*index)++]&0x3F)<<6);
				return  c | ((buf[(*index)++]&0x3F));
			  }

			  // 21 bit Unicode not supported so fall-back to extended ASCII
			  // if ((c & 0xF8) == 0xF0) return c;
			#endif

			  return c; // fall-back to extended ASCII
			}


			/***************************************************************************************
			** Function name:           write
			** Description:             draw characters piped through serial stream
			***************************************************************************************/
			size_t Font::write(uint8_t utf8)
			{
			  if (utf8 == '\r') return 1;

			  uint16_t uniCode = utf8;

			  if (_utf8) uniCode = decodeUTF8(utf8);

			  if (uniCode == 0) return 1;

			#ifdef SMOOTH_FONT
			  if(fontLoaded)
			  {
				//Serial.print("UniCode="); Serial.println(uniCode);
				//Serial.print("UTF8   ="); Serial.println(utf8);

				//fontFile = SPIFFS.open( _gFontFilename, "r" );

				//if(!fontFile)
				//{
				//  fontLoaded = false;
				//  return 1;
				//}

//				drawGlyph(uniCode, *cursor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

				//fontFile.close();
				return 1;
			  }
			#endif

			  if (uniCode == '\n') uniCode+=22; // Make it a valid space character to stop errors
			  else if (uniCode < 32) return 1;

			  T_DispCoords width = 0;
			  T_DispCoords height = 0;

			//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
			  //Serial.print((uint8_t) uniCode); // Debug line sends all printed TFT text to serial port
			  //Serial.println(uniCode, HEX); // Debug line sends all printed TFT text to serial port
			  //delay(5);                     // Debug optional wait for serial port to flush through
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			#ifdef LOAD_GFXFF
			  if(!gfxFont) {
			#endif
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			#ifdef LOAD_FONT2
			  if (textfont == 2)
			  {
				if (uniCode > 127) return 1;

				width = pgm_read_byte(widtbl_f16 + uniCode-32);
				height = chr_hgt_f16;
				// Font 2 is rendered in whole byte widths so we must allow for this
				width = (width + 6) / 8;  // Width in whole bytes for font 2, should be + 7 but must allow for font width change
				width = width * 8;        // Width converted back to pixels
			  }
			  #ifdef LOAD_RLE
			  else
			  #endif
			#endif

			#ifdef LOAD_RLE
			  {
				if ((textfont>2) && (textfont<9))
				{
				  if (uniCode > 127) return 1;
				  // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
				  width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[textfont].widthtbl ) ) + uniCode-32 );
				  height= pgm_read_byte( &fontdata[textfont].height );
				}
			  }
			#endif

			#ifdef LOAD_GLCD
			  if (textfont==1)
			  {
				  width =  6;
				  height = 8;
			  }
			#else
			  if (textfont==1) return 1;
			#endif

			  height = height * textsize;


/*

			  if (utf8 == '\n') {
				ImageBuffer->MoveY(height);
				ImageBuffer->X(0);
			  }
			  else
			  {
				if (textwrapX && (ImageBuffer->X() + width * textsize > _width))
				{
					ImageBuffer->MoveY(height);
					ImageBuffer->X(0);
				}
				if (textwrapY && (ImageBuffer->Y() >= (int32_t)_height)) ImageBuffer->Y(0);
				ImageBuffer->MoveX(drawChar(uniCode, ImageBuffer->X(), ImageBuffer->Y(), textfont));
			  }*///ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics


			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			#ifdef LOAD_GFXFF
			  } // Custom GFX font
			  else
			  {
				if(utf8 == '\n') {
//				  ImageBuffer->X(0); //ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
//				  ImageBuffer->MoveY((int16_t)textsize *
//							  (uint8_t)pgm_read_byte(&gfxFont->yAdvance)); //ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				} else {
				  if (uniCode > pgm_read_word(&gfxFont->last )) return 1;
				  if (uniCode < pgm_read_word(&gfxFont->first)) return 1;

				  uint16_t   c2    = uniCode - pgm_read_word(&gfxFont->first);
				  GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
				  uint8_t   w     = pgm_read_byte(&glyph->width),
							h     = pgm_read_byte(&glyph->height);

/*				  if((w > 0) && (h > 0)) { // Is there an associated bitmap?
					int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);


					if(textwrapX && ((ImageBuffer->X() + textsize * (xo + w)) > _width)) {
					  // Drawing character would go off right edge; wrap to new line
					  ImageBuffer->X(0);
					  ImageBuffer->MoveY((int16_t)textsize *
								  (uint8_t)pgm_read_byte(&gfxFont->yAdvance));
					}
					if (textwrapY && (ImageBuffer->Y() >= (int32_t)_height)) ImageBuffer->Y(0);
					drawChar(ImageBuffer->X(), ImageBuffer->Y(), uniCode, textcolor, textbgcolor, textsize);
				  }
				  ImageBuffer->MoveX(pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize);

				  *///ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				}
			  }
			#endif // LOAD_GFXFF
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			  return 1;
			}


			/***************************************************************************************
			** Function name:           drawChar
			** Description:             draw a Unicode glyph onto the screen
			***************************************************************************************/
			  // Any UTF-8 decoding must be done before calling drawChar()
			T_DispCoords Font::drawChar(uint16_t uniCode, T_DispCoords x, T_DispCoords y)
			{
			  return drawChar(uniCode, x, y, textfont);
			}

			  // Any UTF-8 decoding must be done before calling drawChar()
			T_DispCoords Font::drawChar(uint16_t uniCode, T_DispCoords x, T_DispCoords y, uint8_t font)
			{

Serial.println("\ndrawChar.\n");
			  if (!uniCode) return 0;
Serial.println("\n!drawChar.\n");
			  if (font==1)
			  {
Serial.println("\n font.\n");
			#ifdef LOAD_GLCD
Serial.println("\n Load_GLCD.\n");
			  #ifndef LOAD_GFXFF
Serial.println("\n !LOAD_GFXGG.\n");
				drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
				return 6 * textsize;
			  #endif
			#else
			  #ifndef LOAD_GFXFF
				return 0;
			  #endif
			#endif

			#ifdef LOAD_GFXFF
Serial.println("\n LOAD_GFXFF.\n");
				drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
				if(!gfxFont) { // 'Classic' built-in font
Serial.println("\n!gfxFont.\n");
				#ifdef LOAD_GLCD
Serial.println("\n LOAD_GLCD.\n");
				  return 6 * textsize;
				#else
				  return 0;
				#endif
				}
				else
				{
				  if((uniCode >= pgm_read_word(&gfxFont->first)) && (uniCode <= pgm_read_word(&gfxFont->last) ))
				  {
					uint16_t   c2    = uniCode - pgm_read_word(&gfxFont->first);
					GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
					return pgm_read_byte(&glyph->xAdvance) * textsize;
				  }
				  else
				  {
					return 0;
				  }
				}
			#endif
			  }
Serial.println("\n font!=1.\n");

			  if ((font>1) && (font<9) && ((uniCode < 32) || (uniCode > 127))) return 0;
Serial.println("\n font!=1 next.\n");

			  T_DispCoords width  = 0;
			  T_DispCoords height = 0;
			  uint32_t flash_address = 0;
			  uniCode -= 32;

			#ifdef LOAD_FONT2
Serial.println("\n LOAD_FONT2.\n");

			  if (font == 2)
			  {
				flash_address = pgm_read_dword(&chrtbl_f16[uniCode]);
				width = pgm_read_byte(widtbl_f16 + uniCode);
				height = chr_hgt_f16;
			  }
			  #ifdef LOAD_RLE
			  else
			  #endif
			#endif

			#ifdef LOAD_RLE
Serial.println("\n LOAD_RLE.\n");
Serial.printf("font=%i.\n", (uint) font);
Serial.printf("unicode=%i.\n", (uint) uniCode);


			  {
				if ((font>2) && (font<9))
				{
				  flash_address = pgm_read_dword( (const void*)(pgm_read_dword( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *)) );
				  width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[font].widthtbl ) ) + uniCode );
				  height= pgm_read_byte( &fontdata[font].height );
				}
			  }
Serial.printf("\n w=%i h=%i.\n", (uint) width,(uint)height);

			#endif

			  int32_t w = width;
			  int32_t pX      = 0;
			  int32_t pY      = y;
			  uint8_t line = 0;

			#ifdef LOAD_FONT2 // chop out code if we do not need it
			  if (font == 2) {
Serial.println("\n font==2 LOAD_FONT2.\n");

				w = w + 6; // Should be + 7 but we need to compensate for width increment
				w = w / 8;
//				if (x + width * textsize >= (int16_t)_width) return width * textsize ;//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				if (x + width * textsize >= (int16_t)Device::Display::Display->width()) return width * textsize ;//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

				if (textcolor == textbgcolor || textsize != 1) {
				  //spi_begin();          // Sprite class can use this function, avoiding spi_begin()
//				  inTransaction = true;//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

				  for (int32_t i = 0; i < height; i++)
				  {
//					if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
					if (textcolor != textbgcolor) Device::Display::Graphics::Graph->fillRect(x, pY, width * textsize, textsize, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

					for (int32_t k = 0; k < w; k++)
					{
					  line = pgm_read_byte((uint8_t *)flash_address + w * i + k);

					  if (line) {
						if (textsize == 1) {
						  pX = x + k * 8;
/*						  if (line & 0x80) drawPixel(pX, pY, textcolor);
						  if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
						  if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
						  if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
						  if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
						  if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
						  if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
						  if (line & 0x01) drawPixel(pX + 7, pY, textcolor);*/

						  if (line & 0x80) Device::Display::Graphics::Graph->drawPixel(pX, pY, textcolor);
						  if (line & 0x40) Device::Display::Graphics::Graph->drawPixel(pX + 1, pY, textcolor);
						  if (line & 0x20) Device::Display::Graphics::Graph->drawPixel(pX + 2, pY, textcolor);
						  if (line & 0x10) Device::Display::Graphics::Graph->drawPixel(pX + 3, pY, textcolor);
						  if (line & 0x08) Device::Display::Graphics::Graph->drawPixel(pX + 4, pY, textcolor);
						  if (line & 0x04) Device::Display::Graphics::Graph->drawPixel(pX + 5, pY, textcolor);
						  if (line & 0x02) Device::Display::Graphics::Graph->drawPixel(pX + 6, pY, textcolor);
						  if (line & 0x01) Device::Display::Graphics::Graph->drawPixel(pX + 7, pY, textcolor);

						}
						else {
						  pX = x + k * 8 * textsize;
/*						  if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
						  if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
						  if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
						  if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
						  if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
						  if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
						  if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
						  if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);*/
						}
					  }
					  // * ///ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
					}
					pY += textsize;
				  }

//				  inTransaction = false;//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
//				  spi_end();//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				}
				else
				  // Faster drawing of characters and background using block write
				{
//				  spi_begin();//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

//				  setWindow(x, y, x + width - 1, y + height - 1);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

				  uint8_t mask;
				  for (int32_t i = 0; i < height; i++)
				  {
					pX = width;
					for (int32_t k = 0; k < w; k++)
					{
					  line = pgm_read_byte((uint8_t *) (flash_address + w * i + k) );
					  mask = 0x80;
					  while (mask && pX) {
						if (line & mask) {tft_Write_16(textcolor);}
						else {tft_Write_16(textbgcolor);}
						pX--;
						mask = mask >> 1;
					  }
					}
					if (pX) {tft_Write_16(textbgcolor);}
				  }

//				  spi_end();//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				}
			  }

			  #ifdef LOAD_RLE
			  else
			  #endif
			#endif  //FONT2

			#ifdef LOAD_RLE  //674 bytes of code
			  // Font is not 2 and hence is RLE encoded
			  {
Serial.println("\n LOAD_RLE output.\n");

				w *= height; // Now w is total number of pixels in the character
				if ((textsize != 1) || (textcolor == textbgcolor)) {
Serial.println("\n textsize!=1.\n");
//				  if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
				  if (textcolor != textbgcolor) Device::Display::Graphics::Graph->fillRect(x, pY, width * textsize, textsize * height, textbgcolor);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

				  int32_t px = 0, py = pY; // To hold character block start and end column and row values
				  int32_t pc = 0; // Pixel count
				  uint8_t np = textsize * textsize; // Number of pixels in a drawn pixel

				  uint8_t tnp = 0; // Temporary copy of np for while loop
				  uint8_t ts = textsize - 1; // Temporary copy of textsize
				  // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
				  // w is total number of pixels to plot to fill character block
				  while (pc < w)
				  {
					line = pgm_read_byte((uint8_t *)flash_address);
					flash_address++;
					if (line & 0x80) {
					  line &= 0x7F;
					  line++;
					  if (ts) {
						px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
						py = y + textsize * (pc / width);
					  }
					  else {
						px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
						py = y + pc / width;
					  }
					  while (line--) { // In this case the while(line--) is faster
						pc++; // This is faster than putting pc+=line before while()?
//						setWindow(px, py, px + ts, py + ts);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

						if (ts) {
						  tnp = np;
						  while (tnp--) {tft_Write_16(textcolor);}
						}
						else {tft_Write_16(textcolor);}
						px += textsize;

						if (px >= (x + width * textsize))
						{
						  px = x;
						  py += textsize;
						}
					  }
					}
					else {
					  line++;
					  pc += line;
					}
				  }
				}
				else // Text colour != background && textsize = 1
					 // so use faster drawing of characters and background using block write
				{
Serial.println("\n textsize==1.\n");
				  //setWindow(x, y, x + width - 1, y + height - 1);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics

			#ifdef RPI_WRITE_STROBE
Serial.println("\n RPI_WRITE_STROBE.\n");
				  uint8_t textcolorBin[] = { (uint8_t) (textcolor >> 8), (uint8_t) textcolor };
				  uint8_t textbgcolorBin[] = { (uint8_t) (textbgcolor >> 8), (uint8_t) textbgcolor };
			#endif

				  // Maximum font size is equivalent to 180x180 pixels in area
Serial.printf("\n w=%i.\n", (uint) w);

				  while (w > 0)
				  {
Serial.printf("w=%i.\n", (uint) w);

					line = pgm_read_byte((uint8_t *)flash_address++); // 8 bytes smaller when incrementing here
					if (line & 0x80) {
Serial.println("\n Line & 0x80.\n");

					  line &= 0x7F;
					  line++; w -= line;
			#ifdef RPI_WRITE_STROBE
					  spi.writePattern(&textcolorBin[0], 2, 1); line--;
					  while(line--) {WR_L; WR_H;}
			#else
					  #ifdef ESP32_PARALLEL
						while (line--) {tft_Write_16(textcolor);}
					  #else
//						writeBlock(textcolor,line);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
					  Device::Display::Graphics::Graph->drawLine(x,y,x+width-1,y+height-1,textcolor);
					  #endif
			#endif
					}
					else {
Serial.println("\n line!&0x80.\n");

					  line++; w -= line;
			#ifdef RPI_WRITE_STROBE
					  spi.writePattern(&textbgcolorBin[0], 2, 1); line--;
					  while(line--) {WR_L; WR_H;}
			#else
					  #ifdef ESP32_PARALLEL
						while (line--) {tft_Write_16(textbgcolor);}
					  #else
//						writeBlock(textbgcolor,line);//ToDo: modify it to make able return text-imagebuffer for next drawing with Graphics
					  Device::Display::Graphics::Graph->drawLine(x,y,x+width-1,y+height-1,textbgcolor);
					  #endif
			#endif
					}
				  }
				}
			  }
			  // End of RLE font rendering
			#endif
			  return width * textsize;    // x +
			}
			/***************************************************************************************
			** Function name:           getTextDatum
			** Description:             Return the text datum value (as used by setTextDatum())
			***************************************************************************************/
			uint8_t Font::getTextDatum(void)
			{
			  return textdatum;
			}

			/***************************************************************************************
			** Function name:           drawChar
			** Description:             draw a single character in the Adafruit GLCD font
			***************************************************************************************/
			void Font::drawChar(T_DispCoords x, T_DispCoords y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size)
			{
/*			  if ((x >= _width)            || // Clip right
				  (y >= _height)           || // Clip bottom
				  ((x + 6 * size - 1) < 0) || // Clip left
				  ((y + 8 * size - 1) < 0))   // Clip top
				return;*/ //ToDo: modify to make buffer
Serial.println("\n Begin drawChar.\n");
			  if (c < 32) return;
			#ifdef LOAD_GLCD
			//>>>>>>>>>>>>>>>>>>
			#ifdef LOAD_GFXFF
			  if(!gfxFont) { // 'Classic' built-in font
			#endif
			//>>>>>>>>>>>>>>>>>>

			  boolean fillbg = (bg != color);

			  if ((size==1) && fillbg)
			  {
				uint8_t column[6];
				uint8_t mask = 0x1;

//				setWindow(x, y, x+5, y+8);//ToDo: modify to make buffer
				ImageBuffer->init(2,sizeof(uint32_t),5,8);
Serial.println("\n ImageBuffer->init(2,sizeof(uint32_t),5,8);.\n");
				for (int8_t i = 0; i < 5; i++ ) column[i] = pgm_read_byte(font + (c * 5) + i);
				column[5] = 0;

			#if defined (ESP8266) && !defined (ILI9488_DRIVER)
				color = (color >> 8) | (color << 8);
				bg = (bg >> 8) | (bg << 8);

				for (int8_t j = 0; j < 8; j++) {
				  for (int8_t k = 0; k < 5; k++ ) {
					if (column[k] & mask) {
					  SPI1W0 = color;
					}
					else {
					  SPI1W0 = bg;
					}
					SPI1CMD |= SPIBUSY;
					while(SPI1CMD & SPIBUSY) {}
				  }

				  mask <<= 1;

				  SPI1W0 = bg;
				  SPI1CMD |= SPIBUSY;
				  while(SPI1CMD & SPIBUSY) {}
				}
			#else // for ESP32 or ILI9488

				for (int8_t j = 0; j < 8; j++) {
				  for (int8_t k = 0; k < 5; k++ ) {
					if (column[k] & mask) {tft_Write_16(color);}
					else {tft_Write_16(bg);}
				  }
				  mask <<= 1;
				  tft_Write_16(bg);
				}

			#endif

//				spi_end();//ToDo: modify to make buffer
			  }
			  else
			  {
				//spi_begin();          // Sprite class can use this function, avoiding spi_begin()
//				inTransaction = true;//ToDo: modify to make buffer
				for (int8_t i = 0; i < 6; i++ ) {
				  uint8_t line;
				  if (i == 5)
					line = 0x0;
				  else
					line = pgm_read_byte(font + (c * 5) + i);

				  if (size == 1) // default size
				  {
					for (int8_t j = 0; j < 8; j++) {
//					  if (line & 0x1) drawPixel(x + i, y + j, color);
					  line >>= 1;
					}
				  }
				  else {  // big size
					for (int8_t j = 0; j < 8; j++) {
/*					  if (line & 0x1) fillRect(x + (i * size), y + (j * size), size, size, color);
					  else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);*///ToDo: modify to make buffer
					  line >>= 1;
					}
				  }
				}
			  }

			//>>>>>>>>>>>>>>>>>>>>>>>>>>>
			#ifdef LOAD_GFXFF
			  } else { // Custom font
			#endif
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>
			#endif // LOAD_GLCD

			#ifdef LOAD_GFXFF
				// Filter out bad characters not present in font
				if ((c >= pgm_read_word(&gfxFont->first)) && (c <= pgm_read_word(&gfxFont->last )))
				{
				  //spi_begin();          // Sprite class can use this function, avoiding spi_begin()
//				  inTransaction = true; //ToDo: modify to make buffer
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>

				  c -= pgm_read_word(&gfxFont->first);
				  GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
				  uint8_t  *bitmap = (uint8_t *)pgm_read_dword(&gfxFont->bitmap);

				  uint32_t bo = pgm_read_word(&glyph->bitmapOffset);
				  uint8_t  w  = pgm_read_byte(&glyph->width),
						   h  = pgm_read_byte(&glyph->height);
						   //xa = pgm_read_byte(&glyph->xAdvance);
				  int8_t   xo = pgm_read_byte(&glyph->xOffset),
						   yo = pgm_read_byte(&glyph->yOffset);
				  uint8_t  xx, yy, bits=0, bit=0;
				  int16_t  xo16 = 0, yo16 = 0;

				  if(size > 1) {
					xo16 = xo;
					yo16 = yo;
				  }

			// Here we have 3 versions of the same function just for evaluation purposes
			// Comment out the next two #defines to revert to the slower Adafruit implementation

			// If FAST_LINE is defined then the free fonts are rendered using horizontal lines
			// this makes rendering fonts 2-5 times faster. Particularly good for large fonts.
			// This is an elegant solution since it still uses generic functions present in the
			// stock library.

			// If FAST_SHIFT is defined then a slightly faster (at least for AVR processors)
			// shifting bit mask is used

			// Free fonts don't look good when the size multiplier is >1 so we could remove
			// code if this is not wanted and speed things up

			#define FAST_HLINE
			#define FAST_SHIFT
			//FIXED_SIZE is an option in User_Setup.h that only works with FAST_LINE enabled

			#ifdef FIXED_SIZE
				  x+=xo; // Save 88 bytes of FLASH
				  y+=yo;
			#endif

			#ifdef FAST_HLINE

			  #ifdef FAST_SHIFT
				  uint16_t hpc = 0; // Horizontal foreground pixel count
				  for(yy=0; yy<h; yy++) {
					for(xx=0; xx<w; xx++) {
					  if(bit == 0) {
						bits = pgm_read_byte(&bitmap[bo++]);
						bit  = 0x80;
					  }
					  if(bits & bit) hpc++;
					  else {
					   if (hpc) {
			#ifndef FIXED_SIZE
/*						  if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
						  else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);*///ToDo: modify to make buffer
			#else
						  drawFastHLine(x+xx-hpc, y+yy, hpc, color);
			#endif
						  hpc=0;
						}
					  }
					  bit >>= 1;
					}
				  // Draw pixels for this line as we are about to increment yy
					if (hpc) {
			#ifndef FIXED_SIZE
/*					  if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
					  else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);*///ToDo: modify to make buffer
			#else
					  drawFastHLine(x+xx-hpc, y+yy, hpc, color);
			#endif
					  hpc=0;
					}
				  }
			  #else
				  uint16_t hpc = 0; // Horizontal foreground pixel count
				  for(yy=0; yy<h; yy++) {
					for(xx=0; xx<w; xx++) {
					  if(!(bit++ & 7)) {
						bits = pgm_read_byte(&bitmap[bo++]);
					  }
					  if(bits & 0x80) hpc++;
					  else {
						if (hpc) {
						  if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
						  else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
						  hpc=0;
						}
					  }
					  bits <<= 1;
					}
					// Draw pixels for this line as we are about to increment yy
					if (hpc) {
					  if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
					  else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
					  hpc=0;
					}
				  }
			  #endif

			#else
				  for(yy=0; yy<h; yy++) {
					for(xx=0; xx<w; xx++) {
					  if(!(bit++ & 7)) {
						bits = pgm_read_byte(&bitmap[bo++]);
					  }
					  if(bits & 0x80) {
						if(size == 1) {
						  drawPixel(x+xo+xx, y+yo+yy, color);
						} else {
						  fillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, color);
						}
					  }
					  bits <<= 1;
					}
				  }
			#endif
/*				  inTransaction = false;
				  spi_end();*///ToDo: modify to make buffer
				}
			#endif


			#ifdef LOAD_GLCD
			  #ifdef LOAD_GFXFF
			  } // End classic vs custom font
			  #endif
			#endif

			}


			_CoordsType Font::__width(){
				return 2000;
			}
			_CoordsType Font::__height(){
				return 2000;
			}





		}
	}
}
