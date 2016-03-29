/*//###########################################################################
 *
 * @file main.c
 * @author Ricky Liou <rliou92@gmail.com> 404-451-7986
 * @brief This file contains the CAN API for the microcontrollers to communicate with each other.
 * @warning Has not been thoroughly tested, might have bugs.
 * @ingroup CAN
 *
 * Description:
 *
 * CAN receiver and sender functions
 *
 * Improvements:
 * Can use acceptance masks
 * Support for more interrupt types
 * System level interrupts
 * Error checking
 * Timer functions
 *
 * Concerns:
 * 	In self test mode, I can successfully send and receive messages with no connection to bus!
 *
//###########################################################################*/


#include "DSP28x_Project.h"
#include "CAN.h"
#include <stdio.h>
#include <stdlib.h>
//Global variables
int DEBUG = 1;
int bus_error = 0;
CAN_INFO* CAN_INFO_ARRAY;
Uint32 CAN_ARRAY_LENGTH;

#define CAN_TX_WAIT_CYCLES 150e3
__interrupt void ecan_isr(void);


/*
USER DEFINED CAN_INFO ARRAY EXAMPLE

void imu_send_isr(CAN_ID ID, Uint32 dataH, Uint32 dataL, Uint16 length, int mbox_num){
	char string[] = "Sent";
	//puts(string);
}

void imu_receive_isr(CAN_ID ID, Uint32 dataH, Uint32 dataL, Uint16 length, int mbox_num){
	char string[32];
	//sprintf(string, "Received %d %d", dataH, dataL);
	//puts(string);
}

CAN_INFO SENSOR_CAN_INFO_ARRAY[] = {
	{
		IMU,
		&imu_send_isr,
		&imu_receive_isr
	}
};

TESTING PROJECT MAIN FUNCTION EXAMPLE

void main(void){

	Uint32 test_data[] = {1,2,3};

	CAN_init(SENSOR_CAN_INFO_ARRAY, 1, 1); // Initialize CAN with interrupts.

	CAN_receive(PEDALS, 12, 4, 0);
	CAN_send(test_data, 12, PEDALS, 7, 0);

	CAN_autoreply(test_data, 2, PEDALS, 4, 0);
	CAN_request(PEDALS, 7, 0, 0);


	while(1);
}*/



char interruptsEnabled;
/*
@brief Initializes the CAN module
 */

void CAN_init(CAN_INFO* can_array, Uint32 can_length, char enableInterrupts){
	interruptsEnabled = enableInterrupts;
	struct ECAN_REGS ECanaShadow;
	Uint16 IERShadow;
	CAN_INFO_ARRAY = can_array;
	CAN_ARRAY_LENGTH = can_length;

	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This function is found in the F2806x_SysCtrl.c file.
	// Should be commented out in regular usage. For normal usage initializing the system should be done elsewhere not in the CAN code.
	// For testing purposes this is uncommented to initialize the system.
	//   InitSysCtrl();

	// Step 2. Initialize GPIO:
	// Configure CAN pins using GPIO regs here
	// This function is found in F2806x_ECan.c
	InitECanGpio();

	if (enableInterrupts) {
		// FOR MOTOR CONTROLLER: NO INTERRUPTS!
		// Step 3. Clear all interrupts and initialize PIE vector table:
		// Disable CPU interrupts
		DINT;

		// Disable CPU interrupts and clear all CPU interrupt flags
		// Save interrupts that were previously enabled
		IERShadow = IER;
		IER = 0x0000;
		IFR = 0x0000;

		EALLOW;
		//Direct ECANA1 interrupts to the proper isr
		PieVectTable.ECAN1INTA = &ecan_isr;
		//Direct ECANA0 interupts to proper isr
		PieVectTable.ECAN0INTA = &ecan_isr;
		EDIS;

		// Enable the PIE Vector Table
		PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
	}
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 1;      // eCAN-A
	EDIS;

	// Step 4. Initialize CAN module
	InitECana();



	if (enableInterrupts) {
		EALLOW;
		//Allow all mailboxes to send interrupts
		ECanaRegs.CANMIM.all = 0xFFFFFFFF;
		//Using level 1 CAN interrupts  for mboxes
		ECanaRegs.CANMIL.all = 0xFFFFFFFF;
		//From all system interrupts only use bus-off (not recommended)
		//Enable level 0, 1 interrupts
		ECanaRegs.CANGIM.all = 0x00000403;

		//CAN interrupts are part of IER 9
		IER = IERShadow | M_INT9;

		//Enable ECAN interrupt 0 in PIE
		PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
		//Enable ECAN interrupt 1 in PIE
		PieCtrlRegs.PIEIER9.bit.INTx6 = 1;
	}

	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.STM = 0;    // Disable CAN self-test mode (0=off)
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

	if (enableInterrupts) {
		// Enable global Interrupts and higher priority real-time debug events:
		EINT;   // Enable Global interrupt INTM
		ERTM;   // Enable Global realtime interrupt DBGM
	}
	EDIS;
}

//@brief Data must be array of {Lower 4 bytes, Higher 4 bytes, etc}
//Sends data starting from specified mailbox. If mailbox exceeds 31, then it will wrap around starting from 0.
//Length is in units of bytes.
//Data must be given multiple of 8 bytes, even if you're not sending 8 bytes!
//Data is sent starting from lower 4 bytes.
//	Ex: PEDALS sends out 4 bytes of data from mailbox 4 + 5
//	CAN_SEND(data_array_pointer, 4, PEDALS, 4);
void CAN_send(Uint32* data, int length, CAN_ID ID, Uint32 mbox_num, char block){
	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;
	Uint32 wait_cycles;

	// How many mailboxes need to be used?
	Uint32 numMbox;
	if(length%8){
		numMbox = length/8 + 1;
	}
	else{
		numMbox = length/8;
	}

	// Sets 1s to the mailboxes affected, 0s otherwise.
	Uint32 bitMaskOfOnes = (((Uint32)1 << (numMbox)) - (Uint32)1) << (mbox_num);

	// ECanaRegs.CANTRR.all |= bitMaskOfOnes; //Clear TRS bits
	ECanaShadow.CANTRR.all = ECanaRegs.CANTRR.all;
	ECanaShadow.CANTRR.all |= bitMaskOfOnes;
	ECanaRegs.CANTRR.all = ECanaShadow.CANTRR.all;

	// Wait until transmit request set bits are cleared
	wait_cycles = 0;
	while ((ECanaRegs.CANTRS.all & bitMaskOfOnes) && wait_cycles < CAN_TX_WAIT_CYCLES) wait_cycles++; // TRY to wait for CANTRS to be ready, but do not wait forever.

	// ECanaRegs.CANME.all &= ~bitMaskOfOnes; //Disable the mailboxes to modify their content
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all &= ~bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	// ECanaRegs.CANMD.all &= ~bitMaskOfOnes; //Configure mailboxes for transmit
	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.all &= ~bitMaskOfOnes;// 0 for transmit
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	Uint32 i;
	for (i=0; i< numMbox; i++){
		// Construct pointer to the correct mailbox register.
		// Starts off from mailbox 0 and increments from there.
		Mailbox = &ECanaMboxes.MBOX0 + ((i + mbox_num) % 32);

		if(length >= 8){
			//Data-length code. The number in these bits determines how many data bytes are sent or received. Valid value range is from 0 to 8. Values from 9 to 15 are not allowed.
			Mailbox->MSGCTRL.bit.DLC = 8;
			length -= 8;
		}
		else{
			Mailbox->MSGCTRL.bit.DLC = length;
		}

		Mailbox->MDL.all = data[2*i];

		// Make sure that the next data array value exists
		if(length >= 0) {
			Mailbox->MDH.all = data[2*i+1];
		}
		else {
			Mailbox->MDH.all = 0;
		}

		// The ID is only the last 29 bits. ID cannot be greater than 2^29-1.
		// I am using the extended identifier only.
		Mailbox->MSGID.bit.STDMSGID = (Uint32)ID;
		Mailbox->MSGID.bit.AAM = 0;
		Mailbox->MSGID.bit.AME = 0;
		Mailbox->MSGID.bit.IDE = 0;
	}
	//ECanaRegs.CANME.all |= bitMaskOfOnes; //Enable the mailboxes
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all |= bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	//ECanaRegs.CANTRS.all |= bitMaskOfOnes; //Send the data all at once!
	ECanaShadow.CANTRS.all = ECanaRegs.CANTRS.all;
	ECanaShadow.CANTRS.all |= bitMaskOfOnes;
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

	if(block) {
		while(ECanaRegs.CANTA.all != ~bitMaskOfOnes ) {}  // Wait for all TAn bits to be set..
	}
}

/*
 * @brief Configure mailboxes to receive *length* bytes from desired CAN ID
 * If mailbox number exceeds 31, then it will wrap around starting at 0.
 */
void CAN_receive(CAN_ID ID, int length, Uint32 mbox_num, char block){
	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;

	//How many mailboxes need to be used?
	Uint32 numMbox;
	if(length%8){
		numMbox = length/8 + 1;
	}
	else{
		numMbox = length/8;
	}

	Uint32 bitMaskOfOnes = (((Uint32)1 << (numMbox)) - (Uint32)1) << (mbox_num);

	//ECanaRegs.CANME.all &= ~mbox_mask; //Disable the mailboxes to modify their content
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all &= ~bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	//ECanaRegs.CANMD.all |= mbox_mask; //Configure mailbox for receive
	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.all |= bitMaskOfOnes;
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	Uint32 i;
	for (i=0; i< numMbox; i++){
		// Construct pointer to the correct mailbox register.
		// Starts off from mailbox 0 and increments from there.
		Mailbox = &ECanaMboxes.MBOX0 + ((i + mbox_num) % 32);

		if(length >= 8){
			Mailbox->MSGCTRL.bit.DLC = 8;
			length -= 8;
		}
		else{
			Mailbox->MSGCTRL.bit.DLC = length;
		}

		// The ID is only the last 29 bits. ID cannot be greater than 2^29-1.
		// I am using the extended identifier only.
		Mailbox->MSGID.all = (Uint32)ID<<18;
		Mailbox->MSGID.bit.AAM = 0;
		Mailbox->MSGID.bit.AME = 0;
		Mailbox->MSGID.bit.IDE = 0;
	}

	//ECanaRegs.CANME.all |= mbox_mask; //Enable the mailboxes
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all |= bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	if(block){
		while(ECanaRegs.CANRMP.all ^ bitMaskOfOnes ){}
	}
}
/*
 * @brief Request data from specified mailbox
 * length is in bytes
 */
void CAN_request(CAN_ID ID, int length, Uint32 mbox_num, char block){
	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;

	// How many mailboxes need to be used?
	Uint32 numMbox;
	if(length%8){
		numMbox = length/8 + 1;
	}
	else{
		numMbox = length/8;
	}

	Uint32 bitMaskOfOnes = (((Uint32)1 << (numMbox)) - (Uint32)1) << (mbox_num);

	//ECanaRegs.CANME.all &= ~mbox_mask; //Disable the mailboxes to modify their content
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all &= ~bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	//ECanaRegs.CANMD.all |= mbox_mask; //Configure mailbox for receive
	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.all |= bitMaskOfOnes;
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	Uint32 i;
	for (i=0; i< numMbox; i++){
		// Construct pointer to the correct mailbox register.
		// Starts off from mailbox 0 and increments from there.
		Mailbox = &ECanaMboxes.MBOX0 + ((i + mbox_num) % 32);

		if(length >= 8){
			Mailbox->MSGCTRL.bit.DLC = 8;
			length -= 8;
		}
		else{
			Mailbox->MSGCTRL.bit.DLC = length;
		}
		Mailbox->MSGCTRL.bit.RTR = 1;// Set request flag

		// The ID is only the last 29 bits. ID cannot be greater than 2^29-1.
		// I am using the extended identifier only.
		Mailbox->MSGID.all = (Uint32)ID<<18;
		Mailbox->MSGID.bit.AAM = 0;
		Mailbox->MSGID.bit.AME = 0;
		Mailbox->MSGID.bit.IDE = 0;
	}

	//ECanaRegs.CANME.all |= mbox_mask; // Enable the mailboxes
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all |= bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	//ECanaRegs.CANTRS.all |= mbox_mask;// Request!
	ECanaShadow.CANTRS.all = ECanaRegs.CANTRS.all;
	ECanaShadow.CANTRS.all |= bitMaskOfOnes;
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

	if(block){
		while(ECanaRegs.CANTA.all != ~bitMaskOfOnes ) {}  // Wait for all TAn bits to be set..
	}
}

// @brief Set up a mailbox to automatically send data upon request. Length in bytes.

void CAN_autoreply(Uint32* data, int length, CAN_ID ID, Uint32 mbox_num, char block){
	struct ECAN_REGS ECanaShadow;
	volatile struct MBOX *Mailbox;

	// How many mailboxes need to be used?
	Uint32 numMbox;
	if(length%8) {
		numMbox = length/8 + 1;
	} else {
		numMbox = length/8;
	}

	Uint32 bitMaskOfOnes = (((Uint32)1 << (numMbox)) - (Uint32)1) << (mbox_num);

	//ECanaRegs.CANTRR.all |= bitMaskOfOnes; //Clear TRS bits
	ECanaShadow.CANTRR.all = ECanaRegs.CANTRR.all;
	ECanaShadow.CANTRR.all |= bitMaskOfOnes;
	ECanaRegs.CANTRR.all = ECanaShadow.CANTRR.all;

	while(ECanaRegs.CANTRS.all & bitMaskOfOnes); //Wait until TRS bits are cleared

	//ECanaRegs.CANME.all &= ~bitMaskOfOnes; //Disable the mailboxes to modify their content
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all &= ~bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	//ECanaRegs.CANMD.all &= ~bitMaskOfOnes; //Configure mailboxes for transmit
	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.all &= ~bitMaskOfOnes;
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	int i;
	for (i=0; i< numMbox; i++){
		Mailbox = &ECanaMboxes.MBOX0 + i + mbox_num;

		if(length >= 8){
			Mailbox->MSGCTRL.bit.DLC = 8;
			length -= 8;
		}
		else{
			Mailbox->MSGCTRL.bit.DLC = length;
		}
		Mailbox->MDL.all = data[2*i];

		// Make sure that the next data array value exists
		if(length >= 0) {
			Mailbox->MDH.all = data[2*i+1];
		}
		else {
			Mailbox->MDH.all = 0;
		}

		// The ID is only the last 29 bits. ID cannot be greater than 2^29-1.
		// I am using the extended identifier only.
		Mailbox->MSGID.all = (Uint32) ID;
		Mailbox->MSGID.bit.AAM = 1; // Auto answer mode is on
		Mailbox->MSGID.bit.AME = 0;
		Mailbox->MSGID.bit.IDE = 1;
	}
	//ECanaRegs.CANME.all |= bitMaskOfOnes; //Enable the mailboxes
	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.all |= bitMaskOfOnes;
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	if(block){
		while(ECanaRegs.CANTA.all != ~bitMaskOfOnes ) {}  // Wait for all TAn bits to be set..
	}
}

//@brief Based on the the event which triggered the interrupt (sent or received), calls the user specified function

//checks what threw the interrupt (after a send or receive)
__interrupt void ecan_isr(void){
	puts("ecan_isr called");
	//Extract mailbox number, CAN ID, data, execute desired user function
	struct ECAN_REGS ECanaShadow;

	ECanaShadow.CANGIF0.all = ECanaRegs.CANGIF0.all;
	if(ECanaShadow.CANGIF0.bit.BOIF0){
		bus_error = 1;
		if(DEBUG)
			puts("CAN BUS ERROR");
		ECanaShadow.CANGIF0.bit.BOIF0 |= 1; // Clear bus off interrupt flag by writing a 1. Doesn't seem to actually reset.
		EALLOW;
		ECanaRegs.CANGIF0.all = ECanaShadow.CANGIF0.all;
		ECanaRegs.CANMC.bit.CCR = 0;
		EDIS;
	}
	else{
		Uint32 i;
		Uint32 ID;
		volatile struct MBOX *Mailbox;

		//Determine which mailbox generated the interrupt
		Uint16 mbox_num = ECanaRegs.CANGIF1.bit.MIV1;
		Mailbox = &ECanaMboxes.MBOX0 + mbox_num;
		Uint32 mbox_mask = (Uint32) 1 << (Uint32) mbox_num;

		ID = Mailbox->MSGID.bit.STDMSGID; //The ID is only the last 29 bits.
		for(i=0; i< CAN_ARRAY_LENGTH; i++){
			if(CAN_INFO_ARRAY[i].ID == (Uint32)ID){
				if(ECanaRegs.CANTA.all & mbox_mask){ //If TA bit is set
					CAN_INFO_ARRAY[i].upon_sent_isr(CAN_INFO_ARRAY[i].ID,Mailbox->MDH.all, Mailbox->MDL.all,Mailbox->MSGCTRL.bit.DLC,mbox_num);
					ECanaRegs.CANTA.all |= mbox_mask; //Clear TA bit by writing 1
					if(DEBUG) {
						puts("CAN sent ISR");
					}
				}
				else if(ECanaRegs.CANRMP.all & mbox_mask){ //If RMP bit is set
					CAN_INFO_ARRAY[i].upon_receive_isr(CAN_INFO_ARRAY[i].ID, Mailbox->MDH.all, Mailbox->MDL.all,Mailbox->MSGCTRL.bit.DLC, mbox_num);
					ECanaRegs.CANRMP.all |= mbox_mask; //Clear RMP bit by writing 1
				}
			}
		}
	}
	PieCtrlRegs.PIEACK.bit.ACK9 = 1; //Acknowledge interrupt
}

