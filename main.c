#include "base.h"
#include "timer2.h"
#include "stm32f10x.h"


int init_user(void)
{

delay_ms(10);
LED_Config();
delay_ms(10);
USART2_Init(115200);
delay_ms(10);
USART3_Init(115200);
	delay_ms(10);
send_data(USART2,"I2C test \r\n");
//I2C_GPIO_Config();
delay_ms(10);
LED_wave();
delay_ms(10);

}







extern int flag;
extern u8 buff[64];
u8 lbuff[64];
int cc=0;


u8 at_cwmode_def[]="AT+CWMODE_DEF=1\r\n";
u8 at_cwautoconn[]="AT+CWAUTOCONN=1\r\n";
u8 at_cipmode[]="AT+CIPMODE=1\r\n";
u8 at_cifsr[]="AT+CIFSR\r\n";
u8 at_cipstart[]="AT+CIPSTART=\"TCP\",\"192.168.43.156\",12345\r\n";



void get_conn(void)
{
	

send_data(USART3,at_cifsr);
//send_data(USART2,at_cifsr);
	
while(flag!=1){;}
/*
printf("REC echo info \r\n");
printf("=====%s=====\r\n",buff);
printf("=====%s=====\r\n",str_find(buff,"OK"));
printf("=====%d=====\r\n",strlen(str_find(buff,"OK")));
	*/
while(strlen(str_find(buff,"OK"))!=51)
	{
memset(buff,0x00,sizeof(buff));
send_data(USART3,at_cifsr);
send_data(USART2,"Waiting for connect mobile phone ....\r\n");
//printf("REC echo info \r\n");
//printf("=====%s=====\r\n",buff);
//printf("=====%s=====\r\n",str_find(buff,"OK"));
//printf("=====%d=====\r\n",strlen(str_find(buff,"OK")));
		delay_ms(5000);
		delay_ms(5000);
	}
	

	
	delay_ms(50);
	memset(buff,0x00,sizeof(buff));
	send_data(USART3,at_cipmode);
	send_data(USART2,at_cipmode);
	delay_ms(50);
	/*
	printf("REC echo info \r\n");
printf("=====%s=====\r\n",buff);
printf("=====%s=====\r\n",str_find(buff,"OK"));
printf("=====%d=====\r\n",strlen(str_find(buff,"OK")));
	*/
		delay_ms(50);
	memset(buff,0x00,sizeof(buff));
	send_data(USART3,at_cipstart);
	send_data(USART2,at_cipstart);
	delay_ms(50);
/*
printf("REC echo info \r\n");
printf("=====%s=====\r\n",buff);
printf("=====%s=====\r\n",str_find(buff,"ERROR"));
printf("=====%d=====\r\n",strlen(str_find(buff,"ERROR")));

printf("=====%s=====\r\n",str_find(buff,"CONNECT"));
printf("=====%d=====\r\n",strlen(str_find(buff,"CONNECT")));

printf("=====%s=====\r\n",str_find(buff,"CONNECTED"));
printf("=====%d=====\r\n",strlen(str_find(buff,"CONNECTED")));
*/

while(strlen(str_find(buff,"CONNECTED"))!=13)
	{
memset(buff,0x00,sizeof(buff));
send_data(USART3,at_cipstart);
send_data(USART2,at_cipstart);
		delay_ms(50);
printf("=====%s=====\r\n",str_find(buff,"CONNECTED"));
printf("=====%d=====\r\n",strlen(str_find(buff,"CONNECTED")));
		delay_ms(2000);
	}
}

int main(void)
{	
	PWM_TIM_INIT();
	init_user();
	

	oil_speed(0);
	delay_ms(200);
	get_conn();
  while (1)
  {

if(flag==1)
{
//usart3 oil control command 
if(buff[0]==0xff && buff[2]==0xff)
{
cmd_dispatch(buff[1]);
}
else
{
//usart3 echo data from wifi modern
//strncpy(lbuff,buff,strlen(buff)-2);
strncpy(lbuff,buff,strlen(buff));
/*	
if(strstr(lbuff,(u8)"192.168.")!=0)
{
printf("connection success!\r\n");
	
	send_data(USART3,(u8)at_cipmode);
	delay_ms(50);
send_data(USART3,(u8)at_cipstart);	
	delay_ms(50);
}*/
printf("%s",lbuff);
	
memset(buff,0x00,sizeof(buff));
memset(lbuff,0x00,sizeof(lbuff));

}



flag=0;

}

delay_us(10);

		
	}
}

