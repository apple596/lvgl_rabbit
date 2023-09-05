#include "relay.h"

void Relay_Init(void)
{
    GPIO_InitPara GPIO_InitStructure;
	
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOB,ENABLE);

	GPIO_PinRemapConfig(GPIO_REMAP_SWJ_JTAGDISABLE , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	
	
	Relay_Switch(RELAY_CH1,RELAY_OFF);
	Relay_Switch(RELAY_CH2,RELAY_OFF);
}




void Relay_Switch(RELAY_CHANNEL ch,RELAY_SWITCH sw)
{
	if(ch == RELAY_CH1)
	{
		GPIO_WriteBit(GPIOB, GPIO_PIN_5,(BitState)sw);
	}
	else if(ch == RELAY_CH2)
	{
		GPIO_WriteBit(GPIOB, GPIO_PIN_4,(BitState)sw);
	}
}

