#ifndef TIMER2_H
#define TIMER2_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h" //此句删除会引起main函数里START_TIME报错
 
#define max_oil 200
#define min_oil	100



void oil_control(int num_ccr,u8 speed);
void PWM_TIM_INIT(void);
void TIM_PWM_GPIO_Configuration(void);
void TIM_PWM_RCC_Configuration(void);

void oil_speed(u8 speed);
#endif	/* TIME_TEST_H */
