#include "nrf24l01.h"
#include "spi.h"


uint8_t nrf_rxBuf[32]={0};

const uint8_t sendtest[10] = {0x01,0x23,0x99,0x05,0x09,0xfe,0xad,0xed,0x34,0x02};

//const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10};
//const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10}; 

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10};	//T1
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x12,0x15,0x09,0x45,0x10}; 





//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
	NRF24L01_GPIO_Config();
	NRF24L01_SPI_Config();
	
	NRF24L01_SPI_TransmitReceiveByte(0xff);
	
	NRF24L01_CS(1);
	NRF24L01_CE(0);
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	  uint8_t status;	
   	NRF24L01_CS(0);                 //ʹ��SPI����
  	status =NRF24L01_SPI_TransmitReceiveByte(reg);//���ͼĴ����� 
  	NRF24L01_SPI_TransmitReceiveByte(value);      //д��Ĵ�����ֵ
  	NRF24L01_CS(1);                 //��ֹSPI����	   
  	return(status);       		    //����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	  uint8_t reg_val;	    
   	NRF24L01_CS(0);             //ʹ��SPI����		
  	NRF24L01_SPI_TransmitReceiveByte(reg);    //���ͼĴ�����
  	reg_val=NRF24L01_SPI_TransmitReceiveByte(0XFF);//��ȡ�Ĵ�������
  	NRF24L01_CS(1);             //��ֹSPI����		    
  	return(reg_val);            //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	  uint8_t status,u8_ctr;	       
  	NRF24L01_CS(0);            //ʹ��SPI����
  	status=NRF24L01_SPI_TransmitReceiveByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=NRF24L01_SPI_TransmitReceiveByte(0XFF);//��������
  	NRF24L01_CS(1);            //�ر�SPI����
  	return status;             //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	  uint8_t status,u8_ctr;	    
 	  NRF24L01_CS(0);             //ʹ��SPI����
  	status = NRF24L01_SPI_TransmitReceiveByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)NRF24L01_SPI_TransmitReceiveByte(*pBuf++); //д������	 
  	NRF24L01_CS(1);             //�ر�SPI����
  	return status;              //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;  
	NRF24L01_CE(0);
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE(1);                         //��������	   
	while(NRF24L01_IRQ()!=0);                 //�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)                          //�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);  //���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)                           //�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);  //���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    


void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE(0);	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,60);       //����RFͨ��Ϊ100
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07);  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE(1);//CEΪ��,10us����������
}

void NRF24L01_RX_Mode(void)
{
  NRF24L01_CE(0);	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,60);	     //����RFͨ��Ƶ��		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07);//����TX�������,0db����,2Mbps,���������濪��
		//0x0f=2M		0x07=1M    0x26=250k
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  NRF24L01_CE(1);//CEΪ��,10us����������
}	





/************************************************/
//��Ҫ���õĺ���
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


