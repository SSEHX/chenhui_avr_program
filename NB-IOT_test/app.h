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

#include "eeprom/eeprom.h"
#include "port/port.h"
#include "adc/adc.h"
#include "uart/uart.h"
#include "device/device.h"
#include "lcd/lcd.h"

//uart 接收数组和接收计数
extern unsigned char uart0_rx_array[UART0_RX_ARRAY_LEN];
extern unsigned char uart0_rx_count;

extern unsigned char uart1_rx_array[UART1_RX_ARRAY_LEN];
extern unsigned char uart1_rx_count;

//uart 发送数组
extern unsigned char uart0_tx_array[UART0_TX_ARRAY_LEN];
extern unsigned char uart1_tx_array[UART1_TX_ARRAY_LEN];


//定义变量
unsigned int		adc_channel_data_list[8];
unsigned char		adc_interrupt_channel_cache;

#endif /* APP_H_ */