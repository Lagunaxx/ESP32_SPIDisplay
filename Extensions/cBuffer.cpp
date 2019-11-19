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

		//for (uint n = 0; n < dimensions; n++) {
		ListDimensions(i,0){
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

bool c_Buffer::Initialized(){

	if (dimensions == 0){
		status = CBUF_ERR_NODIMENSIONS;
		return false;
	}
	if (size == NULL){
		status = CBUF_ERR_SIZE;
		return false;
	}
	if (buffer==0){
		status = CBUF_ERR_BUFFER;
		return false;
	}
	if (unitSize==0){
		status=CBUF_ERR_UNITSIZE;
		return false;
	}
	return true;
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
	CheckInit(bool);

	uint64_t position = 0, lastmax = 1;
	va_list args;
	uint param;
	uint cursize; //, lastsize=0;

		// Calculate position in multidimential buffer
		// position = A1+A2*Size(A1)+A3*Size(A2)*Size(A1)+...An*MULT(x=1,x<n-1,Size(Ax)) = SUM(n=1,n<dimensions,An*MULT(x=1,x<n-1,Size(Ax)))

		va_start(args, dimensions);

		//for (uint n = 0; n < dimensions; n++) {
		ListDimensions(n,0){
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
}

void* c_Buffer::get(...) {
	// Set value to specified position(...)
	CheckInit(void*);

	uint64_t position = 0, lastmax = 1;
	va_list args;
	uint param;
	uint cursize; //, lastsize=0;
		void *value = malloc(unitSize);
		// Calculate position in multidimential buffer
		// position = A1+A2*Size(A1)+A3*Size(A2)*Size(A1)+...An*MULT(x=1,x<n-1,Size(Ax)) = SUM(n=1,n<dimensions,An*MULT(x=1,x<n-1,Size(Ax)))

		va_start(args, dimensions);

		//for (uint n = 0; n < dimensions; n++) {
		ListDimensions(n,0){
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
}

cBuffer_Error c_Buffer::Status() {
	return status;
}

uint c_Buffer::getDimensions() {
	return dimensions;
}
uint c_Buffer::getUnitSize() {
	return unitSize;
}

uint* c_Buffer::copySize() {
	/*
	 * Return pointer to copy of 'size'.
	 * Example:
	 * 		c_Buffer cBuffer(2,sizeof(double),50,50);
	 * 		uint* sizes=cBuffer.copySize();
	 * 		// ... use sizes in anything
	 * 		free(sizes); // mast call when 'sizes' not need
	 */
	CheckInit(uint*);

	uint *temp = (uint*) malloc(dimensions * sizeof(uint));
	memcpy(temp, size, (dimensions * sizeof(uint)));
	return temp;
}

uint c_Buffer::getSize(uint dimension){
	CheckInit(uint);
	return *(size+dimension);
}

void* c_Buffer::copyBuffer() {
	/*
	 * Return pointer to copy of all 'buffer'
	 * Example:
	 * 		c_Buffer cBuffer(2,sizeof(double),50,50);
	 * 		void* sizes=cBuffer.copyBuffer();
	 * 		// ... use sizes in anything
	 * 		free(sizes); // mast call when 'sizes' not need
	 */

	CheckInit(void*);

	void *temp;
	uint64_t sizeofbuffer = 1;

	for (int i = 0; i < dimensions; i++) {
		sizeofbuffer *= *(size + i);
	}

	if (sizeofbuffer == 0) {
		status = CBUF_ERR_NULLBUFFERSIZE;
		return NULL;
	}

	temp = malloc(sizeofbuffer * unitSize);
	if (temp == NULL) {
		status = CBUF_ERR_MALLOC;
		return NULL;
	}

	memcpy(temp, buffer, sizeofbuffer * unitSize);

	return temp;
}

void* c_Buffer::copyBuffer(void *needPosition, void *needSize){
	/*
	 * Return part of buffer from specified multidimentional position within specified multydimensional size
	 *
	 */
	CheckInit(void*);

	uint64_t f_dstposition=0, f_srcposition=0, tmpsize=1, tmpsizecur, btsmove;
	uint8_t tmpbyte;
	uint datablocks=1;
	void *tmpSizePosition = (uint*)malloc((dimensions-1)*unitSize); //Contains positions for data-blocks (without first dimension)
	uint8_t *posparam = (uint8_t *) malloc(unitSize); //Contains position of current dimension in dimensions listing
	bool DecreaseNextDimension=false;
	uint64_t lastmax=1, *curmax=(uint64_t*)malloc(unitSize);

Serial.printf("\nstart list");
	ListDimensions(i,0){
	// Calculate size we need to malloc
		tmpsizecur=0;
		btsmove=1;
Serial.printf("\nUnitsize=%i",unitSize);

		for(tmpbyte=0;tmpbyte<unitSize;tmpbyte++){
			// Get size of i-dimension
			tmpsizecur+=uint64_t(*((uint8_t*) needSize + i*unitSize + tmpbyte))*pow(256,tmpbyte);
Serial.printf("\ntmpsizecur=%i,%i",(uint)tmpsizecur,(uint)tmpbyte);
		}

		tmpsize*=tmpsizecur; // count size of matrix need to export (dimension1*dimension2*...*dimensionN)
		if (i>0) datablocks*=tmpsizecur;

		memcpy(posparam,needPosition+i*unitSize,unitSize);
		if (i>0) memcpy(tmpSizePosition+(i-1)*unitSize,needSize+i*unitSize,unitSize); // fill counter for datablocks by dimensions

	}
Serial.printf("\ndatablocks=%i",(uint)datablocks); // blocks of data need to export

	void *tmpbuf=malloc(tmpsize*unitSize); // allocate memory for new data

		f_srcposition = 0;//1;
		memcpy (&f_srcposition,needPosition,unitSize);
		f_dstposition = 0;

	while ( datablocks > 0){
Serial.printf("\n\n");

f_srcposition = 0;
memcpy (&f_srcposition,needPosition,unitSize);

		memcpy(curmax,size,unitSize);
		lastmax=(uint64_t)*curmax;
		ListDimensions(i,1){
			// Count position for current datablock
/*			memcpy(posparam,tmpSizePosition+(i-1)*unitSize, unitSize);
			f_srcposition += uint64_t(*posparam)*lastmax;
			memcpy(curmax,needSize+i*unitSize,unitSize);
*/
			memcpy(posparam,tmpSizePosition+(i-1)*unitSize, unitSize);
			f_srcposition += uint64_t(*posparam)*lastmax;
			memcpy(curmax,size+i*unitSize,unitSize);

			if (i<(dimensions-1))lastmax*=(uint64_t)*curmax;

Serial.printf("\i=%i, lastmax=%i,posparam=%i,size+i*unitSize=%i",i,(uint)lastmax,(uint)*posparam,(uint)*((uint*)size+i*unitSize));
		}
Serial.printf("\nf_srcpos=%i",(uint)f_srcposition);

//			f_dstposition *= *(needSize+i);
//Serial.printf("\n*(tmpSizePosition+%i)=%i",i,(uint)*(tmpSizePosition+i));
//		}
		//f_srcposition += (uint)*needSize;
//Serial.printf("\nf_srcposition=%i",(uint)f_srcposition);
//		memcpy((tmpbuf+f_dstposition),(buffer+f_srcposition),(unitSize*(*needSize)));
//Serial.printf("\tvalue = %i",*((uint *)buffer+f_srcposition));

		ListDimensions(i,1){
			// Decrease position we need by dimensions - ToDo: nop.. mast increase by 1 to needSize[dimension] = ready!

			// make calculation of: *(tmpSizePosition+i) +=1;
			for (uint8_t tmp=0;tmp<unitSize;tmp++){
				memcpy(&tmpbyte,tmpSizePosition+(i-1)*unitSize+tmp, 1);

				// decreas current byte if it is >0 and stop, else make it eq 255 and go to higher byte:
				if (tmpbyte<255){
					tmpbyte++;
					memcpy(tmpSizePosition+(i-1)*unitSize+tmp,&tmpbyte, 1);
					break;
				}
				tmpbyte=0;
				if (tmp<(unitSize-1)){
					memcpy(tmpSizePosition+(i-1)*unitSize+tmp,&tmpbyte, 1);
				}else{
//					for (uint8_t tmp=0;tmp<unitSize;tmp++){
						memcpy(tmpSizePosition+(i-1)*unitSize,needSize+i*unitSize, unitSize); // if this was last block, then return value from needSize for going to next dimension
						DecreaseNextDimension=true;
//					}

				}

			}
			if (!DecreaseNextDimension)break; // break if we do not need to decrease next dimension

		}

		datablocks--;
	}

	free(curmax);
	free(tmpSizePosition);
	free(posparam);
	return tmpbuf;
}

bool c_Buffer::copyBuffer(c_Buffer *srcBuffer, uint *srcPosition, uint *srcSize, uint *dstPosition) {
	/*
	 * Сopy buffer to specified position
	 * srcBuffer mast be same dimensions, all sizes mast be less then same sizes in destination buffer.
	 *
	 */
	CheckInit(bool);

	if ((dimensions != srcBuffer->getDimensions()) | (srcBuffer->getDimensions()==0)){
		status=CBUF_ERR_DIMENSIONS;
		return false;
	}
	if ((unitSize != srcBuffer->getUnitSize()) | (srcBuffer->getUnitSize() == 0)){
		status=CBUF_ERR_BUFFERSIZE;
		return false;
	}

	for (uint n = 0; n < dimensions; n++) {
		if ((*(srcSize+n))>(*(size+n))){
			status=CBUF_ERR_SIZE;
			return false;
		}
		if ( ((*(srcSize+n)) > (srcBuffer->getSize(n) - (*(srcPosition+n)))) | ( (*(srcSize+n)) > ( (*(size+n)) - (*(dstPosition+n)) )) ){
			status=CBUF_ERR_SIZE;
			return false;
		}
	}

	uint64_t f_dstposition = 1, f_srcposition = 0;
	void *srcbuf = srcBuffer->copyBuffer(srcPosition, srcSize);

/*	ListDimensions(n,1){
		f_dstposition *=

	}
*/
	memcpy((buffer+f_dstposition),(srcbuf+f_srcposition),(srcBuffer->getUnitSize()*(*srcSize)));

	free(srcbuf);
	return true;
}

c_Buffer& c_Buffer::operator=(const c_Buffer &other(...)){

	c_Buffer* temp=this;



	return *temp;
}

} /* namespace Memory */
} /* namespace Device */
