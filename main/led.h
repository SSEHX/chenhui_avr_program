/*
 * led.h
 *
 * Created: 2017/11/2 09:37:22
 *  Author: HXHZQ
 */ 


#ifndef LED_H_
#define LED_H_

#include <avr/io.h>

#define LEDOFF  PORTC |= (1<<PC0)
#define LEDON	PORTC &= ~(1<<PC0)
#define LEDRUN  PORTC ^=(1<<PC0)			//Ö¸Ê¾µÆÉÁË¸

void LED_Init(void);

 
#endif /* LED_H_ */