
#include "uart.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  //#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
  
  
  
void Uart_Init(void)
{
	
    /* Enable GPIOA clock */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_GPIOA  , ENABLE );
    
    /* Enable USART1 APB clock */
    RCC_APB2PeriphClock_Enable( RCC_APB2PERIPH_USART1  , ENABLE );
	
	USART_InitPara  USART_InitStructure;

    /* USART1 Pins configuration **************************************************/
    {
        /* Configure the GPIO ports */
        GPIO_InitPara  GPIO_InitStructure;
       
        GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_9 ;
        GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF_PP;
        GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_50MHZ;
        GPIO_Init( GPIOA , &GPIO_InitStructure); 
        GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_10;
        GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN_FLOATING;;
        GPIO_Init( GPIOA , &GPIO_InitStructure); 
    }
    { 
        USART_InitPara  USART_InitStructure;
        
        USART_DeInit( USART1 );        
        USART_InitStructure.USART_BRR                 = 9600;
        USART_InitStructure.USART_WL                  = USART_WL_8B;
        USART_InitStructure.USART_STBits              = USART_STBITS_1;
        USART_InitStructure.USART_Parity              = USART_PARITY_RESET;
        USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
        USART_InitStructure.USART_RxorTx              = USART_RXORTX_RX | USART_RXORTX_TX;
        USART_Init(USART1, &USART_InitStructure);
    }

    /* USART enable */
    USART_Enable(USART1, ENABLE);

}



  
  
  
  
  
  
  
  
  
  
  
 
  
 int fputc(int ch, FILE *f)
 {

    USART_DataSend( USART1 , (uint8_t) ch );

    /* Loop until transmit data register is empty */
    while ( USART_GetBitState( USART1 , USART_FLAG_TBE ) == RESET)
    {}
        
    return ch;
 }
  
  
  
  
  /**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_DataSend( USART1 , (uint8_t) ch );

    /* Loop until transmit data register is empty */
    while ( USART_GetBitState( USART1 , USART_FLAG_TBE ) == RESET)
    {}
        
    return ch;
}

  
  
  
  
  