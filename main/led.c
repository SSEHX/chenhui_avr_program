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
	while(1);
	return 0;
}








