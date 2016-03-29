/**
 * @file clocks.c
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief A library that provides easy ways to set system clocks
 * @ingroup Digital
 * @version 1
 *
 * http://solarracing.gatech.edu/wiki/Main_Page
 */
#include "F2806x_Device.h"
#include "clocks.h"

/*
 * divsel and div make setting these with an FCLKS as trivial as single operations
 *
 * see Table 1-24 in the Technical reference manual for why these values are as they are.
 */
Uint16 divsel[36] = {1, 2, 1, 3, 1, 2,//2.5-15
					 1, 3, 1, 2, 1, 3,//17.5-30
					 1, 2, 1, 3, 1, 2,//32.5-45
					 3, 2, 3, 2, 3, 2,//50-75
					 3, 2, 3, 3, 3, 3,//80-120
					 3, 3, 3, 3, 3, 3};//130-180

Uint16 div[36] =    {1, 1, 3, 1, 5, 3,//2.5-15
					 7, 2, 9, 5, 11,3,//17.5-30
					 13,7, 15,4, 17,9,//32.5-45
					 5, 11,6, 13,7, 15,//50-75
					 8, 17,9, 10,11,12,//80-120
					 13,14,15,16,17,18};//130-180

//possible clock frequencies in MHz
float32 xfclks[36]= {2.5, 5, 7.5, 10, 12.5, 15,
					 17.5, 20, 22.5, 25, 27.5, 30,
					 32.5, 35, 37.5, 40, 42.5, 45,
					 50, 55, 60, 65, 70, 75,
					 80, 85, 90, 100, 110, 120,
					 130, 140, 150, 160, 170, 180};

float32 xfclk;//for saving the the current clock frequency (in MHz)
float32 xftmr;//for saving current timer frequency (in kHz)

/*
 * SysClkInit sets the system clock in MHz. The system clock can only take
 * on a few values, multiples of fosc (the frequency of the oscillator used).
 * So, an enum called FCLKS has been defined to aid the user in choosing a
 * frequency. Note the default oscillator has a frequency of 10MHz; if a
 * different oscillator is used, then this function will set fclk to
 * (new fosc)/10MHz*(number of MHz specified with FLCKS enum).
 *
 * @param fclk An enum of type FCLKS describing the desired clock frequency in MHz
 */
void SysClkInit(FCLKS fclk) {
	xfclk = xfclks[fclk];

	//following lines are from page 82,83,84,85 of Tech ref man
	if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0) {//1. if not in normal operation mode, exit
		return;
	}
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;//2. set DIVSEL to 0
	SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;//3. turn off failed osc detection
	SysCtrlRegs.PLLCR.bit.DIV = div[fclk];//4. set DIV to be some new value
	while (SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1){//5. wait for PLLLOCKS to be 1
		continue;
	}
	SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;//6. reenable failed osc detection
	SysCtrlRegs.PLLSTS.bit.DIVSEL = divsel[fclk];//7. set DIVSEL
}

/**
 * TimerInit sets the system timer, which controls the rate of SPI among
 * other systems. Note this function relies upon knowing the clock frequency,
 * which is only stored when fclk is set by the user. So, this function
 * should be called after SysClkInit(FCLKS fclk).
 *
 * @param ftmr A float describing the desired timer frequency in kHz
 */
void TimerInit(float32 ftmr) {
	xftmr = ftmr;

	//use current fclk to calculate proper divisor so ftmr is as desired
	Uint32 prd = xfclk*1000/ftmr -1;//multiply by 1000 to put xfclk in kHz

	CpuTimer0Regs.TCR.bit.TSS = 1;//stop the timer
	CpuTimer0Regs.TCR.bit.TIE = 1;//enable timer-triggered interrupts
	CpuTimer0Regs.PRD.all = prd;//should set to appropriate value
	CpuTimer0Regs.TPR.all = 0;//the other divisor
	CpuTimer0Regs.TCR.bit.TSS = 0;//start the timer
}

/**
 * @return The current system clock frequency in MHz.
 */
float32 getfclk() {
	return xfclk;
}
