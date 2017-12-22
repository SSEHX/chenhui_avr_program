/*
 * device.h
 *
 * Created: 2017/12/13 20:11:15
 *  Author: 许旗
 */ 


#ifndef DEVICE_H_
#define DEVICE_H_

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../port/port.h"
#include "../adc/adc.h"
#include "../tools/tools.h"

#define LED_OPEN 		PORTC &=    ~(1 << PC0)
#define LED_CLOSE		PORTC |=	 (1 << PC0)
#define LED_REVERSE		PORTC ^=	 (1 << PC0)

#define JINSHUI_OPEN	PORTE |=	 (1 << PE7)
#define JINSHUI_CLOSE	PORTE &=	~(1 << PE7)

#define CHUSHUI_OPEN	PORTE |=	 (1 << PE6)
#define CHUSHUI_CLOSE	PORTE &=	~(1 << PE6)

void beep(unsigned char second);
unsigned int get_hight_switch();
unsigned int get_Low_switch();
void start_create_water();
void stop_create_water();
void start_chongxi();
void stop_chongxi();
void get_all_adc_value();

#endif /* DEVICE_H_ */