
#include "stm32f1xx_hal.h"
#include "IICcom.h"

void I2C_GPIO_Config(void)
{

#ifdef STDLIB
	
  GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
#else
	GPIO_InitTypeDef GPIO_InitStruct;
	

	__HAL_RCC_GPIOB_CLK_ENABLE();
	

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);
	

	GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	

	GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
#endif
}


void I2C_delay(void)
{
                
   uint16_t i=5;
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
                
   int i=5000;  
   while(i) 
   { 
     i--; 
   }  
}

uint8_t I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return FALSE;
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;
	SDA_L;
	I2C_delay();
	return TRUE;
}

void I2C_Stop(void)                                ֹ
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 

void I2C_Ack(void)
{        
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   

void I2C_NoAck(void)        
{        
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 

uint8_t I2C_WaitAck(void)
{
	uint8_t re;
	
	SCL_L;
	I2C_delay();
	SDA_H;                        
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	SCL_L;
	I2C_delay();
	return re;
}

void I2C_SendByte(uint8_t SendByte)
{
	uint8_t i=8;
	while(i--)
	{
		SCL_L;
		I2C_delay();
		
		if(SendByte&0x80)
			SDA_H;  
		else 
			SDA_L;
		
		SendByte<<=1;
		I2C_delay();
		SCL_H;
		I2C_delay();
	}
	SCL_L;
}  

unsigned char I2C_RadeByte(void)
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;

	SDA_H;                                
	while(i--)
	{
		ReceiveByte<<=1;      
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();        
		if(SDA_read)
		{
			ReceiveByte|=0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
} 

uint8_t Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
	if(!I2C_Start())return FALSE;

	I2C_SendByte(SlaveAddress);
	if(!I2C_WaitAck())
	{
		I2C_Stop(); return FALSE;
	}
	I2C_SendByte(REG_Address );
	I2C_WaitAck();        
	I2C_SendByte(REG_data);
	I2C_WaitAck();   
	I2C_Stop(); 
	delay5ms();
	return TRUE;
}


unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   
	unsigned char REG_data;    
	
	if(!I2C_Start())return FALSE;
	
	I2C_SendByte(SlaveAddress);
	
	if(!I2C_WaitAck())
	{
		I2C_Stop();

		return FALSE;
	}
	I2C_SendByte((uint8_t) REG_Address);
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(SlaveAddress+1);
	I2C_WaitAck();

	REG_data= I2C_RadeByte();
	I2C_NoAck();
	I2C_Stop();
	

	return REG_data;

}             

