/*
 * init_1721.c
 *
 * Created: 2017/10/30 09:31:55
 *  Author: HXHZQ
 */ 

#include <avr/io.h>
//#define	 F_CPU	11059200UL//16000000UL		// 单片机主频为7.3728MHz,用于延时子程序

#include <util/delay.h>




int main_(void)
{

	while(1)
	{
		BEEP(3);
		LEDRUN;
		_delay_ms(200);
	}
	return 0;
}