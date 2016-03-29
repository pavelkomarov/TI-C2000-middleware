;// TI File $Revision: /main/2 $ 
;// Checkin $Date: January 4, 2011   10:10:11 $ 
;//###########################################################################
;//
;// FILE:  F2806x_DBGIER.asm
;//
;// TITLE: Set the DBGIER register
;//
;// DESCRIPTION:
;//  
;//  Function to set the DBGIER register (for realtime emulation).
;//  Function Prototype: void SetDBGIER(Uint16)
;//  Useage: SetDBGIER(value);
;//  Input Parameters: Uint16 value = value to put in DBGIER register. 
;//  Return Value: none          
;//
;//###########################################################################
;// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V136 $ 
;// $Release Date: Apr 15, 2013 $ 
;//###########################################################################	
		.global _SetDBGIER
		.text
		
_SetDBGIER:
		MOV 	*SP++,AL
		POP 	DBGIER
		LRETR
		
