/**
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief This project tests the functions of the GPIO Library
 * @ingroup Digital
 * @version 1
 *
 */
#include "F2806x_Device.h"
#include "28069Common.h"
#include "clocks.h"
#include "gpio.h"
#include "interrupts.h"
//Also relies on 28069_RAM_lnk.cmd, F2806x_CodeStartBranch.asm,
//F2806x_Headers_nonBIOS.cmd, F28069_FastFlash.cmd
//targetConfig is "TMS320F28069.ccxml"

//interrupt void recieveISR(void);
interrupt void timerISR(void);

Uint32 loopcnt = 0, tmrcnt = 0;
Uint16 data;
Uint8 flag = 0;

void main(void) {

	EALLOW;//an assembly language thing required to allow access to system control registers
	SysCtrlRegs.WDCR = 0x68;//disable watchdog
	//clock
		SysClkInit(FIFTY);//set the system clock to 50MHz
	//gpio
		Uint8 in[3] = {3, 4, 5};	//1 is recieve pin (goes low upon recieve); 2 is tx pin (goes low during transmission)
		GpioInputsInit(in, 3);	//"2" is length of in array

		GpioInputInit(6);//works

		Uint8 out[4] = {0, 1, 31, 34};//0 is !shdn on wifi
		GpioOutputsInit(out, 4);	//34 is a pin that goes high/low each second

		GpioOutputInit(2);//works

		//GpioFloatAll();//works
		GpioPullUpAll();//works
		//Uint8 pu[2] = {3, 4};//works
		//GpioPullUpPins(pu, 2);
		//GpioPullUpPin(6);//works
		//Uint8 fl[2] = {3, 4};//works
		//GpioFloatPins(fl, 2);
		GpioFloatPin(6);//works
	//interrupts				//31 is ld2: lights when wifi is sending
		IsrInit(TINT0, &timerISR);
	//clock
		TimerInit(1.0);//set the timer to 1kHz and start. Relies on SysClkInit, so call that first.
	EDIS;//disallow access to system control registers

	//set some initial states
	GpioDataRegs.GPASET.bit.GPIO0 = 1;//drive !SHDN pin high during operation
	GpioDataRegs.GPASET.bit.GPIO31 = 1;//set to make ld2 off; clear to turn on

	while(1) {
		loopcnt++;
	}
}

void timerISR() {//called at 1kHz
	if ((tmrcnt % 1000) == 0) {//every second
		flag ^= 1;//toggle send flag
	}

	if (flag) {
		//GpioSetAll();//works

		//GpioSetPin(31);//turn off leds
		//GpioSetPin(34);//works

		//Uint8 off[2] = {31, 34};//works
		//GpioSetPins(off, 2);
	} else {
		//GpioClearAll();//works

		//GpioClearPin(31);//turn on leds
		//GpioClearPin(34);//works

		//Uint8 on[2] = {31, 34};//works
		//GpioClearPins(on, 2);
	}

	data = GpioGetData(6);//works

	tmrcnt++;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;//resets pie flag--necessary at
}									//end of all interrupts (Group varies)

