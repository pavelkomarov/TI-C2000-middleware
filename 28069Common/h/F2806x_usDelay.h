//============================================================================
//============================================================================
//
// FILE:   			F2806x_usDelay.h (2806x version)
// 
// DESCRIPTION:		Contains F2806x Delay routine declarations
//
// VERSION:			10 Feb 2011 - (VSC)
//============================================================================
//============================================================================
#ifndef _F2806X_USDELAY_H_
#define _F2806X_USDELAY_H_

extern void DSP28x_usDelay(Uint32 Count);

// The CPU rate used for delay calculations
#define 	 CPU_RATE		  12.500L		  // 12.5 nS

// DO NOT MODIFY THIS LINE.
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)


#endif //_F2806X_USDELAY_H_
