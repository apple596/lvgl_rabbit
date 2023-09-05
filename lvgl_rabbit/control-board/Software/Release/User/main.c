/**
  ******************************************************************************
  * @brief   The main function file
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f10x.h"
#include <stdio.h>

#include "uart.h"
#include "delay.h"
#include "spi.h"
#include "nrf24l01.h"
#include "relay.h"


/* Private functions ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t count;
uint32_t i;
/**
  * @brief  Configure the GPIO ports
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{    
    GPIO_InitPara GPIO_InitStructure;
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOC |RCC_APB2PERIPH_GPIOF|RCC_APB2PERIPH_AF,ENABLE);
	
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
}

/**
  * @brief  NVIC_Configuration
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_VectTableSet(NVIC_VECTTAB_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_VectTableSet(NVIC_VECTTAB_FLASH, 0x0);      
#endif
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t ID[3];
	uint8_t rxBuf[32];
	uint32_t i;
    GPIO_Configuration();
    NVIC_Configuration();
	Uart_Init();
	Delay_Tim_Init();
	Relay_Init();
	NRF24L01_Init();
	
    /* Enable write access to IWDG_PSR and IWDG_RLDR registers */
    IWDG_Write_Enable(IWDG_WRITEACCESS_ENABLE);

    /* IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz */
    IWDG_SetPrescaler(IWDG_PRESCALER_64);

    /* Set counter reload value to 625 */
    IWDG_SetReloadValue(625);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
	
	
	while(NRF24L01_Check())
	{
		i=65535;
		printf("NRF24L01 Error\r\n");
		while(i--){};
	}
	printf("NRF24L01 Check OK\r\n");
	
	for(i=0;i<3;i++)
	{
		ID[i] = *(vu32*)0x1FFFF7E8+i*4;
	}
	printf("ID:0X%X%X%X\r\n",ID[0],ID[1],ID[2]);
		
	printf("Open Rabbit!\r\n");
	printf("开源兔!\r\n");
	
	GPIO_ResetBits(GPIOA, GPIO_PIN_8);/*Turn off all the LEDs*/
	
	NRF24L01_RX_Mode();
	
//	Relay_Switch(RELAY_CH1,RELAY_ON);
//	Relay_Switch(RELAY_CH2,RELAY_ON);
	
	while(1)
	{
		if(GPIO_ReadInputBit(GPIOA,GPIO_PIN_4)==0)
		{
			if(NRF24L01_RxPacket(rxBuf)==0)
			{
				if(rxBuf[0]==1)
					Relay_Switch(RELAY_CH1,RELAY_ON);
				else
					Relay_Switch(RELAY_CH1,RELAY_OFF);
				if(rxBuf[15]==1)
					Relay_Switch(RELAY_CH2,RELAY_ON);
				else
					Relay_Switch(RELAY_CH2,RELAY_OFF);
			}
			printf("收到24L01数据!\r\n");
		}
		delay_ms(10);
		if(i++>=4096)
			i=0;
		if(i%10==0)
			/* Reload IWDG counter */
			IWDG_ReloadCounter();
		if(i%50==0)
			GPIO_WriteBit(GPIOA, GPIO_PIN_8,(1-(BitState)GPIO_ReadOutputBit(GPIOA, GPIO_PIN_8)));
	}
}

