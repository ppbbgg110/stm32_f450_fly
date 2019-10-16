#include "stm32f10x.h"
#include<stdio.h>

#define LED_OFF GPIO_SetBits(GPIOB, GPIO_Pin_12)       //低电平LED亮
#define LED_ON GPIO_ResetBits(GPIOB, GPIO_Pin_12)      //高电平LED熄
#define LED_PIN	GPIO_Pin_12
#define LED_RCC	RCC_APB2Periph_GPIOB
#define G_SPEED	GPIO_Speed_2MHz
//B12针脚始化


void GPIO_Config(void)                           
{
	
	  GPIO_InitTypeDef GPIO_Inits;
    RCC_APB2PeriphClockCmd(LED_RCC, ENABLE);
    GPIO_Inits.GPIO_Pin = LED_PIN; 
    GPIO_Inits.GPIO_Speed = G_SPEED;
    GPIO_Inits.GPIO_Mode = GPIO_Mode_Out_OD; 
    GPIO_Init(GPIOB, &GPIO_Inits); 
}



//usart2初始化
void USART2_Init(u32 My_BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	// 外设使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);  //复位串口2 -> 可以没有
	
	// 初始化 串口对应IO口  TX-PA2  RX-PA3
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	
	// 初始化 串口模式状态
	USART_InitStrue.USART_BaudRate=My_BaudRate; // 波特率
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // 硬件流控制
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // 发送 接收 模式都使用
	USART_InitStrue.USART_Parity=USART_Parity_No; // 没有奇偶校验
	USART_InitStrue.USART_StopBits=USART_StopBits_1; // 一位停止位
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; // 每次发送数据宽度为8位
	USART_Init(USART2,&USART_InitStrue);
	
	USART_Cmd(USART2,ENABLE);//使能串口
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//开启接收中断
	//USART_ITConfig(USART2,USART_IT_TXE,ENABLE);//开启发送中断
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//开启空闲中断
	
	
	// 初始化 中断优先级
	NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStrue);
}

//usart3初始化
void USART3_Init(u32 My_BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	// 外设使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_DeInit(USART3);  //复位串口3 -> 可以没有
	
	// 初始化 串口对应IO口  TX-PB10  RX-PB11
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_11;
  GPIO_Init(GPIOB,&GPIO_InitStrue);
	
	
	
	// 初始化 串口模式状态
	USART_InitStrue.USART_BaudRate=My_BaudRate; // 波特率
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // 硬件流控制
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // 发送 接收 模式都使用
	USART_InitStrue.USART_Parity=USART_Parity_No; // 没有奇偶校验
	USART_InitStrue.USART_StopBits=USART_StopBits_1; // 一位停止位
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; // 每次发送数据宽度为8位
	USART_Init(USART3,&USART_InitStrue);
	
	USART_Cmd(USART3,ENABLE);//使能串口
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//开启接收中断
	//USART_ITConfig(USART3,USART_IT_TXE,ENABLE);//开启发送中断
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//开启空闲中断
	
	// 初始化 中断优先级
	NVIC_InitStrue.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStrue);
}
 

int counter=0;
u8 Temp[66];
u8 Re_buf[11];
int sign=0;

void USART2_IRQHandler(void){
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
     {
      Temp[counter] = USART_ReceiveData(USART2);   //接收数据
      //网购给的程序
      //if(counter == 0 && Re_buf[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
      if(counter == 0 && Temp[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
      counter++; 
      if(counter==11) //接收到 11 个数据
      { 
         memcpy(Re_buf,Temp,11);
         counter=0; //重新赋值，准备下一帧数据的接收
         sign=1;//缓冲区Re_buf是否有数据标志
      }    
   }
}




//延时
void delay(u32 t)
{
u16 i;
while(t--)
for(i=0;i<1000;i++);
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





int main(void)
{

  while (1)
   {
      if(sign)
      {  
         memcpy(Temp,Re_buf,11);
         sign=0;
         if(Re_buf[0]==0x55)       //检查帧头
         {  
            switch(Re_buf[1])
            {
               case 0x51: //标识这个包是加速度包
                  a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;      //X轴加速度
                  a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;      //Y轴加速度
                  a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;      //Z轴加速度
                  T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //温度
                  break;
               case 0x52: //标识这个包是角速度包
                  w[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X轴角速度
                  w[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y轴角速度
                  w[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z轴角速度
                  T    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;      //温度
                  break;
               case 0x53: //标识这个包是角度包
                  angle[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X轴滚转角（x 轴）
                  angle[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
                  angle[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z轴偏航角（z 轴）
                  T        = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;   //温度

                  //printf("X轴角度：%.2f   Y轴角度：%.2f   Z轴角度：%.2f\r\n",angle[0],angle[1],angle[2]);
                  break;
               default:  break;
            }
            printf("X角度：%.2f  Y角度：%.2f  Z角度：%.2f  X速度：%.2f  Y速度：%.2f  Z速度：%.2f\r\n",angle[0],angle[1],angle[2],w[0],w[1],w[2]);
         }
         
      }
      delay_ms(50);
   }
}


}