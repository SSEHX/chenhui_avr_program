/*
 * prot_init.c
 *
 * Created: 2017/10/27 16:37:46
 *  Author: HXHZQ
 */ 


#include "port_init.h"
//-------------------------------------------------------
void port_init(void) //DDR 1-OUT,0-IN
{
	PORTC = 0x01;
	DDRC = 0xbf;//输入
	DDRE = 0xfe;
	PORTE = 0xc4;
	PORTD = 0xf3;  //6,7为T1,T2.  c3
	DDRD |= (1 << PD3) | 0x03 | 0x30; //串口输出,TWI端口PD3--com1  0x03--iic   0x30--TDS
	PORTF = 0x00;//PINF
	DDRF = 0x00;

	PORTA = 0xf3;//输出低电平 1c
	DDRA = 0xef;//输出
	DDRB = 0x3f;//输入触摸按钮
	PORTB = 0xf4;
	PORTG = 0x03;
	DDRG = 0x08;
}


void Port_Init(void)
{
		PORTB &= ~(0x01 << PB2);	
		PORTE &= ~((0x01 << PE7)|(0x01<<PE6));
	
		PORTB &= ~((0x01 << PB7)|(0x01 << PB6));
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