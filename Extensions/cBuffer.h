/*
 * cBuffer.h
 *
 *  Created on: 27 окт. 2019 г.
 *      Author: Vitaliy Novoselov
 *
 *      Device::Memory::c_Buffer class
 *      Gives functionality of multidimentional buffer
 *
 */

#ifndef COMPONENTS_ARDUINO_LIBRARIES_ESP32_SPIDISPLAY_EXTENSIONS_CBUFFER_H_
#define COMPONENTS_ARDUINO_LIBRARIES_ESP32_SPIDISPLAY_EXTENSIONS_CBUFFER_H_

#include <Arduino.h>

namespace Device {
namespace Memory {
namespace {
#define CheckInit(_returnType) if (!Initialized()) return (_returnType) NULL;
#define ListDimensions(_variable,_startvalue) for (uint _variable = _startvalue; _variable < dimensions; _variable++)

}

typedef enum cBuffer_Error{
	CBUF_OK,			// All ok
	CBUF_ERR_SRC,		// Source not initialized
	CBUF_ERR_MALLOC,	// Memory didn't allocated
	CBUF_ERR_BUFFER,	// Null-pointer in 'buffer' (buffer not initialized)
	CBUF_ERR_NODIMENSIONS,	// Dimensions = 0 in initialization
	CBUF_ERR_DIMENSIONS,	// Any other errors with dimensions
	CBUF_ERR_NULLBUFFERSIZE,// Buffersize=0 (one of initialization units = 0)
	CBUF_ERR_BUFFERSIZE,	// Any other errors with Buffersize
	CBUF_ERR_OVERFLOW,	//Overflow dew copy (source buffer larger then destination
	CBUF_ERR_SIZE,		// Error with sizes of any dimensions
	CBUF_ERR_UNITSIZE,	// unitSize is NULL

	CBUF_ERR_OTHER		// Any other error
};

class c_Buffer {
public:
	c_Buffer();	// initialize defaults, do not initializes sizes and buffer itself
	c_Buffer(uint f_Dimensions, uint f_UnitSize, ...); // initializes buffer with numerous Dimensions, and size of unit in bytes. After f_UnitSize all dimensions' sizes mast be specified
	virtual ~c_Buffer();
	c_Buffer(const c_Buffer &other);
	c_Buffer(c_Buffer &&other);
	//c_Buffer& operator=(const c_Buffer &other);
	//c_Buffer& operator=(c_Buffer &&other);
	c_Buffer& operator=(const c_Buffer &other(...));

	void init(uint f_Dimensions, uint f_UnitSize, ...);
	void remove();

	bool Initialized();

	bool set(void* value, ...); // Store value into specified position(...)
	void * get(...);			// Returns value at specified position

	cBuffer_Error Status();
	uint getDimensions();
	uint getUnitSize();
	uint* copySize();
	uint getSize(uint dimension);
	bool copyBuffer(c_Buffer *srcBuffer, uint *srcPosition, uint *srcSize, uint *dstPosition); // copy source buffer or it's part to specified position in current buffer

	void* copyBuffer(void *needPosition, void *needSize); // copy part of buffer
	void* copyBuffer();	// Return copy of current buffer

private:
	cBuffer_Error status;

	uint dimensions;
	uint unitSize;
	void *buffer;
	uint *size; //for Graphics use st_Size2D or st_Size3D;
};

} /* namespace Memory */
} /* namespace Device */

#endif /* COMPONENTS_ARDUINO_LIBRARIES_ESP32_SPIDISPLAY_EXTENSIONS_CBUFFER_H_ */
