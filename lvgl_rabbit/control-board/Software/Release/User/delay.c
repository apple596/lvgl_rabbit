#include "delay.h"

#if defined ( __CC_ARM   )
extern uint32_t SystemCoreClock;
#endif

static uint8_t  fac_us=0;							//us��ʱ������			   
static uint16_t fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��

void Delay_Tim_Init(void)
{
	
	SysTick->CTRL |= (1<<2);
	//SysTick->CTRL&=~(1<<2);					//SYSTICKʹ���ⲿʱ��Դ	 
	fac_us=SystemCoreClock/108/1000000;			//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
	fac_ms=(uint16_t)fac_us*1000;				//��OS��,����ÿ��ms��Ҫ��systickʱ����   
}


void delay_us(uint16_t us)
{
	uint32_t temp;	    	 
	SysTick->LOAD=us*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL=0x01 ;      				//��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;      	 			//�رռ�����
	SysTick->VAL =0X00;       				//��ռ�����
	
	
}

//�����ʱʱ�� 65535/2 = 32767.5ms
void delay_ms(uint16_t ms)
{
#if 1
	uint32_t temp;		   
	SysTick->LOAD=(u32)ms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL=0x01 ;          			//��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;      	 			//�رռ�����
	SysTick->VAL =0X00;       				//��ռ�����	 
	
#endif
	
#if 0
	//RTOS��ʱ
#endif
	
}

