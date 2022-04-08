// The following touch screen support code by maxpautsch was merged 1/10/17
// https://github.com/maxpautsch

// Define TOUCH_CS is the user setup file to enable this code

// A demo is provided in examples Generic folder

// Additions by Bodmer to double sample, use Z value to improve detection reliability
// and to correct rotation handling

// See license in root directory.

/***************************************************************************************
** Function name:           getTouchRaw
** Description:             read raw touch position.  Always returns true.
***************************************************************************************/
#include "Touch.h"

#include "ESP32_SPIDisplay.h"

namespace Device{
	namespace Input{

		Touch::Touch(){

			// Configure chip select for touchscreen controller if present
			#ifdef TOUCH_CS
			  digitalWrite(TOUCH_CS, HIGH); // Chip select high (inactive)
			  pinMode(TOUCH_CS, OUTPUT);
			#endif

			touchCalibration_x0 = 300;
			  touchCalibration_x1 = 3600;
			  touchCalibration_y0 = 300;
			  touchCalibration_y1 = 3600;
			  touchCalibration_rotate = 1;
			  touchCalibration_invert_x = 2;
			  touchCalibration_invert_y = 0;
			  _pressTime = 200;
			  _pressX=0;
			  _pressY=0;
			  //__width()=0;
			  //__height()=0;

		}

		Touch::~Touch(){

		}

//#if defined (TOUCH_CS) && defined (SPI_TOUCH_FREQUENCY) // && !defined(ESP32_PARALLEL)

	  inline void Touch::spi_begin_touch(void){
	   CS_H; // Just in case it has been left low

	  #if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS)
//		if (locked) {locked = false; spi.beginTransaction(SPISettings(SPI_TOUCH_FREQUENCY, MSBFIRST, SPI_MODE0));}
	  #else
		spi.setFrequency(SPI_TOUCH_FREQUENCY);
	  #endif

	  #ifdef ESP8266
		SPI1U = SPI1U_READ;
	  #endif

	  T_CS_L;
	  }

	  inline void Touch::spi_end_touch(void){
	  T_CS_H;

	  #if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS)
//		if(!inTransaction) {if (!locked) {locked = true; spi.endTransaction();}}
	  #else
		spi.setFrequency(SPI_FREQUENCY);
	  #endif

	  #ifdef ESP8266
		SPI1U = SPI1U_WRITE;
	  #endif
	  }

//#endif


		//uint8_t Screen::getTouchRaw(uint16_t *x, uint16_t *y){
		uint8_t Touch::getTouchRaw(T_DispCoords *x, T_DispCoords *y){
		  uint16_t tmp;

		  spi_begin_touch();

		  // Start YP sample request for x position, read 4 times and keep last sample
		  spi.transfer(0xd0);                    // Start new YP conversion
		  spi.transfer(0);                       // Read first 8 bits
		  spi.transfer(0xd0);                    // Read last 8 bits and start new YP conversion
		  spi.transfer(0);                       // Read first 8 bits
		  spi.transfer(0xd0);                    // Read last 8 bits and start new YP conversion
		  spi.transfer(0);                       // Read first 8 bits
		  spi.transfer(0xd0);                    // Read last 8 bits and start new YP conversion

		  tmp = spi.transfer(0);                   // Read first 8 bits
		  tmp = tmp <<5;
		  tmp |= 0x1f & (spi.transfer(0x90)>>3);   // Read last 8 bits and start new XP conversion

		  *x = tmp;

		  // Start XP sample request for y position, read 4 times and keep last sample
		  spi.transfer(0);                       // Read first 8 bits
		  spi.transfer(0x90);                    // Read last 8 bits and start new XP conversion
		  spi.transfer(0);                       // Read first 8 bits
		  spi.transfer(0x90);                    // Read last 8 bits and start new XP conversion
		  spi.transfer(0);                       // Read first 8 bits
		  spi.transfer(0x90);                    // Read last 8 bits and start new XP conversion

		  tmp = spi.transfer(0);                 // Read first 8 bits
		  tmp = tmp <<5;
		  tmp |= 0x1f & (spi.transfer(0)>>3);    // Read last 8 bits

		  *y = tmp;

		  spi_end_touch();

		  return true;
		}

		/***************************************************************************************
		** Function name:           getTouchRawZ
		** Description:             read raw pressure on touchpad and return Z value.
		***************************************************************************************/
		uint16_t Touch::getTouchRawZ(void){

		  spi_begin_touch();

		  // Z sample request
		  int16_t tz = 0xFFF;
		  spi.transfer(0xb0);               // Start new Z1 conversion
		  tz += spi.transfer16(0xc0) >> 3;  // Read Z1 and start Z2 conversion
		  tz -= spi.transfer16(0x00) >> 3;  // Read Z2

		  spi_end_touch();

		  return (uint16_t)tz;
		}

		/***************************************************************************************
		** Function name:           validTouch
		** Description:             read validated position. Return false if not pressed.
		***************************************************************************************/
		#define _RAWERR 20 // Deadband error allowed in successive position samples
		uint8_t Touch::validTouch(T_DispCoords *x, T_DispCoords *y, uint16_t threshold){
			T_DispCoords x_tmp, y_tmp, x_tmp2, y_tmp2;

		  // Wait until pressure stops increasing to debounce pressure
		  uint16_t z1 = 1;
		  uint16_t z2 = 0;
		  while (z1 > z2)
		  {
			z2 = z1;
			z1 = getTouchRawZ();
			delay(1);
		  }

		  //  Serial.print("Z = ");Serial.println(z1);

		  if (z1 <= threshold) return false;

		  getTouchRaw(&x_tmp,&y_tmp);

		  //  Serial.print("Sample 1 x,y = "); Serial.print(x_tmp);Serial.print(",");Serial.print(y_tmp);
		  //  Serial.print(", Z = ");Serial.println(z1);

		  delay(1); // Small delay to the next sample
		  if (getTouchRawZ() <= threshold) return false;

		  delay(2); // Small delay to the next sample
		  getTouchRaw(&x_tmp2,&y_tmp2);

		  //  Serial.print("Sample 2 x,y = "); Serial.print(x_tmp2);Serial.print(",");Serial.println(y_tmp2);
		  //  Serial.print("Sample difference = ");Serial.print(abs(x_tmp - x_tmp2));Serial.print(",");Serial.println(abs(y_tmp - y_tmp2));

		  if (abs(x_tmp - x_tmp2) > _RAWERR) return false;
		  if (abs(y_tmp - y_tmp2) > _RAWERR) return false;

		  *x = x_tmp;
		  *y = y_tmp;

		  return true;
		}

		/***************************************************************************************
		** Function name:           getTouch
		** Description:             read callibrated position. Return false if not pressed.
		***************************************************************************************/
		#define Z_THRESHOLD 350 // Touch pressure threshold for validating touches
		uint8_t Touch::getTouch(T_DispCoords *x, T_DispCoords *y, uint16_t threshold){
			T_DispCoords x_tmp, y_tmp;

		  if (threshold<20) threshold = 20;
		  if (_pressTime > millis()) threshold=20;

		  uint8_t n = 5;
		  uint8_t valid = 0;
		  while (n--)
		  {
			if (validTouch(&x_tmp, &y_tmp, threshold)) valid++;;
		  }

		  if (valid<1) { _pressTime = 0; return false; }

		  _pressTime = millis() + 50;

		  convertRawXY(&x_tmp, &y_tmp);

		  if (x_tmp >= __width() || y_tmp >= __height()) return false;

		  _pressX = x_tmp;
		  _pressY = y_tmp;
		  *x = _pressX;
		  *y = _pressY;
		  return valid;
		}

		/***************************************************************************************
		** Function name:           convertRawXY
		** Description:             convert raw touch x,y values to screen coordinates
		***************************************************************************************/
		void Touch::convertRawXY(T_DispCoords *x, T_DispCoords *y)
		{
		  uint16_t x_tmp = *x, y_tmp = *y, xx, yy;

		  if(!touchCalibration_rotate){
			xx=(x_tmp-touchCalibration_x0)*__width()/touchCalibration_x1;
			yy=(y_tmp-touchCalibration_y0)*__height()/touchCalibration_y1;
			if(touchCalibration_invert_x)
			  xx = __width() - xx;
			if(touchCalibration_invert_y)
			  yy = __height() - yy;
		  } else {
			xx=(y_tmp-touchCalibration_x0)*__width()/touchCalibration_x1;
			yy=(x_tmp-touchCalibration_y0)*__height()/touchCalibration_y1;
			if(touchCalibration_invert_x)
			  xx = __width() - xx;
			if(touchCalibration_invert_y)
			  yy = __height() - yy;
		  }
		  *x = xx;
		  *y = yy;
		}

		/***************************************************************************************
		** Function name:           calibrateTouch
		** Description:             generates calibration parameters for touchscreen.
		***************************************************************************************/
		void Touch::calibrateTouch(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size){
		  int16_t values[] = {0,0,0,0,0,0,0,0};
		  T_DispCoords x_tmp, y_tmp;



		  for(uint8_t i = 0; i<4; i++){
			fillRect(0, 0, size+1, size+1, color_bg);
			fillRect(0, __height()-size-1, size+1, size+1, color_bg);
			fillRect(__width()-size-1, 0, size+1, size+1, color_bg);
			fillRect(__width()-size-1, __height()-size-1, size+1, size+1, color_bg);

			if (i == 5) break; // used to clear the arrows

			switch (i) {
			  case 0: // up left
				drawLine(0, 0, 0, size, color_fg);
				drawLine(0, 0, size, 0, color_fg);
				drawLine(0, 0, size , size, color_fg);
				break;
			  case 1: // bot left
				drawLine(0, __height()-size-1, 0, __height()-1, color_fg);
				drawLine(0, __height()-1, size, __height()-1, color_fg);
				drawLine(size, __height()-size-1, 0, __height()-1 , color_fg);
				break;
			  case 2: // up right
				drawLine(__width()-size-1, 0, __width()-1, 0, color_fg);
				drawLine(__width()-size-1, size, __width()-1, 0, color_fg);
				drawLine(__width()-1, size, __width()-1, 0, color_fg);
				break;
			  case 3: // bot right
				drawLine(__width()-size-1, __height()-size-1, __width()-1, __height()-1, color_fg);
				drawLine(__width()-1, __height()-1-size, __width()-1, __height()-1, color_fg);
				drawLine(__width()-1-size, __height()-1, __width()-1, __height()-1, color_fg);
				break;
			  }

			// user has to get the chance to release
			if(i>0) delay(1000);

			for(uint8_t j= 0; j<8; j++){
			  // Use a lower detect threshold as corners tend to be less sensitive
			  while(!validTouch(&x_tmp, &y_tmp, Z_THRESHOLD/2));
			  values[i*2  ] += x_tmp;
			  values[i*2+1] += y_tmp;
			  }
			values[i*2  ] /= 8;
			values[i*2+1] /= 8;
		  }


		  // from case 0 to case 1, the y value changed.
		  // If the measured delta of the touch x axis is bigger than the delta of the y axis, the touch and TFT axes are switched.
		  touchCalibration_rotate = false;
		  if(abs(values[0]-values[2]) > abs(values[1]-values[3])){
			touchCalibration_rotate = true;
			touchCalibration_x0 = (values[1] + values[3])/2; // calc min x
			touchCalibration_x1 = (values[5] + values[7])/2; // calc max x
			touchCalibration_y0 = (values[0] + values[4])/2; // calc min y
			touchCalibration_y1 = (values[2] + values[6])/2; // calc max y
		  } else {
			touchCalibration_x0 = (values[0] + values[2])/2; // calc min x
			touchCalibration_x1 = (values[4] + values[6])/2; // calc max x
			touchCalibration_y0 = (values[1] + values[5])/2; // calc min y
			touchCalibration_y1 = (values[3] + values[7])/2; // calc max y
		  }

		  // in addition, the touch screen axis could be in the opposite direction of the TFT axis
		  touchCalibration_invert_x = false;
		  if(touchCalibration_x0 > touchCalibration_x1){
			values[0]=touchCalibration_x0;
			touchCalibration_x0 = touchCalibration_x1;
			touchCalibration_x1 = values[0];
			touchCalibration_invert_x = true;
		  }
		  touchCalibration_invert_y = false;
		  if(touchCalibration_y0 > touchCalibration_y1){
			values[0]=touchCalibration_y0;
			touchCalibration_y0 = touchCalibration_y1;
			touchCalibration_y1 = values[0];
			touchCalibration_invert_y = true;
		  }

		  // pre calculate
		  touchCalibration_x1 -= touchCalibration_x0;
		  touchCalibration_y1 -= touchCalibration_y0;

		  if(touchCalibration_x0 == 0) touchCalibration_x0 = 1;
		  if(touchCalibration_x1 == 0) touchCalibration_x1 = 1;
		  if(touchCalibration_y0 == 0) touchCalibration_y0 = 1;
		  if(touchCalibration_y1 == 0) touchCalibration_y1 = 1;

		  // export parameters, if pointer valid
		  if(parameters != NULL){
			parameters[0] = touchCalibration_x0;
			parameters[1] = touchCalibration_x1;
			parameters[2] = touchCalibration_y0;
			parameters[3] = touchCalibration_y1;
			parameters[4] = touchCalibration_rotate | (touchCalibration_invert_x <<1) | (touchCalibration_invert_y <<2);
		  }
		}


		/***************************************************************************************
		** Function name:           setTouch
		** Description:             imports calibration parameters for touchscreen.
		***************************************************************************************/
		void Touch::setTouch(uint16_t *parameters){
		  touchCalibration_x0 = parameters[0];
		  touchCalibration_x1 = parameters[1];
		  touchCalibration_y0 = parameters[2];
		  touchCalibration_y1 = parameters[3];

		  if(touchCalibration_x0 == 0) touchCalibration_x0 = 1;
		  if(touchCalibration_x1 == 0) touchCalibration_x1 = 1;
		  if(touchCalibration_y0 == 0) touchCalibration_y0 = 1;
		  if(touchCalibration_y1 == 0) touchCalibration_y1 = 1;

		  touchCalibration_rotate = parameters[4] & 0x01;
		  touchCalibration_invert_x = parameters[4] & 0x02;
		  touchCalibration_invert_y = parameters[4] & 0x04;
		}

	}
}
