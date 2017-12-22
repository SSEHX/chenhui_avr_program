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



/*------------------------------------------------------------------------------
* 函数名称：tm1726_start
* 功    能：TM1726启动传输时序
				_____________________
			SCL						 \_________		保持SCL为高电平
				________________
			SDA					\______________		SDL从高电平跳转到低电平认为是开始信号
			
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
* 函数名称：tm1726_stop
* 功    能：TM1726结束传输时序
				_________________
			SCL					 \_________		保持SCL为高电平
						  _________________
			SDA	_________/						SDL从低电平跳转到高电平认为是传输结束信号
			
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
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
	
	unsigned char i, temp;

	temp = 0x01;  
	TM1726_SDA_0;
	TM1726_SCL_0;
	_delay_us(TINGD);
	for (i = 0; i < 8; i++)
	{
		TM1726_SCL_0;
		if ((temp & byte))
		{
			TM1726_SDA_1;
		}
		else
		{
			TM1726_SDA_0;
		}
		_delay_us(TINGD);
		TM1726_SCL_1;				
		byte >>= 1;   
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
* 返 回 值：无
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
* 返 回 值：无
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
* 返 回 值：无
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
* 返 回 值：无
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
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void write_string_address_tm1726(unsigned char Address,unsigned char len,unsigned char *Data)
{
	while(len--){
		write_byte_address_tm1726(Address++, *Data++);		
	}
}

const unsigned char  Device[] = "A1111378";


/*------------------------------------------------------------------------------
* 函数名称：init_lcd
* 功    能：初始化lcd显示屏
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void init_lcd()
{
	init_tm1726();
	_delay_ms(200);
	write_all_ram_tm1726(1);
	_delay_ms(1000);
	write_all_ram_tm1726(0);
	
}

/*------------------------------------------------------------------------------
* 函数名称：lcd_update_flow_display
* 功    能：更新lcd流量显示

* 入口参数：	unsigned long int	number			要显示的数字
									必须传入一个unsigned long int 类型的整数

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void lcd_update_flow_display(unsigned long int number){
	unsigned char	cache[5] = {0};
	make_flow_number(number, cache);
	write_string_address_tm1726(5, 5, cache);
}

/*------------------------------------------------------------------------------
* 函数名称：lcd_update_time_display
* 功    能：更新lcd时间显示

* 入口参数：	unsigned long int	number			要显示的数字
									必须传入一个unsigned long int 类型的整数

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void lcd_update_time_display(unsigned long int number){
	unsigned char cache[5] = {0};
	make_time_number(number, cache);
	
	write_string_address_tm1726(0, 5, cache);
}

/*------------------------------------------------------------------------------
* 函数名称：lcd_update_pure_ppm_display
* 功    能：更新lcd纯水ppm值

* 入口参数：	unsigned long int	number			要显示的数字

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void lcd_update_pure_ppm_display(unsigned int number){
	unsigned char cache[4] = {0};
	make_pure_ppm_number(number, cache);
	write_string_address_tm1726(10, 4, cache);
}

/*------------------------------------------------------------------------------
* 函数名称：lcd_update_raw_ppm_display
* 功    能：更新lcd原水ppm值

* 入口参数：	unsigned long int	number			要显示的数字

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void lcd_update_raw_ppm_display(unsigned int number){
	unsigned char cache[4] = {0};
	make_raw_ppm_number(number, cache);
	
	write_string_address_tm1726(14, 4, cache);
}

/*------------------------------------------------------------------------------
* 函数名称：make_number_array_down					down为数码管位置在下边

* 功    能：将传入的 number 转换为数码管显示的数组		
			用于TM1726流量显示的RAM填充数据
			
* 入口参数：	unsigned long int	number			要转换的数字	长度为5位

* 出口参数：	unsigned char		*number_array	用于存储转换后可填充RAM的字符数组

* 返 回 值：无
*-----------------------------------------------------------------------------*/
void make_time_number(unsigned long int number, unsigned char *number_array){
	if (number > 65535)			//判断是否为长整形					
	{
		number_array[4] =SMGL[ (number%10) ];
		number_array[3] =SMGL[ ((number/10)%10) ];
		number_array[2] =SMGL[ ((number/100)%10) ];
		number_array[1] =SMGL[ ((number/1000)%10) ];
		number_array[0] =SMGL[ ((number/10000)%10) ] | TIME_ICON;
	}else{
		unsigned int cache = (unsigned int)number;
		number_array[4] =SMGL[ (cache%10) ];
		number_array[3] =SMGL[ ((cache/10)%10) ];
		number_array[2] =SMGL[ ((cache/100)%10) ];
		number_array[1] =SMGL[ ((cache/1000)%10) ];
		number_array[0] =SMGL[ ((cache/10000)%10) ] | TIME_ICON;	
	}
	
	//根据设备当前状态，更新lcd
	if (!device_status_lcd.signal == NO)
	{
		for (unsigned char i = 0 ; i < device_status_lcd.signal ; i++)
		{
			number_array[i+1] |= SIGNAL_ICON;
		}
	}else{
		number_array[1] &= ~SIGNAL_ICON;
	}
	
}

void make_flow_number(unsigned long int number, unsigned char *number_array){
	if (number > 65535)			//判断是否为长整形
	{
		number_array[4] =SMGL[ (number%10) ];
		number_array[3] =SMGL[ ((number/10)%10) ] | FLOW_ICON;
		number_array[2] =SMGL[ ((number/100)%10) ];
		number_array[1] =SMGL[ ((number/1000)%10) ];
		number_array[0] =SMGL[ ((number/10000)%10) ];
		}else{
		unsigned int cache = (unsigned int)number;
		number_array[4] =SMGL[ (cache%10) ];
		number_array[3] =SMGL[ ((cache/10)%10) ] | FLOW_ICON;
		number_array[2] =SMGL[ ((cache/100)%10) ];
		number_array[1] =SMGL[ ((cache/1000)%10) ];
		number_array[0] =SMGL[ ((cache/10000)%10) ];
	}
	//根据设备当前状态，更新lcd
	if (device_status_lcd.signal == 5)
	{
		number_array[0] |= SIGNAL_ICON;
	}else{
		number_array[0] &= ~SIGNAL_ICON;
	}
}


/*------------------------------------------------------------------------------
* 函数名称：make_raw_ppm_number					down为数码管位置在上边

* 功    能：将传入的 number 转换为数码管显示的数组		
			用于TM1726流量显示的RAM填充数据
			
* 入口参数：	unsigned int	number				要转换的数字	长度为4位

* 出口参数：	unsigned char	*number_array		用于存储转换后可填充RAM的字符数组

* 返 回 值：无
*-----------------------------------------------------------------------------*/
void make_raw_ppm_number(unsigned int number, unsigned char *number_array){
	
	number_array[0] =SMG[ (number%10) ];
	number_array[1] =SMG[ ((number/10)%10) ];
	number_array[2] =SMG[ ((number/100)%10) ];
	number_array[3] =SMG[ ((number/1000)%10) ] | RAW_WATER_ICON;
	
	//根据设备当前状态，更新lcd
	if (device_status_lcd.no_water == YES)
	{
		number_array[0] |= NO_WATER_ICON;
	}else{
		number_array[0] &= ~NO_WATER_ICON;
	}
}

void make_pure_ppm_number(unsigned int number, unsigned char *number_array){
	
	number_array[0] =SMG[ (number%10) ] | PURE_WATER_ICON;
	number_array[1] =SMG[ ((number/10)%10) ];
	number_array[2] =SMG[ ((number/100)%10) ];
	number_array[3] =SMG[ ((number/1000)%10) ];
	
	//根据设备当前状态，更新lcd
	if (device_status_lcd.leakage)
	{
		number_array[3] |= LEAKAGE_ICON;
	}else{
		number_array[3] &= ~LEAKAGE_ICON;
	}

}


