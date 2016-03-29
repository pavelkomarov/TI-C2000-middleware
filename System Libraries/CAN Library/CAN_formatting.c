/*
 * CAN_formatting.c
 *
 *  Created on: Oct 25, 2014
 *      Author: Andrey
 */
#include "CAN_formatting.h"
#include <stdlib.h>

typedef union ufloat {
	float32 f;
	Uint32 i;
}ufloat;

Uint32 combineChars(char last, char third, char second, char first){
	Uint32 f = ((Uint32) first);
	Uint32 s = ((Uint32) second) << 8;
	Uint32 t = ((Uint32) third) << 16;
	Uint32 l = ((Uint32) last) << 24;
	return l|t|s|f;
}

Uint32 combineIntHalves(Uint16 dataH, Uint16 dataL){
	Uint32 high = ((Uint32)dataH) << 16;
	return high|dataL;
}

Uint32 convertFloat(float32 f){
	ufloat convert;
	convert.f = f;
	return convert.i;
}

float32 convertInt(Uint32 i){
	ufloat convert;
	convert.i = i;
	return convert.f;
}

Uint32* convertFloats(float32* floats, int length){
	int i;
	Uint32* ints=(Uint32*)malloc(4*length);
	for(i=0;i<length;i++)
		ints[i] = convertFloat(floats[i]);
	return ints;
}

float32* convertInts(Uint32* ints, int length){
	int i;
	float32* floats=(float32*)malloc(4*length);
	for(i=0;i<length;i++)
		floats[i] = convertInt(ints[i]);
	return floats;
}

Uint16 getFirstHalf(Uint32 v){
	return (Uint16) (v & 0x0000FFFF);
}

Uint16 getSecondHalf(Uint32 v){
	return (Uint16) (v >>16);
}

Uint32* putIntoBuffer(Uint32 DataH, Uint32 DataL){
	Uint32 buffer[2];
	buffer[0] = DataL;
	buffer[1] = DataH;
	return buffer;
}
