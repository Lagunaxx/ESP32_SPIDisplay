/*
 * cBuffer.cpp
 *
 *  Created on: 27 окт. 2019 г.
 *      Author: Vitaliy Novoselov
 */

#include "cBuffer.h"

namespace Device {
namespace Memory {

c_Buffer::c_Buffer() {
	dimensions = 0;
	unitSize = 0;
	size = NULL;
	buffer = NULL;
	status = CBUF_OK;
}

c_Buffer::c_Buffer(uint f_Dimensions, uint f_UnitSize, ...) {
	/*
	 * 	initializes buffer with numerous Dimensions, and size of unit in bytes.
	 * 	After f_UnitSize all dimensions' sizes mast be specified.
	 *
	 * 	Examples:
	 * 	c_Buffer newBuffer(1, sizeof(uint), 50); // Initializes one-dimensional buffer with 50 elements of uint
	 * 	c_Buffer newBuffer(3, sizeof(double), 500, 600, 700); // Initializes 3-dimensional(3D) buffer
	 * 															with 500 doubles by first dimension,
	 * 															600 doubles by second dimension
	 * 															and 700 doubles by third dimension
	 *
	 */

	va_list args;
	uint param;
	uint64_t sizeofbuffer = 1;
	status = CBUF_OK;

	if (f_Dimensions > 0) {
		unitSize = f_UnitSize;
		dimensions = f_Dimensions;
		size = (uint*) malloc(dimensions * sizeof(uint));
		if (size > 0) {
			va_start(args, f_Dimensions);

			for (int i = 0; i < dimensions; i++) {
				param = va_arg(args, uint);
				*(size + i) = param;
				sizeofbuffer *= param;
			}

			va_end(args);

			if (sizeofbuffer > 0) {
				buffer = malloc(sizeofbuffer * unitSize);
				if (buffer == NULL)
					status = CBUF_ERR_MALLOC;
			} else {
				status = CBUF_ERR_NULLBUFFERSIZE;
			}
		} else {
			status = CBUF_ERR_MALLOC;
		}
	} else {
		dimensions = 0;
		unitSize = 0;
		size = NULL;
		buffer = NULL;
		status = CBUF_ERR_NODIMENSIONS;
	}

}

void c_Buffer::init(uint f_Dimensions, uint f_UnitSize, ...) {
	/*
	 *
	 * Same as c_Buffer::c_Buffer(uint f_Dimensions, uint f_UnitSize, ...); but after initializated defaults
	 *
	 */

	va_list args;
	uint param;
	uint64_t sizeofbuffer = 1;

	if (f_Dimensions > 0) {
		unitSize = f_UnitSize;
		dimensions = f_Dimensions;
		size = (uint*) malloc(dimensions * sizeof(uint));

		va_start(args, f_Dimensions);

		for (int i = 0; i < dimensions; i++) {
			param = va_arg(args, uint);
			*(size + i) = param;
			sizeofbuffer *= param;
		}

		va_end(args);

		if (sizeofbuffer > 0) {
			buffer = malloc(sizeofbuffer * unitSize);
		}

	} else {
		dimensions = 0;
		unitSize = 0;
		size = NULL;
		buffer = NULL;
	}

} // */

void c_Buffer::remove() {
	// TODO Auto-generated destructor stub
	if (buffer != 0)
		free(buffer);
	if (size != 0)
		free(size);
	dimensions = 0;
	unitSize = 0;
}

c_Buffer::~c_Buffer() {
	// TODO Auto-generated destructor stub
	if (buffer != 0)
		free(buffer);
	if (size != 0)
		free(size);
	dimensions = 0;
	unitSize = 0;
}

c_Buffer::c_Buffer(const c_Buffer &other) {
	// TODO Auto-generated constructor stub
	dimensions = 0;
	size = NULL;
	buffer = NULL;
	unitSize = 0;
	buffer = NULL;
}

c_Buffer::c_Buffer(c_Buffer &&other) {
	// TODO Auto-generated constructor stub
	dimensions = 0;
	size = NULL;
	buffer = NULL;
	unitSize = 0;
	buffer = NULL;
}
/*
 c_Buffer& c_Buffer::operator=(const c_Buffer &other) {
 // TODO Auto-generated method stub
 //	dimensions = 0;
 //	size = NULL;
 //	buffer = NULL;
 //	unitSize=0;
 return (c_Buffer&)buffer;
 }

 c_Buffer& c_Buffer::operator=(c_Buffer &&other) {
 // TODO Auto-generated method stub
 //	dimensions = 0;
 //	size = NULL;
 //	buffer = NULL;
 //	unitSize=0;
 return (c_Buffer&)&buffer;
 }

 */

bool c_Buffer::set(void *value, ...) {
	// Set value to specified position(...)

	uint64_t position = 0, lastmax = 1;
	va_list args;
	uint param;
	uint cursize; //, lastsize=0;

	if ((dimensions > 0) & (unitSize > 0) & (size != 0) & (buffer != 0)) {

		// Calculate position in multidimential buffer
		// position = A1+A2*Size(A1)+A3*Size(A2)*Size(A1)+...An*MULT(x=1,x<n-1,Size(Ax)) = SUM(n=1,n<dimensions,An*MULT(x=1,x<n-1,Size(Ax)))

		va_start(args, dimensions);

		for (uint n = 0; n < dimensions; n++) {
			param = va_arg(args, uint);
			cursize = *(size + n);
			if (param > cursize)
				return false; // Parameter is larger then size of current dimmention
			position += param * lastmax;
			lastmax *= cursize;
			//lastsize=cursize;
		}

		memcpy((void*) (buffer + position * unitSize), value, unitSize); // Move value to calculated position
		return true;
	} else {
		return false;
	}
}

void* c_Buffer::get(...) {
	// Set value to specified position(...)

	uint64_t position = 0, lastmax = 1;
	va_list args;
	uint param;
	uint cursize; //, lastsize=0;
	if ((dimensions > 0) & (unitSize > 0) & (size != 0) & (buffer != 0)) {
		void *value = malloc(unitSize);
		// Calculate position in multidimential buffer
		// position = A1+A2*Size(A1)+A3*Size(A2)*Size(A1)+...An*MULT(x=1,x<n-1,Size(Ax)) = SUM(n=1,n<dimensions,An*MULT(x=1,x<n-1,Size(Ax)))

		va_start(args, dimensions);

		for (uint n = 0; n < dimensions; n++) {
			param = va_arg(args, uint);
			cursize = *(size + n);
			if (param > cursize)
				return NULL; // Parameter is larger then size of current dimmention
			position += param * lastmax;
			lastmax *= cursize;
			//lastsize=cursize;
		}
		memcpy((void*) value, (void*) (buffer + position * unitSize), unitSize); // Move value to calculated position

		return value;
	} else {
		return NULL;
	}
}

cBuffer_Error c_Buffer::setBuffer(c_Buffer *srcBuffer, void *position) {
	/*
	 * Сopy buffer to specified position
	 * srcBuffer mast be same dimensions, all sizes mast be less then same sizes in destination buffer.
	 *
	 *
	 */
	return CBUF_OK;
}

} /* namespace Memory */
} /* namespace Device */
