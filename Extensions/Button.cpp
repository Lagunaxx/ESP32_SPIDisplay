/***************************************************************************************
** Code for the GFX button UI element
** Grabbed from Adafruit_GFX library and enhanced to handle any label font
***************************************************************************************/
#include "Button.h"

namespace Device{
	namespace Input{
		onScreen_Button::onScreen_Button(void) {
		  //_gfx = 0;
		  _gfx=new Graphics();
		  _x1=0;
		  _y1=0; // Coordinates of top-left corner
		  _w=0;
		  _h=0;
		  _textsize=0;
		  _outlinecolor=0;
		  _fillcolor=0;
		  _textcolor=0;
		  _label[0]=0;_label[1]=0;_label[2]=0;_label[3]=0;_label[4]=0;_label[5]=0;_label[6]=0;_label[7]=0;_label[8]=0;_label[9]=0;
		  currstate=false;
		  laststate=false;
		}

		// Classic initButton() function: pass center & size
		void onScreen_Button::initButton(
				Graphics *gfx, T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h,
		 uint16_t outline, uint16_t fill, uint16_t textcolor,
		 char *label, uint8_t textsize)
		{
		  // Tweak arguments and pass to the newer initButtonUL() function...
		  initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill,
			textcolor, label, textsize);
		}

		// Newer function instead accepts upper-left corner & size
		void onScreen_Button::initButtonUL(
				Graphics *gfx, T_DispCoords x1, T_DispCoords y1, T_DispCoords w, T_DispCoords h,
		 uint16_t outline, uint16_t fill, uint16_t textcolor,
		 char *label, uint8_t textsize)
		{
		  _x1           = x1;
		  _y1           = y1;
		  _w            = w;
		  _h            = h;
		  _outlinecolor = outline;
		  _fillcolor    = fill;
		  _textcolor    = textcolor;
		  _textsize     = textsize;
		  _gfx          = gfx;
		  strncpy(_label, label, 9);
		}

		void onScreen_Button::drawButton(boolean inverted) {
		  uint16_t fill, outline, text;

		  if(!inverted) {
			fill    = _fillcolor;
			outline = _outlinecolor;
			text    = _textcolor;
		  } else {
			fill    = _textcolor;
			outline = _outlinecolor;
			text    = _fillcolor;
		  }

		  uint8_t r = min(_w, _h) / 4; // Corner radius
		  _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
		  _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);
/*
		  _gfx->setTextColor(text);
		  _gfx->setTextSize(_textsize);

		  uint8_t tempdatum = _gfx->getTextDatum();
		  _gfx->setTextDatum(MC_DATUM);
		  _gfx->drawString(_label, _x1 + (_w/2), _y1 + (_h/2));
		  _gfx->setTextDatum(tempdatum);*/
		}

		boolean onScreen_Button::contains(T_DispCoords x, T_DispCoords y) {
		  return ((x >= _x1) && (x < (_x1 + _w)) &&
				  (y >= _y1) && (y < (_y1 + _h)));
		}

		void onScreen_Button::press(boolean p) {
		  laststate = currstate;
		  currstate = p;
		}

		boolean onScreen_Button::isPressed()    { return currstate; }
		boolean onScreen_Button::justPressed()  { return (currstate && !laststate); }
		boolean onScreen_Button::justReleased() { return (!currstate && laststate); }

	}
}
