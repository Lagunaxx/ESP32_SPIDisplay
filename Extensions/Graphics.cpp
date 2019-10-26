/*
 * Graphics.cpp
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: producer
 */

#include "Graphics.h"

namespace Device {
	namespace Display {
		namespace Graphics {

		/***************************************************************************************
		** Function name:           init
		** Description:             initialize Graphics module
		** Usage:
		** 		Device::Display:Graphics::init(color);	// initialize Graph pointer and
		** 												// fills screen with specified color
		**
		** 		Device::Display::Graphics::init();		// initialize Graph pointer and
		** 												// fills screen with black color
		**
		** 	Return value (bool):
		** 		true - Graph initialized;
		** 		false - Graph do not initialized
		***************************************************************************************/
		/*	ToDo: Do not work due memory problem		bool init(uint32_t background_color){
			Graph=new Graphics();
			if (Graph==0) return false;
			Graph->fillScreen(background_color);
			return true;
		};// */
/*		bool init(){
			return init(TFT_BLACK);
		}//*/

		/***************************************************************************************
		** Function name:           remove
		** Description:             removes Graphics module
		** Usage:
		** 		Device::Display::Graphics::remove();	// removes Graph pointer. Do not change screen
		**
		** 	Return value (bool):
		** 		true - Graph removed;
		** 		false - Graph was not initialized (null-pointer)
		***************************************************************************************/
		/*	ToDo: Do not work due memory problem		bool remove(){
			if (Graph==0) return false;
			delete(Graph);
			return true;
		}
// */
			Graphics::Graphics() {
				// TODO Auto-generated constructor stub
				bitmap_fg = TFT_WHITE;
				bitmap_bg = TFT_BLACK;
				_xpivot = 0;
				_ypivot = 0;
			}

			Graphics::~Graphics() {
				// TODO Auto-generated destructor stub
			}


			/***************************************************************************************
			** Function name:           drawCircle
			** Description:             Draw a circle outline
			***************************************************************************************/
			// Optimised midpoint circle algorithm
			void Graphics::drawCircle(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint32_t color)
			{
			  int32_t  x  = 0;
			  int32_t  dx = 1;
			  int32_t  dy = r+r;
			  int32_t  p  = -(r>>1);

			  // These are ordered to minimise coordinate changes in x or y
			  // drawPixel can then send fewer bounding box commands
			  drawPixel(x0 + r, y0, color);
			  drawPixel(x0 - r, y0, color);
			  drawPixel(x0, y0 - r, color);
			  drawPixel(x0, y0 + r, color);

			  while(x<r){

				if(p>=0) {
				  dy-=2;
				  p-=dy;
				  r--;
				}

				dx+=2;
				p+=dx;

				x++;

				// These are ordered to minimise coordinate changes in x or y
				// drawPixel can then send fewer bounding box commands
				drawPixel(x0 + x, y0 + r, color);
				drawPixel(x0 - x, y0 + r, color);
				drawPixel(x0 - x, y0 - r, color);
				drawPixel(x0 + x, y0 - r, color);

				drawPixel(x0 + r, y0 + x, color);
				drawPixel(x0 - r, y0 + x, color);
				drawPixel(x0 - r, y0 - x, color);
				drawPixel(x0 + r, y0 - x, color);
			  }

			}


			/***************************************************************************************
			** Function name:           drawCircleHelper
			** Description:             Support function for circle drawing
			***************************************************************************************/
			void Graphics::drawCircleHelper( T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint8_t cornername, uint32_t color)
			{
			  int32_t f     = 1 - r;
			  int32_t ddF_x = 1;
			  int32_t ddF_y = -2 * r;
			  int32_t x     = 0;

			  while (x < r) {
				if (f >= 0) {
				  r--;
				  ddF_y += 2;
				  f     += ddF_y;
				}
				x++;
				ddF_x += 2;
				f     += ddF_x;
				if (cornername & 0x4) {
				  drawPixel(x0 + x, y0 + r, color);
				  drawPixel(x0 + r, y0 + x, color);
				}
				if (cornername & 0x2) {
				  drawPixel(x0 + x, y0 - r, color);
				  drawPixel(x0 + r, y0 - x, color);
				}
				if (cornername & 0x8) {
				  drawPixel(x0 - r, y0 + x, color);
				  drawPixel(x0 - x, y0 + r, color);
				}
				if (cornername & 0x1) {
				  drawPixel(x0 - r, y0 - x, color);
				  drawPixel(x0 - x, y0 - r, color);
				}
			  }
			}


			/***************************************************************************************
			** Function name:           fillCircle
			** Description:             draw a filled circle
			***************************************************************************************/
			// Optimised midpoint circle algorithm, changed to horizontal lines (faster in sprites)
			void Graphics::fillCircle(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint32_t color)
			{
			  int32_t  x  = 0;
			  int32_t  dx = 1;
			  int32_t  dy = r+r;
			  int32_t  p  = -(r>>1);

			  drawFastHLine(x0 - r, y0, dy+1, color);

			  while(x<r){

				if(p>=0) {
				  dy-=2;
				  p-=dy;
				  r--;
				}

				dx+=2;
				p+=dx;

				x++;

				drawFastHLine(x0 - r, y0 + x, 2 * r+1, color);
				drawFastHLine(x0 - r, y0 - x, 2 * r+1, color);
				drawFastHLine(x0 - x, y0 + r, 2 * x+1, color);
				drawFastHLine(x0 - x, y0 - r, 2 * x+1, color);

			  }

			}


			/***************************************************************************************
			** Function name:           fillCircleHelper
			** Description:             Support function for filled circle drawing
			***************************************************************************************/
			// Used to support drawing roundrects, changed to horizontal lines (faster in sprites)
			void Graphics::fillCircleHelper(T_DispCoords x0, T_DispCoords y0, T_DispCoords r, uint8_t cornername, int32_t delta, uint32_t color)
			{
			  int32_t f     = 1 - r;
			  int32_t ddF_x = 1;
			  int32_t ddF_y = -r - r;
			  int32_t y     = 0;

			  delta++;
			  while (y < r) {
				if (f >= 0) {
				  r--;
				  ddF_y += 2;
				  f     += ddF_y;
				}
				y++;
				//x++;
				ddF_x += 2;
				f     += ddF_x;

				if (cornername & 0x1)
				{
				  drawFastHLine(x0 - r, y0 + y, r + r + delta, color);
				  drawFastHLine(x0 - y, y0 + r, y + y + delta, color);
				}
				if (cornername & 0x2) {
				  drawFastHLine(x0 - r, y0 - y, r + r + delta, color); // 11995, 1090
				  drawFastHLine(x0 - y, y0 - r, y + y + delta, color);
				}
			  }
			}


			/***************************************************************************************
			** Function name:           drawEllipse
			** Description:             Draw a ellipse outline
			***************************************************************************************/
			void Graphics::drawEllipse(T_DispCoords x0, T_DispCoords y0, T_DispCoords rx, T_DispCoords ry, uint16_t color)
			{
			  if (rx<2) return;
			  if (ry<2) return;
			  T_DispCoords x, y;
			  int32_t rx2 = rx * rx;
			  int32_t ry2 = ry * ry;
			  int32_t fx2 = 4 * rx2;
			  int32_t fy2 = 4 * ry2;
			  int32_t s;

			  for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++)
			  {
				// These are ordered to minimise coordinate changes in x or y
				// drawPixel can then send fewer bounding box commands
				drawPixel(x0 + x, y0 + y, color);
				drawPixel(x0 - x, y0 + y, color);
				drawPixel(x0 - x, y0 - y, color);
				drawPixel(x0 + x, y0 - y, color);
				if (s >= 0)
				{
				  s += fx2 * (1 - y);
				  y--;
				}
				s += ry2 * ((4 * x) + 6);
			  }

			  for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++)
			  {
				// These are ordered to minimise coordinate changes in x or y
				// drawPixel can then send fewer bounding box commands
				drawPixel(x0 + x, y0 + y, color);
				drawPixel(x0 - x, y0 + y, color);
				drawPixel(x0 - x, y0 - y, color);
				drawPixel(x0 + x, y0 - y, color);
				if (s >= 0)
				{
				  s += fy2 * (1 - x);
				  x--;
				}
				s += rx2 * ((4 * y) + 6);
			  }

			}


			/***************************************************************************************
			** Function name:           fillEllipse
			** Description:             draw a filled ellipse
			***************************************************************************************/
			void Graphics::fillEllipse(T_DispCoords x0, T_DispCoords y0, T_DispCoords rx, T_DispCoords ry, uint16_t color)
			{
			  if (rx<2) return;
			  if (ry<2) return;
			  T_DispCoords x, y;
			  int32_t rx2 = rx * rx;
			  int32_t ry2 = ry * ry;
			  int32_t fx2 = 4 * rx2;
			  int32_t fy2 = 4 * ry2;
			  int32_t s;

			  for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++)
			  {
				drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
				drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

				if (s >= 0)
				{
				  s += fx2 * (1 - y);
				  y--;
				}
				s += ry2 * ((4 * x) + 6);
			  }

			  for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++)
			  {
				drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
				drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

				if (s >= 0)
				{
				  s += fy2 * (1 - x);
				  x--;
				}
				s += rx2 * ((4 * y) + 6);
			  }

			}


			/***************************************************************************************
			** Function name:           fillScreen
			** Description:             Clear the screen to defined colour
			***************************************************************************************/
			void Graphics::fillScreen(uint32_t color)
			{
			  fillRect(0, 0, width(), height(), color);
			}


			/***************************************************************************************
			** Function name:           drawRect
			** Description:             Draw a rectangle outline
			***************************************************************************************/
			// Draw a rectangle
			void Graphics::drawRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint32_t color)
			{

			  drawFastHLine(x, y, w, color);
			  drawFastHLine(x, y + h - 1, w, color);
			  // Avoid drawing corner pixels twice
			  drawFastVLine(x, y+1, h-2, color);
			  drawFastVLine(x + w - 1, y+1, h-2, color);

			}


			/***************************************************************************************
			** Function name:           drawRoundRect
			** Description:             Draw a rounded corner rectangle outline
			***************************************************************************************/
			// Draw a rounded rectangle
			void Graphics::drawRoundRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, T_DispCoords r, uint32_t color)
			{
			  // smarter version
			  drawFastHLine(x + r  , y    , w - r - r, color); // Top
			  drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
			  drawFastVLine(x    , y + r  , h - r - r, color); // Left
			  drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right
			  // draw four corners
			  drawCircleHelper(x + r    , y + r    , r, 1, color);
			  drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
			  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
			  drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);

			}


			/***************************************************************************************
			** Function name:           fillRoundRect
			** Description:             Draw a rounded corner filled rectangle
			***************************************************************************************/
			// Fill a rounded rectangle, changed to horizontal lines (faster in sprites)
			void Graphics::fillRoundRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, T_DispCoords r, uint32_t color)
			{
			  // smarter version
			  fillRect(x, y + r, w, h - r - r, color);

			  // draw four corners
			  fillCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
			  fillCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color);

			}


			/***************************************************************************************
			** Function name:           drawTriangle
			** Description:             Draw a triangle outline using 3 arbitrary points
			***************************************************************************************/
			// Draw a triangle
			void Graphics::drawTriangle(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, T_DispCoords x2, T_DispCoords y2, uint32_t color)
			{
			  drawLine(x0, y0, x1, y1, color);
			  drawLine(x1, y1, x2, y2, color);
			  drawLine(x2, y2, x0, y0, color);

			}


			/***************************************************************************************
			** Function name:           fillTriangle
			** Description:             Draw a filled triangle using 3 arbitrary points
			***************************************************************************************/
			// Fill a triangle - original Adafruit function works well and code footprint is small
			void Graphics::fillTriangle ( T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, T_DispCoords x2, T_DispCoords y2, uint32_t color)
			{
			  int32_t a, b, y, last;

			  // Sort coordinates by Y order (y2 >= y1 >= y0)
			  if (y0 > y1) {
				swap_coord(y0, y1); swap_coord(x0, x1);
			  }
			  if (y1 > y2) {
				swap_coord(y2, y1); swap_coord(x2, x1);
			  }
			  if (y0 > y1) {
				swap_coord(y0, y1); swap_coord(x0, x1);
			  }

			  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
				a = b = x0;
				if (x1 < a)      a = x1;
				else if (x1 > b) b = x1;
				if (x2 < a)      a = x2;
				else if (x2 > b) b = x2;
				drawFastHLine(a, y0, b - a + 1, color);
				return;
			  }

			  int32_t
			  dx01 = x1 - x0,
			  dy01 = y1 - y0,
			  dx02 = x2 - x0,
			  dy02 = y2 - y0,
			  dx12 = x2 - x1,
			  dy12 = y2 - y1,
			  sa   = 0,
			  sb   = 0;

			  // For upper part of triangle, find scanline crossings for segments
			  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
			  // is included here (and second loop will be skipped, avoiding a /0
			  // error there), otherwise scanline y1 is skipped here and handled
			  // in the second loop...which also avoids a /0 error here if y0=y1
			  // (flat-topped triangle).
			  if (y1 == y2) last = y1;  // Include y1 scanline
			  else         last = y1 - 1; // Skip it

			  for (y = y0; y <= last; y++) {
				a   = x0 + sa / dy01;
				b   = x0 + sb / dy02;
				sa += dx01;
				sb += dx02;

				if (a > b) swap_coord(a, b);
				drawFastHLine(a, y, b - a + 1, color);
			  }

			  // For lower part of triangle, find scanline crossings for segments
			  // 0-2 and 1-2.  This loop is skipped if y1=y2.
			  sa = dx12 * (y - y1);
			  sb = dx02 * (y - y0);
			  for (; y <= y2; y++) {
				a   = x1 + sa / dy12;
				b   = x0 + sb / dy02;
				sa += dx12;
				sb += dx02;

				if (a > b) swap_coord(a, b);
				drawFastHLine(a, y, b - a + 1, color);
			  }

			}


			/***************************************************************************************
			** Function name:           drawBitmap
			** Description:             Draw an image stored in an array on the TFT
			***************************************************************************************/
			void Graphics::drawBitmap(T_DispCoords x, T_DispCoords y, const uint8_t *bitmap, T_DispCoords w, T_DispCoords h, uint16_t color)
			{

			  int32_t i, j, byteWidth = (w + 7) / 8;

			  for (j = 0; j < h; j++) {
				for (i = 0; i < w; i++ ) {
				  if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
					drawPixel(x + i, y + j, color);
				  }
				}
			  }

			}


			/***************************************************************************************
			** Function name:           drawXBitmap
			** Description:             Draw an image stored in an XBM array onto the TFT
			***************************************************************************************/
			void Graphics::drawXBitmap(T_DispCoords x, T_DispCoords y, const uint8_t *bitmap, T_DispCoords w, T_DispCoords h, uint16_t color)
			{

			  int32_t i, j, byteWidth = (w + 7) / 8;

			  for (j = 0; j < h; j++) {
				for (i = 0; i < w; i++ ) {
				  if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7))) {
					drawPixel(x + i, y + j, color);
				  }
				}
			  }

			}


			/***************************************************************************************
			** Function name:           drawXBitmap
			** Description:             Draw an XBM image with foreground and background colors
			***************************************************************************************/
			void Graphics::drawXBitmap(T_DispCoords x, T_DispCoords y, const uint8_t *bitmap, T_DispCoords w, T_DispCoords h, uint16_t color, uint16_t bgcolor)
			{

			  int32_t i, j, byteWidth = (w + 7) / 8;

			  for (j = 0; j < h; j++) {
				for (i = 0; i < w; i++ ) {
				  if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7)))
					   drawPixel(x + i, y + j,   color);
				  else drawPixel(x + i, y + j, bgcolor);
				}
			  }

			}

			/***************************************************************************************
			** Function name:           setBitmapColor
			** Description:             Set the foreground foreground and background colour
			***************************************************************************************/
			void Graphics::setBitmapColor(uint16_t c, uint16_t b)
			{
			  if (c == b) b = ~c;
			  bitmap_fg = c;
			  bitmap_bg = b;
			}

			/***************************************************************************************
			** Function name:           getPivotX
			** Description:             Get the x pivot position
			***************************************************************************************/
			int16_t Graphics::getPivotX(void)
			{
			  return _xpivot;
			}


			/***************************************************************************************
			** Function name:           getPivotY
			** Description:             Get the y pivot position
			***************************************************************************************/
			int16_t Graphics::getPivotY(void)
			{
			  return _ypivot;
			}

			/***************************************************************************************
			** Function name:           drawImageBuffer
			** Description:             Calling Display function to draw buffer data on screen
			***************************************************************************************/
			void Graphics::drawImageBuffer(T_DispCoords x, T_DispCoords y, void * buffer, T_DispCoords w, T_DispCoords h){
				//ToDo: remove (uint32_t). for this need to modify types in Display::Driver
				pushImage((uint32_t)x,(uint32_t)y,(uint32_t)w,(uint32_t)h,(uint16_t *)buffer);
			}

			/***************************************************************************************
			** Function name:           drawImageBufferAlpha
			** Description:             Calling Display function to draw buffer data on screen
			** 							with alpha-channel support
			***************************************************************************************/
			void Graphics::drawImageBufferAlpha(T_DispCoords x, T_DispCoords y, void * buffer, uint8_t* alpha, T_DispCoords w, T_DispCoords h){
				//ToDo: remove (uint32_t). for this need to modify types in Display::Driver

				uint16_t *data= (uint16_t*)malloc(w*h*sizeof(uint16_t));
				upng_s_rgb16b *pixel_screen = new upng_s_rgb16b();
				upng_s_rgb16b *pixel_buffer = new upng_s_rgb16b();
				uint8_t a;
				readRect(x, y, w, h, data);

				for (T_DispCoords xx=0;xx<w;xx++){
					for (T_DispCoords yy=0;yy<h;yy++){
						memcpy(pixel_screen,data+(yy*w+xx)*1,2);
						memcpy(pixel_buffer,(uint16_t *)buffer+(yy*w+xx)*1,2);
						a=*(alpha+yy*w+xx);
						//memcpy(&a,alpha+yy*w+xx,1);
						pixel_screen->r=pixel_screen->r*(255-a)/255 + pixel_buffer->r*a/255;
						pixel_screen->g=pixel_screen->g*(255-a)/255 + pixel_buffer->g*a/255;
						pixel_screen->b=pixel_screen->b*(255-a)/255 + pixel_buffer->b*a/255;
						memcpy(data+(yy*w+xx),pixel_screen,2);
					}
				}

				pushImage((uint32_t)x,(uint32_t)y,(uint32_t)w,(uint32_t)h,(uint16_t *)data);
				free(data);
			}

			/***************************************************************************************
			** Function name:           setPivot
			** Description:             Set the pivot point on the TFT
			*************************************************************************************x*/
			void Graphics::setPivot(T_DispCoords x, T_DispCoords y)
			{
			  _xpivot = x;
			  _ypivot = y;
			}

			  void Graphics::_bitmap_fg(uint32_t c){
				  bitmap_fg=c;
			  }
			  void Graphics::_bitmap_bg(uint32_t c){
				  bitmap_bg=c;
			  }
			  T_DispCoords Graphics::__xpivot(){
				  return _xpivot;
			  }
			  T_DispCoords Graphics::__ypivot(){
				  return _xpivot;
			  }

		} /* namespace Graphics */
	} /* namespace Display */
} /* namespace Device */
