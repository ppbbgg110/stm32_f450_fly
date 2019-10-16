#include "stm32f10x.h"
#include<stdio.h>

#define LED_OFF GPIO_SetBits(GPIOC, GPIO_Pin_13)       //???LED?
#define LED_ON GPIO_ResetBits(GPIOC, GPIO_Pin_13)      //???LED?

#define P_OFF GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define P_ON GPIO_ResetBits(GPIOB, GPIO_Pin_12)


int fputc(int ch, FILE *f)
{
	/* ?????????USART1 */
	USART_SendData(USART2, (u8) ch);
	
	/* ?????? */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	
	return (ch);
}



void GPIO_Config(void)                           
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	
	  GPIO_InitTypeDef GPIO_Inits;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_Inits.GPIO_Pin = GPIO_Pin_12; 
    GPIO_Inits.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Inits.GPIO_Mode = GPIO_Mode_Out_OD; 
    GPIO_Init(GPIOB, &GPIO_Inits); 

}



//usart2???
void USART2_Init(u32 My_BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	// ??????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);  //????2 -> ????
	
	// ??? ????IO?  TX-PA2  RX-PA3
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	
	// ??? ??????
	USART_InitStrue.USART_BaudRate=My_BaudRate; // ???
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // ?????
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // ?? ?? ?????
	USART_InitStrue.USART_Parity=USART_Parity_No; // ??????
	USART_InitStrue.USART_StopBits=USART_StopBits_1; // ?????
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; // ?????????8?
	USART_Init(USART2,&USART_InitStrue);
	
	USART_Cmd(USART2,ENABLE);//????
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//??????
	//USART_ITConfig(USART2,USART_IT_TXE,ENABLE);//??????
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//??????
	
	
	// ??? ?????
	NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStrue);
}




//??????
void send_char(USART_TypeDef *USARTx,u8 temp)      
{        
    USART_SendData(USARTx,(u8)temp);        
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);         
}

//?????
void send_buff(USART_TypeDef *USARTx,u8 buf[])     
{    
    u32 i;    
    for(i=0;i<strlen(buf);i++)    
		send_char(USARTx,buf[i]);   
    send_char(USARTx,'\r');  
    send_char(USARTx,'\n');  
}

void send(USART_TypeDef *USARTx,u8 buf[])     
{    
    u32 i;    
    for(i=0;i<strlen(buf);i++)    
		send_char(USARTx,buf[i]);   
}

void send_data(USART_TypeDef *USARTx,u8 *s)
{
	while(*s!='\0')
	{ //USART_GetFlagStatus(USARTx,USART_FLAG_TXE )==RESET
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE )==RESET);	
		USART_SendData(USARTx,*s);
		s++;
	}
}

void USART2_IRQHandler(void)
{
    uint8_t ch;
#ifdef SYSTEM_SUPPORT_OS        
    OSIntEnter();    
#endif
    if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        ch = USART_ReceiveData(USART2);
				//USART_SendData(USART3,ch);
       
       
    }
    else if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )
    {
				USART2->SR;
				USART2->DR;
        //USART_ClearITPendingBit(USART2,USART_IT_IDLE);
        //ss2.flag = 1;
        //ch = USART_ReceiveData(USART2);
			
    }
#ifdef SYSTEM_SUPPORT_OS     
    OSIntExit();                                             
#endif
}


int LED_wave(void)
{
int i=0;
while(i++<5)
{
LED_ON;
delay(1000);
LED_OFF;
delay(300);
}
}

//??
void delay(u32 t)
{
u16 i;
while(t--)
for(i=0;i<1000;i++);
}





int init_user(void)
{

delay(100);
GPIO_Config();
delay(100);
P_ON;
delay(10);
P_OFF;
delay(100);
USART2_Init(115200);

delay(100);
LED_wave();
send_data(USART2,"hello,world\r\n");
}




/*????ADC??*/
void  Adc_Config(void)
{ 	

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );	  
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //72M/6=12, ADC???????14MHz  
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //GPIO_Mode_AIN:????(?????????????????1)
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	
	ADC_DeInit(ADC1); //??,?ADC1???????????
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//????:ADC1?ADC2??????  (?????????????????2)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//??????:??(???)??=ENABLE???(???)??=DISABLE
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//??????:??=ENABLE???=DISABLE
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC????????? (?????????????????3)
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC?????   ??????:ADC_DataAlign_Left
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//?????????ADC?????   ???1-16
	ADC_Init(ADC1, &ADC_InitStructure);	//??ADC_InitStruct???????????ADC1????
 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );	  	
	ADC_Cmd(ADC1, ENABLE);	//?????ADC  ??:??ADC_Cmd?????ADC?????????

	ADC_ResetCalibration(ADC1);	//?????ADC??????
	while(ADC_GetResetCalibrationStatus(ADC1)); //?????????
	ADC_StartCalibration(ADC1);	//????ADC?????	
	while(ADC_GetCalibrationStatus(ADC1));//?????????		
 }	



/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	init_user();

	float ADC_ConvertedValue; 
  float ADC_ConvertedValueLocal; 

	Adc_Config();
	while(1)
	{
	  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//????	
	  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));  //??????
	  
	  ADC_ConvertedValue=ADC_GetConversionValue(ADC1); //??????*ADC_ConvertedValue*	
	  ADC_ConvertedValueLocal=(float)ADC_ConvertedValue*(3.3/4096);   //????????*ADC_ConvertedValueLocal*
	  
	 delay(200);
		printf("values is : %.2f\r\n",ADC_ConvertedValueLocal);
		 ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
	}

}
