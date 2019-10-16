/***************STM32F103C8T6**********************
 * ÎÄ¼þÃû  £ºtimer2.c
 * ÃèÊö    : TIM2¶¨Ê±Æ÷
 * ÊµÑéÆ½Ì¨£ºSTM32F103C8T6
 * ±¸×¢    £º
 * ½Ó¿Ú    £º

****************STM32F103C8T6**********************/
#include "TIM.h"
#include "misc.h"

/* TIM2ÖÐ¶ÏÓÅÏÈ¼¶ÅäÖÃ */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*ÖÐ¶ÏÖÜÆÚÎª1us*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=1000-1;	//×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ(1ms) 
    /* ÀÛ¼Æ TIM_Period¸öÆµÂÊºó²úÉúÒ»¸ö¸üÐÂ»òÕßÖÐ¶Ï */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);	//Ê±ÖÓÔ¤·ÖÆµÊý 72M/72      
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //ÏòÉÏ¼ÆÊýÄ£Ê½ 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// Çå³ýÒç³öÖÐ¶Ï±êÖ¾ 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// ¿ªÆôÊ±ÖÓ    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//ÏÈ¹Ø±ÕµÈ´ýÊ¹ÓÃ  
}

/*
void TIM2_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
	//
	}
}
*/