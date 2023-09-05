#ifndef _DHT11_H
#define _DHT11_H


#include "gd32f10x.h"



//IO²Ù×÷º¯Êý											   
#define	DHT11_DQ_OUT(n)	(n?GPIO_WriteBit(GPIOB,GPIO_PIN_2,Bit_SET):GPIO_WriteBit(GPIOB,GPIO_PIN_2,Bit_RESET))
#define	DHT11_DQ_IN  	GPIO_ReadInputBit(GPIOB, GPIO_PIN_2)


#endif

