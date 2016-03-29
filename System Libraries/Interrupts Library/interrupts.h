
/*
 * INTRPT lists a number of kinds of interrupt. See Table 1-118 on
 * page 173 of the Technical Reference Manual or interrupts.png for
 * a complete list. The code required to set up an interrupt from
 * each of these sources is kept inside cases in interrupts.c
 */
#ifndef INTRPT_DEFINED//encased in this thing so that it plays
#define INTRPT_DEFINED//nicely with ADC Library without dependency
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

void IsrInit(INTRPT, void (*ISR)(void));
void IsrAck(INTRPT);
