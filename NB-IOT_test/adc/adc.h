/*
 * adc.h
 *
 * Created: 2017/12/13 14:27:37
 *  Author: 许旗
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "../uart/uart.h"

#define RAW_WATER	6
#define PURE_WATER	7
#define LEAKAGE		5

struct adc_data_list{
	unsigned int raw_water;		//原水tds
	unsigned int pure_water;	//纯水tds
	unsigned int leakage;		//漏水检测
};

struct adc_data_list adc_data;

//声明函数
void init_adc();
unsigned int adc_start_return(unsigned char channel);
void adc_start(unsigned char channel);

void adc_get_raw_water();
void adc_check_leakage();
void adc_get_pure_water();



#endif /* ADC_H_ */