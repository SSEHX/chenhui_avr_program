/*
 * beep.h
 *
 * Created: 2017/11/2 10:01:49
 *  Author: HXHZQ
 */ 


#ifndef BEEP_H_
#define BEEP_H_

#include <avr/io.h>
//#define	 F_CPU	11059200UL
#include <util/delay.h>

void BEEP_Init(void);
void BEEP(unsigned char sencond);//50*20ms = 1s

#endif /* BEEP_H_ */