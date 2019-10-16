#ifndef TIM_H
#define TIM_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h" //此句删除会引起main函数里START_TIME报错
 
#define START_TIME  time=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

void TIM2_NVIC_Configuration(void);
void TIM2_Configuration(void);

#endif	/* TIME_TEST_H */
