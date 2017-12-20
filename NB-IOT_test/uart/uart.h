/*
 * uart.h
 *
 * Created: 2017/12/13 11:54:19
 *  Author: 许旗
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../device/device.h"

//uart 接收数组长度
#define UART0_RX_ARRAY_LEN  200
#define UART1_RX_ARRAY_LEN  200

//uart 发送数组长度
#define UART0_TX_ARRAY_LEN  200
#define UART1_TX_ARRAY_LEN  200

extern unsigned char uart0_rx_array[UART0_RX_ARRAY_LEN];
extern unsigned char uart0_rx_count;

extern unsigned char uart1_rx_array[UART1_RX_ARRAY_LEN];
extern unsigned char uart1_rx_count;

//uart 发送数组
extern unsigned char uart0_tx_array[UART0_TX_ARRAY_LEN];
extern unsigned char uart1_tx_array[UART1_TX_ARRAY_LEN];

void init_uart0(void);
void uart0_send_byte(unsigned char Data);
void uart0_send_string(unsigned char *Data);

void init_uart1(void);
void uart1_send_byte(unsigned char Data);
void uart1_send_string(unsigned char *Data);

void bc95_send_string(unsigned char *Data);

void uart1_rx_array_set_empty();
void uart0_rx_array_set_empty();
#endif /* UART_H_ */