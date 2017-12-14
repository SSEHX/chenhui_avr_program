/*
 * led.c
 *
 * Created: 2017/11/2 09:23:40
 *  Author: HXHZQ
 */ 
#include "led.h"

void LED_Init(void)
{
	DDRC |= (1<<PC0);
	PORTC |= (1<<PC0);
}


int led_main(void)
{
	LED_Init();
	LEDOFF;
	
	PORTA |= (0x01<<PA2);
//	PORTA &=~(0x01 << PA2);//低电平输出1.35V,高电平输出零。
	DDRA &= ~(0x01 << PA2);
	
	while(1);
	return 0;
}








