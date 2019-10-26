/***************************************************
 Arduino TFT graphics library targeted at ESP8266
 and ESP32 based boards.

 This is a standalone library that contains the
 hardware driver. Graphics functions and
 proportional fonts moved to separated classes.

 Created by Bodmer 2/12/16

 Modifyed by Laguna_x 16/oct/2019
 ****************************************************/
#include <Arduino.h>
#include <SPI.h>
#include "ESP32_SPIDisplay.h"

inline void spi_begin(void) {
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
	if (locked) {
		locked = false;
		spi.beginTransaction(
				SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
		CS_L
		;
	}
#else
				  CS_L;
				#endif
#ifdef ESP8266
				  SPI1U = SPI1U_WRITE;
				#endif
}

inline void spi_end(void) {
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
	if (!inTransaction) {
		if (!locked) {
			locked = true;
			CS_H;
			spi.endTransaction();
		}
	}
#ifdef ESP8266
					SPI1U = SPI1U_READ;
				  #endif
#else
				  if(!inTransaction) CS_H;
				#endif
}

inline void spi_begin_read(void) {
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
	if (locked) {
		locked = false;
		spi.beginTransaction(
				SPISettings(SPI_READ_FREQUENCY, MSBFIRST, TFT_SPI_MODE));
		CS_L
		;
	}
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

inline void spi_end_read(void) {
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
	if (!inTransaction) {
		if (!locked) {
			locked = true;
			CS_H;
			spi.endTransaction();
		}
	}
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

namespace Device {
namespace Display {

//Screen* Driver;

/*******************************************************************************
 *                                                                             *
 * Usage:                                                                      *
 *                                                                             *
 *  #include <ESP32-SPIDisplay.h>                                              *
 *                                                                             *
 *  Device::Display::init();                                                   *
 *   //or                                                                      *
 *  Device::Display::init(width, height);                                      *
 *   // where width and height are type of T_DispCoords                        *
 *                                                                             *
 *  Next where have needs use Driver as pointer to Screen class                *
 *                                                                             *
 *  To remove driver from memory call                                          *
 *  Device::Display::remove();                                                 *
 *                                                                             *
 *******************************************************************************/
//Screen* Driver;
/*// Next code for initializing class in memory
 void init(T_DispCoords _W, T_DispCoords _H, uint8_t _R
 #ifdef ST7735_DRIVER
 , uint8_t tc
 #endif
 ) {
 Driver = new Screen(_W, _H);
 #ifdef ST7735_DRIVER
 Driver->init(tc);
 #else
 Driver->init();
 #endif
 Driver->setRotation(_R);
 }
 void init(uint8_t R
 #ifdef ST7735_DRIVER
 , uint8_t tc
 #endif
 ) {
 init(TFT_WIDTH,
 TFT_HEIGHT, R
 #ifdef ST7735_DRIVER
 , tc
 #endif
 );
 }
 void init(
 #ifdef ST7735_DRIVER
 , uint8_t tc
 #endif
 ) {
 init(0
 #ifdef ST7735_DRIVER
 , tc
 #endif
 );
 }
 void remove() {
 delete (Driver);
 }
 */
void Screen::PrintError(const char message[]) {
	//use it for debugging spi. Make print to serial or whatever.
	;
}

/***************************************************************************************
 ** Function name:           Screen
 ** Description:             Constructor , we must use hardware SPI pins
 ***************************************************************************************/
Screen::Screen(T_DispCoords w, T_DispCoords h) // @suppress("Class members should be properly initialized")
		{

	// The control pins are deliberately set to the inactive state (CS high) as setup()
	// might call and initialise other SPI peripherals which would could cause conflicts
	// if CS is floating or undefined.
#ifdef TFT_CS
	digitalWrite(TFT_CS, HIGH); // Chip select high (inactive)
	pinMode(TFT_CS, OUTPUT);
#endif

	// Configure chip select for touchscreen controller if present
#ifdef TOUCH_CS
	  digitalWrite(TOUCH_CS, HIGH); // Chip select high (inactive)
	  pinMode(TOUCH_CS, OUTPUT);
	#endif

#ifdef TFT_WR
	  digitalWrite(TFT_WR, HIGH); // Set write strobe high (inactive)
	  pinMode(TFT_WR, OUTPUT);
	#endif

#ifdef TFT_DC
	digitalWrite(TFT_DC, HIGH); // Data/Command high = data mode
	pinMode(TFT_DC, OUTPUT);
#endif

#ifdef TFT_RST
	if (TFT_RST >= 0) {
		digitalWrite(TFT_RST, HIGH); // Set high, do not share pin with another SPI device
		pinMode(TFT_RST, OUTPUT);
	}
#endif

#ifdef ESP32_PARALLEL

	  // Create a bit set lookup table for data bus - wastes 1kbyte of RAM but speeds things up dramatically
	  for (int32_t c = 0; c<256; c++)
	  {
	    xset_mask[c] = 0;
	    if ( c & 0x01 ) xset_mask[c] |= (1 << TFT_D0);
	    if ( c & 0x02 ) xset_mask[c] |= (1 << TFT_D1);
	    if ( c & 0x04 ) xset_mask[c] |= (1 << TFT_D2);
	    if ( c & 0x08 ) xset_mask[c] |= (1 << TFT_D3);
	    if ( c & 0x10 ) xset_mask[c] |= (1 << TFT_D4);
	    if ( c & 0x20 ) xset_mask[c] |= (1 << TFT_D5);
	    if ( c & 0x40 ) xset_mask[c] |= (1 << TFT_D6);
	    if ( c & 0x80 ) xset_mask[c] |= (1 << TFT_D7);
	  }

	  // Make sure read is high before we set the bus to output
	  digitalWrite(TFT_RD, HIGH);
	  pinMode(TFT_RD, OUTPUT);

	  GPIO.out_w1ts = set_mask(255); // Set data bus to 0xFF

	  // Set TFT data bus lines to output
	  busDir(dir_mask, OUTPUT);

	#endif

	_init_width = _width = w; // Set by specific xxxxx_Defines.h file or by users sketch
	_init_height = _height = h; // Set by specific xxxxx_Defines.h file or by users sketch
	rotation = 0;

	_swapBytes = true;   // Do not swap colour bytes by default

//	  locked = true;        // ESP32 transaction mutex lock flags
//	  inTransaction = false;

	_booted = true;

	addr_row = 0xFFFF;
	addr_col = 0xFFFF;

	cspinmask = 0;
	dcpinmask = 0;
	wrpinmask = 0;
	sclkpinmask = 0;

}

Screen::~Screen() {

}
/***************************************************************************************
 ** Function name:           begin
 ** Description:             Included for backwards compatibility
 ***************************************************************************************/
void Screen::begin(uint8_t tc) {
	init(tc);
}

/***************************************************************************************
 ** Function name:           init (tc is tab color for ST7735 displays only)
 ** Description:             Reset, then initialise the TFT display registers
 ***************************************************************************************/
void Screen::init(uint8_t tc) {
	if (_booted) {
#if !defined (ESP32)
	  #if defined (TFT_CS) && (TFT_CS >= 0)
	    cspinmask = (uint32_t) digitalPinToBitMask(TFT_CS);
	  #endif

	  #if defined (TFT_DC) && (TFT_DC >= 0)
	    dcpinmask = (uint32_t) digitalPinToBitMask(TFT_DC);
	  #endif

	  #if defined (TFT_WR) && (TFT_WR >= 0)
	    wrpinmask = (uint32_t) digitalPinToBitMask(TFT_WR);
	  #endif

	  #if defined (TFT_SCLK) && (TFT_SCLK >= 0)
	    sclkpinmask = (uint32_t) digitalPinToBitMask(TFT_SCLK);
	  #endif

	  #ifdef TFT_SPI_OVERLAP
	    // Overlap mode SD0=MISO, SD1=MOSI, CLK=SCLK must use D3 as CS
	    //    pins(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
	    //spi.pins(        6,          7,           8,          0);
	    spi.pins(6, 7, 8, 0);
	  #endif

	  spi.begin(); // This will set HMISO to input

	#else
#if !defined(ESP32_PARALLEL)
#if defined (TFT_MOSI) && !defined (TFT_SPI_OVERLAP)
		spi.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, -1);
#else
	      spi.begin();
	    #endif
#endif
#endif

		inTransaction = false;
		locked = true;

		// SUPPORT_TRANSACTIONS is mandatory for ESP32 so the hal mutex is toggled
		// so the code here is for ESP8266 only
#if !defined (SUPPORT_TRANSACTIONS) && defined (ESP8266)
	    spi.setBitOrder(MSBFIRST);
	    spi.setDataMode(TFT_SPI_MODE);
	    spi.setFrequency(SPI_FREQUENCY);
	#endif

#if defined(ESP32_PARALLEL)
	    digitalWrite(TFT_CS, LOW); // Chip select low permanently
	    pinMode(TFT_CS, OUTPUT);
	#else
#ifdef TFT_CS
		// Set to output once again in case D6 (MISO) is used for CS
		digitalWrite(TFT_CS, HIGH); // Chip select high (inactive)
		pinMode(TFT_CS, OUTPUT);
#else
	    spi.setHwCs(1); // Use hardware SS toggling
	  #endif
#endif

		// Set to output once again in case D6 (MISO) is used for DC
#ifdef TFT_DC
		digitalWrite(TFT_DC, HIGH); // Data/Command high = data mode
		pinMode(TFT_DC, OUTPUT);
#endif

		_booted = false;
		spi_end();
	} // end of: if just _booted

	// Toggle RST low to reset
	spi_begin();

#ifdef TFT_RST
	if (TFT_RST >= 0) {
		digitalWrite(TFT_RST, HIGH);
		delay(5);
		digitalWrite(TFT_RST, LOW);
		delay(20);
		digitalWrite(TFT_RST, HIGH);
	} else
		writecommand(TFT_SWRST); // Software reset
#else
	  writecommand(TFT_SWRST); // Software reset
	#endif

	spi_end();

	delay(150); // Wait for reset to complete

	spi_begin();

	// This loads the driver specific initialisation code  <<<<<<<<<<<<<<<<<<<<< ADD NEW DRIVERS TO THE LIST HERE <<<<<<<<<<<<<<<<<<<<<<<
#if   defined (ILI9341_DRIVER)
	    #include "TFT_Drivers/ILI9341_Init.h"

	#elif defined (ST7735_DRIVER)
	    tabcolor = tc;
	    #include "TFT_Drivers/ST7735_Init.h"

	#elif defined (ILI9163_DRIVER)
	    #include "TFT_Drivers/ILI9163_Init.h"

	#elif defined (S6D02A1_DRIVER)
	    #include "TFT_Drivers/S6D02A1_Init.h"

	#elif defined (RPI_ILI9486_DRIVER)
	    #include "TFT_Drivers/ILI9486_Init.h"

	#elif defined (ILI9486_DRIVER)
	    #include "TFT_Drivers/ILI9486_Init.h"

	#elif defined (ILI9481_DRIVER)
	    #include "TFT_Drivers/ILI9481_Init.h"

	#elif defined (ILI9488_DRIVER)
	    #include "TFT_Drivers/ILI9488_Init.h"

	#elif defined (HX8357D_DRIVER)
	    #include "TFT_Drivers/HX8357D_Init.h"

	#elif defined (ST7789_DRIVER)
#include "TFT_Drivers/ST7789_Init.h"

#elif defined (R61581_DRIVER)
	    #include "TFT_Drivers/R61581_Init.h"

	#elif defined (RM68140_DRIVER)
		#include "TFT_Drivers/RM68140_Init.h"

	#elif defined (ST7789_2_DRIVER)
	    #include "TFT_Drivers/ST7789_2_Init.h"

	#endif

#ifdef TFT_INVERSION_ON
	writecommand(TFT_INVON);
#endif

#ifdef TFT_INVERSION_OFF
	  writecommand(TFT_INVOFF);
	#endif

	spi_end();

	setRotation(rotation);

#if defined (TFT_BL) && defined (TFT_BACKLIGHT_ON)
	digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
	pinMode(TFT_BL, OUTPUT);
#else
	  #if defined (TFT_BL) && defined (M5STACK)
	    // Turn on the back-light LED
	    digitalWrite(TFT_BL, HIGH);
	    pinMode(TFT_BL, OUTPUT);
	  #endif
	#endif
}

/***************************************************************************************
 ** Function name:           setRotation
 ** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
 ***************************************************************************************/
void Screen::setRotation(uint8_t m) {

	spi_begin();

	// This loads the driver specific rotation code  <<<<<<<<<<<<<<<<<<<<< ADD NEW DRIVERS TO THE LIST HERE <<<<<<<<<<<<<<<<<<<<<<<
#if   defined (ILI9341_DRIVER)
	    #include "TFT_Drivers/ILI9341_Rotation.h"

	#elif defined (ST7735_DRIVER)
	    #include "TFT_Drivers/ST7735_Rotation.h"

	#elif defined (ILI9163_DRIVER)
	    #include "TFT_Drivers/ILI9163_Rotation.h"

	#elif defined (S6D02A1_DRIVER)
	    #include "TFT_Drivers/S6D02A1_Rotation.h"

	#elif defined (RPI_ILI9486_DRIVER)
	    #include "TFT_Drivers/ILI9486_Rotation.h"

	#elif defined (ILI9486_DRIVER)
	    #include "TFT_Drivers/ILI9486_Rotation.h"

	#elif defined (ILI9481_DRIVER)
	    #include "TFT_Drivers/ILI9481_Rotation.h"

	#elif defined (ILI9488_DRIVER)
	    #include "TFT_Drivers/ILI9488_Rotation.h"

	#elif defined (HX8357D_DRIVER)
	    #include "TFT_Drivers/HX8357D_Rotation.h"

	#elif defined (ST7789_DRIVER)
#include "TFT_Drivers/ST7789_Rotation.h"

#elif defined (R61581_DRIVER)
	    #include "TFT_Drivers/R61581_Rotation.h"

	#elif defined (RM68140_DRIVER)
		#include "TFT_Drivers/RM68140_Rotation.h"

	#elif defined (ST7789_2_DRIVER)
	    #include "TFT_Drivers/ST7789_2_Rotation.h"

	#endif

	delayMicroseconds(10);

	spi_end();

	addr_row = 0xFFFF;
	addr_col = 0xFFFF;
}

/***************************************************************************************
 ** Function name:           commandList, used for FLASH based lists only (e.g. ST7735)
 ** Description:             Get initialisation commands from FLASH and send to TFT
 ***************************************************************************************/
void Screen::commandList(const uint8_t *addr) {
	uint8_t numCommands;
	uint8_t numArgs;
	uint8_t ms;

	numCommands = pgm_read_byte(addr++);   // Number of commands to follow

	while (numCommands--)                  // For each command...
	{
		writecommand(pgm_read_byte(addr++)); // Read, issue command
		numArgs = pgm_read_byte(addr++);     // Number of args to follow
		ms = numArgs & TFT_INIT_DELAY;       // If hibit set, delay follows args
		numArgs &= ~TFT_INIT_DELAY;          // Mask out delay bit

		while (numArgs--)                    // For each argument...
		{
			writedata(pgm_read_byte(addr++));  // Read, issue argument
		}

		if (ms) {
			ms = pgm_read_byte(addr++);     // Read post-command delay time (ms)
			delay((ms == 255 ? 500 : ms));
		}
	}
}

/***************************************************************************************
 ** Function name:           spiwrite
 ** Description:             Write 8 bits to SPI port (legacy support only)
 ***************************************************************************************/
void Screen::spiwrite(uint8_t c) {
	tft_Write_8(c);
}

/***************************************************************************************
 ** Function name:           writecommand
 ** Description:             Send an 8 bit command to the TFT
 ***************************************************************************************/
void Screen::writecommand(uint8_t c) {
	spi_begin(); // CS_L;

	DC_C;

	tft_Write_8(c);

	DC_D;

	spi_end();  // CS_H;

}

/***************************************************************************************
 ** Function name:           writedata
 ** Description:             Send a 8 bit data value to the TFT
 ***************************************************************************************/
void Screen::writedata(uint8_t d) {
	spi_begin(); // CS_L;

	DC_D;        // Play safe, but should already be in data mode

	tft_Write_8(d);

	CS_L
	;        // Allow more hold time for low VDI rail

	spi_end();   // CS_H;
}

/***************************************************************************************
 ** Function name:           readcommand8
 ** Description:             Read a 8 bit data value from an indexed command register
 ***************************************************************************************/
uint8_t Screen::readcommand8(uint8_t cmd_function, uint8_t index) {
	uint8_t reg = 0;
#ifdef ESP32_PARALLEL

	  writecommand(cmd_function); // Sets DC and CS high

	  busDir(dir_mask, INPUT);

	  CS_L;

	  // Read nth parameter (assumes caller discards 1st parameter or points index to 2nd)
	  while(index--) reg = readByte();

	  busDir(dir_mask, OUTPUT);

	  CS_H;

	#else
	// for ILI9341 Interface II i.e. IM [3:0] = "1101"
	spi_begin_read();
	index = 0x10 + (index & 0x0F);

	DC_C;
	tft_Write_8(0xD9);
	DC_D;
	tft_Write_8(index);

	CS_H; // Some displays seem to need CS to be pulsed here, or is just a delay needed?
	CS_L
	;

	DC_C;
	tft_Write_8(cmd_function);
	DC_D;
	reg = tft_Read_8();

	spi_end_read();
#endif
	return reg;
}

/***************************************************************************************
 ** Function name:           readcommand16
 ** Description:             Read a 16 bit data value from an indexed command register
 ***************************************************************************************/
uint16_t Screen::readcommand16(uint8_t cmd_function, uint8_t index) {
	uint32_t reg;

	reg = (readcommand8(cmd_function, index + 0) << 8);
	reg |= (readcommand8(cmd_function, index + 1) << 0);

	return reg;
}

/***************************************************************************************
 ** Function name:           readcommand32
 ** Description:             Read a 32 bit data value from an indexed command register
 ***************************************************************************************/
uint32_t Screen::readcommand32(uint8_t cmd_function, uint8_t index) {
	uint32_t reg;

	reg = (readcommand8(cmd_function, index + 0) << 24);
	reg |= (readcommand8(cmd_function, index + 1) << 16);
	reg |= (readcommand8(cmd_function, index + 2) << 8);
	reg |= (readcommand8(cmd_function, index + 3) << 0);

	return reg;
}

/***************************************************************************************
 ** Function name:           read pixel (for SPI Interface II i.e. IM [3:0] = "1101")
 ** Description:             Read 565 pixel colors from a pixel
 ***************************************************************************************/
uint32_t Screen::readPixel(T_DispCoords x0, T_DispCoords y0) {
#if defined(ESP32_PARALLEL)

  readAddrWindow(x0, y0, 1, 1); // Sets CS low

  // Set masked pins D0- D7 to input
  busDir(dir_mask, INPUT);

  // Dummy read to throw away don't care value
  readByte();

  // Fetch the 16 bit BRG pixel
  //uint16_t rgb = (readByte() << 8) | readByte();

  #if defined (ILI9341_DRIVER) | defined (ILI9488_DRIVER) // Read 3 bytes

  // Read window pixel 24 bit RGB values and fill in LS bits
  uint16_t rgb = ((readByte() & 0xF8) << 8) | ((readByte() & 0xFC) << 3) | (readByte() >> 3);

  CS_H;

  // Set masked pins D0- D7 to output
  busDir(dir_mask, OUTPUT);

  return rgb;

  #else // ILI9481 16 bit read

  // Fetch the 16 bit BRG pixel
  uint16_t bgr = (readByte() << 8) | readByte();

  CS_H;

  // Set masked pins D0- D7 to output
  busDir(dir_mask, OUTPUT);

  // Swap Red and Blue (could check MADCTL setting to see if this is needed)
  return  (bgr>>11) | (bgr<<11) | (bgr & 0x7E0);
  #endif

#else // Not ESP32_PARALLEL

	spi_begin_read();

	readAddrWindow(x0, y0, 1, 1); // Sets CS low

#ifdef TFT_SDA_READ
    begin_SDA_Read();
  #endif

	// Dummy read to throw away don't care value
	tft_Read_8();

	//#if !defined (ILI9488_DRIVER)

	// Read the 3 RGB bytes, colour is actually only in the top 6 bits of each byte
	// as the TFT stores colours as 18 bits
	uint8_t r = tft_Read_8();
	uint8_t g = tft_Read_8();
	uint8_t b = tft_Read_8();
	/*
	 #else

	 // The 6 colour bits are in MS 6 bits of each byte, but the ILI9488 needs an extra clock pulse
	 // so bits appear shifted right 1 bit, so mask the middle 6 bits then shift 1 place left
	 uint8_t r = (tft_Read_8()&0x7E)<<1;
	 uint8_t g = (tft_Read_8()&0x7E)<<1;
	 uint8_t b = (tft_Read_8()&0x7E)<<1;

	 #endif
	 */
	CS_H;

#ifdef TFT_SDA_READ
    end_SDA_Read();
  #endif

	spi_end_read();

	return (((uint32_t) 0) | (r << 16) | (g << 8) | b); //retuns 0x00rrggbb;color565(r, g, b);

#endif
}

#ifdef ESP32_PARALLEL
		/***************************************************************************************
		** Function name:           read byte  - supports class functions
		** Description:             Read a byte from ESP32 8 bit data port
		***************************************************************************************/
		// Bus MUST be set to input before calling this function!
		uint8_t Screen::readByte(void)
		{
		  uint8_t b = 0;

		  RD_L;
		  uint32_t reg;           // Read all GPIO pins 0-31
		  reg = gpio_input_get(); // Read three times to allow for bus access time
		  reg = gpio_input_get();
		  reg = gpio_input_get(); // Data should be stable now
		  RD_H;

		  // Check GPIO bits used and build value
		  b  = (((reg>>TFT_D0)&1) << 0);
		  b |= (((reg>>TFT_D1)&1) << 1);
		  b |= (((reg>>TFT_D2)&1) << 2);
		  b |= (((reg>>TFT_D3)&1) << 3);
		  b |= (((reg>>TFT_D4)&1) << 4);
		  b |= (((reg>>TFT_D5)&1) << 5);
		  b |= (((reg>>TFT_D6)&1) << 6);
		  b |= (((reg>>TFT_D7)&1) << 7);

		  return b;
		}
		#endif

/***************************************************************************************
 ** Function name:           masked GPIO direction control  - supports class functions
 ** Description:             Set masked ESP32 GPIO pins to input or output
 ***************************************************************************************/
#ifdef ESP32_PARALLEL
		void Screen::busDir(uint32_t mask, uint8_t mode)
		{
		#ifdef ESP32_PARALLEL

		  // Supports GPIO 0 - 31 on ESP32 only
		  gpio_config_t gpio;

		  gpio.pin_bit_mask = mask;
		  gpio.mode         = GPIO_MODE_INPUT;
		  gpio.pull_up_en   = GPIO_PULLUP_ENABLE;
		  gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
		  gpio.intr_type    = GPIO_INTR_DISABLE;

		  if (mode == OUTPUT) gpio.mode = GPIO_MODE_OUTPUT;

		  gpio_config(&gpio);

		#endif
		}
#endif

/***************************************************************************************
 ** Function name:           read rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
 ** Description:             Read 565 pixel colors from a defined area
 ***************************************************************************************/
void Screen::readRect(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint16_t *data) {
	if ((x > _width) || (y > _height) || (w == 0) || (h == 0))
		return;

#if defined(ESP32_PARALLEL)

		  readAddrWindow(x, y, w, h); // Sets CS low

		  // Set masked pins D0- D7 to input
		  busDir(dir_mask, INPUT);

		  // Dummy read to throw away don't care value
		  readByte();

		  // Total pixel count
		  uint32_t len = w * h;

		#if defined (ILI9341_DRIVER) | defined (ILI9488_DRIVER) // Read 3 bytes
		  // Fetch the 24 bit RGB value
		  while (len--) {
			// Assemble the RGB 16 bit color
			uint16_t rgb = ((readByte() & 0xF8) << 8) | ((readByte() & 0xFC) << 3) | (readByte() >> 3);

			// Swapped byte order for compatibility with pushRect()
			*data++ = (rgb<<8) | (rgb>>8);
		  }
		#else // ILI9481 reads as 16 bits
		  // Fetch the 16 bit BRG pixels
		  while (len--) {
			// Read the BRG 16 bit color
			uint16_t bgr = (readByte() << 8) | readByte();

			// Swap Red and Blue (could check MADCTL setting to see if this is needed)
			uint16_t rgb = (bgr>>11) | (bgr<<11) | (bgr & 0x7E0);

			// Swapped byte order for compatibility with pushRect()
			*data++ = (rgb<<8) | (rgb>>8);
		  }
		#endif
		  CS_H;

		  // Set masked pins D0- D7 to output
		  busDir(dir_mask, OUTPUT);

		#else // Not ESP32_PARALLEL

	spi_begin_read();
	//if (SPIStartRead()) {

	readAddrWindow(x, y, w, h); // Sets CS low

#ifdef TFT_SDA_READ
			begin_SDA_Read();
		  #endif

	// Dummy read to throw away don't care value
	tft_Read_8();

	// Read window pixel 24 bit RGB values
	uint32_t len = w * h;
	while (len--) {

#if !defined (ILI9488_DRIVER)

		// Read the 3 RGB bytes, color is actually only in the top 6 bits of each byte
		// as the TFT stores colors as 18 bits
		uint8_t r = tft_Read_8();
		uint8_t g = tft_Read_8();
		uint8_t b = tft_Read_8();

#else

			// The 6 color bits are in LS 6 bits of each byte but we do not include the extra clock pulse
			// so we use a trick and mask the middle 6 bits of the byte, then only shift 1 place left
			uint8_t r = (tft_Read_8()&0x7E)<<1;
			uint8_t g = (tft_Read_8()&0x7E)<<1;
			uint8_t b = (tft_Read_8()&0x7E)<<1;

		  #endif

		// Swapped color byte order for compatibility with pushRect()
		*data++ = (r & 0xF8) | (g & 0xE0) >> 5 | (b & 0xF8) << 5
				| (g & 0x1C) << 11;
	}

	CS_H;

#ifdef TFT_SDA_READ
			end_SDA_Read();
		  #endif

//		SPIEndRead();
//	} else {
//		PrintError("[]Error: SPI is buisy!");
//	}
	spi_end_read();

#endif
}

/***************************************************************************************
 ** Function name:           tft_Read_8
 ** Description:             Software SPI to read bidirectional SDA line
 ***************************************************************************************/
#if defined (ESP8266) && defined (TFT_SDA_READ)
		uint8_t Screen::tft_Read_8(void)
		{
		  uint8_t  ret = 0;
		  uint32_t reg = 0;

		  for (uint8_t i = 0; i < 8; i++) {  // read results
			ret <<= 1;
			SCLK_L;
			if (digitalRead(TFT_MOSI)) ret |= 1;
			SCLK_H;
		  }

		  return ret;
		}
		#endif

/***************************************************************************************
 ** Function name:           beginSDA
 ** Description:             Detach SPI from pin to permit software SPI
 ***************************************************************************************/
#ifdef TFT_SDA_READ
		void Screen::begin_SDA_Read(void)
		{
		  #ifdef ESP32
			pinMatrixOutDetach(TFT_MOSI, false, false);
			pinMode(TFT_MOSI, INPUT);
			pinMatrixInAttach(TFT_MOSI, VSPIQ_IN_IDX, false);
		  #else // ESP8266
			#ifdef TFT_SPI_OVERLAP
			  // Reads in overlap mode not supported
			#else
			  spi.end();
			#endif
		  #endif
		}
		#endif

/***************************************************************************************
 ** Function name:           endSDA
 ** Description:             Attach SPI pins after software SPI
 ***************************************************************************************/
#ifdef TFT_SDA_READ
		void Screen::end_SDA_Read(void)
		{
		  #ifdef ESP32
			pinMode(TFT_MOSI, OUTPUT);
			pinMatrixOutAttach(TFT_MOSI, VSPID_OUT_IDX, false, false);
			pinMode(TFT_MISO, INPUT);
			pinMatrixInAttach(TFT_MISO, VSPIQ_IN_IDX, false);
		  #else
			#ifdef TFT_SPI_OVERLAP
			  spi.pins(6, 7, 8, 0);
			#else
			  spi.begin();
			#endif
		  #endif
		}
		#endif

/***************************************************************************************
 ** Function name:           push rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
 ** Description:             push 565 pixel colors into a defined area
 ***************************************************************************************/
void Screen::pushRect(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint16_t *data) {
	// Function deprecated, remains for backwards compatibility
	// pushImage() is better as it will crop partly off-screen image blocks
	pushImage(x, y, w, h, data);
}

/***************************************************************************************
 ** Function name:           pushImage
 ** Description:             plot 16 bit color sprite or image onto TFT
 ***************************************************************************************/
void Screen::pushImage(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint16_t *data) {

	if ((x >= _width) || (y >= _height))
		return;

	int32_t dx = 0;
	int32_t dy = 0;
	int32_t dw = w;
	int32_t dh = h;

	if (x < 0) {
		dw += x;
		dx = -x;
		x = 0;
	}
	if (y < 0) {
		dh += y;
		dy = -y;
		y = 0;
	}

	if ((x + w) > _width)
		dw = _width - x;
	if ((y + h) > _height)
		dh = _height - y;

	if (dw < 1 || dh < 1)
		return;
//	spi_begin();
//	inTransaction = true;
	if (SPIStartWrite()) {
	setWindow(x, y, x + dw - 1, y + dh - 1);

	data += dx + dy * w;

	while (dh--) {
		pushColors(data, dw, _swapBytes);
		data += w;
	}
//	inTransaction = false;
//	spi_end();
		SPIEndWrite();
	} else {
		//calling output: Error, SPI is buisy!
//		PrintError("[pushImage] Error: SPI is buisy");
		Serial.printf("SPI is buisy:%i\n",(int)inTransaction);
	}
}

/***************************************************************************************
 ** Function name:           pushImage
 ** Description:             plot 16 bit sprite or image with 1 color being transparent
 ***************************************************************************************/
void Screen::pushImage(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint16_t *data, uint16_t transp) {

	if ((x >= _width) || (y >= _height))
		return;

	int32_t dx = 0;
	int32_t dy = 0;
	int32_t dw = w;
	int32_t dh = h;

	if (x < 0) {
		dw += x;
		dx = -x;
		x = 0;
	}
	if (y < 0) {
		dh += y;
		dy = -y;
		y = 0;
	}

	if ((x + w) > _width)
		dw = _width - x;
	if ((y + h) > _height)
		dh = _height - y;

	if (dw < 1 || dh < 1)
		return;

	if (SPIStartWrite()) {
		data += dx + dy * w;

		int32_t xe = x + dw - 1, ye = y + dh - 1;

		uint16_t lineBuf[dw];

		if (!_swapBytes)
			transp = transp >> 8 | transp << 8;

		while (dh--) {
			int32_t len = dw;
			uint16_t *ptr = data;
			int32_t px = x;
			boolean move = true;
			uint16_t np = 0;

			while (len--) {
				if (transp != *ptr) {
					if (move) {
						move = false;
						setWindow(px, y, xe, ye);
					}
					lineBuf[np] = *ptr;
					np++;
				} else {
					move = true;
					if (np) {
						pushColors((uint16_t*) lineBuf, np, _swapBytes);
						np = 0;
					}
				}
				px++;
				ptr++;
			}
			if (np)
				pushColors((uint16_t*) lineBuf, np, _swapBytes);

			y++;
			data += w;
		}
		SPIEndWrite();
	} else {
		PrintError("[pushImage]Error: SPI is buisy!");
	}
}

/****************************************************************************************************************************

 / ***************************************************************************************
 ** Function name:           pushImage - for FLASH (PROGMEM) stored images
 ** Description:             plot 16 bit image
 ***************************************************************************************/
void Screen::pushImage(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, const uint16_t *data) {
#ifdef ESP32
	pushImage(x, y, w, h, (uint16_t*) data);
#else
		  // Partitioned memory FLASH processor
		  if ((x >= _width) || (y >= _height)) return;

		  int32_t dx = 0;
		  int32_t dy = 0;
		  int32_t dw = w;
		  int32_t dh = h;

		  if (x < 0) { dw += x; dx = -x; x = 0; }
		  if (y < 0) { dh += y; dy = -y; y = 0; }

		  if ((x + w) > _width ) dw = _width  - x;
		  if ((y + h) > _height) dh = _height - y;

		  if (dw < 1 || dh < 1) return;

		if(SPIStartWrite()){

		  data += dx + dy * w;

		  uint16_t  buffer[64];
		  uint16_t* pix_buffer = buffer;

		  setWindow(x, y, x + dw - 1, y + dh - 1);

		  // Work out the number whole buffers to send
		  uint16_t nb = (dw * dh) / 64;

		  // Fill and send "nb" buffers to TFT
		  for (int32_t i = 0; i < nb; i++) {
			for (int32_t j = 0; j < 64; j++) {
			  pix_buffer[j] = pgm_read_word(&data[i * 64 + j]);
			}
			pushColors(pix_buffer, 64, _swapBytes);
		  }

		  // Work out number of pixels not yet sent
		  uint16_t np = (dw * dh) % 64;

		  // Send any partial buffer left over
		  if (np) {
			for (int32_t i = 0; i < np; i++)
			{
			  pix_buffer[i] = pgm_read_word(&data[nb * 64 + i]);
			}
			pushColors(pix_buffer, np, _swapBytes);
		  }
		  SPIEndWrite();
		}else{
		  PrintError("[pushImage]Error: SPI is buisy!");
		}

		#endif // if ESP32 else ESP8266 check
}

/***************************************************************************************
 ** Function name:           pushImage - for FLASH (PROGMEM) stored images
 ** Description:             plot 16 bit image with 1 color being transparent
 ***************************************************************************************/
void Screen::pushImage(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, const uint16_t *data, uint16_t transp) {
#ifdef ESP32
	pushImage(x, y, w, h, (uint16_t*) data, transp);
#else
		  // Partitioned memory FLASH processor
		  if ((x >= _width) || (y >= (int32_t)_height)) return;

		  int32_t dx = 0;
		  int32_t dy = 0;
		  int32_t dw = w;
		  int32_t dh = h;

		  if (x < 0) { dw += x; dx = -x; x = 0; }
		  if (y < 0) { dh += y; dy = -y; y = 0; }

		  if ((x + w) > _width ) dw = _width  - x;
		  if ((y + h) > _height) dh = _height - y;

		  if (dw < 1 || dh < 1) return;

		  if(SPIStartWrite()){

		  data += dx + dy * w;

		  int32_t xe = x + dw - 1, ye = y + dh - 1;

		  uint16_t  lineBuf[dw];

		if (!_swapBytes) transp = transp >> 8 | transp << 8;

		  while (dh--)
		  {
			int32_t len = dw;
			uint16_t* ptr = (uint16_t*)data;
			int32_t px = x;
			boolean move = true;

			uint16_t np = 0;

			while (len--)
			{
			  uint16_t color = pgm_read_word(ptr);
			  if (transp != color)
			  {
				if (move) { move = false; setWindow(px, y, xe, ye); }
				lineBuf[np] = color;
				np++;
			  }
			  else
			  {
				move = true;
				if (np)
				{
				   pushColors(lineBuf, np, _swapBytes);
				   np = 0;
				}
			  }
			  px++;
			  ptr++;
			}
			if (np) pushColors(lineBuf, np, _swapBytes);

			y++;
			data += w;
		  }
		  SPIEndWrite();
		}else{
		  	PrintError("[pushImage]Error: SPI is buisy!");
		}

		#endif // if ESP32 else ESP8266 check
}

/***************************************************************************************
 ** Function name:           pushImage
 ** Description:             plot 8 bit image or sprite using a line buffer
 ***************************************************************************************/
void Screen::pushImage(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint8_t *data, uint32_t bitmap_fg, uint32_t bitmap_bg,
		bool bpp8) {
	if ((x >= _width) || (y >= (int32_t) _height))
		return;

	int32_t dx = 0;
	int32_t dy = 0;
	int32_t dw = w;
	int32_t dh = h;

	if (x < 0) {
		dw += x;
		dx = -x;
		x = 0;
	}
	if (y < 0) {
		dh += y;
		dy = -y;
		y = 0;
	}

	if ((x + w) > _width)
		dw = _width - x;
	if ((y + h) > _height)
		dh = _height - y;

	if (dw < 1 || dh < 1)
		return;

	if (SPIStartWrite()) {

		setWindow(x, y, x + dw - 1, y + dh - 1); // Sets CS low and sent RAMWR

		// Line buffer makes plotting faster
		uint16_t lineBuf[dw];

		if (bpp8) {
			uint8_t blue[] = { 0, 11, 21, 31 }; // blue 2 to 5 bit color lookup table

			_lastColor = -1; // Set to illegal value

			// Used to store last shifted color
			uint8_t msbColor = 0;
			uint8_t lsbColor = 0;

			data += dx + dy * w;
			while (dh--) {
				uint32_t len = dw;
				uint8_t *ptr = data;
				uint8_t *linePtr = (uint8_t*) lineBuf;

				while (len--) {
					uint32_t color = *ptr++;

					// Shifts are slow so check if color has changed first
					if (color != _lastColor) {
						//          =====Green=====     ===============Red==============
						msbColor = (color & 0x1C) >> 2 | (color & 0xC0) >> 3
								| (color & 0xE0);
						//          =====Green=====    =======Blue======
						lsbColor = (color & 0x1C) << 3 | blue[color & 0x03];
						_lastColor = color;
					}

					*linePtr++ = msbColor;
					*linePtr++ = lsbColor;
				}

				pushColors(lineBuf, dw, false);

				data += w;
			}
		} else {
			while (dh--) {
				w = (w + 7) & 0xFFF8;

				int32_t len = dw;
				uint8_t *ptr = data;
				uint8_t *linePtr = (uint8_t*) lineBuf;
				uint8_t bits = 8;
				while (len > 0) {
					if (len < 8)
						bits = len;
					uint32_t xp = dx;
					for (uint16_t i = 0; i < bits; i++) {
						uint8_t col = (ptr[(xp + dy * w) >> 3] << (xp & 0x7))
								& 0x80;
						if (col) {
							*linePtr++ = bitmap_fg >> 8;
							*linePtr++ = (uint8_t) bitmap_fg;
						} else {
							*linePtr++ = bitmap_bg >> 8;
							*linePtr++ = (uint8_t) bitmap_bg;
						}
						//if (col) drawPixel((dw-len)+xp,h-dh,bitmap_fg);
						//else     drawPixel((dw-len)+xp,h-dh,bitmap_bg);
						xp++;
					}
					ptr++;
					len -= 8;
				}

				pushColors(lineBuf, dw, false);

				dy++;
			}
		}
		SPIEndWrite();
	} else {
		PrintError("[pushImage]Error: SPI is buisy!");
	}
}

/***************************************************************************************
 ** Function name:           pushImage
 ** Description:             plot 8 or 1 bit image or sprite with a transparent color
 ***************************************************************************************/
void Screen::pushImage(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint8_t *data, uint32_t bitmap_fg, uint8_t transp,
		bool bpp8) {
	if ((x >= _width) || (y >= _height))
		return;

	int32_t dx = 0;
	int32_t dy = 0;
	int32_t dw = w;
	int32_t dh = h;

	if (x < 0) {
		dw += x;
		dx = -x;
		x = 0;
	}
	if (y < 0) {
		dh += y;
		dy = -y;
		y = 0;
	}

	if ((x + w) > _width)
		dw = _width - x;
	if ((y + h) > _height)
		dh = _height - y;

	if (dw < 1 || dh < 1)
		return;

	if (SPIStartWrite()) {

		int32_t xe = x + dw - 1, ye = y + dh - 1;

		// Line buffer makes plotting faster
		uint16_t lineBuf[dw];

		if (bpp8) {
			data += dx + dy * w;

			uint8_t blue[] = { 0, 11, 21, 31 }; // blue 2 to 5 bit color lookup table

			_lastColor = -1; // Set to illegal value

			// Used to store last shifted color
			uint8_t msbColor = 0;
			uint8_t lsbColor = 0;

			//int32_t spx = x, spy = y;

			while (dh--) {
				int32_t len = dw;
				uint8_t *ptr = data;
				uint8_t *linePtr = (uint8_t*) lineBuf;

				int32_t px = x;
				boolean move = true;
				uint16_t np = 0;

				while (len--) {
					if (transp != *ptr) {
						if (move) {
							move = false;
							setWindow(px, y, xe, ye);
						}
						uint8_t color = *ptr;

						// Shifts are slow so check if color has changed first
						if (color != _lastColor) {
							//          =====Green=====     ===============Red==============
							msbColor = (color & 0x1C) >> 2 | (color & 0xC0) >> 3
									| (color & 0xE0);
							//          =====Green=====    =======Blue======
							lsbColor = (color & 0x1C) << 3 | blue[color & 0x03];
							_lastColor = color;
						}
						*linePtr++ = msbColor;
						*linePtr++ = lsbColor;
						np++;
					} else {
						move = true;
						if (np) {
							pushColors(lineBuf, np, false);
							linePtr = (uint8_t*) lineBuf;
							np = 0;
						}
					}
					px++;
					ptr++;
				}

				if (np)
					pushColors(lineBuf, np, false);

				y++;
				data += w;
			}
		} else {
			w = (w + 7) & 0xFFF8;
			while (dh--) {
				int32_t px = x;
				boolean move = true;
				uint16_t np = 0;
				int32_t len = dw;
				uint8_t *ptr = data;
				uint8_t bits = 8;
				while (len > 0) {
					if (len < 8)
						bits = len;
					uint32_t xp = dx;
					uint32_t yp = (dy * w) >> 3;
					for (uint16_t i = 0; i < bits; i++) {
						//uint8_t col = (ptr[(xp + dy * w)>>3] << (xp & 0x7)) & 0x80;
						if ((ptr[(xp >> 3) + yp] << (xp & 0x7)) & 0x80) {
							if (move) {
								move = false;
								setWindow(px, y, xe, ye);
							}
							np++;
						} else {
							if (np) {
								pushColor(bitmap_fg, np);
								np = 0;
								move = true;
							}
						}
						px++;
						xp++;
					}
					ptr++;
					len -= 8;
				}
				if (np)
					pushColor(bitmap_fg, np);
				y++;
				dy++;
			}
		}
		SPIEndWrite();
	} else {
		PrintError("[pushImage]Error: SPI is buisy!");
	}

}

/***************************************************************************************
 ** Function name:           setSwapBytes
 ** Description:             Used by 16 bit pushImage() to swap byte order in colors
 ***************************************************************************************/
void Screen::setSwapBytes(bool swap) {
	_swapBytes = swap;
}

/***************************************************************************************
 ** Function name:           getSwapBytes
 ** Description:             Return the swap byte order for colors
 ***************************************************************************************/
bool Screen::getSwapBytes(void) {
	return _swapBytes;
}

/***************************************************************************************
 ** Function name:           read rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
 ** Description:             Read RGB pixel colors from a defined area
 ***************************************************************************************/
// If w and h are 1, then 1 pixel is read, *data array size must be 3 bytes per pixel
void Screen::readRectRGB(T_DispCoords x0, T_DispCoords y0, T_DispCoords w,
		T_DispCoords h, uint8_t *data) {
#if defined(ESP32_PARALLEL)

		  // ESP32 parallel bus supported yet

		#else  // Not ESP32_PARALLEL

	if (SPIStartRead()) {
		//spi_begin_read();

		readAddrWindow(x0, y0, w, h); // Sets CS low

#ifdef TFT_SDA_READ
			begin_SDA_Read();
		  #endif

		// Dummy read to throw away don't care value
		tft_Read_8();

		// Read window pixel 24 bit RGB values, buffer must be set in sketch to 3 * w * h
		uint32_t len = w * h;
		while (len--) {

#if !defined (ILI9488_DRIVER)

			// Read the 3 RGB bytes, color is actually only in the top 6 bits of each byte
			// as the TFT stores colors as 18 bits
			*data++ = tft_Read_8();
			*data++ = tft_Read_8();
			*data++ = tft_Read_8();

#else

			// The 6 color bits are in MS 6 bits of each byte, but the ILI9488 needs an extra clock pulse
			// so bits appear shifted right 1 bit, so mask the middle 6 bits then shift 1 place left
			*data++ = (tft_Read_8()&0x7E)<<1;
			*data++ = (tft_Read_8()&0x7E)<<1;
			*data++ = (tft_Read_8()&0x7E)<<1;

		  #endif

		}

		CS_H;

#ifdef TFT_SDA_READ
			end_SDA_Read();
		  #endif

		SPIEndRead();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}
	//spi_end_read();

#endif
}

/***************************************************************************************
 ** Function name:           getRotation
 ** Description:             Return the rotation value (as used by setRotation())
 ***************************************************************************************/
uint8_t Screen::getRotation(void) {
	return rotation;
}

/***************************************************************************************
 ** Function name:           width
 ** Description:             Return the pixel width of display (per current rotation)
 ***************************************************************************************/
// Return the size of the display (per current rotation)
T_DispCoords Screen::width(void) {
	return _width;
}

/***************************************************************************************
 ** Function name:           height
 ** Description:             Return the pixel height of display (per current rotation)
 ***************************************************************************************/
T_DispCoords Screen::height(void) {
	return _height;
}

/***************************************************************************************
 ** Function name:           setAddrWindow
 ** Description:             define an area to receive a stream of pixels
 ***************************************************************************************/
// Chip select is high at the end of this function
void Screen::setAddrWindow(T_DispCoords x0, T_DispCoords y0, T_DispCoords w,
		T_DispCoords h) {
	if (SPIStartWrite()) {
		setWindow(x0, y0, x0 + w - 1, y0 + h - 1);
		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}
}

/***************************************************************************************
 ** Function name:           setWindow
 ** Description:             define an area to receive a stream of pixels
 ** spi_begin(); - Must be called before setWimdow
 ***************************************************************************************/
// Chip select stays low, call spi_begin first. Use setAddrWindow() from sketches
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE) && !defined (RPI_ILI9486_DRIVER)
		void Screen::setWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye)
		{
		#ifdef CGRAM_OFFSET
		  xs+=colstart;
		  xe+=colstart;
		  ys+=rowstart;
		  ye+=rowstart;
		#endif

		  // Column addr set
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_CASET;
		  SPI1CMD |= SPIBUSY;

		  addr_col = 0xFFFF;
		  addr_row = 0xFFFF;

		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  SPI1U1 = (31 << SPILMOSI) | (31 << SPILMISO);
		  // Load the two coords as a 32 bit value and shift in one go
		  SPI1W0 = (xs >> 8) | (uint16_t)(xs << 8) | ((uint8_t)(xe >> 8)<<16 | (xe << 24));
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  // Row addr set
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_PASET;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  SPI1U1 = (31 << SPILMOSI) | (31 << SPILMISO);
		  // Load the two coords as a 32 bit value and shift in one go
		  SPI1W0 = (ys >> 8) | (uint16_t)(ys << 8) | ((uint8_t)(ye >> 8)<<16 | (ye << 24));
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  // write to RAM
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
		  SPI1W0 = TFT_RAMWR;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  SPI1U1 = (15 << SPILMOSI) | (15 << SPILMISO);
		}

		#elif defined (ESP8266) && !defined (RPI_WRITE_STROBE) && defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits

		void Screen::setWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye)
		{

		  addr_col = 0xFFFF;
		  addr_row = 0xFFFF;

		  // Column addr set
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_CASET<<8;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  uint8_t xb[] = { 0, (uint8_t) (xs>>8), 0, (uint8_t) (xs>>0), 0, (uint8_t) (xe>>8), 0, (uint8_t) (xe>>0), };
		  spi.writePattern(&xb[0], 8, 1);

		  // Row addr set
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_PASET<<8;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  uint8_t yb[] = { 0, (uint8_t) (ys>>8), 0, (uint8_t) (ys>>0), 0, (uint8_t) (ye>>8), 0, (uint8_t) (ye>>0), };
		  spi.writePattern(&yb[0], 8, 1);

		  // write to RAM
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
		  SPI1W0 = TFT_RAMWR<<8;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  // Re-instate SPI flags settings corrupted by SPI library writePattern() call
		  SPI1U = SPI1U_WRITE;

		}

		#else

#if defined (ESP8266) && defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits
		void Screen::setWindow(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1)
		{

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  // Column addr set
		  DC_C;

		  SPI1W0 = TFT_CASET<<(CMD_BITS + 1 - 8);
		  SPI1CMD |= SPIBUSY;
		  addr_col = 0xFFFF; // Use the waiting time to do something useful
		  addr_row = 0xFFFF;
		  while(SPI1CMD & SPIBUSY) {}
		  DC_D;

		  SPI1W0 = x0 >> 0;
		  SPI1CMD |= SPIBUSY;
		  x0 = x0 << 8; // Use the waiting time to do something useful
		  while(SPI1CMD & SPIBUSY) {}

		  SPI1W0 = x0;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  SPI1W0 = x1 >> 0;
		  SPI1CMD |= SPIBUSY;
		  x1 = x1 << 8; // Use the waiting time to do something useful
		  while(SPI1CMD & SPIBUSY) {}

		  SPI1W0 = x1;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  // Row addr set
		  DC_C;

		  SPI1W0 = TFT_PASET<<(CMD_BITS + 1 - 8);
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}
		  DC_D;

		  SPI1W0 = y0 >> 0;
		  SPI1CMD |= SPIBUSY;
		  y0 = y0 << 8; // Use the waiting time to do something useful
		  while(SPI1CMD & SPIBUSY) {}

		  SPI1W0 = y0;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  SPI1W0 = y1 >> 0;
		  SPI1CMD |= SPIBUSY;
		  y1 = y1 << 8; // Use the waiting time to do something useful
		  while(SPI1CMD & SPIBUSY) {}

		  SPI1W0 = y1;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  // write to RAM
		  DC_C;

		  SPI1W0 = TFT_RAMWR<<(CMD_BITS + 1 - 8);
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		}

		#else // This is for the ESP32

void Screen::setWindow(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1,
		T_DispCoords y1) {

	addr_col = 0xFFFF;
	addr_row = 0xFFFF;

#ifdef CGRAM_OFFSET
	x0 += colstart;
	x1 += colstart;
	y0 += rowstart;
	y1 += rowstart;
#endif

	DC_C;

	tft_Write_8(TFT_CASET);

	DC_D;

#if defined (RPI_ILI9486_DRIVER)
		  uint8_t xb[] = { 0, (uint8_t) (x0>>8), 0, (uint8_t) (x0>>0), 0, (uint8_t) (x1>>8), 0, (uint8_t) (x1>>0), };
		  spi.writePattern(&xb[0], 8, 1);
		#else
	tft_Write_32(SPI_32(x0, x1));
#endif

	DC_C;

	// Row addr set
	tft_Write_8(TFT_PASET);

	DC_D;

#if defined (RPI_ILI9486_DRIVER)
		  uint8_t yb[] = { 0, (uint8_t) (y0>>8), 0, (uint8_t) (y0>>0), 0, (uint8_t) (y1>>8), 0, (uint8_t) (y1>>0), };
		  spi.writePattern(&yb[0], 8, 1);
		#else
	tft_Write_32(SPI_32(y0, y1));
#endif

	DC_C;

	// write to RAM
	tft_Write_8(TFT_RAMWR);

	DC_D;

}
#endif // end RPI_ILI9486_DRIVER check
#endif // end ESP32 check

/***************************************************************************************
 ** Function name:           readAddrWindow
 ** Description:             define an area to read a stream of pixels
 ***************************************************************************************/
// Chip select stays low
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
		void Screen::readAddrWindow(int32_t xs, int32_t ys, T_DispCoords w, T_DispCoords h)
		{

		  int32_t xe = xs + w - 1;
		  int32_t ye = ys + h - 1;

		  addr_col = 0xFFFF;
		  addr_row = 0xFFFF;

		#ifdef CGRAM_OFFSET
		  xs += colstart;
		  xe += colstart;
		  ys += rowstart;
		  ye += rowstart;
		#endif

		  // Column addr set
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_CASET;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  SPI1U1 = (31 << SPILMOSI) | (31 << SPILMISO);
		  // Load the two coords as a 32 bit value and shift in one go
		  SPI1W0 = (xs >> 8) | (uint16_t)(xs << 8) | ((uint8_t)(xe >> 8)<<16 | (xe << 24));
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  // Row addr set
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_PASET;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  SPI1U1 = (31 << SPILMOSI) | (31 << SPILMISO);
		  // Load the two coords as a 32 bit value and shift in one go
		  SPI1W0 = (ys >> 8) | (uint16_t)(ys << 8) | ((uint8_t)(ye >> 8)<<16 | (ye << 24));
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  // read from RAM
		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
		  SPI1W0 = TFT_RAMRD;
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		}

		#else //ESP32

void Screen::readAddrWindow(int32_t xs, int32_t ys, T_DispCoords w,
		T_DispCoords h) {

	int32_t xe = xs + w - 1;
	int32_t ye = ys + h - 1;

	addr_col = 0xFFFF;
	addr_row = 0xFFFF;

#ifdef CGRAM_OFFSET
	xs += colstart;
	xe += colstart;
	ys += rowstart;
	ye += rowstart;
#endif

	// Column addr set
	DC_C;

	tft_Write_8(TFT_CASET);

	DC_D;

	tft_Write_32(SPI_32(xs, xe));

	// Row addr set
	DC_C;

	tft_Write_8(TFT_PASET);

	DC_D;

	tft_Write_32(SPI_32(ys, ye));

	DC_C;

	tft_Write_8(TFT_RAMRD); // Read CGRAM command

	DC_D;

}

#endif

/***************************************************************************************
 ** Function name:           drawPixel
 ** Description:             push a single pixel at an arbitrary position
 ***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
		void Screen::drawPixel(T_DispCoords x, T_DispCoords y, uint32_t color)
		{
		  // Range checking
		  if ((x < 0) || (y < 0) ||(x >= _width) || (y >= _height)) return;

		#ifdef CGRAM_OFFSET
		  x+=colstart;
		  y+=rowstart;
		#endif

		if(SPIStartWrite()){

		  // No need to send x if it has not changed (speeds things up)
		  if (addr_col != x) {

			DC_C;

			SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
			SPI1W0 = TFT_CASET<<(CMD_BITS + 1 - 8);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			DC_D;

		#if defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits per byte
			uint8_t cBin[] = { 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0)};
			spi.writePattern(&cBin[0], 4, 2);
		#else
			SPI1U1 = (31 << SPILMOSI) | (31 << SPILMISO);
			// Load the two coords as a 32 bit value and shift in one go
			uint32_t xswap = (x >> 8) | (uint16_t)(x << 8);
			SPI1W0 = xswap | (xswap << 16);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
		#endif

			addr_col = x;
		  }

		  // No need to send y if it has not changed (speeds things up)
		  if (addr_row != y) {

			DC_C;

			SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

			SPI1W0 = TFT_PASET<<(CMD_BITS + 1 - 8);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			DC_D;

		#if defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits per byte
			uint8_t cBin[] = { 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0)};
			spi.writePattern(&cBin[0], 4, 2);
		#else
			SPI1U1 = (31 << SPILMOSI) | (31 << SPILMISO);
			// Load the two coords as a 32 bit value and shift in one go
			uint32_t yswap = (y >> 8) | (uint16_t)(y << 8);
			SPI1W0 = yswap | (yswap << 16);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
		#endif

			addr_row = y;
		  }

		  DC_C;

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

		  SPI1W0 = TFT_RAMWR<<(CMD_BITS + 1 - 8);
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		#if  defined (ILI9488_DRIVER)
		  tft_Write_16(color);
		#else
		  SPI1U1 = (15 << SPILMOSI) | (15 << SPILMISO);

		  SPI1W0 = (color >> 8) | (color << 8);
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}
		#endif

		  SPIEndWrite();
			}else{
				PrintError("[]Error: SPI is buisy!");
			}
		}

		#else

#if defined (ESP8266) && defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits

		void Screen::drawPixel(T_DispCoords x, T_DispCoords y, uint32_t color)
		{
		  // Range checking
		  if ((x < 0) || (y < 0) ||(x >= _width) || (y >= _height)) return;

		  if(SPIStartWrite()){

		  SPI1U1 = (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
		  // No need to send x if it has not changed (speeds things up)
		  if (addr_col != x) {
			DC_C;

			SPI1W0 = TFT_CASET<<(CMD_BITS + 1 - 8);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
			DC_D;

			SPI1W0 = x >> 0;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			SPI1W0 = x << 8;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			SPI1W0 = x >> 0;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			SPI1W0 = x << 8;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			addr_col = x;
		  }

		  // No need to send y if it has not changed (speeds things up)
		  if (addr_row != y) {
			DC_C;

			SPI1W0 = TFT_PASET<<(CMD_BITS + 1 - 8);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
			DC_D;

			SPI1W0 = y >> 0;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			SPI1W0 = y << 8;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			SPI1W0 = y >> 0;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			SPI1W0 = y << 8;
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}

			addr_row = y;
		  }

		  DC_C;

		  SPI1W0 = TFT_RAMWR<<(CMD_BITS + 1 - 8);
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  DC_D;

		  SPI1W0 = (color >> 8) | (color << 8);
		  SPI1CMD |= SPIBUSY;
		  while(SPI1CMD & SPIBUSY) {}

		  SPIEndWrite();
			}else{
				PrintError("[]Error: SPI is buisy!");
			}
		}

		#else // ESP32

void Screen::drawPixel(T_DispCoords x, T_DispCoords y, uint32_t color) {
	// Range checking
	if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
		return;

//	if (SPIStartWrite()) {
	spi_begin();

#ifdef CGRAM_OFFSET
	x += colstart;
	y += rowstart;
#endif

	DC_C;

	// No need to send x if it has not changed (speeds things up)
	if (addr_col != x) {

		tft_Write_8(TFT_CASET);

		DC_D;

#if defined (RPI_ILI9486_DRIVER)
			uint8_t xb[] = { 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0), 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0), };
			spi.writePattern(&xb[0], 8, 1);
		#else
		tft_Write_32(SPI_32(x, x));
#endif

		DC_C;

		addr_col = x;
	}

	// No need to send y if it has not changed (speeds things up)
	if (addr_row != y) {

		tft_Write_8(TFT_PASET);

		DC_D;

#if defined (RPI_ILI9486_DRIVER)
			uint8_t yb[] = { 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0), 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0), };
			spi.writePattern(&yb[0], 8, 1);
		#else
		tft_Write_32(SPI_32(y, y));
#endif

		DC_C;

		addr_row = y;
	}

	tft_Write_8(TFT_RAMWR);

	DC_D;

	tft_Write_16(color);

	spi_end();
//		SPIEndWrite();
//	} else {
//		PrintError("[]Error: SPI is buisy!");
//	}

}
#endif
#endif

/***************************************************************************************
 ** Function name:           pushColor
 ** Description:             push a single pixel
 ***************************************************************************************/
void Screen::pushColor(uint16_t color) {
	/*	if (SPIStartWrite()) {
	 tft_Write_16(color);
	 SPIEndWrite();
	 } else {
	 PrintError("[]Error: SPI is buisy!");
	 }
	 */
	spi_begin();

	tft_Write_16(color);

	spi_end();
}

/***************************************************************************************
 ** Function name:           pushColor
 ** Description:             push a single color to "len" pixels
 ***************************************************************************************/
void Screen::pushColor(uint16_t color, uint32_t len) {
	if (SPIStartWrite()) {

#ifdef RPI_WRITE_STROBE
		  uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
		  if(len) spi.writePattern(&colorBin[0], 2, 1); len--;
		  while(len--) {WR_L; WR_H;}
		#else
#if defined (ESP32_PARALLEL)
			while (len--) {tft_Write_16(color);}
		  #else
		writeBlock(color, len);
#endif
#endif

		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}

/***************************************************************************************
 ** Function name:           SPIStartWrite
 ** Description:             begin transaction with CS low, MUST later call SPIEndWrite
 ***************************************************************************************/
bool Screen::SPIStartWrite(void) {
	if (!inTransaction) {
		spi_begin();
		inTransaction = true;
		return true;
	} else {
		return false;
	}
}

/***************************************************************************************
 ** Function name:           SPIEndWrite
 ** Description:             end transaction with CS high
 ***************************************************************************************/
void Screen::SPIEndWrite(void) {
	if (inTransaction) {
		inTransaction = false;
		spi_end();
	}
}

/***************************************************************************************
 ** Function name:           SPIStartRead
 ** Description:             begin transaction with CS low, MUST later call SPIEndRead
 ** 							(set spi_speed to read)
 ***************************************************************************************/
bool Screen::SPIStartRead(void) {
	if (!inTransaction) {
		spi_begin_read();
		inTransaction = true;
		return true;
	} else {
		return false;
	}
}

/***************************************************************************************
 ** Function name:           SPIEndRead
 ** Description:             end transaction with CS high
 ** 							(revert spi_speed to write)
 ***************************************************************************************/
void Screen::SPIEndRead(void) {
	if (inTransaction) {
		inTransaction = false;
		spi_end_read();
	}
}

/***************************************************************************************
 ** Function name:           writeColor (use SPIStartWrite() and SPIEndWrite() before & after)
 ** Description:             raw write of "len" pixels avoiding transaction check
 ***************************************************************************************/
void Screen::writeColor(uint16_t color, uint32_t len) {
#ifdef RPI_WRITE_STROBE
		  uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
		  if(len) spi.writePattern(&colorBin[0], 2, 1); len--;
		  while(len--) {WR_L; WR_H;}
		#else
#if defined (ESP32_PARALLEL)
			while (len--) {tft_Write_16(color);}
		  #else
	writeBlock(color, len);
#endif
#endif
}

/***************************************************************************************
 ** Function name:           pushColors
 ** Description:             push an array of pixels for 16 bit raw image drawing
 ***************************************************************************************/
// Assumed that setAddrWindow() has previously been called
void Screen::pushColors(uint8_t *data, uint32_t len) {
	if (SPIStartWrite()) {

#if defined (RPI_WRITE_STROBE)
		  while ( len >=64 ) {spi.writePattern(data, 64, 1); data += 64; len -= 64; }
		  if (len) spi.writePattern(data, len, 1);
		#else
#ifdef ESP32_PARALLEL
			while (len--) {tft_Write_8(*data); data++;}
		  #elif  defined (ILI9488_DRIVER)
			uint16_t color;
			while (len>1) {color = (*data++); color |= ((*data++)<<8); tft_Write_16(color); len-=2;}
		  #else
#if (SPI_FREQUENCY == 80000000)
			  while ( len >=64 ) {spi.writePattern(data, 64, 1); data += 64; len -= 64; }
			  if (len) spi.writePattern(data, len, 1);
			#else
		spi.writeBytes(data, len);
#endif
#endif
#endif

		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}

/***************************************************************************************
 ** Function name:           pushColors
 ** Description:             push an array of pixels, for image drawing
 ***************************************************************************************/
void Screen::pushColors(uint16_t *data, uint32_t len, bool swap) {
	//spi_begin();//if (SPIStartWrite()) {

#if defined (ESP32) || defined (ILI9488_DRIVER)
#if defined (ESP32_PARALLEL) || defined (ILI9488_DRIVER)
			if (swap) while ( len-- ) {tft_Write_16(*data); data++;}
			else while ( len-- ) {tft_Write_16S(*data); data++;}
		  #else
		if (swap)
			spi.writePixels(data, len << 1);
		else
			spi.writeBytes((uint8_t*) data, len << 1);
#endif
#else

		  uint32_t color[8];

		  SPI1U1 = (255 << SPILMOSI) | (255 << SPILMISO);


		  while(len>15)
		  {

			if (swap)
			{
			  uint32_t i = 0;
			  while(i<8)
			  {
				color[i]  = (*data >> 8) | (uint16_t)(*data << 8);
				data++;
				color[i] |= ((*data >> 8) | (*data << 8)) << 16;
				data++;
				i++;
			  }
			}
			else
			{
			  memcpy(color,data,32);
			  data+=16;
			}

			len -= 16;

			// ESP8266 wait time here at 40MHz SPI is ~5.45us
			while(SPI1CMD & SPIBUSY) {}
			SPI1W0 = color[0];
			SPI1W1 = color[1];
			SPI1W2 = color[2];
			SPI1W3 = color[3];
			SPI1W4 = color[4];
			SPI1W5 = color[5];
			SPI1W6 = color[6];
			SPI1W7 = color[7];
			SPI1CMD |= SPIBUSY;
		  }

		  if(len)
		  {
			uint32_t bits = (len*16-1); // bits left to shift - 1
			if (swap)
			{
			  uint16_t* ptr = (uint16_t*)color;
			  while(len--)
			  {
				*ptr++ = (*(data) >> 8) | (uint16_t)(*(data) << 8);
				data++;
			  }
			}
			else
			{
			  memcpy(color,data,len<<1);
			}
			while(SPI1CMD & SPIBUSY) {}
			SPI1U1 = (bits << SPILMOSI) | (bits << SPILMISO);
			SPI1W0 = color[0];
			SPI1W1 = color[1];
			SPI1W2 = color[2];
			SPI1W3 = color[3];
			SPI1W4 = color[4];
			SPI1W5 = color[5];
			SPI1W6 = color[6];
			SPI1W7 = color[7];
			SPI1CMD |= SPIBUSY;
		  }

		  while(SPI1CMD & SPIBUSY) {}

		#endif

		  //spi_end();
		  /*SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}
*/
}

/***************************************************************************************
 ** Function name:           drawLine
 ** Description:             draw a line between 2 arbitrary points
 ***************************************************************************************/
// Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
// an efficient FastH/V Line draw routine for line segments of 2 pixels or more
#if defined (RPI_ILI9486_DRIVER) || defined (ESP32) || defined (RPI_WRITE_STROBE) || defined (HX8357D_DRIVER) || defined (ILI9488_DRIVER)

void Screen::drawLine(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1,
		T_DispCoords y1, uint32_t color) {

	if (SPIStartWrite()) {

		boolean steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			swap_coord(x0, y0);
			swap_coord(x1, y1);
		}

		if (x0 > x1) {
			swap_coord(x0, x1);
			swap_coord(y0, y1);
		}

		int32_t dx = x1 - x0, dy = abs(y1 - y0);
		;

		int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

		if (y0 < y1)
			ystep = 1;

		// Split into steep and not steep for FastH/V separation
		if (steep) {
			for (; x0 <= x1; x0++) {
				dlen++;
				err -= dy;
				if (err < 0) {
					err += dx;
					if (dlen == 1)
						drawPixel(y0, xs, color);
					else
						drawFastVLine(y0, xs, dlen, color);
					dlen = 0;
					y0 += ystep;
					xs = x0 + 1;
				}
			}
			if (dlen)
				drawFastVLine(y0, xs, dlen, color);
		} else {
			for (; x0 <= x1; x0++) {
				dlen++;
				err -= dy;
				if (err < 0) {
					err += dx;
					if (dlen == 1)
						drawPixel(xs, y0, color);
					else
						drawFastHLine(xs, y0, dlen, color);
					dlen = 0;
					y0 += ystep;
					xs = x0 + 1;
				}
			}
			if (dlen)
				drawFastHLine(xs, y0, dlen, color);
		}
		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}

#else

		// This is a weeny bit faster
		void Screen::drawLine(T_DispCoords x0, T_DispCoords y0, T_DispCoords x1, T_DispCoords y1, uint32_t color)
		{

		  boolean steep = abs(y1 - y0) > abs(x1 - x0);

		  if (steep) {
			swap_coord(x0, y0);
			swap_coord(x1, y1);
		  }

		  if (x0 > x1) {
			swap_coord(x0, x1);
			swap_coord(y0, y1);
		  }

		  if (x1 < 0) return;

		  int16_t dx, dy;
		  dx = x1 - x0;
		  dy = abs(y1 - y0);

		  int16_t err = dx / 2;
		  int8_t ystep = (y0 < y1) ? 1 : (-1);

		  if(SPIStartWrite()){

		  int16_t swapped_color = (color >> 8) | (color << 8);

		  if (steep)  // y increments every iteration (y0 is x-axis, and x0 is y-axis)
		  {
			if (x1 >= (int32_t)_height) x1 = _height - 1;

			for (; x0 <= x1; x0++) {
			  if ((x0 >= 0) && (y0 >= 0) && (y0 < _width)) break;
			  err -= dy;
			  if (err < 0) {
				err += dx;
				y0 += ystep;
			  }
			}

			if (x0 > x1) {
				SPIEndWrite();
				return;}
			}

			setWindow(y0, x0, y0, _height);
			SPI1W0 = swapped_color;
			for (; x0 <= x1; x0++) {
			  while(SPI1CMD & SPIBUSY) {}
			  SPI1CMD |= SPIBUSY;

			  err -= dy;
			  if (err < 0) {
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _width)) break;
				err += dx;
				while(SPI1CMD & SPIBUSY) {}
				setWindow(y0, x0+1, y0, _height);
				SPI1W0 = swapped_color;
			  }
			}
		  }
		  else    // x increments every iteration (x0 is x-axis, and y0 is y-axis)
		  {
			if (x1 >= _width) x1 = _width - 1;

			for (; x0 <= x1; x0++) {
			  if ((x0 >= 0) && (y0 >= 0) && (y0 < (int32_t)_height)) break;
			  err -= dy;
			  if (err < 0) {
				  err += dx;
				  y0 += ystep;
			  }
			}

			if (x0 > x1) {
				SPIEndWrite();
				return;}

			setWindow(x0, y0, _width, y0);
			SPI1W0 = swapped_color;
			for (; x0 <= x1; x0++) {
			  while(SPI1CMD & SPIBUSY) {}
			  SPI1CMD |= SPIBUSY;

			  err -= dy;
			  if (err < 0) {
				y0 += ystep;
				if ((y0 < 0) || (y0 >= (int32_t)_height)) break;
				err += dx;
				while(SPI1CMD & SPIBUSY) {}
				setWindow(x0+1, y0, _width, y0);
				SPI1W0 = swapped_color;
			  }
			}
		  }

		  while(SPI1CMD & SPIBUSY) {}

		  SPIEndWrite();
			}else{
				PrintError("[]Error: SPI is buisy!");
			}

		}

		#endif

/***************************************************************************************
 ** Function name:           drawFastVLine
 ** Description:             draw a vertical line
 ***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
		void Screen::drawFastVLine(T_DispCoords x, T_DispCoords y, T_DispCoords h, uint32_t color)
		{
		  // Clipping
		  if ((x < 0) || (x >= _width) || (y >= _height)) return;

		  if (y < 0) { h += y; y = 0; }

		  if ((y + h) > _height) h = _height - y;

		  if (h < 1) return;

		  if(SPIStartWrite()){

		  setWindow(x, y, x, y + h - 1);

		  writeBlock(color, h);

		  SPIEndWrite();
			}else{
				PrintError("[]Error: SPI is buisy!");
			}

		}

		#else

void Screen::drawFastVLine(T_DispCoords x, T_DispCoords y, T_DispCoords h,
		uint32_t color) {
	// Clipping
	if ((x < 0) || (x >= _width) || (y >= _height))
		return;

	if (y < 0) {
		h += y;
		y = 0;
	}

	if ((y + h) > _height)
		h = _height - y;

	if (h < 1)
		return;

	if (SPIStartWrite()) {

		setWindow(x, y, x, y + h - 1);

#ifdef RPI_WRITE_STROBE
		  #if defined (ESP8266)
			SPI1W0 = (color >> 8) | (color << 8);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
		  #else
			tft_Write_16(color);
		  #endif
			h--;
			while(h--) {WR_L; WR_H;}
		#else
#ifdef ESP32_PARALLEL
			while (h--) {tft_Write_16(color);}
		  #else
		writeBlock(color, h);
#endif
#endif

		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}
#endif

/***************************************************************************************
 ** Function name:           drawFastHLine
 ** Description:             draw a horizontal line
 ***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
		void Screen::drawFastHLine(T_DispCoords x, T_DispCoords y, T_DispCoords w, uint32_t color)
		{
		  // Clipping
		  if ((y < 0) || (x >= _width) || (y >= _height)) return;

		  if (x < 0) { w += x; x = 0; }

		  if ((x + w) > _width)  w = _width  - x;

		  if (w < 1) return;

		  if(SPIStartWrite()){
			  setWindow(x, y, x + w - 1, y);
			  writeBlock(color, w);
			  SPIEndWrite();
			}else{
				PrintError("[]Error: SPI is buisy!");
			}

		}

		#else

void Screen::drawFastHLine(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		uint32_t color) {
	// Clipping
	if ((y < 0) || (x >= _width) || (y >= _height))
		return;
	if (x < 0) {
		w += x;
		x = 0;
	}
	if ((x + w) > _width)
		w = _width - x;
	if (w < 1)
		return;

	if (SPIStartWrite()) {

		setWindow(x, y, x + w - 1, y);

#ifdef RPI_WRITE_STROBE
				#if defined (ESP8266)
			SPI1W0 = (color >> 8) | (color << 8);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
				#else
			tft_Write_16(color);
				#endif
			w--;
			while(w--) {WR_L; WR_H;}
			#else
#ifdef ESP32_PARALLEL
			while (w--) {tft_Write_16(color);}
				#else
		writeBlock(color, w);
#endif
#endif

		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}
#endif

/***************************************************************************************
 ** Function name:           fillRect
 ** Description:             draw a filled rectangle
 ***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
		void Screen::fillRect(T_DispCoords x, T_DispCoords y, T_DispCoords w, T_DispCoords h, uint32_t color)
		{
		  // Clipping
		  if ((x >= _width) || (y >= _height)) return;

		  if (x < 0) { w += x; x = 0; }
		  if (y < 0) { h += y; y = 0; }

		  if ((x + w) > _width)  w = _width  - x;
		  if ((y + h) > _height) h = _height - y;

		  if ((w < 1) || (h < 1)) return;

		  if(SPIStartWrite()){

		  setWindow(x, y, x + w - 1, y + h - 1);

		  writeBlock(color, w * h);

		  SPIEndWrite();
			}else{
				PrintError("[]Error: SPI is buisy!");
			}

		}

		#else

void Screen::fillRect(T_DispCoords x, T_DispCoords y, T_DispCoords w,
		T_DispCoords h, uint32_t color) {

	// Clipping
	if ((x >= _width) || (y >= _height))
		return;

	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}

	if ((x + w) > _width)
		w = _width - x;
	if ((y + h) > _height)
		h = _height - y;

	if ((w < 1) || (h < 1))
		return;

	if (SPIStartWrite()) {

		setWindow(x, y, x + w - 1, y + h - 1);

		uint32_t n = (uint32_t) w * (uint32_t) h;

#ifdef RPI_WRITE_STROBE
		  tft_Write_16(color);
		  while(n--) {WR_L; WR_H;}
		#else
#ifdef ESP32_PARALLEL
			if (color>>8 == (uint8_t)color)
			{
			  tft_Write_8(color);
			  n--; WR_L; WR_H;
			  while (n) {WR_L; WR_H; n--; WR_L; WR_H;}
			}
			else
			{
			  while (n--) {tft_Write_16(color);}
			}
		  #else
		writeBlock(color, n);
#endif
#endif

		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}
#endif

/***************************************************************************************
 ** Function name:           invertDisplay
 ** Description:             invert the display colors i = 1 invert, i = 0 normal
 ***************************************************************************************/
void Screen::invertDisplay(boolean i) {
	if (SPIStartWrite()) {
		// Send the command twice as otherwise it does not always work!
		writecommand(i ? TFT_INVON : TFT_INVOFF);
		writecommand(i ? TFT_INVON : TFT_INVOFF);
		SPIEndWrite();
	} else {
		PrintError("[]Error: SPI is buisy!");
	}

}

/***************************************************************************************
 ** Function name:           writeBlock
 ** Description:             Write a block of pixels of the same color
 ***************************************************************************************/
//Clear screen test 76.8ms theoretical. 81.5ms Screen, 967ms Adafruit_ILI9341
//Performance 26.15Mbps@26.66MHz, 39.04Mbps@40MHz, 75.4Mbps@80MHz SPI clock
//Efficiency:
//       Screen       98.06%              97.59%          94.24%
//       Adafruit_GFX   19.62%              14.31%           7.94%
//
#if defined (ESP8266) && !defined (ILI9488_DRIVER)
		void Screen::writeBlock(uint16_t color, uint32_t repeat)
		{
		  uint16_t color16 = (color >> 8) | (color << 8);
		  uint32_t color32 = color16 | color16 << 16;

		  SPI1W0 = color32;
		  SPI1W1 = color32;
		  SPI1W2 = color32;
		  SPI1W3 = color32;
		  if (repeat > 8)
		  {
			SPI1W4 = color32;
			SPI1W5 = color32;
			SPI1W6 = color32;
			SPI1W7 = color32;
		  }
		  if (repeat > 16)
		  {
			SPI1W8 = color32;
			SPI1W9 = color32;
			SPI1W10 = color32;
			SPI1W11 = color32;
		  }
		  if (repeat > 24)
		  {
			SPI1W12 = color32;
			SPI1W13 = color32;
			SPI1W14 = color32;
			SPI1W15 = color32;
		  }
		  if (repeat > 31)
		  {
			SPI1U1 = (511 << SPILMOSI);
			while(repeat>31)
			{
		#if defined SPI_FREQUENCY && (SPI_FREQUENCY == 80000000)
			  if(SPI1CMD & SPIBUSY) // added to sync with flag change
		#endif
			  while(SPI1CMD & SPIBUSY) {}
			  SPI1CMD |= SPIBUSY;
			  repeat -= 32;
			}
			while(SPI1CMD & SPIBUSY) {}
		  }

		  if (repeat)
		  {
			repeat = (repeat << 4) - 1;
			SPI1U1 = (repeat << SPILMOSI);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
		  }

		}

		#elif defined (ILI9488_DRIVER)

		#ifdef ESP8266
		void writeBlock(uint16_t color, uint32_t repeat)
		{

		  // Split out the colors
		  uint8_t r = (color & 0xF800)>>8;
		  uint8_t g = (color & 0x07E0)>>3;
		  uint8_t b = (color & 0x001F)<<3;
		  // Concatenate 4 pixels into three 32 bit blocks
		  uint32_t r0 = r<<24 | b<<16 | g<<8 | r;
		  uint32_t r1 = g<<24 | r<<16 | b<<8 | g;
		  uint32_t r2 = b<<24 | g<<16 | r<<8 | b;

		  SPI1W0 = r0;
		  SPI1W1 = r1;
		  SPI1W2 = r2;

		  if (repeat > 4)
		  {
			SPI1W3 = r0;
			SPI1W4 = r1;
			SPI1W5 = r2;
		  }
		  if (repeat > 8)
		  {
			SPI1W6 = r0;
			SPI1W7 = r1;
			SPI1W8 = r2;
		  }
		  if (repeat > 12)
		  {
			SPI1W9  = r0;
			SPI1W10 = r1;
			SPI1W11 = r2;
			SPI1W12 = r0;
			SPI1W13 = r1;
			SPI1W14 = r2;
			SPI1W15 = r0;
		  }

		  if (repeat > 20)
		  {
			SPI1U1 = (503 << SPILMOSI);
			while(repeat>20)
			{
			  while(SPI1CMD & SPIBUSY) {}
			  SPI1CMD |= SPIBUSY;
			  repeat -= 21;
			}
			while(SPI1CMD & SPIBUSY) {}
		  }

		  if (repeat)
		  {
			repeat = (repeat * 24) - 1;
			SPI1U1 = (repeat << SPILMOSI);
			SPI1CMD |= SPIBUSY;
			while(SPI1CMD & SPIBUSY) {}
		  }

		}
		#else // Now the code for ESP32 and ILI9488

		void writeBlock(uint16_t color, uint32_t repeat)
		{
		  // Split out the colors
		  uint32_t r = (color & 0xF800)>>8;
		  uint32_t g = (color & 0x07E0)<<5;
		  uint32_t b = (color & 0x001F)<<19;
		  // Concatenate 4 pixels into three 32 bit blocks
		  uint32_t r0 = r<<24 | b | g | r;
		  uint32_t r1 = r0>>8 | g<<16;
		  uint32_t r2 = r1>>8 | b<<8;

		  if (repeat > 19)
		  {
			SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(SPI_PORT), SPI_USR_MOSI_DBITLEN, 479, SPI_USR_MOSI_DBITLEN_S);

			while(repeat>19)
			{
			  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);
			  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W1_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W2_REG(SPI_PORT), r2);
			  WRITE_PERI_REG(SPI_W3_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W4_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W5_REG(SPI_PORT), r2);
			  WRITE_PERI_REG(SPI_W6_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W7_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W8_REG(SPI_PORT), r2);
			  WRITE_PERI_REG(SPI_W9_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W10_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W11_REG(SPI_PORT), r2);
			  WRITE_PERI_REG(SPI_W12_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W13_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W14_REG(SPI_PORT), r2);
			  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR);
			  repeat -= 20;
			}
			while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);
		  }

		  if (repeat)
		  {
			SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(SPI_PORT), SPI_USR_MOSI_DBITLEN, (repeat * 24) - 1, SPI_USR_MOSI_DBITLEN_S);
			WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), r0);
			WRITE_PERI_REG(SPI_W1_REG(SPI_PORT), r1);
			WRITE_PERI_REG(SPI_W2_REG(SPI_PORT), r2);
			WRITE_PERI_REG(SPI_W3_REG(SPI_PORT), r0);
			WRITE_PERI_REG(SPI_W4_REG(SPI_PORT), r1);
			WRITE_PERI_REG(SPI_W5_REG(SPI_PORT), r2);
			if (repeat > 8 )
			{
			  WRITE_PERI_REG(SPI_W6_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W7_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W8_REG(SPI_PORT), r2);
			  WRITE_PERI_REG(SPI_W9_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W10_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W11_REG(SPI_PORT), r2);
			  WRITE_PERI_REG(SPI_W12_REG(SPI_PORT), r0);
			  WRITE_PERI_REG(SPI_W13_REG(SPI_PORT), r1);
			  WRITE_PERI_REG(SPI_W14_REG(SPI_PORT), r2);
			}

			SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR);
			while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);
		  }

		}
		#endif

		#else // Low level register based ESP32 code for 16 bit color SPI TFTs

void Screen::writeBlock(uint16_t color, uint32_t repeat) {
	uint32_t color32 = COL_32(color, color);

	if (repeat > 31) // Revert legacy toggle buffer change
			{
		WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), 511);
		while (repeat > 31) {
			while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT)) & SPI_USR)
				;
			WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W1_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W2_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W3_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W4_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W5_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W6_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W7_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W8_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W9_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W10_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W11_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W12_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W13_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W14_REG(SPI_PORT), color32);
			WRITE_PERI_REG(SPI_W15_REG(SPI_PORT), color32);
			SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR);
			repeat -= 32;
		}
		while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT)) & SPI_USR)
			;
	}

	if (repeat) {
		// Revert toggle buffer change
		WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), (repeat << 4) - 1);
		for (uint32_t i = 0; i <= (repeat >> 1); i++)
			WRITE_PERI_REG((SPI_W0_REG(SPI_PORT) + (i << 2)), color32);
		SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR);
		while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT)) & SPI_USR)
			;
	}
}
#endif

/***************************************************************************************
 ** Function name:           getSPIinstance
 ** Description:             Get the instance of the SPI class (for ESP32 only)
 ***************************************************************************************/
#ifndef ESP32_PARALLEL
SPIClass& Screen::getSPIinstance(void) {
	return spi;
}
#endif

/***************************************************************************************
 ** Function name:           getSetup
 ** Description:             Get the setup details for diagnostic and sketch access
 ***************************************************************************************/
void Screen::getSetup(T_DisplaySettings &tft_settings) {
	// tft_settings.version is set in header file

#if defined (ESP8266)
		  tft_settings.esp = 8266;
		#elif defined (ESP32)
	tft_settings.esp = 32;
#else
		  tft_settings.esp = -1;
		#endif

#if defined (SUPPORT_TRANSACTIONS)
	tft_settings.trans = true;
#else
		  tft_settings.trans = false;
		#endif

#if defined (ESP32_PARALLEL)
		  tft_settings.serial = false;
		  tft_settings.spi_freq = 0;
		#else
	tft_settings.serial = true;
	tft_settings.spi_freq = SPI_FREQUENCY / 100000;
#ifdef SPI_READ_FREQUENCY
	tft_settings.rd_freq = SPI_READ_FREQUENCY / 100000;
#endif
#endif

#if defined(TFT_SPI_OVERLAP)
		  tft_settings.overlap = true;
		#else
	tft_settings.overlap = false;
#endif

	tft_settings.driver = TFT_DRIVER;
	tft_settings.width = _init_width;
	tft_settings.height = _init_height;

#ifdef CGRAM_OFFSET
	tft_settings.r0_x_offset = colstart;
	tft_settings.r0_y_offset = rowstart;
	tft_settings.r1_x_offset = 0;
	tft_settings.r1_y_offset = 0;
	tft_settings.r2_x_offset = 0;
	tft_settings.r2_y_offset = 0;
	tft_settings.r3_x_offset = 0;
	tft_settings.r3_y_offset = 0;
#else
		  tft_settings.r0_x_offset = 0;
		  tft_settings.r0_y_offset = 0;
		  tft_settings.r1_x_offset = 0;
		  tft_settings.r1_y_offset = 0;
		  tft_settings.r2_x_offset = 0;
		  tft_settings.r2_y_offset = 0;
		  tft_settings.r3_x_offset = 0;
		  tft_settings.r3_y_offset = 0;
		#endif

#if defined (TFT_MOSI)
	tft_settings.pin_mosi = TFT_MOSI;
#else
		  tft_settings.pin_mosi = -1;
		#endif

#if defined (TFT_MISO)
	tft_settings.pin_miso = TFT_MISO;
#else
		  tft_settings.pin_miso = -1;
		#endif

#if defined (TFT_SCLK)
	tft_settings.pin_clk = TFT_SCLK;
#else
		  tft_settings.pin_clk  = -1;
		#endif

#if defined (TFT_CS)
	tft_settings.pin_cs = TFT_CS;
#else
		  tft_settings.pin_cs   = -1;
		#endif

#if defined (TFT_DC)
	tft_settings.pin_dc = TFT_DC;
#else
		  tft_settings.pin_dc  = -1;
		#endif

#if defined (TFT_RD)
		  tft_settings.pin_rd  = TFT_RD;
		#else
	tft_settings.pin_rd = -1;
#endif

#if defined (TFT_WR)
		  tft_settings.pin_wr  = TFT_WR;
		#else
	tft_settings.pin_wr = -1;
#endif

#if defined (TFT_RST)
	tft_settings.pin_rst = TFT_RST;
#else
		  tft_settings.pin_rst = -1;
		#endif

#if defined (ESP32_PARALLEL)
		  tft_settings.pin_d0 = TFT_D0;
		  tft_settings.pin_d1 = TFT_D1;
		  tft_settings.pin_d2 = TFT_D2;
		  tft_settings.pin_d3 = TFT_D3;
		  tft_settings.pin_d4 = TFT_D4;
		  tft_settings.pin_d5 = TFT_D5;
		  tft_settings.pin_d6 = TFT_D6;
		  tft_settings.pin_d7 = TFT_D7;
		#else
	tft_settings.pin_d0 = -1;
	tft_settings.pin_d1 = -1;
	tft_settings.pin_d2 = -1;
	tft_settings.pin_d3 = -1;
	tft_settings.pin_d4 = -1;
	tft_settings.pin_d5 = -1;
	tft_settings.pin_d6 = -1;
	tft_settings.pin_d7 = -1;
#endif

#if defined (TOUCH_CS)
		  tft_settings.pin_tch_cs   = TOUCH_CS;
		  tft_settings.tch_spi_freq = SPI_TOUCH_FREQUENCY/100000;
		#else
	tft_settings.pin_tch_cs = -1;
	tft_settings.tch_spi_freq = 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////

/*
 c_Cursor* Screen::Cursor(){
 return cursor; //returns cursor
 }
 void Screen::Cursor(c_Cursor& _cursor){
 ;
 }
 */

}
}
