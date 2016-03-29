typedef enum{
	MOTOR_CONTROLLER,
	PEDALS,
	BMS,
	IMU,
	WIFI,
	SCREEN,
	MPPT=0x600
	//Number of CAN nodes must be less than 2^29! Not going to be a problem...ever
} CAN_ID;

#include "F2806x_Cla_typedefs.h"
typedef struct{
	CAN_ID ID;
	void (*upon_sent_isr)(CAN_ID ID, Uint32 dataH, Uint32 dataL, Uint16 length, int mbox_num);
	void (*upon_receive_isr)(CAN_ID ID, Uint32 dataH, Uint32 dataL, Uint16 length, int mbox_num);
}CAN_INFO;

void CAN_send(Uint32* data, int length, CAN_ID ID, Uint32 mbox_num, char block);
void CAN_receive(CAN_ID ID, int length, Uint32 mbox_num, char block);
void CAN_request(CAN_ID ID, int length, Uint32 mbox_num, char block);
void CAN_autoreply(Uint32* data, int length, CAN_ID ID, Uint32 mbox_num, char block);
void CAN_init(CAN_INFO* can_array, Uint32 can_length, char enableInterrupts);

