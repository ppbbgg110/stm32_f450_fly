#ifndef __I2C_hard_H__
#define __I2C_hard_H__
 
#include "stm32f10x.h"
 
void I2C_GPIO_Config(void);
int I2C1_Init(void);
//void I2C_Write(u8 addr, u8 data);
void I2C_Write(u8 data);
u8 I2C1_Read(u8 nAddr);
#endif
