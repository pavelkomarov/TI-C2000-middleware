/**
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @author Andrey Kurenkov
 * @author Ricky Liou
 * @author Alex 'Apop' Popescue
 */

#include "F2806x_Device.h"
#include "28069Common.h"
#include "wifi.h"
#include "sci.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Send a single floating-point number with a description
 *
 * @param scisys A or B
 * @param name A string describing the data
 * @param send A float to send
 */
void sendFloat(char scisys, char* name, float32 send){
	char* toSend= malloc(sizeof(char)*(strlen(name)+1));//have some padding for non decimal places

	sprintf(toSend,"%s:%.5f", name, send);//"%11.5f specifies a format with 11 total
	sendString(scisys, toSend);				//characters: 5 digits before the dot, the
	free(toSend);							//dot itself, and 5 digits after the dot.
}

/**
 * Send an array of floating-point numbers.
 *
 * @param name A string describing the data
 * @param send An array of floats to send as data
 * @param len The length of the data-array
 */
void sendFloats(char scisys, char* name, float32* send, Uint16 len) {
	char* floatStr = (char*) malloc(sizeof(char)*12);//one for ;, one for ., five above ., five below
	char* toSend = (char*) malloc(sizeof(char)*(strlen(name)+1 + len*12));

	sprintf(toSend,"%s:", name);
	int i;
	for(i = 0; i < len; i++){
		sprintf(floatStr, ";%.5f", send[i]);
		strcat(toSend, floatStr);
	}
	sendString(scisys, toSend);
	free(toSend);
	free(floatStr);
}
