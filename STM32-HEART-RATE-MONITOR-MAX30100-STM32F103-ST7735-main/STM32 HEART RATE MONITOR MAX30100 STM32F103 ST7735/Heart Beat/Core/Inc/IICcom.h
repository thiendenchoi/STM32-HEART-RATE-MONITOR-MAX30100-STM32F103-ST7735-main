Ư#ifndef __IICCOM_H
#define __IICCOM_H

#include "stm32f1xx_hal.h"

//#define STDLIB	//��׼���HAL����л�

/*ģ��IIC�˿�������붨��*/

#ifdef STDLIB

#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7

#else

#define SCL_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)//GPIOB->BSRR = GPIO_PIN_6 
#define SCL_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)
   
#define SDA_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET)
#define SDA_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)

#define SCL_read      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)//GPIOB->IDR  & GPIO_PIN_6
#define SDA_read      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)

#endif /*STDLIB*/


#define FALSE 0
#define TRUE	1

void I2C_GPIO_Config(void);                                //IIC���ų�ʼ��

void I2C_delay(void);

uint8_t I2C_Start(void); //IIC��ʼ
void I2C_Stop(void);//IICֹͣ

void I2C_Ack(void);//IIC����
void I2C_NoAck(void);

uint8_t I2C_WaitAck(void); //����Ϊ:=1��ACK,=0��ACK  �ȴ�����
void I2C_SendByte(uint8_t SendByte);//���ݴӸ�λ����λ//��������
unsigned char I2C_RadeByte(void);//���ݴӸ�λ����λ//��ȡ�ֽ�
uint8_t Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);//���ֽ�д��
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);//���ֽڶ�ȡ


#endif /*__IICCOM_H*/

