/***************STM32F103C8T6**********************
 * 文件名  ：timer2.c
 * 描述    : TIM2定时器
 * 实验平台：STM32F103C8T6
 * 备注    ：
 * 接口    ：

****************STM32F103C8T6**********************/
#include "timer2.h"

/* 
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=125-1;	//自动重装载寄存器周期的值(125us) 
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);	//时钟预分频数 72M/72      
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, DISABLE);	// 开启时钟    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//先关闭等待使用  
}


*/


void TIM_PWM_RCC_Configuration(void)
{
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
}

void TIM_PWM_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Configuration:TIM3 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void PWM_TIM_INIT(void)
{
	
TIM_PWM_RCC_Configuration();	
TIM_PWM_GPIO_Configuration();
	
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t CCR1_Val = max_oil;
uint16_t CCR3_Val = max_oil;
uint16_t CCR2_Val = max_oil;
uint16_t CCR4_Val = max_oil;
uint16_t PrescalerValue = 0;
	
	
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 100000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 2000;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);

}


void oil_control(int num_ccr,u8 speed)
{
TIM_Cmd(TIM3, DISABLE);
delay_us(50);
speed+=100;
printf("oil_control values : %d ,%d\r\n",num_ccr,speed);
switch(num_ccr)
{
	case 1:
		TIM3->CCR1=speed;
	case 2:
		TIM3->CCR2=speed;
	case 3:
		TIM3->CCR3=speed;
	case 4:
		TIM3->CCR4=speed;
	default:
		break;
}
delay_us(50);
	 TIM_Cmd(TIM3, ENABLE);

}


void oil_speed(u8 speed)
{

if(speed>=0 && speed <=100)
{
printf("\r\n oil speed : %d % \r\n",speed);
oil_control(1,speed);
oil_control(2,speed);
oil_control(3,speed);
oil_control(4,speed);

}
delay_us(1);
}


typedef enum {ff = 100, f0 = 0,f1=10,f2=20,f3=30,f4=40,f5=50,f6=60,f7=70,f8=80,f9=90,fa=99} oil_value;

u8 oil_num[14]={0,10,20,30,40,50,60,70,80,90,99,100,5};	
u8 oil_hex[14]={0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xff,0xfe};	

typedef enum {up = 0xE1, down = 0xE2,forward=0xE3,back=0xE4,left=0xE5,right=0xe6} control;
void cmd_dispatch(u8 ch)
{
ch&=0xFF;
int k=0;
while(k<13)
{
if(ch==oil_hex[k]) oil_speed(oil_num[k]);
k++;
}
	
}