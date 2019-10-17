/***************************************************************************************
// The following button class has been ported over from the Adafruit_GFX library so
// should be compatible.
// A slightly different implementation in this Screen library allows the button
// legends to be in any font
***************************************************************************************/

#ifndef DEVIACE_BUTTON_H
#define DEVIACE_BUTTON_H
#include <ESP32-SPIDisplay.h>
#include <Extensions/Cursor.h>
#include <Extensions/Graphics.h>

	namespace Device{
		namespace Input{

			//using namespace Device::Display;
			using Device::Display::Graphics::Graphics;
			//class Screen;
			using Device::Display::Cursor::_CoordsType;

			class onScreen_Button {
				// Creates Input device as On-screen button within touch-screens

			 public:
				onScreen_Button(void);
			  // "Classic" initButton() uses center & size
			  void     initButton(Graphics *gfx, _CoordsType x, _CoordsType y,
					  _CoordsType w, _CoordsType h, uint16_t outline, uint16_t fill,
					  uint16_t textcolor, char *label, uint8_t textsize);

			  // New/alt initButton() uses upper-left corner & size
			  void     initButtonUL(Graphics *gfx, _CoordsType x1, _CoordsType y1,
					  _CoordsType w, _CoordsType h, uint16_t outline, uint16_t fill,
					  uint16_t textcolor, char *label, uint8_t textsize);
			  void     drawButton(boolean inverted = false);
			  boolean  contains(_CoordsType x, _CoordsType y);

			  void     press(boolean p);
			  boolean  isPressed();
			  boolean  justPressed();
			  boolean  justReleased();

			 private:
			  Graphics *_gfx;
			  _CoordsType  _x1, _y1; // Coordinates of top-left corner
			  _CoordsType _w, _h;
			  uint8_t  _textsize;
			  uint16_t _outlinecolor, _fillcolor, _textcolor;
			  char     _label[10];

			  boolean  currstate, laststate;
			};
		}
	}
#endif
