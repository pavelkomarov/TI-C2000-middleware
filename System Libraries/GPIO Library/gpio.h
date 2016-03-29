
//initialization
void GpioInputsInit(Uint8[], Uint8);	//verified
void GpioInputInit(Uint8);				//verified

void GpioOutputsInit(Uint8[], Uint8);	//verified
void GpioOutputInit(Uint8);				//verified

//clearing and setting
void GpioClearAll(void);				//verified
void GpioClearPins(Uint8[], Uint8);
void GpioClearPin(Uint8);

void GpioSetAll(void);					//verified
void GpioSetPins(Uint8[], Uint8);
void GpioSetPin(Uint8);

void GpioToggleAll(void);				//verified
void GpioTogglePins(Uint8[], Uint8);
void GpioTogglePin(Uint8);				//verified

//getting state
Uint16 GpioGetData(Uint8);				//verified

//pulling up and floating inputs (all are pulled up by default)
void GpioFloatAll(void);				//verified
void GpioFloatPins(Uint8[], Uint8);		//verified
void GpioFloatPin(Uint8);				//verified

void GpioPullUpAll(void);				//verified
void GpioPullUpPins(Uint8[], Uint8);	//verified
void GpioPullUpPin(Uint8);				//verified
