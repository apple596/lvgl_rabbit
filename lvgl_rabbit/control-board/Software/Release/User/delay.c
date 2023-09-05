#include "delay.h"

#if defined ( __CC_ARM   )
extern uint32_t SystemCoreClock;
#endif

static uint8_t  fac_us=0;							//us延时倍乘数			   
static uint16_t fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

void Delay_Tim_Init(void)
{
	
	SysTick->CTRL |= (1<<2);
	//SysTick->CTRL&=~(1<<2);					//SYSTICK使用外部时钟源	 
	fac_us=SystemCoreClock/108/1000000;			//不论是否使用OS,fac_us都需要使用
	fac_ms=(uint16_t)fac_us*1000;				//非OS下,代表每个ms需要的systick时钟数   
}


void delay_us(uint16_t us)
{
	uint32_t temp;	    	 
	SysTick->LOAD=us*fac_us; 				//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL=0x01 ;      				//开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;      	 			//关闭计数器
	SysTick->VAL =0X00;       				//清空计数器
	
	
}

//最大延时时间 65535/2 = 32767.5ms
void delay_ms(uint16_t ms)
{
#if 1
	uint32_t temp;		   
	SysTick->LOAD=(u32)ms*fac_ms;			//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           			//清空计数器
	SysTick->CTRL=0x01 ;          			//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;      	 			//关闭计数器
	SysTick->VAL =0X00;       				//清空计数器	 
	
#endif
	
#if 0
	//RTOS延时
#endif
	
}

