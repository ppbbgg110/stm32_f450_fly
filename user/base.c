#include"base.h"
#include "stm32f10x_usart.h"
u8 speed;
int counter=0;
int length=0;
u8 buff[64];
int flag=0;
int fputc(int ch, FILE *f)
{

	USART_SendData(USART2, (u8) ch);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	return (ch);
}


char *str_find(char *sc,char *dc)
{
char *fs=sc,*fd=dc;
	char *dffs,*dffd;
	int i,j=0;
	for(i=0;*fd;fd++)
	i++;
	fd-=i;j=i;
	for(;*fs;fs++){
		if(*fs==*fd){
			dffs=fs;dffd=fd;
			while(*++dffs==*++dffd && dffs && --j);
			if(j ==1)
					return (char *)fs;
				else
					j=i;
			}
		}
		return (0);



}


void LED_Config(void)                           
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
}



void USART2_Init(u32 My_BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);  
	
	// TX-PA2  RX-PA3
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	
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
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStrue);
}

//usart3???
void USART3_Init(u32 My_BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	// ??????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_DeInit(USART3);  //????3 -> ????
	
	// ??? ????IO?  TX-PB10  RX-PB11
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_11;
  GPIO_Init(GPIOB,&GPIO_InitStrue);
	
	
	
	// ??? ??????
	USART_InitStrue.USART_BaudRate=My_BaudRate; // ???
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // ?????
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // ?? ?? ?????
	USART_InitStrue.USART_Parity=USART_Parity_No; // ??????
	USART_InitStrue.USART_StopBits=USART_StopBits_1; // ?????
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; // ?????????8?
	USART_Init(USART3,&USART_InitStrue);
	
	USART_Cmd(USART3,ENABLE);//????
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//??????
	//USART_ITConfig(USART3,USART_IT_TXE,ENABLE);//??????
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//??????
	
	// ??? ?????
	NVIC_InitStrue.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStrue);
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

u8 u2_buff[64];
int u2_counter=0;
void USART2_IRQHandler(void)
{
    uint8_t ch;
	

#ifdef SYSTEM_SUPPORT_OS        
    OSIntEnter();    
#endif
		if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) != RESET)
		{
		USART_ClearFlag(USART2,USART_FLAG_ORE);	
		USART_ReceiveData(USART2);
		}
    if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        ch = USART_ReceiveData(USART2);
					
				//memset(cmd,0x00,8);
				
    }
    else if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )
    {
				USART2->SR;
				USART2->DR;
			
			//USART_SendData(USART2, (u8) ch);
			
    }
#ifdef SYSTEM_SUPPORT_OS     
    OSIntExit();                                             
#endif
}



void USART3_IRQHandler(void)
{
    u8 ch;

#ifdef SYSTEM_SUPPORT_OS        
    OSIntEnter();    
#endif
		if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) != RESET)
		{
		 USART_ClearFlag(USART3,USART_FLAG_ORE);	
		USART_ReceiveData(USART3);
		}
    if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
				buff[counter]=USART_ReceiveData(USART3);
				counter++;
			if(counter==64)
				{flag=1;
				counter=0;}
			
    }
    else if( USART_GetITStatus( USART3, USART_IT_IDLE ) == SET )
    {
				USART3->SR;
				USART3->DR;
				flag=1;
				counter=0;

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
delay_ms(150);
LED_OFF;
delay_ms(50);
}
}



void delay_us(u32 i)
{
    u32 temp;
    SysTick->LOAD=9*i;         //??????, 72MHZ?
    SysTick->CTRL=0X01;         //??,???????,???????
    SysTick->VAL=0;                //?????
    do
    {
        temp=SysTick->CTRL;           //????????
    }
    while((temp&0x01)&&(!(temp&(1<<16))));     //??????
    SysTick->CTRL=0;    //?????
    SysTick->VAL=0;        //?????
}

void delay_ms(u32 i)
{
    u32 temp;
    SysTick->LOAD=9000*i;      //??????, 72MHZ?
    SysTick->CTRL=0X01;        //??,???????,???????
    SysTick->VAL=0;            //?????
    do
    {
        temp=SysTick->CTRL;       //????????
    }
    while((temp&0x01)&&(!(temp&(1<<16))));    //??????
    SysTick->CTRL=0;    //?????
    SysTick->VAL=0;        //?????
}


