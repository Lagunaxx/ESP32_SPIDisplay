/*
 * SPIDevice.h
 *
 *  Created on: 17 окт. 2019 г.
 *      Author: producer
 */

#ifndef COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_SPIDEVICE_H_
#define COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_SPIDEVICE_H_

#include <Arduino.h>
#include <SPI.h>
#include "Definations.h"


namespace Device {
	namespace SPI {



		class SPIDevice {
		public:
			SPIDevice();
			virtual ~SPIDevice();

		};

	} /* namespace SPI */
} /* namespace Device */

#endif /* COMPONENTS_ARDUINO_LIBRARIES_TFT_ESPI_EXTENSIONS_SPIDEVICE_H_ */
