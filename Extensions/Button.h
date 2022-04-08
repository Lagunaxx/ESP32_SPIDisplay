/***************************************************************************************
// The following button class has been ported over from the Adafruit_GFX library so
// should be compatible.
// A slightly different implementation in this Screen library allows the button
// legends to be in any font
***************************************************************************************/

#ifndef DEVIACE_BUTTON_H
#define DEVIACE_BUTTON_H
#include "Cursor.h"
#include "Graphics.h"
#include "ESP32_SPIDisplay.h"
#include "Types.h"

	namespace Device{
		namespace Input{

			//using namespace Device::Display;
			using Device::Display::Graphics::Graphics;
			using Device::Display::T_DispCoords;
			//class Screen;

			class onScreen_Button {
				// Creates Input device as On-screen button within touch-screens

			 public:
				onScreen_Button(void);
			  // "Classic" initButton() uses center & size
			  void     initButton(Graphics *gfx, T_DispCoords x, T_DispCoords y,
					  T_DispCoords w, T_DispCoords h, uint16_t outline, uint16_t fill,
					  uint16_t textcolor, char *label, uint8_t textsize);

			  // New/alt initButton() uses upper-left corner & size
			  void     initButtonUL(Graphics *gfx, T_DispCoords x1, T_DispCoords y1,
					  T_DispCoords w, T_DispCoords h, uint16_t outline, uint16_t fill,
					  uint16_t textcolor, char *label, uint8_t textsize);
			  void     drawButton(boolean inverted = false);
			  boolean  contains(T_DispCoords x, T_DispCoords y);

			  void     press(boolean p);
			  boolean  isPressed();
			  boolean  justPressed();
			  boolean  justReleased();

			 private:
			  Graphics *_gfx;
			  T_DispCoords  _x1, _y1; // Coordinates of top-left corner
			  T_DispCoords _w, _h;
			  uint8_t  _textsize;
			  uint16_t _outlinecolor, _fillcolor, _textcolor;
			  char     _label[10];

			  boolean  currstate, laststate;
			};
		}
	}
#endif
