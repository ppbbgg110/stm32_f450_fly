#include "GPIO.h"


void F450_oil_GPIO_Config()
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(F450_PORT_RCC, ENABLE);
GPIO_InitStructure.GPIO_Pin = F450_oil1 | F450_oil2 | F450_oil3| F450_oil4 ;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_Init(GPIOA, &GPIO_InitStructure);
}



int OIL_control(u8 gpio_pin,unsigned int number)
{
//number of percent   number%,
//1ms-2ms high voltage  number%,% oil

GPIO_SetBits(F450_PORT, gpio_pin);
	printf("H\r\n");
delay_ms(300+200*number/100);
	
GPIO_ResetBits(F450_PORT, gpio_pin);
		printf("L\r\n");
	return 0;
}
