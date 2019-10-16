#include"i2c.h"

 sw_i2c_gpio_t dac1_i2c_port; //??DAC1?I2C??
 sw_i2c_gpio_t dac2_i2c_port; //??DAC2?I2C??

/**************************************************************************
***                          读取SDA脚的状态                             ***
***************************************************************************/


static uint8_t sw_i2c_sda_status(sw_i2c_gpio_t *gpio)
{
	uint8_t sda_status;
	
	sda_status = GPIO_ReadInputDataBit(gpio->SDA_Port,gpio->SDA_Pin);    
	return sda_status?1:0;
}
/**************************************************************************
***                          设置SDA脚为输入                             ***
***************************************************************************/
static void sw_i2c_set_sda_input(sw_i2c_gpio_t *gpio)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = gpio->SDA_Pin;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (gpio->SDA_Port, & GPIO_InitStructure );
}
/**************************************************************************
***                          设置SDA脚为输出                             ***
***************************************************************************/
static void sw_i2c_set_sda_output(sw_i2c_gpio_t *gpio)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = gpio->SDA_Pin;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //开漏输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (gpio->SDA_Port, & GPIO_InitStructure );
}



static void sw_i2c_start(sw_i2c_gpio_t *gpio)
{
	// I2C 开始时序：SCL=1时，SDA由1变成0.
	SW_I2C_SDA_HIGH;         
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;           
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SDA_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}





static void sw_i2c_stop(sw_i2c_gpio_t *gpio)
{
	// I2C 开始时序：SCL=1时，SDA由0变成1.
	SW_I2C_SDA_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SDA_HIGH;
}




static uint8_t sw_i2c_wait_ack(sw_i2c_gpio_t *gpio)
{
	uint8_t sda_status;
	uint8_t wait_time=0;
	uint8_t ack_nack = 1;
	
	//先设置SDA脚为输入
	SW_I2C_SDA_INPUT;
	//等待SDA脚被从机拉低
	while(SW_I2C_SDA_STATUS)
	{
		wait_time++;
		//如果等待时间过长，则退出等待
		if (wait_time>=10)
		{
			ack_nack = 0;
			break;
		}
	}
	// SCL由0变为1，读入ACK状态
	// 如果此时SDA=0，则是ACK
	// 如果此时SDA=1，则是NACK
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	
	//再次将SCL=0，并且将SDA脚设置为输出
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SDA_OUTPUT;
	i2c_delay_us(I2C_DELAY);
	return ack_nack;
}



static void sw_i2c_send_ack(sw_i2c_gpio_t *gpio)
{
	// 发送ACK就是在SDA=0时，SCL由0变成1
	SW_I2C_SDA_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}



static void sw_i2c_send_nack(sw_i2c_gpio_t *gpio)
{
	// 发送NACK就是在SDA=1时，SCL由0变成1
	SW_I2C_SDA_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}



static void sw_i2c_write_byte(sw_i2c_gpio_t *gpio,uint8_t aByte)
{
	uint8_t i;
	for (i=0;i<8;i++)
	{
		//先将SCL拉低；
		SW_I2C_SCL_LOW;
		i2c_delay_us(I2C_DELAY);
		//然后在SDA输出数据
		if(aByte&0x80)
		{
			SW_I2C_SDA_HIGH;
		}
		else
		{
			SW_I2C_SDA_LOW;
		}
		i2c_delay_us(I2C_DELAY);
		//最后将SCL拉高，在SCL上升沿写入数据
		SW_I2C_SCL_HIGH;
		i2c_delay_us(I2C_DELAY);
		
		aByte = aByte<<1;//数据位移
	}
	//写完一个字节只后要将SCL拉低
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}




static uint8_t sw_i2c_read_byte(sw_i2c_gpio_t *gpio)
{
	uint8_t i,aByte;
	
	//先将SDA脚设置为输入
	SW_I2C_SDA_INPUT;
	for (i=0;i<8;i++)
	{
		//数据位移
		aByte = aByte << 1;
		//延时等待SDA数据稳定
		i2c_delay_us(I2C_DELAY);
		//SCL=1，锁定SDA数据
		SW_I2C_SCL_HIGH;
		i2c_delay_us(I2C_DELAY);
		//读取SDA状态
		if(SW_I2C_SDA_STATUS)
		{
			aByte |= 0x01;
		}
		//SCL=0，解除锁定
		SW_I2C_SCL_LOW;
	}
	//读完一个字节，将SDA重新设置为输出
	SW_I2C_SDA_OUTPUT;
	return aByte;
}



void sw_i2c_init(sw_i2c_gpio_t *gpio)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd ( gpio->SCL_RCC_APB2Periph, ENABLE );                                                                
	GPIO_InitStructure.GPIO_Pin = gpio->SCL_Pin;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //开漏输出模式   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (gpio->SCL_Port, & GPIO_InitStructure );
	
	RCC_APB2PeriphClockCmd ( gpio->SDA_RCC_APB2Periph, ENABLE );                                                                
	GPIO_InitStructure.GPIO_Pin = gpio->SDA_Pin;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //开漏输出模式  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (gpio->SDA_Port, & GPIO_InitStructure );
	
	SW_I2C_SCL_HIGH;
	SW_I2C_SDA_HIGH;
}



void sw_i2c_write_nBytes(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint8_t *data,uint8_t len)
{
	uint8_t j;
	
	//如果使用的是7bit地址，需要左位移一位
#ifdef I2C_USE_7BIT_ADDR
	i2c_addr = i2c_addr<<1;
#endif
	
	//启动I2C
	sw_i2c_start(gpio);
	//写I2C从机地址，写操作
	sw_i2c_write_byte(gpio,i2c_addr);
	//如果从机响应ACC则继续，如果从机未响应ACK则停止
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	
	//开始写n个字节数据
	for (j=0;j<len;j++)
	{
		sw_i2c_write_byte(gpio,data[j]);
		// 每写一个字节数据后，都要等待从机回应ACK
		if (!sw_i2c_wait_ack(gpio))
			goto err;
	}
	
	//停止I2C
	err:
	sw_i2c_stop(gpio);
}


void sw_i2c_read_nBytes(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint8_t *buf,uint8_t len)
{
	uint8_t j;
	
	//如果使用的是7bit地址，需要左位移一位
#ifdef I2C_USE_7BIT_ADDR
	i2c_addr = i2c_addr<<1;
#endif
	
	//启动I2C
	sw_i2c_start(gpio);
	//写I2C从机地址，读操作
	sw_i2c_write_byte(gpio,i2c_addr|0x01);
	//如果从机响应ACC则继续，如果从机未响应ACK则停止
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	//开始读n个字节数据
	for (j=0;j<len;j++)
	{
		buf[j]=sw_i2c_read_byte(gpio);
		// 每读一个字节数据后，都要发送ACK给从机
		sw_i2c_send_ack(gpio);
	}
	
	//停止I2C
	err:
	sw_i2c_stop(gpio);
}

void sw_i2c_send2read_16bit(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t j;
	
	//如果使用的是7bit地址，需要左位移一位
#ifdef I2C_USE_7BIT_ADDR
	i2c_addr = i2c_addr<<1;
#endif
	//启动I2C
	sw_i2c_start(gpio);
	//写I2C从机地址，写操作
	sw_i2c_write_byte(gpio,i2c_addr);
	//如果从机响应ACC则继续，如果从机未响应ACK则停止
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	
	//写寄存器地址高8位
	sw_i2c_write_byte(gpio,(reg>>8)&0xff);
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	//写寄存器地址低8位
	sw_i2c_write_byte(gpio,reg&0xff);
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	
	//重新启动I2C
	sw_i2c_start(gpio);
	//写I2C从机地址，读操作
	sw_i2c_write_byte(gpio,i2c_addr|0x01);
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	//开始读n个字节数据
	for (j=0;j<len;j++)
	{
		buf[j]=sw_i2c_read_byte(gpio);
		// 每读一个字节数据后，都要发送ACK给从机
		sw_i2c_send_ack(gpio);
	}
	//停止I2C
	err:
	sw_i2c_stop(gpio);
}



void sw_i2c_send2read_8bit(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint8_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t j;
	
	//如果使用的是7bit地址，需要左位移一位
#ifdef I2C_USE_7BIT_ADDR
	i2c_addr = i2c_addr<<1;
#endif
	//启动I2C
	sw_i2c_start(gpio);
	//写I2C从机地址，写操作
	sw_i2c_write_byte(gpio,i2c_addr);
	//如果从机响应ACC则继续，如果从机未响应ACK则停止
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	
	//写寄存器地址
	sw_i2c_write_byte(gpio,reg);
		if (!sw_i2c_wait_ack(gpio))
			goto err;
	
	//重新启动I2C
	sw_i2c_start(gpio);
	//写I2C从机地址，读操作
	sw_i2c_write_byte(gpio,i2c_addr|0x01);
	if (!sw_i2c_wait_ack(gpio))
		goto err;
	//开始读n个字节数据
	for (j=0;j<len;j++)
	{
		buf[j]=sw_i2c_read_byte(gpio);
		// 每读一个字节数据后，都要发送ACK给从机
		sw_i2c_send_ack(gpio);
	}
	//停止I2C
	err:
	sw_i2c_stop(gpio);
}




/**************************************************************************
***                           对DAC芯片进行初始化                         ***
***************************************************************************/
void dac_init(void)  
{      
	dac1_i2c_port.SCL_RCC_APB2Periph = RCC_APB2Periph_GPIOA;
	dac1_i2c_port.SCL_Port=GPIOA;
	dac1_i2c_port.SCL_Pin=GPIO_Pin_10;
	dac1_i2c_port.SDA_RCC_APB2Periph = RCC_APB2Periph_GPIOA;
	dac1_i2c_port.SDA_Port=GPIOA;
	dac1_i2c_port.SDA_Pin=GPIO_Pin_11;
	
	sw_i2c_init(&dac1_i2c_port);//对DAC1的I2C进行初始化
	
	//对DAC2的I2C引脚进行负值
	dac2_i2c_port.SCL_RCC_APB2Periph = RCC_APB2Periph_GPIOA;
	dac2_i2c_port.SCL_Port=GPIOA;
	dac2_i2c_port.SCL_Pin=GPIO_Pin_8;
	dac2_i2c_port.SDA_RCC_APB2Periph = RCC_APB2Periph_GPIOA;
	dac2_i2c_port.SDA_Port=GPIOA;
	dac2_i2c_port.SDA_Pin=GPIO_Pin_9;

	sw_i2c_init(&dac2_i2c_port);//对DAC2的I2C进行初始化
	

}  
/**************************************************************************
***                       读取DAC芯片寄存器的值                           ***
***************************************************************************/
void mcp4725_read_reg(uint8_t ch,uint8_t *reg_data)
{
	if(ch == 1)//读取DAC1寄存器的值   
		sw_i2c_read_nBytes(&dac1_i2c_port,MCP4725_DAC1_I2C_ADDR,reg_data,5);
	else if(ch == 2)//读取DAC2寄存器的值  
		sw_i2c_read_nBytes(&dac2_i2c_port,MCP4725_DAC2_I2C_ADDR,reg_data,5);
}
/**************************************************************************
***                 写DAC芯片寄存器，实现DAC输出模拟电压                    ***
***************************************************************************/
void mcp4725_write_data_voltage(uint8_t mode,uint16_t voltage)   //电压单位mV
//void mcp4725_write_data_voltage(uint8_t ch,uint8_t mode,uint16_t voltage)   //电压单位mV
{
uint8_t data_buf[3];
uint16_t Dn;
	Dn = ( 4096 * voltage) / MCP4725_VREF;
data_buf[0] = ((Dn&0x0F00)>>8);// | 0x6F;
data_buf[1] = Dn & 0x00FF;
sw_i2c_write_nBytes(&dac1_i2c_port,MCP4725_DAC1_I2C_ADDR,data_buf,2);
		
}
/*


	uint8_t data_buf[3];
	uint16_t Dn;
	
	Dn = ( 4096 * voltage) / MCP4725_VREF;
	
	if(mode == 0) // 快速模式写dac 寄存器
	{
		data_buf[0] = ((Dn&0x0F00)>>8);// | 0x6F;
		data_buf[1] = Dn & 0x00FF;
		//if(ch == 1)
			sw_i2c_write_nBytes(&dac1_i2c_port,MCP4725_DAC1_I2C_ADDR,data_buf,2);
		//else if(ch == 2)
		//	sw_i2c_write_nBytes(&dac2_i2c_port,MCP4725_DAC2_I2C_ADDR,data_buf,2);
	}
	else if(mode == 1) // 写DAC寄存器
	{
		data_buf[0] = 0x40;
		data_buf[1] = (Dn&0x0FFF)>>4;// | 0x6F;
		data_buf[2] = ((Dn&0x0FFF)<<4) & 0x00F0;
		
		//if(ch == 1)
			sw_i2c_write_nBytes(&dac1_i2c_port,MCP4725_DAC1_I2C_ADDR,data_buf,3);
		//else if(ch == 2)
		//	sw_i2c_write_nBytes(&dac2_i2c_port,MCP4725_DAC2_I2C_ADDR,data_buf,3);
	}
	else if(mode == 2) // 写DAC寄存器和EEPROM
	{
		data_buf[0] = 0x60;
		data_buf[1] = (Dn&0x0FFF)>>4;// | 0x6F;
		data_buf[2] = ((Dn&0x0FFF)<<4) & 0x00F0;
		
	//	if(ch == 1)
			sw_i2c_write_nBytes(&dac1_i2c_port,MCP4725_DAC1_I2C_ADDR,data_buf,3);
		//else if(ch == 2)
		//	sw_i2c_write_nBytes(&dac2_i2c_port,MCP4725_DAC2_I2C_ADDR,data_buf,3);
	}

}
*/


/////////////////////


int i2c_start(void)
{
	// I2C 开始时序：SCL=1时，SDA由1变成0.
	SW_I2C_SDA_HIGH;         
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;           
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SDA_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}

int i2c_stop(void)
{
	// I2C 开始时序：SCL=1时，SDA由0变成1.
	SW_I2C_SDA_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SDA_HIGH;
}

static void i2c_set_sda_input()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入模式  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
}
static void i2c_set_sda_output()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //开漏输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (GPIOA, & GPIO_InitStructure );
}

static uint8_t i2c_sda_status()
{
	uint8_t sda_status;
	
	sda_status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);    
	return sda_status?1:0;
}

int i2c_wait_ack()
{
	uint8_t sda_status;
	uint8_t wait_time=0;
	uint8_t ack_nack = 1;
	
	//先设置SDA脚为输入
	i2c_set_sda_input();
	//等待SDA脚被从机拉低
	while(i2c_sda_status())
	{
		wait_time++;
		//如果等待时间过长，则退出等待
		if (wait_time>=10)
		{
			ack_nack = 0;
			break;
		}
	}
	// SCL由0变为1，读入ACK状态
	// 如果此时SDA=0，则是ACK
	// 如果此时SDA=1，则是NACK
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	
	//再次将SCL=0，并且将SDA脚设置为输出
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
	i2c_set_sda_output();
	i2c_delay_us(I2C_DELAY);
	return ack_nack;
}

int i2c_send_ack()
{
	// 发送ACK就是在SDA=0时，SCL由0变成1
	SW_I2C_SDA_LOW;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}

int i2c_send_nack()
{
	// 发送NACK就是在SDA=1时，SCL由0变成1
	SW_I2C_SDA_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_HIGH;
	i2c_delay_us(I2C_DELAY);
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}

int i2c_init()
{
	  GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10| GPIO_Pin_11; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	SW_I2C_SCL_HIGH;
	SW_I2C_SDA_HIGH;

}
int i2c_send_byte(uint8_t aByte)
{
uint8_t i;
	for (i=0;i<8;i++)
	{
		//先将SCL拉低；
		SW_I2C_SCL_LOW;
		i2c_delay_us(I2C_DELAY);
		//然后在SDA输出数据
		if(aByte&0x80)
		{
			SW_I2C_SDA_HIGH;
		}
		else
		{
			SW_I2C_SDA_LOW;
		}
		i2c_delay_us(I2C_DELAY);
		//最后将SCL拉高，在SCL上升沿写入数据
		SW_I2C_SCL_HIGH;
		i2c_delay_us(I2C_DELAY);
		
		aByte = aByte<<1;//数据位移
	}
	//写完一个字节只后要将SCL拉低
	SW_I2C_SCL_LOW;
	i2c_delay_us(I2C_DELAY);
}
int i2c_send_nbytes(uint8_t *data)
{
	uint8_t j;
	
	//如果使用的是7bit地址，需要左位移一位
#ifdef I2C_USE_7BIT_ADDR
	uint8_t addr= MCP4725_DAC1_I2C_ADDR<<1;// address=(0xc0>>1);
#endif
	
	//启动I2C
	i2c_start();
	//写I2C从机地址，写操作
	i2c_send_byte(addr);
	//如果从机响应ACC则继续，如果从机未响应ACK则停止
	if (!i2c_wait_ack())
		goto err;
	
	//开始写n个字节数据
	for (j=0;j<sizeof(data);j++)
	{
		i2c_send_byte(data[j]);
		// 每写一个字节数据后，都要等待从机回应ACK
		if (!i2c_wait_ack())
			goto err;
	}
	
	//停止I2C
	err:
	i2c_stop();
}


void send_voltage(uint16_t voltage)   //电压单位mV
{
uint8_t data_buf[2];
uint16_t Dn;
	Dn = ( 4096 * voltage) / MCP4725_VREF;
data_buf[0] = ((Dn&0x0F00)>>8);// | 0x6F;
data_buf[1] = Dn & 0x00FF;
i2c_send_nbytes(data_buf);
		
}
