/*
 * Cursor.h
 *
 *  Created on: 15 окт. 2019 г.
 *      Author: producer
 */

#ifndef EXTENSIONS_CURSOR_H_
	#define EXTENSIONS_CURSOR_H_

#include <Arduino.h>
#include "Types.h"

	namespace Device {
		namespace Display {
			namespace Cursor{

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
					void			Set(t_DispCoords x, t_DispCoords y);					//	Set cursor in x,y position (do not change z position)
//									Set(T_DispCoords x, T_DispCoords y, T_DispCoords z);	//	Set cursor to x,y,z position (for 3D space)

					t_DispCoords		X(),		//	Get X coordinate
									Y();		//	Get Y coordinate
//									Z();		//	Get Z coordinate

					void			X(t_DispCoords coord),		//	Set X coordinate
									Y(t_DispCoords coord);		//	Set Y coordinate
//									Z(T_DispCoords coord);		//	Set Z coordinate

					bool			MoveX(t_DispCoords coord),		//	Move X coordinate by coord
									MoveY(t_DispCoords coord);		//	Move Y coordinate by coord
//									MoveZ(T_DispCoords coord);		//	Move Z coordinate by coord

					//virtual
					void Draw();
					//virtual
					void Redraw();

				private:
					t_DispCoords			x, y;//, z;					//Cursor's coordinates
					bool			visible;				//Does cursor visible
				};
			}	/* namespace Cursor */
		} /* namespace Display */
	} /* namespace Device */

#endif /* EXTENSIONS_CURSOR_H_ */
