/*
 * beep.c
 *
 * Created: 2017/10/30 09:23:28
 *  Author: HXHZQ
 */ 

#include "beep.h"
#include "led.h"

/*********************************
¹¦ÄÜ£º·äÃùÆ÷ÏìÉù
*********************************/

void BEEP_Init(void)
{
	DDRB |= (1<<PB0);
	PORTB &= ~(1<<PB0);
}

void BEEP(unsigned char sencond)//50*20ms = 1s
{
	unsigned char i;
	PORTB |= (1 << PB0);
	for (i = 0; i < sencond; i++)
	{
		_delay_ms(20);
	}
	PORTB &= ~(1 << PB0);
}

int beepmain(void)
{
	LED_Init();
	BEEP_Init();
	while(1)
	{
		BEEP(10);
		LEDRUN;
		_delay_ms(1000);//F_CPU/10^6us
		
	}
	return 0;
}