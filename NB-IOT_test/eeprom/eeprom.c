/*
 * eeprom.c
 *
 * Created: 2017/12/12 21:21:51
 *  Author: 许旗
 */ 

#include <avr/io.h>
#include <string.h>
#include "eeprom.h"

/*------------------------------------------------------------------------------
* 函数名称：eeprom_write_byte
* 功    能：向eeprom写入一个字节

* 入口参数：	unsigned int	Address		写入地址
				unsigned char	byte		要写入的数据
				
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void eeprom_write_byte(unsigned int Address, unsigned char byte)
{
	while (EECR&(1 << EEWE));		//等待上一次写操作结束
	EEAR = Address;					//设置地址寄存器
	EEDR = byte;					//将数据放入数据缓存器
	EECR |= (1 << EEMWE);			//置位EEMWE,主机写使能
	EECR |= (1 << EEWE);			//置位EEWE,写使能，启动写操作
}

/*------------------------------------------------------------------------------
* 函数名称：eeprom_write_string
* 功    能：向eeprom写入一串字符串

* 入口参数：	unsigned int	Address			要写入的首地址
				unsigned char	Len				要写入的字符串长度
				unsigned char	*Data_buff		要写入的字符串指针
				
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void eeprom_write_string(unsigned int Address, unsigned char Len, unsigned char *Data_buff)
{
	EEARH = 0x00;
	while (Len--) {
		eeprom_write_byte(Address++, *Data_buff++);
	}
}

/*------------------------------------------------------------------------------
* 函数名称：eeprom_read_byte
* 功    能：从eeprom读取一个字节数据

* 入口参数：	unsigned int	Address			要写入的首地址

* 出口参数：无
* 返 回 值：	unsigned char					Address地址的数据
*-----------------------------------------------------------------------------*/
unsigned char eeprom_read_byte(unsigned int Address)
{
	while (EECR &(1 << EEWE));  //等待上一次写操作结束
	EEAR = Address;				//设置地址寄存器
	EECR |= (1 << EERE);		//读使能
	return EEDR;				//返回读入EEDR里面的数据
}

/*------------------------------------------------------------------------------
* 函数名称：eeprom_read_string
* 功    能：从eeprom读取Len长度的字符串

* 入口参数：	unsigned int	Address			要读取数据的开始地址
				unsigned char	Len				要读取的字符串长度
	
				
* 出口参数：	unsigned char	*Data_buff		要保存的字符串指针

* 返 回 值：无
*-----------------------------------------------------------------------------*/
void eeprom_read_string(unsigned int Address, unsigned char Len, unsigned char *Data_buff)
{
	while (EECR & (1 << EEWE));             //等待前面的写完成
	EEARH = 0x00;                           //写高字节地址
	while (Len--) {
		*Data_buff++ = eeprom_read_byte(Address++);
	}
}



