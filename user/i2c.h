#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f10x.h"
#include<stdio.h>
typedef  struct {
	uint32_t SDA_RCC_APB2Periph;// SDA???
	GPIO_TypeDef* SDA_Port;//SDA?Port
	uint16_t SDA_Pin;//SDA?Pin
	
	uint32_t SCL_RCC_APB2Periph;//SCL???
	GPIO_TypeDef* SCL_Port;//SCL?Port
	uint16_t SCL_Pin;//SCL?Pin
} sw_i2c_gpio_t;



#define MCP4725_VREF	3300

#define TRUE	1
#define FALSE 0
#define I2C_USE_7BIT_ADDR //??????????7Bit??,??????,????????
#define I2C_DELAY 1 // I2C??Bit???????,????I2C?????

/*
#define SW_I2C_SCL_LOW          GPIO_ResetBits(gpio->SCL_Port,gpio->SCL_Pin) // I2C SCL???0
#define SW_I2C_SCL_HIGH         GPIO_SetBits(gpio->SCL_Port,gpio->SCL_Pin) // I2C SCL???1
#define SW_I2C_SDA_LOW          GPIO_ResetBits(gpio->SDA_Port,gpio->SDA_Pin) // I2C SDA???0
#define SW_I2C_SDA_HIGH         GPIO_SetBits(gpio->SDA_Port,gpio->SDA_Pin) // I2C SDA???1
*/
/////my define
#define SW_I2C_SCL_LOW          GPIO_ResetBits(GPIOA,GPIO_Pin_10) // I2C SCL???0
#define SW_I2C_SCL_HIGH         GPIO_SetBits(GPIOA,GPIO_Pin_10) // I2C SCL???1
#define SW_I2C_SDA_LOW          GPIO_ResetBits(GPIOA,GPIO_Pin_11) // I2C SDA???0
#define SW_I2C_SDA_HIGH         GPIO_SetBits(GPIOA,GPIO_Pin_11) // I2C SDA???1
////
#define SW_I2C_SDA_INPUT        sw_i2c_set_sda_input(gpio) // ?SDA????????
#define SW_I2C_SDA_OUTPUT        sw_i2c_set_sda_output(gpio) // ?SDA????????
#define SW_I2C_SDA_STATUS        sw_i2c_sda_status(gpio) // ??SDA???????  
  
#define i2c_delay_us(a)           delay_us(a) // ??SDA???????
#define MCP4725_DAC1_I2C_ADDR       (0xC0>>1)//DAC1 ?I2C??(7Bit??,??????I2C_USE_7BIT_ADDR??)
#define MCP4725_DAC2_I2C_ADDR       (0xC0>>1)//DAC2 ?I2C??(7Bit??,??????I2C_USE_7BIT_ADDR??)



static uint8_t sw_i2c_sda_status(sw_i2c_gpio_t *gpio);
static void sw_i2c_set_sda_input(sw_i2c_gpio_t *gpio);
static void sw_i2c_set_sda_output(sw_i2c_gpio_t *gpio);
static void sw_i2c_start(sw_i2c_gpio_t *gpio);
static void sw_i2c_stop(sw_i2c_gpio_t *gpio);
static uint8_t sw_i2c_wait_ack(sw_i2c_gpio_t *gpio);
static void sw_i2c_send_ack(sw_i2c_gpio_t *gpio);
static void sw_i2c_send_nack(sw_i2c_gpio_t *gpio);
static void sw_i2c_write_byte(sw_i2c_gpio_t *gpio,uint8_t aByte);
static uint8_t sw_i2c_read_byte(sw_i2c_gpio_t *gpio);
void sw_i2c_init(sw_i2c_gpio_t *gpio);
void sw_i2c_write_nBytes(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint8_t *data,uint8_t len);
void sw_i2c_read_nBytes(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint8_t *buf,uint8_t len);
void sw_i2c_send2read_16bit(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint16_t reg,uint8_t *buf,uint8_t len);
void sw_i2c_send2read_8bit(sw_i2c_gpio_t *gpio,uint8_t i2c_addr,uint8_t reg,uint8_t *buf,uint8_t len);
void dac_init(void)  ;
void mcp4725_read_reg(uint8_t ch,uint8_t *reg_data);
void mcp4725_write_data_voltage(uint8_t mode,uint16_t voltage);
//void mcp4725_write_data_voltage(uint8_t ch,uint8_t mode,uint16_t voltage);

#endif // __MYI2C_H__
