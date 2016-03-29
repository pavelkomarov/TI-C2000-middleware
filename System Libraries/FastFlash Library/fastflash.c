/**
 * @file fastflash.c
 * @author Pavel Komarov <pkomarov@gatech.edu> 941-545-7573, Drew Harris
 * @brief A library that encapsulates all the C code necessary to make a project run quickly from flash
 * @ingroup Digital
 * @version 1
 *
 * http://solarracing.gatech.edu/wiki/Main_Page
 * Note that it is also necessary to add a few lines to a project's .cmd file to enable it to
 * execute quickly from flash. WiFiTest uses a .cmd file modified in this way, so copy it.
 * Also note that it is necessary to include CodeStartBranch.asm in the root directory of all
 * flash-based projects.
 */

#pragma CODE_SECTION(InitFlash, "secureRamFuncs")

#include "F2806x_Device.h"
#include "fastflash.h"

extern unsigned int secureRamFuncs_loadstart;
extern unsigned int secureRamFuncs_loadsize;
extern unsigned int secureRamFuncs_runstart;

/**
 * Initialize a program to run from flash quickly. Call this first, at the very start of a
 * project's main method, as I have done in WiFiTest.
 */
void InitFlash() {
	memcpy(&secureRamFuncs_runstart, &secureRamFuncs_loadstart, (Uint32)&secureRamFuncs_loadsize);
	FlashRegs.FPWR.bit.PWR = 3; //Change flash wait states
	FlashRegs.FBANKWAIT.bit.RANDWAIT = 2;
	FlashRegs.FBANKWAIT.bit.PAGEWAIT = 2;
	FlashRegs.FOPT.bit.ENPIPE = 1;
	asm(" RPT #6 || NOP");
}
