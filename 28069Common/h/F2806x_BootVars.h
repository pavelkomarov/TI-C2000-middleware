//###########################################################################
//
// FILE:    F2806x_BootVars.h
//
// TITLE:   F2806x Boot Variable Definitions.
//
// NOTES:
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V136 $
// $Release Date: Apr 15, 2013 $
//###########################################################################

#ifndef F2806x_BOOT_VARS_H
#define F2806x_BOOT_VARS_H

#ifdef __cplusplus
extern "C" {
#endif



//---------------------------------------------------------------------------
// External Boot ROM variable definitions:
//
extern Uint16 EmuKey;
extern Uint16 EmuBMode;
extern Uint32 Flash_CPUScaleFactor;
extern void (*Flash_CallbackPtr) (void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of F2806x_BOOT_VARS_H definition

//===========================================================================
// End of file.
//===========================================================================

