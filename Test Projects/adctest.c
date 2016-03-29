/**
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief This project tests the ADC Library
 * @ingroup Digital
 * @version 0
 *
 * This project tests the functionality of the ADC Library
 */
#include "F2806x_Device.h"
#include "28069Common.h"
#include "clocks.h"
#include "gpio.h"
#include "interrupts.h"
#include "adc.h"
//Also relies on 28069_RAM_lnk.cmd, F2806x_CodeStartBranch.asm,
//F2806x_Headers_nonBIOS.cmd
//targetConfig is "TMS320F28069.ccxml"

interrupt void timerISR(void);
interrupt void adcISR(void);

Uint32 loopcnt = 0, tmrcnt = 0, adccnt = 0;
float32 result[900];

//Code blocks in the preamble are labelled with the names
//of the libraries in which functions-used-therein are defined.
//Look in these to discover fundamental definitions.
void main(void) {

	EALLOW;//an assembly language thing required to allow access to system control registers
	SysCtrlRegs.WDCR = 0x68;//disable watchdog
	//clock
		SysClkInit(FIFTY);//set the system clock to 50MHz
	//gpio
		Uint8 out[3] = {31, 34};//0 is !shdn on wifi
		GpioOutputsInit(out, 2);	//34 is a pin that goes high/low each second
	//interrupts				//31 is ld2: lights when wifi is sending
		IsrInit(TINT0, &timerISR);
		IsrInit(ADCINT2, &adcISR);
	//adc
		AdcInit();
		AdcSetClock(FOURTH);//the ADC clock is 1/4 the speed of the system clock
		AdcSetupSOC(SOC3, ADCB6, SOFTWARE);//SOC3 is set to listen to the ADCB6 pin when directed by software
		AdcEnableIsr(SOC3, ADCINT2);//SOC3 will throw ADCINT2 interrupts as we told the PIE vector table
	//clock
		TimerInit(1.0);//set the timer to 1kHz and start. Relies on SysClkInit, so call that first.
	EDIS;//disallow access to system control registers

	while(1) {//loop forever waiting for interrupts
		loopcnt++;//in a not-test project, something more useful may be done here
	}
}

void timerISR() {//called at 1kHz
	if ((tmrcnt % 10) == 0) {//every second
		AdcStartMeas(SOC3);
	}

	if ((tmrcnt % 1000) == 0) {
		GpioToggleAll();
	}

	tmrcnt++;
	IsrAck(TINT0);//reset PIE flag
}

interrupt void adcISR(void) {
	result[adccnt++ % 900] = AdcRes(SOC3);
	IsrAck(ADCINT2);//reset PIE and ADC flags
}
