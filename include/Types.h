/*
 * Types.h
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_TYPES_H_
#define COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_TYPES_H_

#include <Arduino.h>



namespace Device{
	namespace Display{

#ifndef DEVICE_DISPLAY_DRIVER_VERSION
	#define DEVICE_DISPLAY_DRIVER_VERSION "Version not defined!"
	#warning "Display driver version not defined! need #define DEVICE_DISPLAY_DRIVER_VERSION \"version\""
#endif

// define how much bits for coordinate will consists in type
	// Need to be be defined in User_Settings/display-type.h depends on TFT_WIDTH and TFT_HEIGHT values.
	// Mostly 8b for screens wtih less then 256 px for both values
#ifdef COORDINATES_8b
	typedef uint8_t T_DispCoords;
#else
	#ifdef COORDINATES_16b
		typedef uint16_t T_DispCoords;
	#else
		#ifdef COORDINATES_32b
			typedef uint32_t T_DispCoords;
		#else
			#ifdef COORDINATES_64b
				typedef uint64_t T_DispCoords;
			#else
				typedef uint64_t T_DispCoords; //by default using 64bit coordinates
				#warning "COORDINATES_Xb Need to be be defined in User_Settings/display-type.h depends on TFT_WIDTH and TFT_HEIGHT values.\
					(X=8,16,32,64). By default used int64_t for T_DispCoords"
			#endif
		#endif
	#endif
#endif


	struct upng_s_rgb16b{
		int b:5;
		int g:6;
		int r:5;
		int null : 16;
	};
	struct upng_s_rgb18b{
		int b:6;
		int g:6;
		int r:6;
		int null:14;
	};
	struct upng_s_rgb24b{
		int r:8;
		int g:8;
		int b:8;
		int null:8;
	};

	struct upng_s_rgba32b{
		upng_s_rgb24b rgb;
		byte alpha;
	};

	struct st_Size2D{
		T_DispCoords width;
		T_DispCoords height;
	};

	struct st_Size3D{
		T_DispCoords width;
		T_DispCoords height;
		T_DispCoords deep;
	};



		// This structure allows sketches to retrieve the user setup parameters at runtime
		// by calling getSetup(), zero impact on code size unless used, mainly for diagnostics
		typedef struct
		{
			String  version = DEVICE_DISPLAY_DRIVER_VERSION;	//	Driver version
			int16_t esp;										//
			uint8_t trans;										//
			uint8_t serial;										//
			uint8_t overlap;									//

			#if defined (ESP32)
				#if defined (USE_HSPI_PORT)
					uint8_t  port = HSPI;							//
				#else
					uint8_t  port = VSPI;							//
				#endif
			#endif

			uint16_t driver; // Hexadecimal code, contains TFT_DRIVER from User_Setup_Select.h
				// Rotation 0 width and height
			uint16_t width;  	// Contains width, given on class declaration (TFT_WIDTH from user's setup (./User_Setups/selected_setup.h)- by default)
			uint16_t height;	// contains height (TFT_HEIGHT - look remark to 'width')

			uint8_t r0_x_offset; // Offsets, not all used yet
			uint8_t r0_y_offset;
			uint8_t r1_x_offset;
			uint8_t r1_y_offset;
			uint8_t r2_x_offset;
			uint8_t r2_y_offset;
			uint8_t r3_x_offset;
			uint8_t r3_y_offset;

			int8_t pin_mosi;
			int8_t pin_miso;
			int8_t pin_clk;
			int8_t pin_cs;

			int8_t pin_dc;
			int8_t pin_rd;
			int8_t pin_wr;
			int8_t pin_rst;

			int8_t pin_d0;
			int8_t pin_d1;
			int8_t pin_d2;
			int8_t pin_d3;
			int8_t pin_d4;
			int8_t pin_d5;
			int8_t pin_d6;
			int8_t pin_d7;

			int8_t pin_tch_cs;

			int16_t spi_freq;
			int16_t rd_freq;
			int16_t tch_spi_freq;
		} T_DisplaySettings;


	}
}



#endif /* COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_TYPES_H_ */
