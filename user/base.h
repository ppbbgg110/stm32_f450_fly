#ifndef __BASE_H__

#define __BASE_H__
#include<stdio.h>
#include "stm32f10x.h"

#define printf_d 1;

#define LED_OFF GPIO_SetBits(GPIOC, GPIO_Pin_13)       //???LED?
#define LED_ON GPIO_ResetBits(GPIOC, GPIO_Pin_13)      //???LED?

typedef struct usart
{
unsigned int length;
unsigned int counter;
u8	buff[32];
}ss;

char *str_find(char *sc,char *dc);
int fputc(int ch, FILE *f);
void LED_Config(void);
void USART2_Init(u32 My_BaudRate);
void send_data(USART_TypeDef *USARTx,u8 *s);
void USART2_IRQHandler(void);
int LED_wave(void);
void delay_ms(u32 t);
void delay_us(u32 t);

#endif // __MYI2C_H__