/**
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief This project tests the xtend telemetry system
 * @ingroup Digital
 * @version 3
 *
 * This project is meant to test the wireless system onboard the car. It is
 * also intended as a demonstration of how to use a few of the various libraries
 * I have authored. Note that projects need to link to these libraries in order
 * to use their functions. I wrote a page about setting this up on the Electrical
 * subwiki called "C2000 Libraries".
 */
#include "F2806x_Device.h"
#include "28069Common.h"
#include "clocks.h"
#include "gpio.h"
#include "sci.h"
#include "wifi.h"
#include "fastflash.h"
#include "interrupts.h"
//Also relies on 28069_RAM_lnk.cmd, F2806x_CodeStartBranch.asm,
//F2806x_Headers_nonBIOS.cmd, F28069_FastFlash.cmd
//targetConfig is "TMS320F28069.ccxml"

//interrupt void recieveISR(void);
interrupt void timerISR(void);

char message[39];
Uint32 loopcnt = 0, tmrcnt = 0;
Uint16 transmit = 0, tx = 0;

//Code blocks in the preamble are labelled with the names
//of the libraries in which functions-used-therein are defined.
//Look in these to discover fundamental definitions.
void main(void) {

	EALLOW;//an assembly language thing required to allow access to system control registers
	SysCtrlRegs.WDCR = 0x68;//disable watchdog
	//fastflash
		InitFlash();//call at the beginning
	//clock
		SysClkInit(FIFTY);//set the system clock to 50MHz
	//sci
		SciInit(Bin23, Bout22);//set up the SCI system (B) //"in" and "out" mean "of microcontroller"
		SetSciBaudRate('B', getfclk(), 115.2);//115.200);//set the SCIB baud rate to be ~115.2 KHz
	//gpio
		Uint8 in[2] = {24, 25};//24 is recieve pin (goes high upon recieve); 25 is tx pin (goes low during transmission)
		GpioInputsInit(in, 2);//"2" is length of in array
		Uint8 out[3] = {26, 31, 34};//26 is !shdn on wifi, 31 is led 2: lights when wifi is sending
		GpioOutputsInit(out, 3);//34 goes high/low each second
	//interrupts
		IsrInit(TINT0, &timerISR);
	//clock
		TimerInit(1.0);//set the timer to 1kHz and start. Relies on SysClkInit, so call that first.
	EDIS;//disallow access to system control registers

	//set some initial states
	GpioSetPin(26);//drive !SHDN pin high during operation

	while(1) {
		//alternate sending and not sending each second
		if (transmit) {
			sendString('B', "So long and thanks for all the fish!");
		}
		loopcnt++;
	}
}

void timerISR() {//called at 1kHz
	if ((tmrcnt % 1000) == 0) {//every second
		GpioTogglePin(34);//Toggle LD3 (led 3)
		transmit ^= 1;//toggle send flag
	}

	tx = GpioGetData(25);//check whether TX is low (Xtend is transmitting)
	if (tx) {
		GpioSetPin(31);//turn off LD2
	} else {
		GpioClearPin(31);//turn on LD2
	}

	tmrcnt++;
	IsrAck(TINT0);//resets pie flag (necessary at end of all interrupts)
}

/*void recieveISR() {


 	//enable interrupts from GPIO1
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 1;

	signed char err = recieveString(message);

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;//reset interrupt status
}*/
