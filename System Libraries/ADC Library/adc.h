/**
 * @author Pavel Komarov <pvlkmrv@gmail.com> 941-545-7573
 * @version 1
 *
 * I went a little ham with the enums. Sorry not sorry.
 */

//#include "interrupts.h"

/**
 * Used to set the ADC module clock as a fraction
 * of the system clock.
 */
typedef enum {
    WHOLE,	HALF,	FOURTH
} DIVISOR;

/**
 * Specify which Start-Of-Conversion (soc) module you are
 * using. It's arbitrary and up to you, the user.
 */
typedef enum {
	SOC0 = 0,	SOC1 = 1,	SOC2 = 2,
	SOC3 = 3,	SOC4 = 4,	SOC5 = 5,
	SOC6 = 6,	SOC7 = 7,	SOC8 = 8,
	SOC9 = 9,	SOC10 = 10,	SOC11 = 11,
	SOC12 = 12,	SOC13 = 13,	SOC14 = 14,
	SOC15 = 15
} SOC;

/**
 * Specify the channel (pin) to which an soc is
 * connected. Depends on the pin you use as input.
 */
typedef enum {
    ADCA0 = 0,	ADCA1 = 1,	ADCA2 = 2,
    ADCA3 = 3,	ADCA4 = 4,	ADCA5 = 5,
    ADCA6 = 6,	ADCA7 = 7,	ADCB0 = 8,
    ADCB1 = 9,	ADCB2 = 10,	ADCB3 = 11,
    ADCB4 = 12,	ADCB5 = 13,	ADCB6 = 14,
    ADCB7 = 15
} CHANNEL;

/**
 * Specify the event that sets off a Start-Of-Convsion.
 * User choice.
 */
typedef enum {
	SOFTWARE = 0,	TINT0t = 1,		TINT1 = 2,//goofy t postpension is goofy,
	TINT2 = 3,		XINT2t = 4,		PWM1A = 5,//but C does not allow
	PWM1B = 6,		PWM2A = 7,		PWM2B = 8,//duplicate enum names, and
	PWM3A = 9,		PWM3B = 10,		PWM4A = 11,//INTRPT already has a few of
	PWM4B = 12,		PWM5A = 13,		PWM5B = 14,//of these. t is for "trigger"
	PWM6A = 15,		PWM6B = 16,		PWM7A = 17,
	PWM7B = 18,		PWM8A = 19,		PWM8B = 20
} TRIGGER;

/**
 * Avoid dependency on Interrupts Library. I have to repeat myself a little
 * here, but it averts a potentially frustrating-to-compile chained linkage
 * as well as the confusion of not having to include interrupts.h when
 * adc.h is included.
 */
#ifndef INTRPT_DEFINED//This is the first time I have ever used #ifndef. It's cool.
#define INTRPT_DEFINED
typedef enum {
    TINT0,		ADCINT1,	ADCINT2,
    ADCINT3,	ADCINT4,	ADCINT5,
    ADCINT6,	ADCINT7,	ADCINT8,
    ADCINT9,	WAKEINT,	XINT1,
    XINT2,		XINT3,		EPWM1,
    EPWM2,		EPWM3,		EPWM4,
    EPWM5,		EPWM6,		EPWM7,
    EPWM8,		ECAN0,		ECAN1,
    SCIARX,		SCIATX,		SCIBRX,
    SCIBTX,		SPIARX,		SPIATX,
    SPIBRX,		SPIBTX
} INTRPT;
#endif

void AdcInit(void);
void AdcSetClock(DIVISOR);
void AdcSetupSOC(SOC, CHANNEL, TRIGGER);
void AdcEnableIsr(SOC, INTRPT);
void AdcStartMeas(SOC);
float32 AdcRes(SOC);
