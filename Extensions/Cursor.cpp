/*
 * Cursor.cpp
 *
 *  Created on: 15 окт. 2019 г.
 *      Author: producer
 */

#include "Cursor.h"

namespace Device {
	namespace Display {
		namespace Cursor {

			c_Cursor::c_Cursor() {
				// TODO Auto-generated constructor stub
				x=0;
				y=0;
//				z=0;
				visible=false;

			}

			c_Cursor::~c_Cursor() {
				// TODO Auto-generated destructor stub
			}
	/*
			c_Cursor::c_Cursor(const Cursor &other) {
				// TODO Auto-generated constructor stub
	#ifdef MEMINIT_NEW
				 &other=new Cursor();
	#else
				 &other=(Cursor)malloc(sizeof(Cursor));
				 other.Cursor();
	#endif
			}

			c_Cursor::c_Cursor(Cursor &&other) {
				// TODO Auto-generated constructor stub
				//&other=this;
			}

			c_Cursor& c_Cursor::operator=(const Cursor &other) {
				// TODO Auto-generated method stub
				return this;
			}

			c_Cursor& c_Cursor::operator=(Cursor &&other) {
				// TODO Auto-generated method stub
				return this;
			}
	*/
			void c_Cursor::Set(T_DispCoords x, T_DispCoords y){
				//	Set cursor in x,y position (do not change z position)
				this->x=x;
				this->y=y;
			}
/*			void c_Cursor::Set(T_DispCoords x, T_DispCoords y, T_DispCoords z){
				//	Set cursor to x,y,z position (for 3D space)
				this->x=x;
				this->y=y;
				this->z=z;
			}
*/
			T_DispCoords			c_Cursor::X(){
				return this->x;
			}
			T_DispCoords			c_Cursor::Y(){
				return this->y;
			}
/*			T_DispCoords			c_Cursor::Z(){
				return this->z;
			}
*/

			void			c_Cursor::X(T_DispCoords coord){
				this->x=coord;
			}
			void			c_Cursor::Y(T_DispCoords coord){
				this->y=coord;
			}
/*			void			c_Cursor::Z(T_DispCoords coord){
				this->z=coord;
			}
*/
			bool			c_Cursor::MoveX(T_DispCoords coord){
				//	Move X coordinate by coord
				if (coord>0){
					if ( (0-pow(256,sizeof(T_DispCoords))/2) >= (this->x+=coord) ){
						this->x+=coord;
						return true;
					}else{
						return false; // false if movement larger then available space (depends on _CoordsType)
					}
				}else{
					if ( (pow(256,sizeof(T_DispCoords))/2-1) <= (this->x+=coord) ){
						this->x+=coord;
						return true;
					}else{
						return false; // false if movement larger then available space (depends on _CoordsType)
					}
				}
			}
			bool			c_Cursor::MoveY(T_DispCoords coord){
				//	Move Y coordinate by coord
				if (coord>0){
					if ( (0-pow(256,sizeof(T_DispCoords))/2) >= (this->y+=coord) ){
						this->y+=coord;
						return true;
					}else{
						return false; // false if movement larger then available space (depends on _CoordsType)
					}
				}else{
					if ( (pow(256,sizeof(T_DispCoords))/2-1) <= (this->y+=coord) ){
						this->y+=coord;
						return true;
					}else{
						return false; // false if movement larger then available space (depends on _CoordsType)
					}
				}
			}
/*			bool			c_Cursor::MoveZ(T_DispCoords coord){
				//	Move Z coordinate by coord
				if (coord>0){
					if ( (0-pow(256,sizeof(T_DispCoords))/2) >= (this->z+=coord) ){
						this->z+=coord;
						return true;
					}else{
						return false; // false if movement larger then available space (depends on _CoordsType)
					}
				}else{
					if ( (pow(256,sizeof(T_DispCoords))/2-1) <= (this->z+=coord) ){
						this->z+=coord;
						return true;
					}else{
						return false; // false if movement larger then available space (depends on _CoordsType)
					}
				}
			}
*/
			void Draw(){
				;
			}
			void Redraw(){
				;
			}


			//virtual void Cursor::Draw();
		} /* namespace Cursor */
	} /* namespace Display */
} /* namespace Device */
