#ifndef _RELAY_H
#define _RELAY_H

#include "gd32f10x.h"

typedef enum{
	RELAY_CH1=0,
	RELAY_CH2
}RELAY_CHANNEL;

typedef enum{
	RELAY_ON=0,
	RELAY_OFF
}RELAY_SWITCH;


void Relay_Init(void);
void Relay_Switch(RELAY_CHANNEL ch,RELAY_SWITCH sw);

#endif



