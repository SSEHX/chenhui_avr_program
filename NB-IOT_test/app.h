/*
 * app.h
 *
 * Created: 2017/12/12 20:56:02
 *  Author: 许旗
 */ 

#ifndef APP_H_
#define APP_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "eeprom/eeprom.h"
#include "port/port.h"
#include "adc/adc.h"
#include "uart/uart.h"
#include "device/device.h"
#include "lcd/lcd.h"
#include "bc95/bc95.h"
#include "init/init.h"


//定义变量
unsigned char		adc_channel_data_list[8];
unsigned char		adc_interrupt_channel_cache;

#endif /* APP_H_ */