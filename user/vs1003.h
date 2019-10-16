
#ifndef __VS1003_H__
#define __VS1003_H__
//#include "main.h"
#include "stm32f10x.h"
#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
//VS1003�Ĵ�������
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
  
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f   
#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 		 

#define VS_XCS_SET() GPIOE->ODR|=(1<<12)	  	//Ƭѡ�ź�	PE12
#define VS_XCS_CLR() GPIOE->ODR&=~(1<<12)	  	//Ƭѡ�ź�

#define VS_XRESET_SET() GPIOB->ODR|=(1<<5)  	//��λ�ź� PB5
#define VS_XRESET_CLR() GPIOB->ODR&=~(1<<5)  	//��λ�ź�

#define VS_XDCS_SET()  	GPIOE->ODR|=(1<<13)		//����ƬѡPE13
#define VS_XDCS_CLR()  	GPIOE->ODR&=~(1<<13)	

#define VS_DREQ    (GPIOE->IDR&(1<<14)) 			//��������PE14

u16  Vs1003_REG_Read(u8 address);//���Ĵ���
void Vs1003_DATA_Write(u8 data);//д����
void Vs1003_CMD_Write(u8 address,u16 data);//д����
void Vs1003_Init(void);			//��ʼ��VS1003
void Vs1003_DATA_Write(u8 data);//��vs1003д����
void Mp3Reset(void);			//Ӳ��λ
void Vs1003SoftReset(void);     //��λ
void VsRamTest(void);           //RAM����
void VsSineTest(void);          //���Ҳ���
u16 GetDecodeTime(void);   //�õ�����ʱ��
u16 GetHeadInfo(void);     //�õ�������
void ResetDecodeTime(void);//�������ʱ��
void LoadPatch(void);      //����Ƶ�׷�������	 
void GetSpec(u8 *p);       //�õ���������
void set1003(void);
void Read_VS_Set(void);
void Save_VS_Set(void);
void VS_SET_VOL(u8 VS_VOLT,u8 VS_BASS);
#endif

















