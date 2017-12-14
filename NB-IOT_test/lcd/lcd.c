/*
 * lcd.c
 *
 * lcd驱动文件
 *
 * Created: 2017/12/14 9:40:24
 *  Author: 许旗
 */ 

#include "lcd.h"


unsigned char SMG[10] = { 0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,0xef,0xcf };//10后
unsigned char SMGL[10] = { 0xfa,0x60,0xbc,0xf4,0x66,0xd6,0xde,0x70,0xfe,0xf6 };//数码管转换 abcd xfge
unsigned char LCDNUM[32];        //LCD的显示数组



void tm1726_start()
{
	TM1726_SDA_1;
	TM1726_SCL_1;
	_delay_us(10);
	TM1726_SDA_0;
	_delay_us(10);
	TM1726_SCL_0;
	_delay_us(10);
}


void tm1726_stop()
{
	TM1726_SDA_0;
	TM1726_SCL_1;
	_delay_us(10);
	TM1726_SDA_1;
	_delay_us(10);
	TM1726_SCL_0;
	_delay_us(10);
}

/*------------------------------------------------------------------------------
* 函数名称：write_byte_tm1726
* 功    能：写一字节数据到TM1726

* 入口参数：	unsigned char byte			要写入的数据

* 出口参数：无
* 返 回 值：
*-----------------------------------------------------------------------------*/
void write_byte_tm1726(unsigned char byte)
{
	/*
	unsigned char i, temp;

	temp = 0x01;  //80
	TM1726_SDA_0;
	TM1726_SCL_0;
	_delay_us(TINGD);
	for (i = 0; i < 8; i++)
	{
		TM1726_SCL_0;
		if ((temp & byte) == 0)
		{
			TM1726_SDA_0;
		}
		else
		{
			TM1726_SDA_1;
		}
		_delay_us(TINGD);
		TM1726_SCL_1;				//clk = 1 for data write into 1632
		byte >>= 1;   //>>
	}
	*/
	
	unsigned char i ;
	TM1726_SDA_0;
	TM1726_SCL_0;
	_delay_us(TINGD);
	for (i=0x01 ; i<=128 ; i*=2)     
	{
		TM1726_SCL_0;
		if (i & byte)
		{
			TM1726_SDA_1;
		}
		else
		{
			TM1726_SDA_0;
		}
		_delay_us(TINGD);
		TM1726_SCL_1;				//clk = 1 for data write into 1632

	}
	
	TM1726_SCL_0;
	TM1726_SDA_0;
}

/*------------------------------------------------------------------------------
* 函数名称：write_all_ram_tm1726
* 功    能：填充TM1726的RAM数据

* 入口参数：	unsigned char write_empty		是否要写空
								1				全部RAM写为1
								0				全部RAM写为0

* 出口参数：无
* 返 回 值：
*-----------------------------------------------------------------------------*/
void write_all_ram_tm1726(unsigned char write_empty)
{
	unsigned char i;
	TM1726_SDA_1;
	TM1726_SCL_1;
	tm1726_start();
	_delay_us(TINGD);
	
	/*-------------------------------------------------
			   设置起始地址			要写入的地址		
	BIT		{	7	6	5	}{	4	3	2	1	0	}
				1	1	0			00H	...	11H
	--------------------------------------------------*/
	write_byte_tm1726(0xc0);
	
	for (i = 0; i < 18; i++)		//共有18个地址位
	{
		TM1726_SDA_1;
		TM1726_SCL_1;
		if (write_empty)
		{
			write_byte_tm1726(0xff);
		}	
		else
		{
			write_byte_tm1726(0x00);
		}	
	}
	tm1726_stop();
}

/*------------------------------------------------------------------------------
* 函数名称：update_all_ram_tm1726
* 功    能：从	LCDNUM[]	数组更新TM1726的RAM数据

* 入口参数：	unsigned char write_empty		是否要写空
								1				全部RAM写为1
								0				全部RAM写为0

* 出口参数：无
* 返 回 值：
*-----------------------------------------------------------------------------*/
void update_all_ram_tm1726()
{
	unsigned char i;
	TM1726_SDA_1;
	TM1726_SCL_1;
	tm1726_start();
	_delay_us(TINGD);
	write_byte_tm1726(0xc0);
	for (i = 0; i < 18; i++)
	{
		TM1726_SDA_1;
		TM1726_SCL_1;
		write_byte_tm1726(LCDNUM[i]);
	}
	tm1726_stop();
}

/*------------------------------------------------------------------------------
* 函数名称：init_tm1726
* 功    能：初始化TM1726	向TM1726写入一个字节控制TM1726显示状态
			BIT		7		6		5	4	3		2	1	0
					0	{	0	 }	1	1	0	{	1	1	1	}
				   命令 {正常模式}				{  驱动5V显示屏 }
			PS:详见数据手册
* 入口参数：无
* 出口参数：无
* 返 回 值：
*-----------------------------------------------------------------------------*/
void init_tm1726(void)
{
	TM1726_SDA_1;
	TM1726_SCL_1;
	tm1726_start();
	_delay_us(TINGD);
	write_byte_tm1726(0x37);
	tm1726_stop();
}

/*------------------------------------------------------------------------------
* 函数名称：write_byte_address_tm1726
* 功    能：向TM1726的固定地址写入固定数据

* 入口参数：	unsigned char Address		要写入的地址
				unsigned char byte			要写入的数据
				
* 出口参数：无
* 返 回 值：
*-----------------------------------------------------------------------------*/
void write_byte_address_tm1726(unsigned char Address, unsigned char byte)
{
	TM1726_SDA_1;
	TM1726_SCL_1;
	tm1726_start();
	_delay_ms(10);
	write_byte_tm1726(0xc0 + Address);
	TM1726_SDA_1;
	TM1726_SCL_1;
	_delay_ms(10);
	write_byte_tm1726(byte);
	tm1726_stop();
}

/*------------------------------------------------------------------------------
* 函数名称：write_byte_address_tm1726
* 功    能：向TM1726的指定地址为起点写入一串数据

* 入口参数：	unsigned char Address		要写入的地址
				unsigned char *Data			要写入的数据地址
				
* 出口参数：无
* 返 回 值：
*-----------------------------------------------------------------------------*/
void write_string_address_tm1726(unsigned char Address,unsigned char len,unsigned char *Data)
{
	unsigned char i;
	TM1726_SDA_1;
	TM1726_SCL_1;
	tm1726_start();
	_delay_us(TINGD);
	write_byte_tm1726(0xc0 + Address);
	// all RAM have 96
	for (i = 0; i < len; i++)
	{
		TM1726_SDA_1;
		TM1726_SCL_1;
		write_byte_tm1726((*(Data + i)));
	}
	tm1726_stop();
}


void lcd_init()
{
	init_tm1726();
	_delay_ms(200);
	write_all_ram_tm1726(1);
	_delay_ms(1000);
	//WRITE_ALLRAM_1726(0);
	
	//flow_set(&Device[3]);//显示设备码
	//REFRESH_ALLRAM_1726();
	//_delay_ms(3000);
}