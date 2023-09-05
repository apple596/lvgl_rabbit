#include "nrf24l01.h"
#include "spi.h"


uint8_t nrf_rxBuf[32]={0};

const uint8_t sendtest[10] = {0x01,0x23,0x99,0x05,0x09,0xfe,0xad,0xed,0x34,0x02};

//const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10};
//const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10}; 

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10};	//T1
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10}; 





//初始化24L01的IO口
void NRF24L01_Init(void)
{
	NRF24L01_GPIO_Config();
	NRF24L01_SPI_Config();
	
	NRF24L01_SPI_TransmitReceiveByte(0xff);
	
	NRF24L01_CS(1);
	NRF24L01_CE(0);
}
//检测24L01是否存在
//返回值:0，成功;1，失败	
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	  uint8_t status;	
   	NRF24L01_CS(0);                 //使能SPI传输
  	status =NRF24L01_SPI_TransmitReceiveByte(reg);//发送寄存器号 
  	NRF24L01_SPI_TransmitReceiveByte(value);      //写入寄存器的值
  	NRF24L01_CS(1);                 //禁止SPI传输	   
  	return(status);       		    //返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	  uint8_t reg_val;	    
   	NRF24L01_CS(0);             //使能SPI传输		
  	NRF24L01_SPI_TransmitReceiveByte(reg);    //发送寄存器号
  	reg_val=NRF24L01_SPI_TransmitReceiveByte(0XFF);//读取寄存器内容
  	NRF24L01_CS(1);             //禁止SPI传输		    
  	return(reg_val);            //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	  uint8_t status,u8_ctr;	       
  	NRF24L01_CS(0);            //使能SPI传输
  	status=NRF24L01_SPI_TransmitReceiveByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=NRF24L01_SPI_TransmitReceiveByte(0XFF);//读出数据
  	NRF24L01_CS(1);            //关闭SPI传输
  	return status;             //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	  uint8_t status,u8_ctr;	    
 	  NRF24L01_CS(0);             //使能SPI传输
  	status = NRF24L01_SPI_TransmitReceiveByte(reg);//发送寄存器值(位置),并读取状态值
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)NRF24L01_SPI_TransmitReceiveByte(*pBuf++); //写入数据	 
  	NRF24L01_CS(1);             //关闭SPI传输
  	return status;              //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;  
	NRF24L01_CE(0);
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE(1);                         //启动发送	   
	while(NRF24L01_IRQ()!=0);                 //等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);          //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)                          //达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);  //清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)                           //发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);          //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);  //清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
}					    


void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE(0);	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,60);       //设置RF通道为100
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE(1);//CE为高,10us后启动发送
}

void NRF24L01_RX_Mode(void)
{
  NRF24L01_CE(0);	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,60);	     //设置RF通信频率		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启
		//0x0f=2M		0x07=1M    0x26=250k
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  NRF24L01_CE(1);//CE为高,10us后启动发送
}	





/************************************************/
//需要配置的函数
/************************************************/




void NRF24L01_GPIO_Config(void)
{
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOA , ENABLE); 
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_GPIOB , ENABLE);
	
	//NRF_IRQ--PA4   NRF_CS--PB0  NRF_CE--PB1
	GPIO_InitPara  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_4; 
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IPU;                      
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0 | GPIO_PIN_1; 
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;                      
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
	
	
}


void NRF24L01_SPI_Config(void)
{
	SPI1_Init();
}


void NRF24L01_CS(uint8_t select)
{
	GPIO_WriteBit(GPIOB,GPIO_PIN_0,(BitState)select);
}


void NRF24L01_CE(uint8_t enable)
{
	GPIO_WriteBit(GPIOB,GPIO_PIN_1,(BitState)enable);
}

uint8_t NRF24L01_IRQ(void)
{
	return GPIO_ReadInputBit(GPIOA,GPIO_PIN_4);
}

uint8_t NRF24L01_SPI_TransmitReceiveByte(uint8_t txData)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_TBE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, txData);

    /* Wait to receive a byte */
    while (SPI_I2S_GetBitState(SPI1, SPI_FLAG_RBNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}


