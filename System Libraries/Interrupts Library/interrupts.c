/**
 * @file interrupts.c
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573
 * @brief A library makes setting up common interrupts simple
 * @ingroup Digital
 * @version 2
 *
 * http://solarracing.gatech.edu/wiki/Main_Page
 * There are many, many kinds of interrupt, so here I have only covered
 * the ones I think are most common. I encourage later contributors to expand.
 *
 * Note that this library only sets Peripheral Interrupt Expansion registers;
 * various initialization registers will often still need to be set in other
 * modules to make interrupts work.
 */
#include "F2806x_Device.h"
#include "interrupts.h"

Uint8 called = 0;//keep track of whether IsrInit has already been called

/**
 * @param type An INTRPT enum describing which system will trigger the ISR
 * @param *ISR A function pointer to an interrupt service routine
 */
void IsrInit(INTRPT type, void (*ISR)(void)) {

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;//allow vectors to be fetched from the PIE vector table

	switch (type) {
		case TINT0:
			PieVectTable.TINT0 = ISR;//add interrupts to table
			PieCtrlRegs.PIEIER1.bit.INTx7 = 1;//1.7
			IER = (called) ? IER | M_INT1 : M_INT1;//connect the 1 path. If this is the first
			break;								//call, then just set IER; otherwise OR it to
		case ADCINT1:							//preserve connections made in former calls.
			PieVectTable.ADCINT1 = ISR;
			PieCtrlRegs.PIEIER1.bit.INTx1 = 1;//1.1 <-Table 1-118 row.column
			IER = (called) ? IER | M_INT1 : M_INT1;
			break;
		case ADCINT2:
			PieVectTable.ADCINT2 = ISR;
			PieCtrlRegs.PIEIER1.bit.INTx2 = 1;//1.2
			IER = (called) ? IER | M_INT1 : M_INT1;
			break;
		case ADCINT3:
			PieVectTable.ADCINT3 = ISR;
			PieCtrlRegs.PIEIER10.bit.INTx3 = 1;//10.3
			IER = (called) ? IER | M_INT10 : M_INT10;//connect the 10 path
			break;
		case ADCINT4:
			PieVectTable.ADCINT4 = ISR;
			PieCtrlRegs.PIEIER10.bit.INTx4 = 1;//10.4
			IER = (called) ? IER | M_INT10 : M_INT10;
			break;
		case ADCINT5:
			PieVectTable.ADCINT5 = ISR;
			PieCtrlRegs.PIEIER10.bit.INTx5 = 1;//10.5
			IER = (called) ? IER | M_INT10 : M_INT10;
			break;
		case ADCINT6:
			PieVectTable.ADCINT6 = ISR;
			PieCtrlRegs.PIEIER10.bit.INTx6 = 1;//10.6
			IER = (called) ? IER | M_INT10 : M_INT10;
			break;
		case ADCINT7:
			PieVectTable.ADCINT7 = ISR;
			PieCtrlRegs.PIEIER10.bit.INTx7 = 1;//10.7
			IER = (called) ? IER | M_INT10 : M_INT10;
			break;
		case ADCINT8:
			PieVectTable.ADCINT8 = ISR;
			PieCtrlRegs.PIEIER10.bit.INTx8 = 1;//10.8
			IER = (called) ? IER | M_INT10 : M_INT10;
			break;
		case ADCINT9:
			PieVectTable.ADCINT9 = ISR;
			PieCtrlRegs.PIEIER1.bit.INTx6 = 1;//1.6
			IER = (called) ? IER | M_INT1 : M_INT1;
			break;
		case WAKEINT:
			PieVectTable.WAKEINT = ISR;
			PieCtrlRegs.PIEIER1.bit.INTx8 = 1;//1.8
			IER = (called) ? IER | M_INT1 : M_INT1;
			break;
		case XINT1:
			PieVectTable.XINT1 = ISR;
			PieCtrlRegs.PIEIER1.bit.INTx4 = 1;//1.4
			IER = (called) ? IER | M_INT1 : M_INT1;
			break;
		case XINT2:
			PieVectTable.XINT2 = ISR;
			PieCtrlRegs.PIEIER1.bit.INTx5 = 1;//1.5
			IER = (called) ? IER | M_INT1 : M_INT1;
			break;
		case XINT3:
			PieVectTable.XINT3 = ISR;
			PieCtrlRegs.PIEIER12.bit.INTx1 = 1;//12.1
			IER = (called) ? IER | M_INT12 : M_INT12;
			break;
		case EPWM1:
			PieVectTable.EPWM1_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//3.1
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM2:
			PieVectTable.EPWM2_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx2 = 1;//3.2
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM3:
			PieVectTable.EPWM3_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx3 = 1;//3.3
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM4:
			PieVectTable.EPWM4_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx4 = 1;//3.4
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM5:
			PieVectTable.EPWM5_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx5 = 1;//3.5
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM6:
			PieVectTable.EPWM6_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx6 = 1;//3.6
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM7:
			PieVectTable.EPWM7_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx7 = 1;//3.7
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case EPWM8:
			PieVectTable.EPWM8_INT = ISR;
			PieCtrlRegs.PIEIER3.bit.INTx8 = 1;//3.8
			IER = (called) ? IER | M_INT3 : M_INT3;
			break;
		case ECAN0:
			PieVectTable.ECAN0INTA = ISR;
			PieCtrlRegs.PIEIER9.bit.INTx5 = 1;//9.5
			IER = (called) ? IER | M_INT9 : M_INT9;
			break;
		case ECAN1:
			PieVectTable.ECAN1INTA = ISR;
			PieCtrlRegs.PIEIER9.bit.INTx6 = 1;//9.6
			IER = (called) ? IER | M_INT9 : M_INT9;
			break;
		case SCIARX:
			PieVectTable.SCIRXINTA = ISR;
			PieCtrlRegs.PIEIER9.bit.INTx1 = 1;//9.1
			IER = (called) ? IER | M_INT9 : M_INT9;
			break;
		case SCIATX:
			PieVectTable.SCITXINTA = ISR;
			PieCtrlRegs.PIEIER9.bit.INTx2 = 1;//9.2
			IER = (called) ? IER | M_INT9 : M_INT9;
			break;
		case SCIBRX:
			PieVectTable.SCIRXINTB = ISR;
			PieCtrlRegs.PIEIER9.bit.INTx3 = 1;//9.3
			IER = (called) ? IER | M_INT9 : M_INT9;
			break;
		case SCIBTX:
			PieVectTable.SCITXINTB = ISR;
			PieCtrlRegs.PIEIER9.bit.INTx4 = 1;//9.4
			IER = (called) ? IER | M_INT9 : M_INT9;
			break;
		case SPIARX:
			PieVectTable.SPIRXINTA = ISR;
			PieCtrlRegs.PIEIER6.bit.INTx1 = 1;//6.1
			IER = (called) ? IER | M_INT6 : M_INT6;
			break;
		case SPIATX:
			PieVectTable.SPITXINTA = ISR;
			PieCtrlRegs.PIEIER6.bit.INTx2 = 1;//6.2
			IER = (called) ? IER | M_INT6 : M_INT6;
			break;
		case SPIBRX:
			PieVectTable.SPIRXINTB = ISR;
			PieCtrlRegs.PIEIER6.bit.INTx3 = 1;//6.3
			IER = (called) ? IER | M_INT6 : M_INT6;
			break;
		case SPIBTX:
			PieVectTable.SPITXINTB = ISR;
			PieCtrlRegs.PIEIER6.bit.INTx4 = 1;//6.4
			IER = (called) ? IER | M_INT6 : M_INT6;
			break;
	}

	EINT;//enable interrupts
	called++;
}

/**
 * Acknoweledging interrupts is just a part of life, but it can be painful
 * if you don't know to which group an interrupt belongs. In the likely event
 * you forget or just don't care about interrupt-group, this function has
 * your back.
 *
 * I use a switch because it's faster to execute than a series of if-checks
 * even if it is longer in code. Libraries are primarily intended to be used;
 * readability is secondary. Also, it is useful in the ADC cases where each
 * one requires a unique additional call.
 *
 * @param type An INTRPT describing which system triggered the ISR
 */
void IsrAck(INTRPT type) {
	switch (type) {//"Reading a 1 indicates if an interrupt from the respective group has been
		case TINT0://sent to the CPU and all other interrupts from the group are currently blocked."
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;//"Writing a 1 to the respective interrupt bit
			break;								//clears the bit and enables the PIE block to drive
		case ADCINT1:							//a pulse into the CPU interrupt input if an interrupt
			AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;//is pending for that group." -Table 1-122 Tech Ref Man
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
			break;
		case ADCINT2:
			AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;//When an ADC interrrupt is thrown, a flag is not
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;//only set in the interrupts module, but in the ADC
			break;								//module. Before a new interrupt can be thrown from
		case ADCINT3:							//the ADC, the flag must be cleared. Annoying.
			AdcRegs.ADCINTFLGCLR.bit.ADCINT3 = 1;//Note that I do this here because by the point this is
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;//ever called, the user will have initialized the
			break;								//ADC. I do not enable ADC interrupts in this library
		case ADCINT4:							//because the commands will be useless of the ADC clock
			AdcRegs.ADCINTFLGCLR.bit.ADCINT4 = 1;//has not been turned on and because setting the
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;//corresponding SOCs is not under this lib's purview.
			break;
		case ADCINT5:
			AdcRegs.ADCINTFLGCLR.bit.ADCINT5 = 1;
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
			break;
		case ADCINT6:
			AdcRegs.ADCINTFLGCLR.bit.ADCINT6 = 1;
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
			break;
		case ADCINT7:
			AdcRegs.ADCINTFLGCLR.bit.ADCINT7 = 1;
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
			break;
		case ADCINT8:
			AdcRegs.ADCINTFLGCLR.bit.ADCINT8 = 1;
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
			break;
		case ADCINT9:
			AdcRegs.ADCINTFLGCLR.bit.ADCINT9 = 1;
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
			break;
		case WAKEINT:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
			break;
		case XINT1:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
			break;
		case XINT2:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
			break;
		case XINT3:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
			break;
		case EPWM1:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM2:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM3:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM4:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM5:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM6:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM7:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case EPWM8:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
			break;
		case ECAN0:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
			break;
		case ECAN1:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
			break;
		case SCIARX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
			break;
		case SCIATX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
			break;
		case SCIBRX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
			break;
		case SCIBTX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
			break;
		case SPIARX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
			break;
		case SPIATX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
			break;
		case SPIBRX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
			break;
		case SPIBTX:
			PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
			break;
	}
}
