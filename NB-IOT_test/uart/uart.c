/*
 * uart.c
 *
 * Created: 2017/12/13 11:54:07
 *  Author: 许旗
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

//uart 接收数组和接收计数
unsigned char uart0_rx_array[UART0_RX_ARRAY_LEN] = {0};
unsigned char uart0_rx_count	 = 0;

unsigned char uart1_rx_array[UART1_RX_ARRAY_LEN] = {0};
unsigned char uart1_rx_count	 = 0;

//uart 发送数组
unsigned char uart0_tx_array[UART0_TX_ARRAY_LEN] = {0};
unsigned char uart1_tx_array[UART1_TX_ARRAY_LEN] = {0};

/*------------------------------------------------------------------------------
* 函数名称：uart0_init
* 功    能：初始化串口0
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void uart0_init(void)
{
	
//	UCSR0A=0x00;		
	UCSR0C=0x06;		//0000 0110
	
	//波特率控制
	UBRR0L=5;			//(F_CPU/16/(baud+1)%256);
	UBRR0H=0;			//(F_CPU/16/(baud+1)/256);
	
	//		   接收完成中断使能		  TX发送使能		   RX接收使能
	UCSR0B	|=	(1 << RXCIE0)	|	 (1 << TXEN0)	|	 (1 <<  RXEN0);
}

/*------------------------------------------------------------------------------
* 函数名称：uart1_init
* 功    能：初始化串口1
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void uart1_init(void)
{	
	UCSR1C|=0x06;//0x06
	
	//波特率9600
	UBRR1L=5;//(F_CPU/(16*baud)-1)%256; ok //(F_CPU/16/(baud+1)%256);8   5-11.0592
	UBRR1H=0;//(F_CPU/(16*baud)-1)/256; ok //(F_CPU/16/(baud+1)/256);0
	
	//		接收完成中断使能		  TX发送使能		   RX接收使能
	UCSR1B	|=	(1 << RXCIE1)	|	 (1 << TXEN1)	 |	 (1 << RXEN1);
}

/*------------------------------------------------------------------------------
* 函数名称：uart0_send_byte
* 功    能：uart0 发送一字节数据

* 入口参数：	unsigned char Data		要发送的字节

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void uart0_send_byte(unsigned char Data)
{
	while( !(UCSR0A & (1<<UDRE0)));		//判断发送缓冲区是否可用写入数据
	UDR0 = Data;
}

/*------------------------------------------------------------------------------
* 函数名称：uart0_send_string
* 功    能：uart0 发送一个字符串

* 入口参数：	unsigned char *Data		要发送的字符串指针

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void uart0_send_string(unsigned char *Data){
	unsigned char i;
	for (i = 0 ; (*Data++) != '\0'  ; i++)
	{
		uart0_send_byte(*Data);
	}
}

/*------------------------------------------------------------------------------
* 函数名称：uart1_send_byte
* 功    能：uart1 发送一字节数据

* 入口参数：	unsigned char Data		要发送的字节

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void uart1_send_byte(unsigned char Data)
{
	while( !(UCSR1A & (1<<UDRE1)));		//判断发送缓冲区是否可用写入数据
	UDR1 = Data;
}

/*------------------------------------------------------------------------------
* 函数名称：uart1_send_string
* 功    能：uart1 发送一个字符串

* 入口参数：	unsigned char *Data		要发送的字符串指针

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void uart1_send_string(unsigned char *Data){
	unsigned char i;
	for (i = 0 ; (*Data++) != '\0'  ; i++)
	{
		uart1_send_byte(*Data);
	}
}

/*------------------------------------------------------------------------------
* 函数名称：SIGNAL(USART0_RX_vect)
* 功    能：uart0 接收中断，将数据存入uart0_rx_array[]数组内

* 中断向量：USART0_RX_vect	接收完成

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
SIGNAL(USART0_RX_vect){
	while( !(UCSR0A & (1<<RXC0)) );		//判断缓冲区是否有数据
	uart0_rx_array[uart0_rx_count] = UDR0;
	uart0_rx_count++;
	
	if (uart0_rx_count >= UART0_RX_ARRAY_LEN)
	{
		uart0_rx_count = 0;
	}
}

/*------------------------------------------------------------------------------
* 函数名称：SIGNAL(USART1_RX_vect)
* 功    能：uart1 接收中断，将数据存入uart1_rx_array[]数组内

* 中断向量：USART1_RX_vect	接收完成

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
SIGNAL(USART1_RX_vect){
	while( !(UCSR1A & (1<<RXC1)) );				//判断缓冲区是否有数据
	uart1_rx_array[uart1_rx_count] = UDR1;		//保存接收到的数据
	uart1_rx_count++;							//计数
	
	//如果接收数据的数组长度大于定义的最大长度则将计数清0
	if (uart1_rx_count >= UART1_RX_ARRAY_LEN)
	{
		uart1_rx_count = 0;
	}
}
