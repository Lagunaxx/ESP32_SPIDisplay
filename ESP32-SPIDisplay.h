/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

  Current library reworked for better scale-ability.

  Driver offers main display functionality:
  - stores configuration with display characteristics
  - provides image-buffer for input/output
  - provides main output to display.
 ****************************************************/

// Stop fonts etc being loaded multiple times
#ifndef DEVICE_DISPLAY_H_
#define DEVICE_DISPLAY_H_

	#define DEVICE_DISPLAY_DRIVER_VERSION "2.0.0"

#ifndef ESP32
	#define ESP32 //Just used to test ESP32 options
#endif

	// Include header file that defines the fonts loaded, the TFT drivers
	// available and the pins to be used
	#include <Arduino.h>
	#include <SPI.h>
	#include <User_Setup_Select.h>
	#include <pgmspace.h>


	/*********************************************************
	 * Default initialization                                *
	 *********************************************************/
	#ifdef ESP32
		#include <soc/spi_reg.h>
		#ifdef USE_HSPI_PORT
			#define SPI_PORT HSPI
		#else
			#define SPI_PORT VSPI
		#endif
	#endif

	#include "Definations.h"
	#include "Types.h"

	/*********************************************************
	 * Adding Extensions                                     *
	 *********************************************************/

	#ifdef DEVICE_INPUT_TOUCH
		//Load the Touch extension
		#include "Extensions/Touch.h"
	#endif
	#ifdef DEVICE_INPUT_ONSCREENBUTTON
		#include <Extensions/Button.h>
	#endif
	#ifdef SMOOTH_FONT
//	  #include <Extensions/Font.h>
	#endif
	#ifdef DEVICE_DISPLAY_GRAPHICS_SPRITE
		#include "Extensions/Sprite.h"
	#endif



#ifndef COMPONENTS_ARDUINO_LIBRARIES_ESP32_SPIDISPLAY_DEFAULTS_H_
	#error "\"Definations.h\" do not loaded!"
#endif
#ifndef COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_TYPES_H_
	#error "\"Types.h\" do not loaded!"
#endif

	/*********************************************************
	 * Starting Display driver declaration                   *
	 *********************************************************/

	namespace Device{
		namespace Display{



			// Class functions and variables
			class Screen{
			public:
				Screen(T_DispCoords _W = TFT_WIDTH, T_DispCoords _H = TFT_HEIGHT);
				virtual ~Screen();

				void	init(uint8_t tc = TAB_COLOUR), begin(uint8_t tc = TAB_COLOUR); // Same - begin included for backwards compatibility
				T_DispCoords	height(void),
										width(void);

//			protected:

				void	getSetup(T_DisplaySettings& tft_settings), // Sketch provides the instance to populate

						setWindow(T_DispCoords xs, T_DispCoords ys, T_DispCoords xe, T_DispCoords ye),
						setAddrWindow(T_DispCoords xs, T_DispCoords ys, T_DispCoords w, T_DispCoords h),

					// Write pixels to display SPI
						writeBlock(uint16_t color, uint32_t repeat),				// Fast block write prototype
						pushColor(uint16_t color),									//
						pushColor(uint16_t color, uint32_t len),					//
						pushColors(uint16_t  *data, uint32_t len, bool swap = true),// With byte swap option
						pushColors(uint8_t  *data, uint32_t len),					//
					// Compatibility additions
						writeColor(uint16_t color, uint32_t len); // Write colors without transaction overhead

				// These are virtual so the TFT_eSprite class can override them with sprite specific functions
				virtual void	drawPixel(T_DispCoords x, T_DispCoords y, uint32_t color),
								drawLine(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, uint32_t color),
								drawFastVLine(T_DispCoords x, T_DispCoords y, T_DispCoords h, uint32_t color),
								drawFastHLine(T_DispCoords x, T_DispCoords y, T_DispCoords w, uint32_t color),
								fillRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint32_t color);

				// Read info from SPI:
				uint8_t  readcommand8(uint8_t cmd_function, uint8_t index = 0);		// 8-bit read
				uint16_t readcommand16(uint8_t cmd_function, uint8_t index = 0);	// 16-bit read
				uint32_t readcommand32(uint8_t cmd_function, uint8_t index = 0);	// 32-bit read

					   // Read the color of a pixel at x,y and return value in 565 format
				uint32_t readPixel(T_DispCoords x0, T_DispCoords y0);

					   // The next functions can be used as a pair to copy screen blocks (or horizontal/vertical lines) to another location
					   // Read a block of pixels to a data buffer, buffer is 16 bit and the array size must be at least w * h
				void     readRect(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint16_t *data);
					   // Write a block of pixels to the screen
				void     pushRect(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint16_t *data);

					   // These are used to render images or sprites stored in RAM arrays
				void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint16_t *data);
				void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint16_t *data, uint16_t transparent);

/*	*/				   // These are used to render images stored in FLASH (PROGMEM)
				void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, const uint16_t *data, uint16_t transparent);
				void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, const uint16_t *data);
/**/
/*	*/				   // These are used by pushSprite for 1 and 8 bit colors
				void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint8_t  *data, bool bpp8 = true);
				void     pushImage(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint8_t  *data, uint8_t  transparent, bool bpp8 = true);
/**/
					   // Swap the byte order for pushImage() - corrects endianness
				void     setSwapBytes(bool swap);
				bool     getSwapBytes(void);

					   // This next function has been used successfully to dump the TFT screen to a PC for documentation purposes
					   // It reads a screen area and returns the RGB 8 bit color values of each pixel
					   // Set w and h to 1 to read 1 pixel's color. The data buffer must be at least w * h * 3 bytes
				void     readRectRGB(T_DispCoords x0, T_DispCoords y0, T_DispCoords w, T_DispCoords h, uint8_t *data);

				uint8_t	getRotation(void);

				#ifdef TFT_SDA_READ
				  #if defined (ESP8266) && defined (TFT_SDA_READ)
				  uint8_t  tft_Read_8(void);
				  #endif
				  void     begin_SDA_Read(void);
				  void     end_SDA_Read(void);
				#endif


			//END protected//
//			 private:
					static	SPIClass&	getSPIinstance(void);

				#ifdef ESP32_PARALLEL
					// GPIO parallel input/output control
					void busDir(uint32_t mask, uint8_t mode);
					// Byte read prototype
					uint8_t readByte(void);
				#endif

				  // SPI-commands


				void
				// Display configuration
					setRotation(uint8_t r),		// Set display rotation
					invertDisplay(boolean i),	// Set/reset color inversion

					readAddrWindow(int32_t xs, int32_t ys, T_DispCoords w, T_DispCoords h); //define an area to read a stream of pixels
				bool	SPIStartWrite(void),			// Begin SPI transaction
						SPIStartRead(void);
				void	SPIEndWrite(void),				// End SPI transaction
						SPIEndRead(void),
					spiwrite(uint8_t),			// Write any to SPI
					writecommand(uint8_t c),	// Send command to SPI
					writedata(uint8_t d),		// Send data to SPI
					commandList(const uint8_t *addr);	// Send list of commands to SPI,
														// first byte contains number of commands in array

				uint8_t  tabcolor,
					   colstart = 0, rowstart = 0; // some ST7735 displays need this changed

				volatile uint32_t *dcport, *csport;

				uint32_t cspinmask, dcpinmask, wrpinmask, sclkpinmask;

				#if defined(ESP32_PARALLEL)
					uint32_t  xclr_mask, xdir_mask, xset_mask[256];
				#endif

				uint32_t lastColor = 0xFFFF;




				uint32_t bitmap_fg, bitmap_bg; //ToDo: remove it from here




				T_DispCoords  _init_width, _init_height; // Display w/h as input, used by setRotation()
				T_DispCoords  _width, _height;           // Display w/h as modified by current rotation


			 protected:


				inline void spi_begin(void){
				#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
				  if (locked) {locked = false; spi.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE)); CS_L;}
				#else
				  CS_L;
				#endif
				#ifdef ESP8266
				  SPI1U = SPI1U_WRITE;
				#endif
				}

				inline void spi_end(void){
				#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
				  if(!inTransaction) {if (!locked) {locked = true; CS_H; spi.endTransaction();}}
				  #ifdef ESP8266
					SPI1U = SPI1U_READ;
				  #endif
				#else
				  if(!inTransaction) CS_H;
				#endif
				}

				inline void spi_begin_read(void){
				#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
				  if (locked) {locked = false; spi.beginTransaction(SPISettings(SPI_READ_FREQUENCY, MSBFIRST, TFT_SPI_MODE)); CS_L;}
				#else
				  #if !defined(ESP32_PARALLEL)
					spi.setFrequency(SPI_READ_FREQUENCY);
				  #endif
				   CS_L;
				#endif
				#ifdef ESP8266
				  SPI1U = SPI1U_READ;
				#endif
				}

				inline void spi_end_read(void){
				#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
				  if(!inTransaction) {if (!locked) {locked = true; CS_H; spi.endTransaction();}}
				#else
				  #if !defined(ESP32_PARALLEL)
					spi.setFrequency(SPI_FREQUENCY);
				  #endif
				   if(!inTransaction) CS_H;
				#endif
				#ifdef ESP8266
				  SPI1U = SPI1U_WRITE;
				#endif
				}

				virtual void PrintError(const char []);

				int32_t  addr_row, addr_col;
				uint32_t _lastColor; // Buffered value of last color used
				uint8_t  rotation;  // Display rotation (0-3)
				bool     _swapBytes; // Swap the byte order for TFT pushImage()
				bool     locked, inTransaction; // Transaction and mutex lock flags for ESP32
				bool     _booted;    // init() or begin() has already run once



			}; // End of class Screen

			static Screen* Driver;
//			void init(T_DispCoords _W = TFT_WIDTH, T_DispCoords _H = TFT_HEIGHT);
			void init();
			void remove();
		}
	}
#endif
