/***************************************************************************************
// The following class creates Sprites in RAM, graphics can then be drawn in the Sprite
// and rendered quickly onto the TFT screen. The class inherits the graphics functions
// from the Screen class. Some functions are overridden by this class so that the
// graphics are written to the Sprite rather than the TFT.
***************************************************************************************/
#ifndef ESPRITE_CLASS_DEFINATION
#define ESPRITE_CLASS_DEFINATION

#include <Extensions/Graphics.h>
#include <Extensions/Cursor.h>
#include <Extensions/Font/Font.h>

namespace Device{
	namespace Display{
		namespace Graphics{

		using namespace Device::Display::Cursor;
		//using namespace Device::Display::Graphics;

			class TFT_eSprite {

			 public:

			  TFT_eSprite(Device::Display::Graphics::Graphics *tft);

					   // Create a sprite of width x height pixels, return a pointer to the RAM area
					   // Sketch can cast returned value to (uint16_t*) for 16 bit depth if needed
					   // RAM required is 1 byte per pixel for 8 bit colour depth, 2 bytes for 16 bit
			  void*    createSprite(T_DispCoords width, T_DispCoords height, uint8_t frames = 1);

					   // Delete the sprite to free up the RAM
			  void     deleteSprite(void);

					   // Select the frame buffer for graphics
			  void*    frameBuffer(int8_t f);

					   // Set or get the colour depth to 8 or 16 bits. Can be used to change depth an existing
					   // sprite, but clears it to black, returns a new pointer if sprite is re-created.
			  void*    setColorDepth(int8_t b);
			  int8_t   getColorDepth(void);

			  void     setBitmapColor(uint16_t c, uint16_t b);

			  void     drawPixel(T_DispCoords x, T_DispCoords y, uint32_t color);

			  void     drawChar(T_DispCoords x, T_DispCoords y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size),

					   fillSprite(uint32_t color),

					   // Define a window to push 16 bit colour pixels into in a raster order
					   // Colours are converted to 8 bit if depth is set to 8
					   setWindow(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1),
					   pushColor(uint32_t color),
					   pushColor(uint32_t color, uint16_t len),
					   // Push a pixel preformatted as a 8 or 16 bit colour (avoids conversion overhead)
					   writeColor(uint16_t color),

					   // Set the scroll zone, top left corner at x,y with defined width and height
					   // The colour (optional, black is default) is used to fill the gap after the scroll
					   setScrollRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint16_t color = TFT_BLACK),
					   // Scroll the defined zone dx,dy pixels. Negative values left,up, positive right,down
					   // dy is optional (default is then no up/down scroll).
					   // The sprite coordinate frame does not move because pixels are moved
					   scroll(T_DispCoords dx, T_DispCoords dy = 0),

					   drawLine(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, uint32_t color),
					   drawFastVLine(T_DispCoords x, T_DispCoords y, T_DispCoords h, uint32_t color),
					   drawFastHLine(T_DispCoords x, T_DispCoords y, T_DispCoords w, uint32_t color),

					   fillRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint32_t color);

					   // Set the sprite text cursor position for print class (does not change the TFT screen cursor)
					   //setCursor(int16_t x, int16_t y);

					   // Set the rotation of the Sprite (for 1bpp Sprites only)
			  void     setRotation(uint8_t rotation);
			  uint8_t  getRotation(void);

					   // Push a rotated copy of Sprite to TFT with optional transparent colour
			  bool     pushRotated(int16_t angle, int32_t transp = -1);
					   // Push a rotated copy of Sprite to another different Sprite with optional transparent colour
			  bool     pushRotated(TFT_eSprite *spr, int16_t angle, int32_t transp = -1);
					   // Set and get the pivot point for this Sprite
			  void     setPivot(T_DispCoords x, T_DispCoords y);
			  T_DispCoords  getPivotX(void),
					   getPivotY(void);

					   // Get the bounding box for a rotated copy of this Sprite
			  void     getRotatedBounds(float sina, float cosa, T_DispCoords w, T_DispCoords h, T_DispCoords xp, T_DispCoords yp,
					  T_DispCoords *min_x, T_DispCoords *min_y, T_DispCoords *max_x, T_DispCoords *max_y);

					   // Read the colour of a pixel at x,y and return value in 565 format
			  uint16_t readPixel(T_DispCoords x0, T_DispCoords y0);

					   // Write an image (colour bitmap) to the sprite
			  void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint16_t *data);
			  void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, const uint16_t *data);

					   // Swap the byte order for pushImage() - corrects different image endianness
			  void     setSwapBytes(bool swap);
			  bool     getSwapBytes(void);

					   // Push the sprite to the TFT screen, this fn calls pushImage() in the TFT class.
					   // Optionally a "transparent" colour can be defined, pixels of that colour will not be rendered
			  void     pushSprite(T_DispCoords x, T_DispCoords y);
			  void     pushSprite(T_DispCoords x, T_DispCoords y, uint16_t transparent);

			  T_DispCoords  drawChar(uint16_t uniCode, T_DispCoords x, T_DispCoords y, uint8_t font),
					   drawChar(uint16_t uniCode, T_DispCoords x, T_DispCoords y);

					   // Return the width and height of the sprite
			  T_DispCoords  width(void),
						   height(void);

					   // Used by print class to print text to cursor position
			  size_t   write(uint8_t);

					   // Functions associated with anti-aliased fonts
			  void     drawGlyph(uint16_t code);
			  void     printToSprite(String string);
			  void     printToSprite(char *cbuffer, uint16_t len);
			  int16_t  printToSprite(T_DispCoords x, T_DispCoords y, uint16_t index);

			 private:

			//  Device::Display::Graphics::Graphics *_tft;

					   // Reserve memory for the Sprite and return a pointer
			  void*    callocSprite(T_DispCoords width, T_DispCoords height, uint8_t frames = 1);

			 protected:

			  uint8_t  _bpp;     // bits per pixel (1, 8 or 16)
			  uint16_t *_img;    // pointer to 16 bit sprite
			  uint8_t  *_img8;   // pointer to  8 bit sprite
			  uint8_t  *_img8_1; // pointer to  frame 1
			  uint8_t  *_img8_2; // pointer to  frame 2

			  T_DispCoords _xpivot;   // x pivot point coordinate
			  T_DispCoords _ypivot;   // y pivot point coordinate

			  bool     _created;    // A Sprite has been created and memory reserved
			  bool     _gFont = false;

			//  T_DispCoords  _icursor_x, _icursor_y;
			  uint8_t  _rotation = 0;
			  T_DispCoords  _xs, _ys, _xe, _ye, _xptr, _yptr; // for setWindow
			  T_DispCoords  _sx, _sy; // x,y for scroll zone
			  T_DispCoords _sw, _sh; // w,h for scroll zone
			  uint32_t _scolor;  // gap fill colour for scroll zone

			  boolean  _iswapBytes; // Swap the byte order for Sprite pushImage()

			  T_DispCoords  _iwidth, _iheight; // Sprite memory image bit width and height (swapped during rotations)
			  T_DispCoords  _dwidth, _dheight; // Real display width and height (for <8bpp Sprites)
			  int32_t  _bitwidth;         // Sprite image bit width for drawPixel (for <8bpp Sprites, not swapped)
//			  Device::Display::Cursor::c_Cursor* cursor;
			};
		}
	}
}

#endif
