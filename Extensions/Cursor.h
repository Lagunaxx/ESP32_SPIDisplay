/*
 * Cursor.h
 *
 *  Created on: 15 окт. 2019 г.
 *      Author: producer
 */

#ifndef EXTENSIONS_CURSOR_H_
	#define EXTENSIONS_CURSOR_H_

#include <Arduino.h>

	namespace Device {
		namespace Display {
			namespace Cursor{

				// define how much bits coordinate will consists
				#ifdef COORDINATES_8b
					typedef int8_t _CoordsType;
				#else
					#ifdef COORDINATES_16b
						typedef int16_t _CoordsType;
					#else
						#ifdef COORDINATES_32b
							typedef int32_t _CoordsType;
						#else
							typedef int64_t _CoordsType; //by default using 64bit coordinates
						#endif
					#endif
				#endif


				class c_Cursor {
				public:
					c_Cursor();
					virtual ~c_Cursor();
	/*
					Cursor(const Cursor &other);
					Cursor(Cursor &&other);
					Cursor& operator=(const Cursor &other);
					Cursor& operator=(Cursor &&other);
	*/
					void			Set(_CoordsType x, _CoordsType y),					//	Set cursor in x,y position (do not change z position)
									Set(_CoordsType x, _CoordsType y, _CoordsType z);	//	Set cursor to x,y,z position (for 3D space)

					_CoordsType		X(),		//	Get X coordinate
									Y(),		//	Get Y coordinate
									Z();		//	Get Z coordinate

					void			X(_CoordsType coord),		//	Set X coordinate
									Y(_CoordsType coord),		//	Set Y coordinate
									Z(_CoordsType coord);		//	Set Z coordinate

					bool			MoveX(_CoordsType coord),		//	Move X coordinate by coord
									MoveY(_CoordsType coord),		//	Move Y coordinate by coord
									MoveZ(_CoordsType coord);		//	Move Z coordinate by coord

					virtual void Draw();
					virtual void Redraw();

				private:
					_CoordsType			x, y, z;					//Cursor's coordinates
					bool			visible;				//Does cursor visible
				};
			}	/* namespace Cursor */
		} /* namespace Display */
	} /* namespace Device */

#endif /* EXTENSIONS_CURSOR_H_ */
