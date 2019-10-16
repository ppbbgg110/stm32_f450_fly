#ifndef __GPIO_H
#define __GPIO_H
#include "stm32f10x_gpio.h"

#define F450_PORT	GPIOA
#define F450_PORT_RCC	RCC_APB2Periph_GPIOA
#define F450_oil1	GPIO_Pin_8
#define F450_oil2	GPIO_Pin_9
#define F450_oil3	GPIO_Pin_10
#define F450_oil4	GPIO_Pin_11



int OIL_control(u8 gpio_pin,unsigned int number);
#endif