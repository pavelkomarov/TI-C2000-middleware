/**
 * @author Drew Harris, soon to be refined by Pavel K
 * @version 0
 */

#include "F2806x_Device.h"

//Initialize SPI module to use GPIOs 16,17,18,19
//GPIO 16 = SIMO
//GPIO 17 = SOMI
//GPIO 18 = CLK
//GPIO 19 = STE
void SpiInit()
{
	// Initialize SPI Clocking
	SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;
	asm(" NOP");     asm(" NOP");     // Wait 2 clock cycles

	/* Enable internal pull-up for the selected pins */
	//   GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;    // Enable pull-up on GPIO3 (SPISOMIA)
	//   GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;    // Enable pull-up on GPIO5 (SPISIMOA)
	GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19 (SPISTEA)

	/* Set qualification for selected pins to ASYNC only */
	// This will select asynch (no qualification) for the selected pins.
	//   GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;  // Asynch input GPIO3 (SPISOMIA)
	//   GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;  // Asynch input GPIO5 (SPISIMOA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

	/* Configure SPI-A pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be SPI functional pins.
	//   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;  // Configure GPIO3 as SPISOMIA
	//   GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 2;  // Configure GPIO5 as SPISIMOA
	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; 	// Configure GPIO16 as SPISIMOA
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; 	// Configure GPIO17 as SPISOMIA
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; 	// Configure GPIO18 as SPICLKA
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; 	// Configure GPIO19 as SPISTEA, or ChipSelect

	/* Configuring SIMO, CLK, CS as Output pins, SOMI as Input pin */
	GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;

	/* Our SPI configuration does not utilize FIFO mode. Instead, use regular SPI interrupts and communication mode.
	 * The following configuration was based upon page 839 and 849 of Technical Reference Manual.
	 */
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;	//Enabling Sequence Step 1
	SpiaRegs.SPIBRR = 24;					//Step 2, Set Baud Rate to 900kHz
	SpiaRegs.SPICCR.bit.SPILBK = 0;		//Disable Loopback Mode;
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;	//Enabling Master Mode on microcontroller
	SpiaRegs.SPICTL.bit.TALK = 1;
	SpiaRegs.SPICCR.bit.SPICHAR = 7;		//Transmit 8 bits per clock cycle.
	SpiaRegs.SPIFFTX.bit.SPIFFENA = 0;		//As Described on page 849 of Manual, in order to trigger SPIINT w/o FIFO, this bit needs to be set to 0.
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;	//Demanded by LTC6803-3 datasheet, page 17, under "Clock Phase And Polarity". Both Phase and Polarity need to be set to 1 in order to initiate communication.
	SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
	SpiaRegs.SPICCR.bit.SPISWRESET = 1;  	//Step 3, Enable SPI
	SpiaRegs.SPIPRI.bit.FREE = 1;
}
