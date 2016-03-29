/*
 * CAN_formatting.h
 *
 *  Created on: Oct 25, 2014
 *      Author: Andrey
 */
#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions

#ifndef CAN_FORMATTING_H_
#define CAN_FORMATTING_H_

Uint32 convertFloat(float32 f);
float32 convertInt(Uint32 i);
Uint32* convertFloats(float32* floats, int length);
float32* convertInts(Uint32* ints, int length);
Uint32 combineChars(char last, char third, char second, char first);
Uint32 combineIntHalves(Uint16 dataH, Uint16 dataL);
Uint16 getFirstHalf(Uint32 v);
Uint16 getSecondHalf(Uint32 v);
Uint32* putIntoBuffer(Uint32 DataH, Uint32 DataL);
#endif /* CAN_FORMATTING_H_ */
