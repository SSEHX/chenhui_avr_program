/*
 * wdt.c
 *
 * Created: 2017/11/17 17:50:59
 *  Author: HXHZQ
 */ 

#include "wdt_use.h"
#include "led.h"



int wdt_main(void)
{
	cli();
	wdt_enable(WDTO_2S);
	LED_Init();
	LEDON;
	BEEP(1);
	
	sei();

	while(1)
	{
		
		_delay_ms(1000);
		wdt_reset();
		LEDRUN;
	}
	
	return 0;
}