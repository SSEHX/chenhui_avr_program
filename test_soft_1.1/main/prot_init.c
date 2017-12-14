/*
 * prot_init.c
 *
 * Created: 2017/10/27 16:37:46
 *  Author: HXHZQ
 */ 


#include "port_init.h"
//-------------------------------------------------------


void Port_Init(void)
{
		PORTB &= ~(0x01 << PB2);	
		PORTE &= ~((0x01 << PE7)|(0x01<<PE6));
	
		PORTB &= ~((0x01 << PB7)|(0x01 << PB6));//ธ฿ันฃฌตอัน
		DDRB  &= ~((0x01 << PB7)|(0x01 << PB6));
		
		
		DDRB |= (0x01 << PB2) ;
		PORTB &= ~(0x01 << PB2);
		
		
		DDRE |= (0x01 << PE7)|(0x01<<PE6);
		PORTE &= ~((0x01 << PE7)|(0x01<<PE6));
}

void zhishui(void)
{
	PORTE |= (0x01 << PE7);
	PORTB |= (0x01 << PB2);
		
}
void close_zhishui(void)
{
	PORTB &= ~(0x01 << PB2);
	PORTE &= ~(0x01 << PE7);
}

void chongxi(void)
{
		PORTE |=(0x01<<PE6);
		zhishui();
}

void close_chongxi(void)
{
		close_zhishui();
		PORTE &= ~((0x01<<PE6));
}

int  cao_main(void)
{
	
	Port_Init();
	
	while(1)
	{
		zhishui();
		_delay_ms(1000);
		close_zhishui();
		_delay_ms(1000);
	}
	return 0;
}