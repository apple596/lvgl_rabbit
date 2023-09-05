#include "spi.h"




void SPI1_Init(void)
{
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA , ENABLE); 
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_SPI1, ENABLE);
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_AF  , ENABLE);
	
	
	GPIO_InitPara  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_5 |GPIO_PIN_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;                      
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_3;                              
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
    SPI_InitPara  SPI_InitStructure;
    SPI_InitStructure.SPI_TransType = SPI_TRANSTYPE_FULLDUPLEX;
    SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
    SPI_InitStructure.SPI_FrameFormat = SPI_FRAMEFORMAT_8BIT;
    SPI_InitStructure.SPI_SCKPL = SPI_SCKPL_LOW;
    SPI_InitStructure.SPI_SCKPH = SPI_SCKPH_1EDGE;
    SPI_InitStructure.SPI_SWNSSEN = SPI_SWNSS_SOFT;
    SPI_InitStructure.SPI_PSC = SPI_PSC_32; 
    SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
    SPI_InitStructure.SPI_CRCPOL = 7;
  
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Enable(SPI1, ENABLE);      
}





