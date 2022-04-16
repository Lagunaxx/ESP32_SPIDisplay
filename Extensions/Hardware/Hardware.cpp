/*
 * Hardware.cpp
 *
 *  Created on: 13 апр. 2022 г.
 *      Author: producer
 */

#include "Hardware.h"

namespace Device {
namespace Hardware {

Hardware* Bus;

void Handle(){
	delay(1);
	Bus->run();
}
/***************************************************************************************
** Function name:           init
** Description:             initialize Hardware module
** Usage:
** 		Device::Display::Graphics::init();		// initialize Bus pointer
**
** 	Return value (bool):
** 		true - Bus initialized;
** 		false - Bus do not initialized
***************************************************************************************/
bool init(){
				Bus=new Hardware();
				if (Bus==0) return false;
				return true;
			};

/***************************************************************************************
** Function name:           remove
** Description:             removes Hardware bus
** Usage:
** 		Device::Hardware::remove();	// removes Bus pointer.
**
** 	Return value (bool):
** 		true - Bus removed;
** 		false - Bus was not initialized (null-pointer)
***************************************************************************************/
 bool remove(){
	if (Bus==0) return false;
	delete(Bus);
	return true;
}


Hardware::Hardware() {
	// TODO Auto-generated constructor stub
	Devices=new(t_Device[40]);
	if(Devices){
		for(uint8_t id=0; id<40; id++) {
			Devices[id].ID = id;
			Devices[id].callbackHandler = 0;
			Devices[id].deviceHandler = 0;
			Devices[id].deviceType = HW_TYPE_NOP;
			Devices[id].pin = GPIO_NOTDEFINED;
		}
	}
}

Hardware::~Hardware() {
	// TODO Auto-generated destructor stub
	if(Devices)delete(Devices);
}

/***************************************************************************************
** Function name:			add
** Description :			initializes new device (pin)
** Args:
**			arg1 - value
**
** Return:	id, or 0 if error
***************************************************************************************/
uint8_t Hardware::addDI(uint8_t pin, void callbackHandler(t_Data*)) {
	uint8_t id=0;

	if (!callbackHandler) return 0; // check for null-pointer

	while (id<40) {
		if (Devices[id].pin == pin) return 0; // if pin initialized then exit
		id++;
	}

	id = 0;
	while (true) {
		if (Devices[id].pin == GPIO_NOTDEFINED)break;
		id++;
		if(id == 40) return 0; // all pins are buizy
	}

	Devices[id].pin = pin;
	Devices[id].deviceType = HW_TYPE_DI;
	Devices[id].callbackHandler = callbackHandler;
	pinMode(pin, INPUT);

	return id;
}

/***************************************************************************************
** Function name:			run
** Description :			loop-task for handle work of Bus
***************************************************************************************/
void Hardware::run() {

	t_Data * rvalue;
	int rv;
	// Check every initiate pin

	for (uint8_t id = 0; id < 40; id++){
		if(Devices[id].pin != GPIO_NOTDEFINED) {
			// check pin type
			if(Devices[id].deviceType == HW_TYPE_DI) {
				// check input
				rvalue=new(t_Data);
				rvalue->data = malloc(sizeof(int));
				rvalue->deviceType=Devices[id].deviceType;
				rvalue->pin=Devices[id].pin;
				rv = digitalRead(Devices[id].pin);
				memcpy(rvalue->data,(const void *)&rv,sizeof(int));
				(*Devices[id].callbackHandler)( rvalue);
				free(rvalue->data);
				delete(rvalue);
			}
		}
	}


}
// Default function to work with devices
uint8_t Hardware::msg(uint8_t deviceID, uint8_t msg, void * payload){
	return MSG_SYS_NOP; // Just
}

} /* namespace Hardware */
} /* namespace Device */
