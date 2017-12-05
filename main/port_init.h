/*
 * port_init.h
 *
 * Created: 2017/11/27 09:10:51
 *  Author: HXHZQ
 */ 


#ifndef PORT_INIT_H_
#define PORT_INIT_H_

#include <avr/io.h>
#include "led.h"
#include "beep.h"


void Port_Init(void);
void zhishui(void);
void close_zhishui(void);
void chongxi(void);
void close_chongxi(void);

#endif /* PORT_INIT_H_ */