#include "i2c_hard.h"

void I2C_GPIO_Config()
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
GPIO_Init(GPIOB, &GPIO_InitStructure);
}

int I2C1_Init(void)
{

	
I2C_InitTypeDef I2C_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
I2C_DeInit(I2C1);
I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
I2C_InitStructure.I2C_OwnAddress1 = 0x77;
I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
I2C_InitStructure.I2C_ClockSpeed = 800000;
I2C_Init(I2C1, &I2C_InitStructure);
I2C_Cmd(I2C1, ENABLE);
	return 1;
}
 
//void I2C1_Write(u8 addr, u8 data)
void I2C_Write(u8 data)
{
I2C_AcknowledgeConfig(I2C1,ENABLE); 
I2C_GenerateSTART(I2C1,ENABLE); 
		//	printf("START ok \r\n");
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;}//EV5
//I2C_Send7bitAddress(I2C1,0x3a,I2C_Direction_Transmitter); 
I2C_Send7bitAddress(I2C1,0xc0,I2C_Direction_Transmitter); 
	   //printf("EV5 ok \r\n");
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;} //EV6
//I2C_SendData(I2C1,addr); 
I2C_SendData(I2C1,0xC0); 
		//printf("EV6 ok \r\n");
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //EV8
I2C_SendData(I2C1,data);
		//	printf("EV8 ok \r\n");
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
I2C_GenerateSTOP(I2C1,ENABLE); 
			//printf("STOP ok \r\n");
}
 
u8 I2C1_Read(u8 nAddr)
{
I2C_AcknowledgeConfig(I2C1,ENABLE); //????
I2C_GenerateSTART(I2C1,ENABLE); //???????
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} //??EV5
I2C_Send7bitAddress(I2C1,0x3a,I2C_Direction_Transmitter); //????????
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}//??EV6
I2C_SendData(I2C1,nAddr);//?????
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //??EV8
 
I2C_GenerateSTART(I2C1,ENABLE); //???????
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} //??EV5
I2C_Send7bitAddress(I2C1,0x3a,I2C_Direction_Receiver); //???????
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} //??EV6
I2C_AcknowledgeConfig(I2C1,DISABLE); //??????
while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;} //??EV7
I2C_GenerateSTOP(I2C1,ENABLE); //???????
return I2C_ReceiveData(I2C1); //???????
}

