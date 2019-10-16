#include "base.h"
#include "vs1003.h"
#include "spec_rew.h"	 
//VS1003的全功能函数
//支持SIN测试和RAM测试
//并加入了VS1003的频谱显示代码，不过说实话不咋地，还不如自己写的频谱分析，怀疑是不是真实的频谱变换？  
//正点原子@SCUT
//V1.1
#define SPI1_SPEED_2   0
#define SPI1_SPEED_8   1
#define SPI1_SPEED_16  2
#define SPI1_SPEED_256 3 //281K
//VS1003设置参数
//0,高音控制；1，最低频限；2，低音控制；3，最低频限
//0,henh.1,hfreq.2,lenh.3,lfreq 5,主音量


//EEPROM地址：488~489 共2个
u8 VS_VOLT=200;	 //默认音量200
u8 VS_BASS=0; 	//默认非超重低音

					 
//SPI1口读写一个字节
//TxData:要发送的字节		 
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
	while((SPI1->SR&1<<1)==0);//等待发送区空				  
	SPI1->DR=TxData;	 	  //发送一个byte   
	while((SPI1->SR&1<<0)==0);//等待接收完一个byte  
	return SPI1->DR;          //返回收到的数据				    
}
/*******************************************************************************
* Function Name  : SPI1_SetSpeed
* Description    : SPI1设置速度为高速
* Input          : u8 SpeedSet 
*                  如果速度设置输入0，则低速模式，非0则高速模式
*                  SPI_SPEED_HIGH   1
*                  SPI_SPEED_LOW    0
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI1_SPEED_2)//二分频
	{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI1_SPEED_8)//八分频 
	{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI1_SPEED_16)//十六分频
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256分频
	{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz 低速模式
	}
	SPI1->CR1|=1<<6; //SPI设备使能	  
} 

void VS_SET_VOL(u8 volt,u8 bass)
{
	u16 temp_volt,temp_bass;
	volt=255-volt;
	temp_volt=volt;
   	temp_volt=(temp_volt<<8)+volt;
	temp_bass=(temp_bass<<8)+bass;
    Vs1003_CMD_Write(SPI_VOL,temp_volt); //设音量 
	Vs1003_CMD_Write(SPI_BASS,temp_bass);//BASS 
}
	 
//软复位VS1003
void Vs1003SoftReset(void)
{	 
	u8 retry; 				   
	while((VS_DREQ)==0);//等待软件复位结束
	SPI1_ReadWriteByte(0X00);//启动传输
	retry=0;
	while(Vs1003_REG_Read(SPI_MODE)!=0x0804)// 软件复位,新模式  
	{
		Vs1003_CMD_Write(SPI_MODE,0x0804);// 软件复位,新模式
		delay_ms(2);//等待至少1.35ms 
		if(retry++>100)break; 
	}	 				  
	while (VS_DREQ == 0);//等待软件复位结束	   

	retry=0;
	while(Vs1003_REG_Read(SPI_CLOCKF)!=0X9800)//设置vs1003的时钟,3倍频 ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_CLOCKF,0X9800);//设置vs1003的时钟,3倍频 ,1.5xADD
		if(retry++>100)break; 
	}		   
	retry=0;
	while(Vs1003_REG_Read(SPI_AUDATA)!=0XBB81)//设置vs1003的时钟,3倍频 ,1.5xADD 
	{
		Vs1003_CMD_Write(SPI_AUDATA,0XBB81);
		if(retry++>100)break; 
	}
	//Vs1003_CMD_Write(SPI_CLOCKF,0X9800); 	    
	//Vs1003_CMD_Write(SPI_AUDATA,0XBB81); //采样率48k，立体声	
	VS_SET_VOL(VS_VOLT,VS_BASS);		 
	ResetDecodeTime();//复位解码时间	    
    //向vs1003发送4个字节无效数据，用以启动SPI发送
    VS_XDCS_CLR();//选中数据传输
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	SPI1_ReadWriteByte(0XFF);
	VS_XDCS_SET();//取消数据传输
	delay_ms(20);
} 
//硬复位MP3
void Mp3Reset(void)
{
	VS_XRESET_CLR();
	delay_ms(20);
	VS_XDCS_SET();	//取消数据传输
	VS_XCS_SET();//取消数据传输
	VS_XRESET_SET();    
	while(VS_DREQ==0);	//等待DREQ为高
	delay_ms(20);				 
}
//正弦测试 
void VsSineTest(void)
{											    
	Mp3Reset();	 
	Vs1003_CMD_Write(0x0b,0X2020);	  //设置音量	 
 	Vs1003_CMD_Write(SPI_MODE,0x0820);//进入vs1003的测试模式	    
	while (VS_DREQ == 0);     //等待DREQ为高
 	//向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设定vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
    VS_XDCS_CLR();//选中数据传输
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
    //退出正弦测试
    VS_XDCS_CLR();//选中数据传输
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

    //再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值
    VS_XDCS_CLR();//选中数据传输      
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
    //退出正弦测试
    VS_XDCS_CLR();//选中数据传输
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
//ram 测试 	
u16 regvalue ;																			 
void VsRamTest(void)
{
		   
	Mp3Reset();     
 	Vs1003_CMD_Write(SPI_MODE,0x0820);// 进入vs1003的测试模式
	while (VS_DREQ==0); // 等待DREQ为高
 	VS_XDCS_CLR();       			  // xDCS = 1，选择vs1003的数据接口
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
	regvalue = Vs1003_REG_Read(SPI_HDAT0); // 如果得到的值为0x807F，则表明完好。
	///printf("regvalueH:%x\n",regvalue>>8);//输出结果 
	//printf("regvalueL:%x\n",regvalue&0xff);//输出结果 
}     
//向VS1003写命令
//address:命令地址
//data:命令数据
void Vs1003_CMD_Write(u8 address,u16 data)
{  
    while(VS_DREQ==0);//等待空闲
	SPI1_SetSpeed(SPI1_SPEED_256);//低速 
	 
	VS_XDCS_SET(); //MP3_DATA_CS=1;
	VS_XCS_CLR(); //MP3_CMD_CS=0; 
	
	SPI1_ReadWriteByte(VS_WRITE_COMMAND);//发送VS1003的写命令
	SPI1_ReadWriteByte(address); //地址
	SPI1_ReadWriteByte(data>>8); //发送高八位
	SPI1_ReadWriteByte(data);	 //第八位
	VS_XCS_SET();         //MP3_CMD_CS=1; 
	SPI1_SetSpeed(SPI1_SPEED_16);//高速
} 
//向VS1003写数据
void Vs1003_DATA_Write(u8 data)
{
	VS_XDCS_CLR();   //MP3_DATA_CS=0;
	//while(VS_DREQ==0);  //VS1003的DREQ为高才能写入数据？？？？这句可以不要？？
	SPI1_ReadWriteByte(data);
	VS_XDCS_SET();  //MP3_DATA_CS=1;
	VS_XCS_SET();   //MP3_CMD_CS=1; 
}         
//读VS1003的寄存器           
//读VS1003
//注意不要用倍速读取,会出错
u16 Vs1003_REG_Read(u8 address)
{ 
	u16 temp=0; 
    while(VS_DREQ==0);//非等待空闲状态 
	SPI1_SetSpeed(SPI1_SPEED_256);//低速 
	VS_XDCS_SET();       //MP3_DATA_CS=1;
	VS_XCS_CLR();        //MP3_CMD_CS=0;
	SPI1_ReadWriteByte(VS_READ_COMMAND);//发送VS1003的读命令
	SPI1_ReadWriteByte(address);        //地址
	temp=SPI1_ReadWriteByte(0xff);		//读取高字节
	temp=temp<<8;
	temp+=SPI1_ReadWriteByte(0xff); 	//读取低字节
	VS_XCS_SET();       //MP3_CMD_CS=1; 
	SPI1_SetSpeed(SPI1_SPEED_16);//高速
    return temp; 
}  
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//比特率预定值
const u16 bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//返回Kbps的大小
//得到mp3&wma的波特率
u16 GetHeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;            
    HEAD0=Vs1003_REG_Read(SPI_HDAT0); 
    HEAD1=Vs1003_REG_Read(SPI_HDAT1);
    switch(HEAD1)
    {        
        case 0x7665:return 0;//WAV格式
        case 0X4D54:return 1;//MIDI格式 
        case 0X574D://WMA格式
        {
            HEAD1=HEAD0*2/25;
            if((HEAD1%10)>5)return HEAD1/10+1;
            else return HEAD1/10;
        }
        default://MP3格式
        {
            HEAD1>>=3;
            HEAD1=HEAD1&0x03; 
            if(HEAD1==3)HEAD1=1;
            else HEAD1=0;
            return bitrate[HEAD1][HEAD0>>12];
        }
    } 
}  
//重设解码时间                          
void ResetDecodeTime(void)
{
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);
	Vs1003_CMD_Write(SPI_DECODE_TIME,0x0000);//操作两次
}
//得到mp3的播放时间n sec
u16 GetDecodeTime(void)
{ 
    return Vs1003_REG_Read(SPI_DECODE_TIME);   
} 
//加载频谱分析的代码到VS1003
void LoadPatch(void)
{
	u16 i;
	for (i=0;i<943;i++)Vs1003_CMD_Write(atab[i],dtab[i]); 
	delay_ms(10);
}
//得到频谱数据
void GetSpec(u8 *p)
{
	u8 byteIndex=0;
	u8 temp;
	Vs1003_CMD_Write(SPI_WRAMADDR,0x1804);                                                                                             
	for (byteIndex=0;byteIndex<14;byteIndex++) 
	{                                                                               
		temp=Vs1003_REG_Read(SPI_WRAM)&0x63;//取小于100的数    
		*p++=temp;
	} 
}
 	  



 void SPI1_Init(void)
{	 
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能   
	RCC->APB2ENR|=1<<12;      //SPI1时钟使能 

	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0XBBB00000;//PA567复用	    
	GPIOA->ODR|=7<<5;      //PA567上拉
		
	SPI1->CR1|=0<<10;//全双工模式	
	SPI1->CR1|=1<<9; //软件nss管理
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; //SPI主机
	SPI1->CR1|=0<<11;//8bit数据格式	
	SPI1->CR1|=1<<1; //空闲模式下SCK为1 CPOL=1
	SPI1->CR1|=1<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	SPI1->CR1|=6<<3; //Fsck=Fcpu/256 //SPI1->CR1|=7<<3; //Fsck=Fcpu/256
	SPI1->CR1|=0<<7; //MSBfirst 
	SPI1->CR1|=1<<6; //SPI设备使能		
	SPI1_ReadWriteByte(0xff);//启动传输		 
} 
 
//初始化VS1003的IO口	 
	 
void Vs1003_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
     SPI1_Init();  //初始化VS1003的SPI及端口
	 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOB , ENABLE);
  
  	/* Configure PA4  PA8  as output push-pull */
  	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_12|GPIO_Pin_13;//选择PA4 PA8
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//管脚频率为50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//模式为推挽输出
  	GPIO_Init(GPIOE, &GPIO_InitStructure);   //初始化GPIOA寄存器

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_5 ;//选择PX.8-11
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//管脚频率为50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//模式为推挽输出
  	GPIO_Init(GPIOB, &GPIO_InitStructure);   //初始化GPIOB寄存器

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_14;//选择PX.8-11
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//管脚频率为50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;	//上拉输入
  	GPIO_Init(GPIOE, &GPIO_InitStructure);   //初始化GPIOB寄存器

	Mp3Reset();   						//复位VS1003
}




