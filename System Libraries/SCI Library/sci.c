/**
 * @file sci.c
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief A library makes communicating over SCI simple
 * @ingroup Digital
 * @version 2
 *
 * http://solarracing.gatech.edu/wiki/Main_Page
 * Initialize and use SCI with relative ease.
 */
#include "F2806x_Device.h"
#include "sci.h"
#include "string.h"

/**
 * Pass SCIPINs corresponding to GPIO pins you wish to make SCI .
 *
 * @param scisys A or B
 * @param in An SCIPIN to be used for input to the microcontroller
 * @param out An SCIPIN to be used for output from the microcontroller
 *
 */
void SciInit(SCIPIN in, SCIPIN out) {

	//The pin-assignment values are too unique to do some clever bitwise ops
	//I have just used couple of switches.

	//set up input pin
	switch (in) {
		case Ain7:
			GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;     // Enable pull-up for GPIO7  (SCIRXDA)
			GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;   // Asynch input GPIO7 (SCIRXDA)
			GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;    // Configure GPIO7 for SCIRXDA operation
			break;
		case Ain28:
			GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
			GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
			GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
			break;
		case Bin11:
			GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;    // Enable pull-up for GPIO11 (SCIRXDB)
			GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
			GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;   // Configure GPIO11 for SCIRXDB operation
			break;
		case Bin15:
			GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)
			GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)
			GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation
			break;
		case Bin19:
			GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;    // Enable pull-up for GPIO19 (SCIRXDB)
			GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)
			GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   // Configure GPIO19 for SCIRXDB operation
			break;
		case Bin23:
			GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0;    // Enable pull-up for GPIO23 (SCIRXDB)
			GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)
			GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;   // Configure GPIO23 for SCIRXDB operation
			break;
		case Bin41:
			GpioCtrlRegs.GPBPUD.bit.GPIO41 = 0;    // Enable pull-up for GPIO41 (SCIRXDB)
			GpioCtrlRegs.GPBQSEL1.bit.GPIO41 = 3;  // Asynch input GPIO41 (SCIRXDB)
			GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 2;   // Configure GPIO41 for SCIRXDB operation
			break;
		case Bin44:
			GpioCtrlRegs.GPBPUD.bit.GPIO44 = 0;    // Enable pull-up for GPIO44 (SCIRXDB)
			GpioCtrlRegs.GPBQSEL1.bit.GPIO44 = 3;  // Asynch input GPIO44 (SCIRXDB)
			GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 2;   // Configure GPIO44 for SCIRXDB operation
			break;
	}

	//set up output pin
	switch (out) {
		case Aout12:
			GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;	   // Enable pull-up for GPIO12 (SCITXDA)
			GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;   // Configure GPIO12 for SCITXDA operation
			break;
		case Aout29:
			GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	   // Enable pull-up for GPIO29 (SCITXDA)
			GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
			break;
		case Bout9:
			GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;	   // Enable pull-up for GPIO9 (SCITXDB)
			GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;    // Configure GPIO9 for SCITXDB operation
			break;
		case Bout14:
			GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;	   // Enable pull-up for GPIO14 (SCITXDB)
			GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation
			break;
		case Bout18:
			GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	   // Enable pull-up for GPIO18 (SCITXDB)
			GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;   // Configure GPIO18 for SCITXDB operation
			break;
		case Bout22:
			GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;	   // Enable pull-up for GPIO22 (SCITXDB)
			GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;   // Configure GPIO22 for SCITXDB operation
			break;
		case Bout40:
			GpioCtrlRegs.GPBPUD.bit.GPIO40 = 0;	   // Enable pull-up for GPIO40 (SCITXDB)
			GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 2;   // Configure GPIO40 for SCITXDB operation
			break;
		case Bout58:
			GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;	   // Enable pull-up for GPIO58 (SCITXDB)
			GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 2;   // Configure GPIO58 for SCITXDB operation
			break;
	}

	//init sci module control registers and fifo
	if (in == Ain7 || in == Ain28) {
		SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;//enable clock to SCIA
		asm(" NOP");
		asm(" NOP");

		SciaRegs.SCICCR.all = 0x7;//one stop bit, odd parity, parity disabled,
									//loop-back disabled, character length = 8 (See Table
									//13-9 in Technical Reference Manual)
		SciaRegs.SCICTL1.all = 0x3;//TX enabled, RX enabled, skip reset state, disable
									//RXERR, SLEEP, TXWAKE (see Table 13-10 in Tech Ref Man)
		SciaRegs.SCICTL2.all = 0x0;//see Table 13-12 in Tech Ref Man

		SciaRegs.SCIFFTX.all = 0xE404;//SCI reset, SCI FIFO enhancements are enabled,
									//Re-enable transmit FIFO operation, clear TXFFINT flag,
									//set buffer size to 4 (see Table 13-15 in Technical
									//Reference Manual)
		SciaRegs.SCIFFRX.all = 0x2424;//Re-enable receive FIFO operation,
									//clear RXFFINT bit, set buffer size to 4
									//(see Table 13-16 in Technical Reference Manual)
		SciaRegs.SCIFFCT.all = 0;//see Table 13-17 in Technical Reference Manual
									//NO autobaud
		SciaRegs.SCICTL1.bit.SWRESET = 1;
	} else {
		SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;//enable clock to SCIB
		asm(" NOP");
		asm(" NOP");

		ScibRegs.SCICCR.all =0x7;//see above comments
		ScibRegs.SCICTL1.all =0x3;
		ScibRegs.SCICTL2.all =0x0;

		ScibRegs.SCIFFTX.all=0xE404;//see above comments
		ScibRegs.SCIFFRX.all=0x2424;
		ScibRegs.SCIFFCT.all=0x0;
		ScibRegs.SCICTL1.bit.SWRESET = 1;
	}
}//holy crap


/**
 * Set the SCI baud rate in kHz. Do this only after initializing the module with
 * SciInit. Note this method requires knowledge of the system clock frequency (in MHz).
 * Make a call to getfclk from the Clock Library if needed.
 *
 * Note this method is not completely fool-proof: not all combinations of fclk and
 * baud rate are possible. For example, if baud needs to be < 350 Hz for fclk =
 * 180MHz (its max possible value), then the low-speed clock will need to be set
 * to some fraction of fclk. Here I have left the low-speed clock as large as
 * possible to increase fidelity between desired and actually-set baud rate.
 *
 * @param scisys A or B
 * @param fclk The system clock frequency in MHz as set with SysClkInit and obtained with getfclk
 * @param baudrate The desired baud rate of the sci module in kHz
 */
void SetSciBaudRate(char scisys, float32 fclk, float32 baudrate) {
	SysCtrlRegs.LOSPCP.all = 0x0000;//low-speed clock = sysclk
								//see Table 1-19 in Tech Ref Man
	asm(" NOP");
	asm(" NOP");

	Uint16 brr = fclk*1000/(8*baudrate)-1;//find brr with formula from Table 13-11
										//of Tech Ref Man. x1000 to put MHz in kHz.
	switch (scisys) {
		case 'A':
			SciaRegs.SCICTL1.bit.SWRESET = 0;//is this necessary? It works.
			SciaRegs.SCILBAUD = brr;//truncated
			SciaRegs.SCIHBAUD = (brr >> 8);
			SciaRegs.SCICTL1.bit.SWRESET = 1;//relinquish from reset mode
			break;
		case 'B':
			ScibRegs.SCICTL1.bit.SWRESET = 0;
			ScibRegs.SCILBAUD = brr;
			ScibRegs.SCIHBAUD = (brr >> 8);
			ScibRegs.SCICTL1.bit.SWRESET = 1;
			break;
	}

}

/**
 * Send a character to the SCI pin.
 *
 * @param scisys A or B
 * @param c The character to send to the SCI module.
 */
void sendCharacter(char scisys, char c) {
	switch (scisys) {
		case'A':
			while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}//wait for current transmit to finish
			SciaRegs.SCITXBUF = c;//write new message element
			break;
		case 'B':
			while (ScibRegs.SCIFFTX.bit.TXFFST != 0) {}//wait for current transmit to finish
			ScibRegs.SCITXBUF = c;//write new message element
			break;
	}
}

/**
 * Send a byte array via SCI. Note this function is BLOCKING: no other code will be
 * executed while this function is running (unless that other code is an ISR).
 *
 * @param scisys A or B
 * @param arr Data to be sent.
 * @param len The length of the data.
 */
void sendCharArray(char scisys, char* arr, Uint16 len) {
	Uint16 i;
	for(i = 0; i < len; i++) {
		sendCharacter(scisys, arr[i]);
	}
}

/**
 * Send a string via SCI. Note this function is BLOCKING: no other code will be
 * executed while this function is running (unless that other code is an ISR).
 *
 * @param scisys A or B
 * @param arr A string to be sent. No length parameter necessary.
 */
void sendString(char scisys, char* str) {
	sendCharArray(scisys, str, strlen(str)+1);//+1 for null character
}

/**
 * Read from the RX register when its value changes.
 *
 * @param scisys A or B
 * @return The character read.
 */
char recieveChar(char scisys) {
		switch (scisys) {
		case 'A':
			while (SciaRegs.SCIFFRX.bit.RXFFST == 0) {} //wait for a change
			return SciaRegs.SCIRXBUF.all;
		case 'B':
			while (ScibRegs.SCIFFRX.bit.RXFFST == 0) {} //wait for a change
			return ScibRegs.SCIRXBUF.all;
		default:
			return NULL;//to make the compiler stop whining
	}
}

/**
 * Receives data as a byte array.
 *
 * @param scisys A or B
 * @param arr A pointer to a data-buffer to be loaded with data.
 * @param len The length of the message to be recieved.
 */
void recieveCharArray(char scisys, char* arr, Uint16 len) {
	Uint16 i = 0;
	while(i < len) {
		arr[i] = recieveChar(scisys);
		i++;
	}
}

/**
 * Receives a message and puts it in a null-terminated string. Note that the
 * null-termination is done inside, so pass any character array and get back
 * a string.
 *
 * @param scisys A or B
 * @param str A character array to be filled with null-terminated data. No
 * 				length specification necessary.
 * @return -1 if the string recieved has not ended by the time the end of the
 * 				data-buffer is reached, +1 if there is no error
 */
signed char recieveString(char scisys, char* str) {
	Uint16 i = 0;
	char c = 't';
	while(c != '\0') {
		if (i > strlen(str)) {//strlen("thing") = 5, and ['t','h','i','n','g','\0'] has indices 0->5
			return -1;
		}
		c = recieveChar(scisys);
		str[i] = c;
		i++;
	}
	return 1;
}

/**
 * UNTESTED
 * Enable interrupts from the SCI
 *
 * @param scisys A or B
 * @param sendISR A function pointer to an interrupt to be executed when data is sent
 * @param recieveISR A function pointer to an interrupt to be executed when data is recieved
 */
void EnableSciInterrupts(char scisys) {
	switch (scisys) {
		case 'A':
			SciaRegs.SCIFFTX.bit.SCIFFENA = 1;//enable SCIA FIFO
			SciaRegs.SCIFFTX.bit.TXFFIENA = 1;//enable transmit interrupts from the SCI module
			SciaRegs.SCIFFTX.bit.TXFFIL = 4;//interrupts occur when fifo holds 4 words.
			SciaRegs.SCIFFRX.bit.RXFFIENA = 1;//enable recieve interrupts from the SCI module
			SciaRegs.SCIFFRX.bit.RXFFIL = 4;//interrupts occur when fifo holds 4 words.
			break;
		case 'B':
			ScibRegs.SCIFFTX.bit.SCIFFENA = 1;//enable SCIB FIFO
			ScibRegs.SCIFFTX.bit.TXFFIENA = 1;//enable transmit interrupts from the SCI module
			ScibRegs.SCIFFTX.bit.TXFFIL = 4;//interrupts occur when fifo holds 4 words.
			ScibRegs.SCIFFRX.bit.RXFFIENA = 1;//enable recieve interrupts from the SCI module
			ScibRegs.SCIFFRX.bit.RXFFIL = 4;//interrupts occur when fifo holds 4 words.
			break;
	}
}


