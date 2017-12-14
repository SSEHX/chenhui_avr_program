/*
 * device.c
 *
 * Created: 2017/12/13 11:19:33
 *  Author: 许旗
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "device.h"

/*------------------------------------------------------------------------------
* 函数名称：beep
* 功    能：蜂鸣器蜂鸣

* 入口参数：	unsigned char second	蜂鸣时间长短
				
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void beep(unsigned char second)
{
	unsigned char i;
	PORTB |= (1 << PB0);
	for (i = 0; i < second; i++)
	{
		_delay_ms(20);
	}
	PORTB &= ~(1 << PB0);
}
