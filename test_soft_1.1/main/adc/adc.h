/*
 * adc.h
 *
 * Created: 2017/11/3 15:25:58
 *  Author: HXHZQ
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <util/delay.h>

#include "usart.h"
#include "led.h"
#include "lcd.h"

void ADC_GPIO_Init(void);
void ADC_Init(void);
unsigned int  ADC_Get(unsigned char n);//n «Õ®µ¿


#endif /* ADC_H_ */