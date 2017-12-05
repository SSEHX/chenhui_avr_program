/*
 * init.c
 *
 * Created: 2017/10/30 09:41:19
 *  Author: HXHZQ
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

//-----------------------------------------------------------------------------------------------
//***********************************************************定时器3中断处理程序end**********************************************************************
//--------------------
//------系统初始化--------

void init_devices(void)
{
	cli();
	  XDIV=0x00;
	  XMCRA=0x00;
	  port_init();
	  uart0_init();
	  uart1_init();
	  adc_init();
	  MCUCR=0x00;
	  EICRA=0x00;
	  EICRB=0x00;
	  EIMSK=0x00;
	  TIMSK=0x00;//中断T0
	  ETIMSK=0x00;
	  timer0_init();//时钟0初始化，定时
	  T1_init();
	  T2_init();
	  T3_init();
	  sei();
	  
}