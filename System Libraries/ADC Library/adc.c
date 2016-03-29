/**
 * @file adc.c
 * @author Pavel Komarov <pvlkmrv@gmail.edu> 941-545-7573
 * @brief A library that provides an easy way to configure and use analog inputs
 * @ingroup Digital
 * @version 2
 *
 * http://solarracing.gatech.edu/wiki/Main_Page
 * The ADC works with units called socs (Starts of Conversions). To use one,
 * you have to specify which you want to use, what it is listening to (the
 * channel), and what kind of event will cause it to start converting (the
 * trigger). After a conversion is begun, the result will be accessible in
 * the soc's corresponding result register.
 *
 * The result is a raw number between 0 and 4095 (12 bits), so you will have
 * to be aware of the internal circuitry and do some math to convert this to
 * a voltage or a current. For example,
 *
 * Uint16 meas = AdcResult.ADCRESULT0;
 * float32 y = 3.3*meas/4095;
 *
 * will yield y in Volts between 0 and 3.3, but this might not match the actual,
 * external voltage being measured. "An interface circuit performs scaling and
 * shifting so that the sensor output signal is translated into an appropriately
 * small voltage range." This interface circuit is in the hands of the user:
 * it is whatever they decide to build between the system and the ADC input pin.
 * By incorporating resistors into this interface circuit, one can measure
 * current. In 4550 we used this to check for over-current conditions. Examples:
 *
 * float32 i = 6.060727*(3.3*meas)/4096 - 9.93964;
 * float32 i = AdcResult.ADCRESULT0*3.3/(4.9974359*4095);
 *
 * where the strange-looking decimals are the results of combining many
 * resistances.
 *
 *
 * To enable interrupts from the ADC, the user must enable them from both
 * the ADC side and the PIE-vector-table side.
 *
 *
 * Note that ADC pins can be made digital I/Os (like GPIO) with
 * GpioCtrlRegs.AIOMUX1.bit.AIO2 = 0; But there are so many digital
 * GPIOs you will probably never need to do this.
 */

#include "F2806x_Device.h"
#include "F2806x_usDelay.h"
#include "adc.h"

/**
 * Power on all the necessities of the ADC module. This stuff is common
 * to all soc-units.
 */
void AdcInit() {
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;//enable the clock
	asm(" NOP"); asm(" NOP");//preprended space is mandatory

	//initialize and enable ADC
	AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;//"INT pulse generation occurs 1 cycle prior to ADC
									//result latching into its result register." Id est,
									//interrupts are thrown at the EOC (End Of Conversion).
	AdcRegs.ADCCTL1.bit.ADCBGPWD = 1;//"Bandgap buffer's curcuitry inside core is powered up"
	AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;//"Reference buffers circuitry inside the core is powered up"
	AdcRegs.ADCCTL1.bit.ADCPWDN = 1;//"The analog circuitry inside the core is powered up"
	AdcRegs.ADCCTL1.bit.ADCENABLE = 1;//enable the ADC
	DSP28x_usDelay(1000);//wait a millisecond
	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;//"Overlap of sample is not allowed"
	AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;//set fadc = fclk/4 by default
	AdcRegs.ADCCTL2.bit.CLKDIV4EN = 1;
}

/**
 * Sets fadc to be either fclk, fclk/2, or fclk/4. Note that the init() function
 * sets the clock speed to be fclk/4 (slow) by default to save power.
 *
 * @param div A DIVISOR enum describing the magnitude fadc/fclck
 */
void AdcSetClock(DIVISOR div) {
	switch (div) {
		case WHOLE:
			AdcRegs.ADCCTL2.bit.CLKDIV2EN = 0;//fadc = fclk
			AdcRegs.ADCCTL2.bit.CLKDIV4EN = 0;
			break;
		case HALF:
			AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;//fadc = fclk/2
			AdcRegs.ADCCTL2.bit.CLKDIV4EN = 0;
			break;
		case FOURTH:
			AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;//fadc = fclk/4
			AdcRegs.ADCCTL2.bit.CLKDIV4EN = 1;
			break;
	}
}

/**
 * Set up the channel to which an soc is connected as well as
 * which event will trigger it.
 *
 * @param pin An enum specifying which soc you are talking about
 * @param chan An enum specifying which channel you want to connect to the soc
 * @param trig An enum specifying the even that begins the soc
 */
void AdcSetupSOC(SOC soc, CHANNEL chan, TRIGGER trig) {
	switch (soc) {
		case SOC0:
			AdcRegs.ADCSOC0CTL.bit.CHSEL = chan;//Set socx to listen to ADCINyx
			AdcRegs.ADCSOC0CTL.bit.TRIGSEL = trig;//Set soc to occur upon event
			AdcRegs.ADCSOC0CTL.bit.ACQPS = 6;//"Sample window is 7 cycles long" (as brief as possible)
			break;
		case SOC1:
			AdcRegs.ADCSOC1CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC1CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC1CTL.bit.ACQPS = 6;
			break;
		case SOC2:
			AdcRegs.ADCSOC2CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC2CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC2CTL.bit.ACQPS = 6;
			break;
		case SOC3:
			AdcRegs.ADCSOC3CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC3CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC3CTL.bit.ACQPS = 6;
			break;
		case SOC4:
			AdcRegs.ADCSOC4CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC4CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC4CTL.bit.ACQPS = 6;
			break;
		case SOC5:
			AdcRegs.ADCSOC5CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC5CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC5CTL.bit.ACQPS = 6;
			break;
		case SOC6:
			AdcRegs.ADCSOC6CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC6CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC6CTL.bit.ACQPS = 6;
			break;
		case SOC7:
			AdcRegs.ADCSOC7CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC7CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC7CTL.bit.ACQPS = 6;
			break;
		case SOC8:
			AdcRegs.ADCSOC8CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC8CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC8CTL.bit.ACQPS = 6;
			break;
		case SOC9:
			AdcRegs.ADCSOC9CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC9CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC9CTL.bit.ACQPS = 6;
			break;
		case SOC10:
			AdcRegs.ADCSOC10CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC10CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC10CTL.bit.ACQPS = 6;
			break;
		case SOC11:
			AdcRegs.ADCSOC11CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC11CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC11CTL.bit.ACQPS = 6;
			break;
		case SOC12:
			AdcRegs.ADCSOC12CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC12CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC12CTL.bit.ACQPS = 6;
			break;
		case SOC13:
			AdcRegs.ADCSOC13CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC13CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC13CTL.bit.ACQPS = 6;
			break;
		case SOC14:
			AdcRegs.ADCSOC14CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC14CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC14CTL.bit.ACQPS = 6;
			break;
		case SOC15:
			AdcRegs.ADCSOC15CTL.bit.CHSEL = chan;
			AdcRegs.ADCSOC15CTL.bit.TRIGSEL = trig;
			AdcRegs.ADCSOC15CTL.bit.ACQPS = 6;
			break;
	}
}

/**
 * This should be called after AdcInit(), lest the registers herein
 * not actually be set. Note that this alone does not set up interrupts
 * from the ADC; this just makes the necessary connections on the ADC-
 * module end. An additional call with the Interrupts Library is required
 * to register an interrupt with the PIE vector table and set up
 * connections on that side.
 *
 * @param soc The soc module in question
 * @param intrpt The ADCINTx you want the soc to throw
 */
void AdcEnableIsr(SOC soc, INTRPT type) {
	switch (type) {
		case ADCINT1:
			AdcRegs.INTSEL1N2.bit.INT1SEL = soc;//set socx to throw to interrupt to ADCINTy
			AdcRegs.INTSEL1N2.bit.INT1E = 1;//at its EOC (End Of Conversion)
			break;
		case ADCINT2:
			AdcRegs.INTSEL1N2.bit.INT2SEL = soc;
			AdcRegs.INTSEL1N2.bit.INT2E = 1;//Enable interrupt from ADCINTy
			break;
		case ADCINT3:
			AdcRegs.INTSEL3N4.bit.INT3SEL = soc;
			AdcRegs.INTSEL3N4.bit.INT3E = 1;
			break;
		case ADCINT4:
			AdcRegs.INTSEL3N4.bit.INT4SEL = soc;
			AdcRegs.INTSEL3N4.bit.INT4E = 1;
			break;
		case ADCINT5:
			AdcRegs.INTSEL5N6.bit.INT5SEL = soc;
			AdcRegs.INTSEL5N6.bit.INT5E = 1;
			break;
		case ADCINT6:
			AdcRegs.INTSEL5N6.bit.INT6SEL = soc;
			AdcRegs.INTSEL5N6.bit.INT6E = 1;
			break;
		case ADCINT7:
			AdcRegs.INTSEL7N8.bit.INT7SEL = soc;
			AdcRegs.INTSEL7N8.bit.INT7E = 1;
			break;
		case ADCINT8:
			AdcRegs.INTSEL7N8.bit.INT8SEL = soc;
			AdcRegs.INTSEL7N8.bit.INT8E = 1;
			break;
		case ADCINT9:
			AdcRegs.INTSEL9N10.bit.INT9SEL = soc;
			AdcRegs.INTSEL9N10.bit.INT9E = 1;
			break;
	}
}

/**
 * I predict that calling ADC conversions from software might be fairly
 * commonplace, so I've encapsulated it. Note that if you have set to
 * trigger on anything aside from software, you will not need to use this.
 *
 * @param soc An soc denoting which system should be directed to begin a measurement
 */
void AdcStartMeas(SOC soc) {
	switch (soc) {
		case SOC0:
			AdcRegs.ADCSOCFRC1.bit.SOC0 = 1;
			break;
		case SOC1:
			AdcRegs.ADCSOCFRC1.bit.SOC1 = 1;
			break;
		case SOC2:
			AdcRegs.ADCSOCFRC1.bit.SOC2 = 1;
			break;
		case SOC3:
			AdcRegs.ADCSOCFRC1.bit.SOC3 = 1;
			break;
		case SOC4:
			AdcRegs.ADCSOCFRC1.bit.SOC4 = 1;
			break;
		case SOC5:
			AdcRegs.ADCSOCFRC1.bit.SOC5 = 1;
			break;
		case SOC6:
			AdcRegs.ADCSOCFRC1.bit.SOC6 = 1;
			break;
		case SOC7:
			AdcRegs.ADCSOCFRC1.bit.SOC7 = 1;
			break;
		case SOC8:
			AdcRegs.ADCSOCFRC1.bit.SOC8 = 1;
			break;
		case SOC9:
			AdcRegs.ADCSOCFRC1.bit.SOC9 = 1;
			break;
		case SOC10:
			AdcRegs.ADCSOCFRC1.bit.SOC10 = 1;
			break;
		case SOC11:
			AdcRegs.ADCSOCFRC1.bit.SOC11 = 1;
			break;
		case SOC12:
			AdcRegs.ADCSOCFRC1.bit.SOC12 = 1;
			break;
		case SOC13:
			AdcRegs.ADCSOCFRC1.bit.SOC13 = 1;
			break;
		case SOC14:
			AdcRegs.ADCSOCFRC1.bit.SOC14 = 1;
			break;
		case SOC15:
			AdcRegs.ADCSOCFRC1.bit.SOC15 = 1;
			break;
	}
}


/**
 * Read a result in Volts. Performs conversion from [0, 4095]
 * to [0.0, 3.3] V.
 *
 * @param soc An SOC denoting which system did the conversion
 */
float32 AdcRes(SOC soc) {
	switch (soc) {
		case SOC0:
			return 3.3*AdcResult.ADCRESULT0/4095;
		case SOC1:
			return 3.3*AdcResult.ADCRESULT1/4095;
		case SOC2:
			return 3.3*AdcResult.ADCRESULT2/4095;
		case SOC3:
			return 3.3*AdcResult.ADCRESULT3/4095;
		case SOC4:
			return 3.3*AdcResult.ADCRESULT4/4095;
		case SOC5:
			return 3.3*AdcResult.ADCRESULT5/4095;
		case SOC6:
			return 3.3*AdcResult.ADCRESULT6/4095;
		case SOC7:
			return 3.3*AdcResult.ADCRESULT7/4095;
		case SOC8:
			return 3.3*AdcResult.ADCRESULT8/4095;
		case SOC9:
			return 3.3*AdcResult.ADCRESULT9/4095;
		case SOC10:
			return 3.3*AdcResult.ADCRESULT10/4095;
		case SOC11:
			return 3.3*AdcResult.ADCRESULT11/4095;
		case SOC12:
			return 3.3*AdcResult.ADCRESULT12/4095;
		case SOC13:
			return 3.3*AdcResult.ADCRESULT13/4095;
		case SOC14:
			return 3.3*AdcResult.ADCRESULT14/4096;
		case SOC15:
			return 3.3*AdcResult.ADCRESULT15/4095;
		default:
			return 0;//to silence a warning
	}
}

