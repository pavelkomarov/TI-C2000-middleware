/**
 * @file gpio.c
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief A library that provides an easier way to set up GPIO pins
 * @ingroup Digital
 * @version 2
 *
 * http://solarracing.gatech.edu/wiki/Main_Page
 * This library abstracts GPIO initialization for inputs and outputs,
 * setting GPIO pins low and high, disabling and enabling pin pull-up,
 * and reading GPIO data values.
 *
 * The author recommends knowing how to directly call GpioDataRegs.GPxSET,
 * GPxCLEAR, &c. since it is more efficient to do this for a few GPIO
 * registers directly than to call a function from this library.
 *
 * x is A or B
 * y is 1 or 2
 *
 * GPxMUXy sets pin categories (GPIO or ADC input or PWM or ...)
 * GPxDIR sets whether pins are inputs or outputs
 * GPxPUD enables or disables pin-pullup
 * GPxDAT can be used to read state of an input pin
 * GPxSET sets a gpio output pin high
 * GPxCLEAR sets a gpio output pin low
 *
 * I am using Uint8s because they are more than wide enough to accommodate the
 * few GPIO possibilities
 *
 * I hate all the length parameters, but they are necessarry to tell C how long
 * these variable-length arrays are. C is simplistic like that.
 */

#include "F2806x_Device.h"
#include "gpio.h"
#include <stdlib.h>

Uint32* getMasks(Uint8[], Uint8);//private helpers
Uint32* getSixMasks(Uint8[], Uint8);

//-------------------------initialization

/**
 * Pass in an array of Uint8s corresponding to GPIO pins you wish to make inputs.
 * Note that all optionally-GPIO pins are configured to be GPIO and to be inputs
 * by default, so calling this function might be unnecessary unless you are unsure
 * which registers have already been set or want to be really explicit.
 *
 * @param inputs[] An array of pin-identifying Uint16s
 * @param lenin Length of the input array and a Communist
 */
void GpioInputsInit(Uint8 inputs[], Uint8 lenin) {
	Uint32* masks = getSixMasks(inputs, lenin);

	//GPIO pins are set by making GPxMUXy bits low. Direction is set to 'in'
	//by making GPxMUX bits low. Set low while leaving other bits unchanged by
	//logical ANDing with NOTs. E.g., set second bit low: 1010 & ~0010 = 1000
	GpioCtrlRegs.GPAMUX1.all = GpioCtrlRegs.GPAMUX1.all & ~masks[0];
	GpioCtrlRegs.GPAMUX2.all = GpioCtrlRegs.GPAMUX2.all & ~masks[1];
	GpioCtrlRegs.GPBMUX1.all = GpioCtrlRegs.GPBMUX1.all & ~masks[2];
	GpioCtrlRegs.GPBMUX2.all = GpioCtrlRegs.GPBMUX2.all & ~masks[3];
	GpioCtrlRegs.GPADIR.all = GpioCtrlRegs.GPADIR.all & ~masks[4];
	GpioCtrlRegs.GPBDIR.all = GpioCtrlRegs.GPBDIR.all & ~masks[5];
	free(masks);//do not forget to free
}

/**
 * Not the most computationally-efficient way to do this, but the simplest to
 * code and not so bad since the user will probably only call init functions
 * once. Note the parameter is a raw value this time rather than a pointer, so
 * its address must be passed on to the more general function.
 *
 * @param input A single pin number
 */
void GpioInputInit(Uint8 input) {
	GpioInputsInit(&input, 1);
}

/**
 * Pass in an array of Uint8s corresponding to GPIO pins you wish to make outputs.
 *
 * @param inputs[] An array of pin-identifying Uint16s
 * @param lenout Length of the outputs array
 */
void GpioOutputsInit(Uint8 outputs[], Uint8 lenout) {
	Uint32* masks = getSixMasks(outputs, lenout);

	//Direction is set to 'out' by making GPxDIR bits high. Set locations
	//high by ORing. E.g., 1000 | 0010 = 1010
	GpioCtrlRegs.GPAMUX1.all = GpioCtrlRegs.GPAMUX1.all & ~masks[0];
	GpioCtrlRegs.GPAMUX2.all = GpioCtrlRegs.GPAMUX2.all & ~masks[1];
	GpioCtrlRegs.GPBMUX1.all = GpioCtrlRegs.GPBMUX1.all & ~masks[2];
	GpioCtrlRegs.GPBMUX2.all = GpioCtrlRegs.GPBMUX2.all & ~masks[3];
	GpioCtrlRegs.GPADIR.all = GpioCtrlRegs.GPADIR.all | masks[4];
	GpioCtrlRegs.GPBDIR.all = GpioCtrlRegs.GPBDIR.all | masks[5];
	free(masks);//no memory leakage is key
}

/**
 * Not the most computationally-efficient way to do this, but the simplest to
 * code and not so bad since the user will probably only call init functions
 * once. Note the parameter is a raw value this time rather than a pointer, so
 * its address must be passed on to the more general function.
 *
 * @param output A single pin number
 */
void GpioOutputInit(Uint8 output) {
	GpioOutputsInit(&output, 1);
}

//---------------------------clearing and setting

/**
 * Clears all the GPIO data registers, making all output-pins low.
 * Input-pins are unaffected.
 */
void GpioClearAll() {
	GpioDataRegs.GPADAT.all = 0x00000000;//32 bits
	GpioDataRegs.GPBDAT.all = 0x00000000;
}

/**
 * Clear a group of specific registers without cluttering your code.
 * Slightly less computationally efficient than clearing each directly.
 *
 * @param toClear An array listing the pins to be cleared (made low).
 * @param lenclr Length of the array of pins to clear
 */
void GpioClearPins(Uint8 toClear[], Uint8 lenclr) {
	Uint32* masks = getMasks(toClear, lenclr);

	//Set locations low by ANDing with NOTs. E.g., 1010 & ~0010 = 1000
	GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all & ~masks[0];
	GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all & ~masks[1];
	free(masks);
}

/**
 * Clear a single pin. Note is equally effective and slightly more efficient
 * to simply use GpioDataRegs.GPxCLEAR.bit.GPIOy = 1 in your code.
 *
 * @param toClear A single pin number
 */
void GpioClearPin(Uint8 toClear) {
	Uint32 one = 1;

	if (toClear <= 31) { //GPA
		GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all & ~(one << toClear);
	} else if (toClear >= 32 && toClear <= 58) { //GPB
		GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all & ~(one << (toClear-32));
	}
}

/**
 * Sets all the GPIO data registers, making all output-pins high.
 * Input-pins are unaffected.
 */
void GpioSetAll() {
	GpioDataRegs.GPADAT.all = 0xFFFFFFFF;//32 bits
	GpioDataRegs.GPBDAT.all = 0xFFFFFFFF;
}

/**
 * Set a group of specific registers without cluttering your code. Slightly
 * less efficient than setting each directly.
 *
 * @param toSet An array listing the pins to be set (made high).
 * @param lenSet Length of the array of pins to set
 */
void GpioSetPins(Uint8 toSet[], Uint8 lenset) {
	Uint32* masks = getMasks(toSet, lenset);

	//Set locations high by ORing. E.g., 1000 | 0010 = 1010
	GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all | masks[0];
	GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all | masks[1];
	free(masks);
}

/**
 * Set a single pin. Note is equally effective and slightly more efficient to
 * simply use GpioDataRegs.GPxSET.bit.GPIOy = 1 in your code.
 *
 * @param toSet A single pin number
 */
void GpioSetPin(Uint8 toSet) {
	Uint32 one = 1;

	if (toSet <= 31) { //GPA
		GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all | (one << toSet);
	} else if (toSet >= 32 && toSet <= 58) { //GPB
		GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all | (one << (toSet-32));
	}
}


/**
 * Toggles all the bits in GPIO data registers, making all output-pins
 * the opposite of whatever they were. Input-pins are unaffected.
 */
void GpioToggleAll() {
	GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all ^ 0xFFFFFFFF;//32 bits
	GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all ^ 0xFFFFFFFF;
}

/**
 * Toggle a group of specific registers without cluttering your code. Slightly
 * less efficient than toggling each directly.
 *
 * @param toToggle An array listing the pins to be set (made high).
 * @param lentog Length of the array of pins to set
 */
void GpioTogglePins(Uint8 toToggle[], Uint8 lentog) {
	Uint32* masks = getMasks(toToggle, lentog);

	//Set locations high by ORing. E.g., 1000 | 0010 = 1010
	GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all ^ masks[0];
	GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all ^ masks[1];
	free(masks);
}

/**
 * Toggle a single pin. Note is equally effective and slightly more efficient to
 * simply use GpioDataRegs.GPxTOGGLE.bit.GPIOy = 1 in your code.
 *
 * @param toToggle A single pin number
 */
void GpioTogglePin(Uint8 toToggle) {
	Uint32 one = 1;

	if (toToggle <= 31) { //GPA
		GpioDataRegs.GPADAT.all = GpioDataRegs.GPADAT.all ^ (one << toToggle);
	} else if (toToggle >= 32 && toToggle <= 58) { //GPB
		GpioDataRegs.GPBDAT.all = GpioDataRegs.GPBDAT.all ^ (one << (toToggle-32));
	}
}

//----------------------------getting state

/**
 * This is purely for the sake of completeness. If you know the GPIO pin you want
 * to read from, you should really just use Uint16 value = GpioDataRegs.GPxDAT.bit.GPIOy
 * Note I return a Uint16 to match the fact that this thing ^ is 16 bits.
 *
 * @param pin The pin from which the user wishes to read
 */
Uint16 GpioGetData(Uint8 pin) {
	Uint32 n, one = 1;

	if (pin <= 31) { //GPA
		n = GpioDataRegs.GPADAT.all & (one << pin);//use 32-bit 1 for shifting
	} else if (pin >= 32 && pin <= 58) { //GPB
		n = GpioDataRegs.GPBDAT.all & (one << (pin-32));
	}

	//n is either 0 or 000...010...0, so sum(bits of n) is the answer
	Uint16 bitsum = 0;
	while (n) {
		bitsum += n & 1;
		n >>= 1;
	}
	return bitsum;
}

//------------------------pulling up and floating

/**
 * GPIO pins are pulled-up by default. Use this function to make all
 * pins floating.
 */
void GpioFloatAll() {
	GpioCtrlRegs.GPAPUD.all = 0xFFFFFFFF;//32 bits
	GpioCtrlRegs.GPBPUD.all = 0xFFFFFFFF;
}

/**
 * GPIO pins are pulled-up by default. Use this function to make a group of
 * pins floating.
 *
 * @param toFloat An array listing the pins to be made floating.
 * @param lenflt Length of the array of pins to make floating
 */
void GpioFloatPins(Uint8 toFloat[], Uint8 lenflt) {
	Uint32* masks = getMasks(toFloat, lenflt);

	//Set locations high by ORing. E.g., 1000 | 0010 = 1010
	GpioCtrlRegs.GPAPUD.all = GpioCtrlRegs.GPAPUD.all | masks[0];
	GpioCtrlRegs.GPBPUD.all = GpioCtrlRegs.GPBPUD.all | masks[1];
	free(masks);
}

/**
 * Make a GPIO pin floating rather than pulled-up, as all are by default.
 * Note is equally effective and slightly more efficient to simply use
 * GpioCtrlRegs.GPxPUD.bit.GPIOy = 1 in your code.
 *
 * @param toFloat A single pin number
 */
void GpioFloatPin(Uint8 toFloat) {
	Uint32 one = 1;

	if (toFloat <= 31) { //GPA
		GpioCtrlRegs.GPAPUD.all = GpioCtrlRegs.GPAPUD.all | (one << toFloat);
	} else if (toFloat >= 32 && toFloat <= 58) { //GPB
		GpioCtrlRegs.GPBPUD.all = GpioCtrlRegs.GPBPUD.all | (one << (toFloat-32));
	}
}


/**
 * GPIO inputs are pulled-up (connected to a VCC through a resistor) by default.
 * Use this function to re-reach that state.
 */
void GpioPullUpAll() {
	GpioCtrlRegs.GPAPUD.all = 0x00000000;//32 bits
	GpioCtrlRegs.GPBPUD.all = 0x00000000;
}

/**
 * GPIO inputs are pulled-up by default, so you probably would not want to use this
 * unless you earlier used GpioFloatPins and then changed your mind. I include it only
 * for symmetry and completeness. Use this function to pull-up a group of GPIO pins.
 *
 * @param toPullUp An array listing the pins to be pulled-up.
 * @param lenplup Length of the array of pins to be pulled-up
 */
void GpioPullUpPins(Uint8 toPullUp[], Uint8 lenplup) {
	Uint32* masks = getMasks(toPullUp, lenplup);

	//Set locations low by ANDing with NOTs. E.g., 1010 & ~0010 = 1000
	GpioCtrlRegs.GPAPUD.all = GpioCtrlRegs.GPAPUD.all & ~masks[0];
	GpioCtrlRegs.GPBPUD.all = GpioCtrlRegs.GPBPUD.all & ~masks[1];
	free(masks);
}

/**
 * Make a GPIO input pulled-up rather than floating. Note is equally effective and
 * slightly more efficient to simply use GpioCtrlRegs.GPxPUD.bit.GPIOy = 0 in your
 * code.
 *
 * @param toSet A single pin number
 */
void GpioPullUpPin(Uint8 toPullUp) {
	Uint32 one = 1;

	if (toPullUp <= 31) {//GPA
		GpioCtrlRegs.GPAPUD.all = GpioCtrlRegs.GPAPUD.all & ~(one << toPullUp);
	} else if (toPullUp >= 32 && toPullUp <= 58) { //GPB
		GpioCtrlRegs.GPBPUD.all = GpioCtrlRegs.GPBPUD.all & ~(one << (toPullUp-32));
	}
}

//----------------------------private helper functions

/**
 * Returns a pointer to an array of bitmasks to be used in setting a pair
 * of registers.
 *
 * @param pins The pins affected
 * @param len The length of the pins array
 */
Uint32* getMasks(Uint8 pins[], Uint8 len) {
	Uint16 i, pin;
	Uint32 ax = 0, bx = 0;//the masks
	Uint32 one = 1;//for bitshifting

	for (i = 0; i < len; i++) {
		pin = pins[i];
		if (pin <= 31) { //GPA
			ax = ax | (one << pin);//position a 1 over a key location
		} else if (pin >= 32 && pin <= 58) { //GPB
			bx = bx | (one << (pin-32));
		}
	}

	Uint32* masks = malloc(sizeof(Uint32)*2);
	masks[0] = ax; masks[1] = bx;
	return masks;
}

/**
 * Returns full set of masks fitting register-sets of sizes both two and four.
 * Called by the Init functions because they need masks for the MUXes (to set
 * pins to be GPIO rather than something else) as well as for the DIRs
 * (directions: input or output).
 *
 * @param pins The pins affected
 * @param len The length of the pins array
 */
Uint32* getSixMasks(Uint8 pins[], Uint8 len) {
	Uint8 i, pin;
	Uint32 a1 = 0, a2 = 0, b1 = 0, b2 = 0, ax = 0, bx = 0;//the masks
	Uint32 one = 1, three = 3;//because bitshifting bare numbers is like bitshifting int16s (terrible)

	for (i = 0; i < len; i++) {
		pin = pins[i];
		if (pin <= 31) { //GPA
			if (pin < 16) {	//MUX1
				a1 = a1 | (three << (pin*2));//position two 1s properly
			} else { //MUX2
				a2 = a2 | (three << ((pin-16)*2));//multiply by 2 to move two 1s correctly
			}
			ax = ax | (one << pin);//position a 1 properly
		} else if (pin >= 32 && pin <= 58) { //GPB highest GPIO is 58
			if (pin < 48) {	//MUX1
				b1 = b1 | (three << ((pin-32)*2));
			} else { //MUX2
				b2 = b2 | (three << ((pin-48)*2));
			}
			bx = bx | (one << (pin-32));//subtract to account for bits stored in lower registers
		}
	}

	Uint32* masks = malloc(sizeof(Uint32)*6);
	masks[0] = a1; masks[1] = a2; masks[2] = b1;
	masks[3] = b2; masks[4] = ax; masks[5] = bx;
	return masks;
}
