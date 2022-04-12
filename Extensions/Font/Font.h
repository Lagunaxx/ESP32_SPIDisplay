 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the Screen class and is associated with anti-aliased font functions



#ifndef DEVICE_DISPLAY_GRAPHICS_FONT_H
#define DEVICE_DISPLAY_GRAPHICS_FONT_H

	#ifndef ESP32
		#define ESP32 //Just used to test ESP32 options
	#endif

	#include <Arduino.h>
	#include <SPI.h>
	#include <Print.h>

	// Call up the SPIFFS FLASH filing system for the anti-aliased fonts
	#include <FS.h>
	#ifdef ESP32
		#include <SPIFFS.h>
	#endif

	#include "ESP32_SPIDisplay.h"
	#include "Cursor.h"
	#include "cBuffer.h"
	#include "Graphics.h"
	#include "Types.h"
	//#include <Extensions/Sprite.h>

// ToDo: move color convertion (888 to 565, 888 to 555, etc) to external extension
#include "upng.h"

	#include "FontTypes.h"
	#include "GFXFF/gfxfont.h"



	namespace Device{
		namespace Display{
			namespace Graphics{

			class c_TextBuffer: public Device::Display::Cursor::c_Cursor{
			public:
				c_TextBuffer();
				c_TextBuffer(uint16_t length);
				~c_TextBuffer();

				t_DispCoords GetX(),
						GetY();
				void SetX(t_DispCoords coord),
						SetY(t_DispCoords coord);

			private:
				t_DispCoords x, y;
				Device::Memory::c_Buffer *buffer;

			};




				using namespace Cursor;

				// Use GLCD font in error case where user requests a smooth font file
				// that does not exist (this is a temporary fix to stop ESP32 reboot)
				#ifdef SMOOTH_FONT
				  #ifndef LOAD_GLCD
					#define LOAD_GLCD
				  #endif
				#endif

				// Only load the fonts defined in User_Setup.h (to save space)
				// Set flag so RLE rendering code is optionally compiled
				#ifdef LOAD_GLCD
				  #include "Fonts/glcdfont.h"
				#endif

				#ifdef LOAD_FONT2
				  #include <Fonts/Font16.h>
				#endif

				#ifdef LOAD_FONT4
				  #include <Fonts/Font32rle.h>
				  #define LOAD_RLE
				#endif

				#ifdef LOAD_FONT6
				  #include <Fonts/Font64rle.h>
				  #ifndef LOAD_RLE
					#define LOAD_RLE
				  #endif
				#endif

				#ifdef LOAD_FONT7
				  #include <Fonts/Font7srle.h>
				  #ifndef LOAD_RLE
					#define LOAD_RLE
				  #endif
				#endif

				#ifdef LOAD_FONT8
				  #include <Fonts/Font72rle.h>
				  #ifndef LOAD_RLE
					#define LOAD_RLE
				  #endif
				#elif defined LOAD_FONT8N
				  #define LOAD_FONT8
				  #include <Fonts/Font72x53rle.h>
				  #ifndef LOAD_RLE
					#define LOAD_RLE
				  #endif
				#endif

				#ifdef LOAD_GFXFF
				  // We can include all the free fonts and they will only be built into
				  // the sketch if they are used

				  #include <Fonts/GFXFF/gfxfont.h>

				  // Call up any user custom fonts
				  #include <User_Setups/User_Custom_Fonts.h>

				  // Original Adafruit_GFX "Free Fonts"
				  #include <Fonts/GFXFF/TomThumb.h>  // TT1

				  #include <Fonts/GFXFF/FreeMono9pt7b.h>  // FF1 or FM9
				  #include <Fonts/GFXFF/FreeMono12pt7b.h> // FF2 or FM12
				  #include <Fonts/GFXFF/FreeMono18pt7b.h> // FF3 or FM18
				  #include <Fonts/GFXFF/FreeMono24pt7b.h> // FF4 or FM24

				  #include <Fonts/GFXFF/FreeMonoOblique9pt7b.h>  // FF5 or FMO9
				  #include <Fonts/GFXFF/FreeMonoOblique12pt7b.h> // FF6 or FMO12
				  #include <Fonts/GFXFF/FreeMonoOblique18pt7b.h> // FF7 or FMO18
				  #include <Fonts/GFXFF/FreeMonoOblique24pt7b.h> // FF8 or FMO24

				  #include <Fonts/GFXFF/FreeMonoBold9pt7b.h>  // FF9  or FMB9
				  #include <Fonts/GFXFF/FreeMonoBold12pt7b.h> // FF10 or FMB12
				  #include <Fonts/GFXFF/FreeMonoBold18pt7b.h> // FF11 or FMB18
				  #include <Fonts/GFXFF/FreeMonoBold24pt7b.h> // FF12 or FMB24

				  #include <Fonts/GFXFF/FreeMonoBoldOblique9pt7b.h>  // FF13 or FMBO9
				  #include <Fonts/GFXFF/FreeMonoBoldOblique12pt7b.h> // FF14 or FMBO12
				  #include <Fonts/GFXFF/FreeMonoBoldOblique18pt7b.h> // FF15 or FMBO18
				  #include <Fonts/GFXFF/FreeMonoBoldOblique24pt7b.h> // FF16 or FMBO24

				  // Sans serif fonts
				  #include <Fonts/GFXFF/FreeSans9pt7b.h>  // FF17 or FSS9
				  #include <Fonts/GFXFF/FreeSans12pt7b.h> // FF18 or FSS12
				  #include <Fonts/GFXFF/FreeSans18pt7b.h> // FF19 or FSS18
				  #include <Fonts/GFXFF/FreeSans24pt7b.h> // FF20 or FSS24

				  #include <Fonts/GFXFF/FreeSansOblique9pt7b.h>  // FF21 or FSSO9
				  #include <Fonts/GFXFF/FreeSansOblique12pt7b.h> // FF22 or FSSO12
				  #include <Fonts/GFXFF/FreeSansOblique18pt7b.h> // FF23 or FSSO18
				  #include <Fonts/GFXFF/FreeSansOblique24pt7b.h> // FF24 or FSSO24

				  #include <Fonts/GFXFF/FreeSansBold9pt7b.h>  // FF25 or FSSB9
				  #include <Fonts/GFXFF/FreeSansBold12pt7b.h> // FF26 or FSSB12
				  #include <Fonts/GFXFF/FreeSansBold18pt7b.h> // FF27 or FSSB18
				  #include <Fonts/GFXFF/FreeSansBold24pt7b.h> // FF28 or FSSB24

				  #include <Fonts/GFXFF/FreeSansBoldOblique9pt7b.h>  // FF29 or FSSBO9
				  #include <Fonts/GFXFF/FreeSansBoldOblique12pt7b.h> // FF30 or FSSBO12
				  #include <Fonts/GFXFF/FreeSansBoldOblique18pt7b.h> // FF31 or FSSBO18
				  #include <Fonts/GFXFF/FreeSansBoldOblique24pt7b.h> // FF32 or FSSBO24

				  // Serif fonts
				  #include <Fonts/GFXFF/FreeSerif9pt7b.h>  // FF33 or FS9
				  #include <Fonts/GFXFF/FreeSerif12pt7b.h> // FF34 or FS12
				  #include <Fonts/GFXFF/FreeSerif18pt7b.h> // FF35 or FS18
				  #include <Fonts/GFXFF/FreeSerif24pt7b.h> // FF36 or FS24

				  #include <Fonts/GFXFF/FreeSerifItalic9pt7b.h>  // FF37 or FSI9
				  #include <Fonts/GFXFF/FreeSerifItalic12pt7b.h> // FF38 or FSI12
				  #include <Fonts/GFXFF/FreeSerifItalic18pt7b.h> // FF39 or FSI18
				  #include <Fonts/GFXFF/FreeSerifItalic24pt7b.h> // FF40 or FSI24

				  #include <Fonts/GFXFF/FreeSerifBold9pt7b.h>  // FF41 or FSB9
				  #include <Fonts/GFXFF/FreeSerifBold12pt7b.h> // FF42 or FSB12
				  #include <Fonts/GFXFF/FreeSerifBold18pt7b.h> // FF43 or FSB18
				  #include <Fonts/GFXFF/FreeSerifBold24pt7b.h> // FF44 or FSB24

				  #include <Fonts/GFXFF/FreeSerifBoldItalic9pt7b.h>  // FF45 or FSBI9
				  #include <Fonts/GFXFF/FreeSerifBoldItalic12pt7b.h> // FF46 or FSBI12
				  #include <Fonts/GFXFF/FreeSerifBoldItalic18pt7b.h> // FF47 or FSBI18
				  #include <Fonts/GFXFF/FreeSerifBoldItalic24pt7b.h> // FF48 or FSBI24

				#endif // #ifdef LOAD_GFXFF



				// This is a structure to conveniently hold information on the default fonts
				// Stores pointer to font character image address table, width table and height

				// Create a null set in case some fonts not used (to prevent crash)
				const  uint8_t widtbl_null[1] = {0};
				PROGMEM const uint8_t chr_null[1] = {0};
				PROGMEM const uint8_t* const chrtbl_null[1] = {chr_null};


				// Now fill the structure
				const PROGMEM fontinfo fontdata [] = {
				  #ifdef LOAD_GLCD
				   { (const uint8_t *)font, widtbl_null, 0, 0 },
				  #else
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
				  #endif
				   // GLCD font (Font 1) does not have all parameters
				   { (const uint8_t *)chrtbl_null, widtbl_null, 8, 7 },

				  #ifdef LOAD_FONT2
				   { (const uint8_t *)chrtbl_f16, widtbl_f16, chr_hgt_f16, baseline_f16},
				  #else
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
				  #endif

				   // Font 3 current unused
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

				  #ifdef LOAD_FONT4
				   { (const uint8_t *)chrtbl_f32, widtbl_f32, chr_hgt_f32, baseline_f32},
				  #else
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
				  #endif

				   // Font 5 current unused
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

				  #ifdef LOAD_FONT6
				   { (const uint8_t *)chrtbl_f64, widtbl_f64, chr_hgt_f64, baseline_f64},
				  #else
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
				  #endif

				  #ifdef LOAD_FONT7
				   { (const uint8_t *)chrtbl_f7s, widtbl_f7s, chr_hgt_f7s, baseline_f7s},
				  #else
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
				  #endif

				  #ifdef LOAD_FONT8
				   { (const uint8_t *)chrtbl_f72, widtbl_f72, chr_hgt_f72, baseline_f72}
				  #else
				   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 }
				  #endif
				};


				class Font: public Screen {
				 public:
					Font();
					virtual ~Font();

					uint16_t fontsLoaded(void);

					//virtual c_Cursor* Cursor();
					//virtual void Cursor(c_Cursor& _cursor);
					//virtual t_DispCoords __width();
					//virtual t_DispCoords __height();


					virtual t_DispCoords	drawChar(uint16_t uniCode, t_DispCoords x, t_DispCoords y, uint8_t font),
											drawChar(uint16_t uniCode, t_DispCoords x, t_DispCoords y);

					void	drawChar(t_DispCoords x, t_DispCoords y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size),
							setTextColor(uint16_t color),
							setTextColor(uint16_t fgcolor, uint16_t bgcolor),
							setTextSize(uint8_t size),

							setTextWrap(boolean wrapX, boolean wrapY = false),
							setTextDatum(uint8_t datum),
							setTextPadding(t_DispCoords x_width),

							setSymbolSpace(t_DispCoords size),

					#ifdef LOAD_GFXFF
							setFreeFont(const GFXfont *f = NULL),
							setTextFont(uint8_t font),
							fillBackground(bool fill);
					#else
							setFreeFont(uint8_t font),
							setTextFont(uint8_t font);
					#endif

					t_DispCoords	textWidth(const char *string, uint8_t font),
								textWidth(const char *string),
								textWidth(const String& string, uint8_t font),
								textWidth(const String& string),
								fontHeight(int16_t font),
								fontHeight(void);
					uint16_t	decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
					uint16_t	decodeUTF8(uint8_t c);
					size_t		write(uint8_t);
					// Set or get an arbitrary library attribute or configuration option
					void		setAttribute(uint8_t id = 0, uint8_t a = 0);
					uint8_t		getAttribute(uint8_t id = 0);
					uint32_t	_textcolor(), _textbgcolor(); //ToDo: text mast be with transparent background
					uint8_t		_textsize();

					t_DispCoords	drawNumber(long long_num, t_DispCoords poX, t_DispCoords poY, uint8_t font),
									drawNumber(long long_num, t_DispCoords poX, t_DispCoords poY),
									drawFloat(float floatNumber, uint8_t decimal, t_DispCoords poX, t_DispCoords poY, uint8_t font),
									drawFloat(float floatNumber, uint8_t decimal, t_DispCoords poX, t_DispCoords poY),

									// Handle char arrays
									drawString(const char *string, t_DispCoords poX, t_DispCoords poY, uint8_t font),
									drawString(const char *string, t_DispCoords poX, t_DispCoords poY),
									drawCentreString(const char *string, t_DispCoords dX, t_DispCoords poY, uint8_t font), // Deprecated, use setTextDatum() and drawString()
									drawRightString(const char *string, t_DispCoords dX, t_DispCoords poY, uint8_t font),  // Deprecated, use setTextDatum() and drawString()

									// Handle String type
									drawString(const String& string, t_DispCoords poX, t_DispCoords poY, uint8_t font),
									drawString(const String& string, t_DispCoords poX, t_DispCoords poY),
									drawCentreString(const String& string, t_DispCoords dX, t_DispCoords poY, uint8_t font), // Deprecated, use setTextDatum() and drawString()
									drawRightString(const String& string, t_DispCoords dX, t_DispCoords poY, uint8_t font);  // Deprecated, use setTextDatum() and drawString()
					uint8_t	getTextDatum(void);

					// These are for the new antialiased fonts
					void     loadFont(String fontName, fs::FS &ffs);
					void     loadFont(String fontName, bool flash = true);
					void     unloadFont( void );
					bool     getUnicodeIndex(uint16_t unicode, uint16_t *index);
#ifdef FONT_ALPHABLEND
					uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);
#endif
					virtual uint8_t drawGlyph(uint16_t code, t_DispCoords x, t_DispCoords y);//, c_Cursor &cursor);

					void     showFont(uint32_t td, Device::Display::Screen &display);


				 protected:
//					uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
//					uint16_t decodeUTF8(uint8_t c);

//				 private:

					fontMetrics gFont;

					// These are for the metrics for each individual glyph (so we don't need to seek this in file and waste time)
					uint16_t* gUnicode;  //UTF-16 code, the codes are searched so do not need to be sequential
					uint8_t*  gHeight;   //cheight
					uint8_t*  gWidth;    //cwidth
					uint8_t*  gxAdvance; //setWidth
					int16_t*  gdY;       //topExtent
					int8_t*   gdX;       //leftExtent
					uint32_t* gBitmap;   //file pointer to greyscale bitmap

					bool     fontLoaded; // Flags when a anti-aliased font is loaded
					fs::File *fontFile;

					//_CoordsType  _width, _height;           // Display w/h as modified by current rotation
					bool     textwrapX, textwrapY;   // If set, 'wrap' text at right and optionally bottom edge of display

					uint32_t	textcolor, textbgcolor;


				  	uint8_t	textfont,  // Current selected font
							textsize,  // Current font size multiplier
							textdatum; // Text reference datum


					void     loadMetrics(uint16_t gCount);
					uint32_t readInt32(void);

					fs::FS   &fontFS = SPIFFS;
					bool     spiffs = true;

					bool bgfill = false; // fill background with bgcolor while printing

				#ifdef LOAD_GFXFF
					GFXfont  *gfxFont;
				#endif
					uint8_t		decoderState = 0;   // UTF8 decoder state
					uint16_t	decoderBuffer;      // Unicode code-point buffer
					bool		_cp437;     // If set, use correct CP437 charset (default is ON)
					bool		_utf8;      // If set, use UTF-8 decoder in print stream 'write()' function (default ON)
					t_DispCoords  padX;		// Padding (from top, from bottom, from left, from tight)
					bool		isDigits;   // adjust bounding box for numbers to reduce visual jiggling
					uint32_t	fontsloaded;
					uint8_t		glyph_ab,   // glyph delta Y (height) above baseline
								glyph_bb;   // glyph delta Y (height) below baseline
					t_DispCoords	symbolSpace; // space between symbols in line

//					c_TextBuffer *ImageBuffer;
					//Device::Memory::c_Buffer *ImageBuffer;
					void* buf_Bitmap;


				};
			}
		}
	}

#endif /* DEVICE_DISPLAY_GRAPHICS_FONT_H */
