#include "base.h"
#include "vs1003.h"
#include "spec_rew.h"	 
//VS1003��ȫ���ܺ���
//֧��SIN���Ժ�RAM����
//��������VS1003��Ƶ����ʾ���룬����˵ʵ����զ�أ��������Լ�д��Ƶ�׷����������ǲ�����ʵ��Ƶ�ױ任��  
//����ԭ��@SCUT
//V1.1
#define SPI1_SPEED_2   0
#define SPI1_SPEED_8   1
#define SPI1_SPEED_16  2
#define SPI1_SPEED_256 3 //281K
//VS1003���ò���
//0,�������ƣ�1�����Ƶ�ޣ�2���������ƣ�3�����Ƶ��
//0,henh.1,hfreq.2,lenh.3,lfreq 5,������


//EEPROM��ַ��488~489 ��2��
u8 VS_VOLT=200;	 //Ĭ������200
u8 VS_BASS=0; 	//Ĭ�Ϸǳ��ص���

					 
//SPI1�ڶ�дһ���ֽ�
//TxData:Ҫ���͵��ֽ�		 
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
	while((SPI1->SR&1<<1)==0);//�ȴ���������				  
	SPI1->DR=TxData;	 	  //����һ��byte   
	while((SPI1->SR&1<<0)==0);//�ȴ�������һ��byte  
	return SPI1->DR;          //�����յ�������				    
}
/*******************************************************************************
* Function Name  : SPI1_SetSpeed
* Description    : SPI1�����ٶ�Ϊ����
* Input          : u8 SpeedSet 
*                  ����ٶ���������0�������ģʽ����0�����ģʽ
*                  SPI_SPEED_HIGH   1
*                  SPI_SPEED_LOW    0
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI1_SPEED_2)//����Ƶ
	{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI1_SPEED_8)//�˷�Ƶ 
	{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI1_SPEED_16)//ʮ����Ƶ
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256��Ƶ
	{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz ����ģʽ
	}
	SPI1->CR1|=1<<6; //SPI�豸ʹ��	  
} 

void VS_SET_VOL(u8 volt,u8 bass)
{
	u16 temp_volt,temp_bass;
	volt=255-volt;
	temp_volt=volt;
   	temp_volt=(temp_volt<<8)+volt;
	temp_bass=(temp_bass<<8)+bass;
    Vs1003_CMD_Write(SPI_VOL,temp_volt); //������ 
	Vs1003_CMD_Write(SPI_BASS,temp_bass);//BASS 
}
	 
//��λVS1003
void Vs1003SoftReset(void)
{	 
	u8 retry; 				   
	while((VS_DREQ)==0);//�ȴ������λ����
	SPI1_ReadWriteByte(0X00);//��������
	retry=0;
	while(Vs1003_REG_Read(SPI_MODE)!=0x0804)// �����λ,��ģʽ  
	{
		Vs1003_CMD_Write(SPI_MODE,0x0804);// �����λ,��ģʽ
		delay_ms(2);//�ȴ�����1.35ms 
		if(retry++>100)break; 
	}	 				  
	while (VS_DREQ == 0);//�ȴ������λ����	   

	retry=0;
	while(Vs1003_REG_Read(SPI_CLOCKF)!=0X9800)//����vs1003��ʱ��,3��Ƶ ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_CLOCKF,0X9800);//����vs1003��ʱ��,3��Ƶ ,1.5xADD
		if(retry++>100)break; 
	}		   
	retry=0;
	while(Vs1003_REG_Read(SPI_AUDATA)!=0XBB81)//����vs1003��ʱ��,3��Ƶ ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_AUDATA,0XBB81);
		if(retry++>100)break; 
	}
	//Vs1003_CMD_Write(SPI_CLOCKF,0X9800); 	    
	//Vs1003_CMD_Write(SPI_AUDATA,0XBB81); //������48k��������	
	VS_SET_VOL(VS_VOLT,VS_BASS);		 
	ResetDecodeTime();//��λ����ʱ��	    
    //��vs1003����4���ֽ���Ч���ݣ���������SPI����
    VS_XDCS_CLR();//ѡ�����ݴ���
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	VS_XDCS_SET();//ȡ�����ݴ���
	delay_ms(20);
} 
//Ӳ��λMP3
void Mp3Reset(void)
{
	VS_XRESET_CLR();
	delay_ms(20);
	VS_XDCS_SET();	//ȡ�����ݴ���
	VS_XCS_SET();//ȡ�����ݴ���
	VS_XRESET_SET();    
	while(VS_DREQ==0);	//�ȴ�DREQΪ��
	delay_ms(20);				 
}
//���Ҳ��� 
void VsSineTest(void)
{											    
	Mp3Reset();	 
	Vs1003_CMD_Write(0x0b,0X2020);	  //��������	 
 	Vs1003_CMD_Write(SPI_MODE,0x0820);//����vs1003�Ĳ���ģʽ	    
	while (VS_DREQ == 0);     //�ȴ�DREQΪ��
 	//��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨vs1003�����������Ҳ���Ƶ��ֵ��������㷽����vs1003��datasheet
    VS_XDCS_CLR();//ѡ�����ݴ���
	SPI1_ReadWriteByte(0x53);
	SPI1_ReadWriteByte(0xef);
	SPI1_ReadWriteByte(0x6e);
	SPI1_ReadWriteByte(0x24);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	delay_ms(100);
	VS_XDCS_SET();
    //�˳����Ҳ���
    VS_XDCS_CLR();//ѡ�����ݴ���
	SPI1_ReadWriteByte(0x45);
	SPI1_ReadWriteByte(0x78);
	SPI1_ReadWriteByte(0x69);
	SPI1_ReadWriteByte(0x74);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	delay_ms(100);
	VS_XDCS_SET();		 

    //�ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ
    VS_XDCS_CLR();//ѡ�����ݴ���      
	SPI1_ReadWriteByte(0x53);
	SPI1_ReadWriteByte(0xef);
	SPI1_ReadWriteByte(0x6e);
	SPI1_ReadWriteByte(0x44);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	delay_ms(100);
	VS_XDCS_SET();	
    //�˳����Ҳ���
    VS_XDCS_CLR();//ѡ�����ݴ���
	SPI1_ReadWriteByte(0x45);
	SPI1_ReadWriteByte(0x78);
	SPI1_ReadWriteByte(0x69);
	SPI1_ReadWriteByte(0x74);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	delay_ms(100);
	VS_XDCS_SET();
}	 
//ram ���� 	
u16 regvalue ;																			 
void VsRamTest(void)
{
		   
	Mp3Reset();     
 	Vs1003_CMD_Write(SPI_MODE,0x0820);// ����vs1003�Ĳ���ģʽ
	while (VS_DREQ==0); // �ȴ�DREQΪ��
 	VS_XDCS_CLR();       			  // xDCS = 1��ѡ��vs1003�����ݽӿ�
	SPI1_ReadWriteByte(0x4d);
	SPI1_ReadWriteByte(0xea);
	SPI1_ReadWriteByte(0x6d);
	SPI1_ReadWriteByte(0x54);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	delay_ms(50);  
	VS_XDCS_SET();
	regvalue = Vs1003_REG_Read(SPI_HDAT0); // ����õ���ֵΪ0x807F���������á�
	///printf("regvalueH:%x\n",regvalue>>8);//������ 
	//printf("regvalueL:%x\n",regvalue&0xff);//������ 
}     
//��VS1003д����
//address:�����ַ
//data:��������
void Vs1003_CMD_Write(u8 address,u16 data)
{  
    while(VS_DREQ==0);//�ȴ�����
	SPI1_SetSpeed(SPI1_SPEED_256);//���� 
	 
	VS_XDCS_SET(); //MP3_DATA_CS=1;
	VS_XCS_CLR(); //MP3_CMD_CS=0; 
	
	SPI1_ReadWriteByte(VS_WRITE_COMMAND);//����VS1003��д����
	SPI1_ReadWriteByte(address); //��ַ
	SPI1_ReadWriteByte(data>>8); //���͸߰�λ
	SPI1_ReadWriteByte(data);	 //�ڰ�λ
	VS_XCS_SET();         //MP3_CMD_CS=1; 
	SPI1_SetSpeed(SPI1_SPEED_16);//����
} 
//��VS1003д����
void Vs1003_DATA_Write(u8 data)
{
	VS_XDCS_CLR();   //MP3_DATA_CS=0;
	//while(VS_DREQ==0);  //VS1003��DREQΪ�߲���д�����ݣ������������Բ�Ҫ����
	SPI1_ReadWriteByte(data);
	VS_XDCS_SET();  //MP3_DATA_CS=1;
	VS_XCS_SET();   //MP3_CMD_CS=1; 
}         
//��VS1003�ļĴ���           
//��VS1003
//ע�ⲻҪ�ñ��ٶ�ȡ,�����
u16 Vs1003_REG_Read(u8 address)
{ 
	u16 temp=0; 
    while(VS_DREQ==0);//�ǵȴ�����״̬ 
	SPI1_SetSpeed(SPI1_SPEED_256);//���� 
	VS_XDCS_SET();       //MP3_DATA_CS=1;
	VS_XCS_CLR();        //MP3_CMD_CS=0;
	SPI1_ReadWriteByte(VS_READ_COMMAND);//����VS1003�Ķ�����
	SPI1_ReadWriteByte(address);        //��ַ
	temp=SPI1_ReadWriteByte(0xff);		//��ȡ���ֽ�
	temp=temp<<8;
	temp+=SPI1_ReadWriteByte(0xff); 	//��ȡ���ֽ�
	VS_XCS_SET();       //MP3_CMD_CS=1; 
	SPI1_SetSpeed(SPI1_SPEED_16);//����
    return temp; 
}  
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//������Ԥ��ֵ
const u16 bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//����Kbps�Ĵ�С
//�õ�mp3&wma�Ĳ�����
u16 GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;            
    HEAD0=Vs1003_REG_Read(SPI_HDAT0); 
    HEAD1=Vs1003_REG_Read(SPI_HDAT1);
    switch(HEAD1)
    {        
        case 0x7665:return 0;//WAV��ʽ
        case 0X4D54:return 1;//MIDI��ʽ 
        case 0X574D://WMA��ʽ
        {
            HEAD1=HEAD0*2/25;
            if((HEAD1%10)>5)return HEAD1/10+1;
            else return HEAD1/10;
        }
        default://MP3��ʽ
        {
            HEAD1>>=3;
            HEAD1=HEAD1&0x03; 
            if(HEAD1==3)HEAD1=1;
            else HEAD1=0;
            return bitrate[HEAD1][HEAD0>>12];
        }
    } 
}  
//�������ʱ��                          
void ResetDecodeTime(void)
{
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);//��������
}
//�õ�mp3�Ĳ���ʱ��n sec
u16 GetDecodeTime(void)
{ 
    return Vs1003_REG_Read(SPI_DECODE_TIME);   
} 
//����Ƶ�׷����Ĵ��뵽VS1003
void LoadPatch(void)
{
	u16 i;
	for (i=0;i<943;i++)Vs1003_CMD_Write(atab[i],dtab[i]); 
	delay_ms(10);
}
//�õ�Ƶ������
void GetSpec(u8 *p)
{
	u8 byteIndex=0;
	u8 temp;
	Vs1003_CMD_Write(SPI_WRAMADDR,0x1804);                                                                                             
	for (byteIndex=0;byteIndex<14;byteIndex++) 
	{                                                                               
		temp=Vs1003_REG_Read(SPI_WRAM)&0x63;//ȡС��100����    
		*p++=temp;
	} 
}
 	  



 void SPI1_Init(void)
{	 
	RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ��   
	RCC->APB2ENR|=1<<12;      //SPI1ʱ��ʹ�� 

	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0XBBB00000;//PA567����	    
	GPIOA->ODR|=7<<5;      //PA567����
		
	SPI1->CR1|=0<<10;//ȫ˫��ģʽ	
	SPI1->CR1|=1<<9; //���nss����
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; //SPI����
	SPI1->CR1|=0<<11;//8bit���ݸ�ʽ	
	SPI1->CR1|=1<<1; //����ģʽ��SCKΪ1 CPOL=1
	SPI1->CR1|=1<<0; //���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	SPI1->CR1|=6<<3; //Fsck=Fcpu/256 //SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	SPI1->CR1|=0<<7; //MSBfirst 
	SPI1->CR1|=1<<6; //SPI�豸ʹ��		
	SPI1_ReadWriteByte(0xff);//��������		 
} 
 
//��ʼ��VS1003��IO��	 
	 
void Vs1003_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
     SPI1_Init();  //��ʼ��VS1003��SPI���˿�
	 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOB , ENABLE);
  
  	/* Configure PA4  PA8  as output push-pull */
  	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_12|GPIO_Pin_13;//ѡ��PA4 PA8
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ܽ�Ƶ��Ϊ50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//ģʽΪ�������
  	GPIO_Init(GPIOE, &GPIO_InitStructure);   //��ʼ��GPIOA�Ĵ���

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_5 ;//ѡ��PX.8-11
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ܽ�Ƶ��Ϊ50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//ģʽΪ�������
  	GPIO_Init(GPIOB, &GPIO_InitStructure);   //��ʼ��GPIOB�Ĵ���

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_14;//ѡ��PX.8-11
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ܽ�Ƶ��Ϊ50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;	//��������
  	GPIO_Init(GPIOE, &GPIO_InitStructure);   //��ʼ��GPIOB�Ĵ���

	Mp3Reset();   						//��λVS1003
}




