
/*
 * In/out pin pairs should be in the same system (A or B).
 * I have not protected against cases when they are not.
 */
typedef enum {
    Ain7,			Ain28,			Aout12,
    Aout29,			Bin11,			Bin15,
    Bin19,			Bin23,			Bin41,
    Bin44,			Bout9,			Bout14,
    Bout18,			Bout22,			Bout40,
    Bout58
} SCIPIN;

void SciInit(SCIPIN in, SCIPIN out);
void SetSciBaudRate(char scisys, float32 fclk, float32 baudrate);

void sendCharArray(char, char*, Uint16);
void sendString(char, char*);
void recieveCharArray(char, char*, Uint16);
signed char recieveString(char, char*);

//untested
void EnableSciInterrupts(char);
