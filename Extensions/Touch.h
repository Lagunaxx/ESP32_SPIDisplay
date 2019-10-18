 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the Screen class and is associated with the Touch Screen handlers
#ifndef DEVICE_TOUCH_H
#define DEVICE_TOUCH_H

	#include <Arduino.h>
	#include <SPI.h>
	#include "../ESP32_SPIDisplay.h"

namespace Device{
	namespace Input{

		using Device::Display::T_DispCoords;
		using Device::Display::Screen;

		class Touch: protected Screen{
		 public:

			Touch();
			virtual ~Touch();
				   // Get raw x,y ADC values from touch controller
		  uint8_t  getTouchRaw(T_DispCoords *x, T_DispCoords *y);
				   // Get raw z (i.e. pressure) ADC value from touch controller
		  uint16_t getTouchRawZ(void);
				   // Convert raw x,y values to calibrated and correctly rotated screen coordinates
		  void     convertRawXY(T_DispCoords *x, T_DispCoords *y);
				   // Get the screen touch coordinates, returns true if screen has been touched
				   // if the touch cordinates are off screen then x and y are not updated
		  uint8_t  getTouch(T_DispCoords *x, T_DispCoords *y, uint16_t threshold = 600);

				   // Run screen calibration and test, report calibration values to the serial port
		  void     calibrateTouch(uint16_t *data, uint32_t color_fg, uint32_t color_bg, uint8_t size);
				   // Set the screen calibration values
		  void     setTouch(uint16_t *data);

			virtual T_DispCoords __width();
			virtual T_DispCoords __height();

		 private:
				   // Handlers for the SPI settings and clock speed change
		  inline void spi_begin_touch() __attribute__((always_inline));
		  inline void spi_end_touch()   __attribute__((always_inline));

				   // Private function to validate a touch, allow settle time and reduce spurious coordinates
		  uint8_t  validTouch(T_DispCoords *x, T_DispCoords *y, uint16_t threshold = 600);

				   // Initialise with example calibration values so processor does not crash if setTouch() not called in setup()
		  uint16_t touchCalibration_x0 = 300, touchCalibration_x1 = 3600, touchCalibration_y0 = 300, touchCalibration_y1 = 3600;
		  uint8_t  touchCalibration_rotate = 1, touchCalibration_invert_x = 2, touchCalibration_invert_y = 0;

		  uint32_t _pressTime;        // Press and hold time-out
		  uint16_t _pressX, _pressY;  // For future use (last sampled calibrated coordinates)

		 protected:
		  // int32_t  _width, _height;
		  virtual void drawLine(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, uint32_t color),
				  fillRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint32_t color);
		};

	}
}

#endif

