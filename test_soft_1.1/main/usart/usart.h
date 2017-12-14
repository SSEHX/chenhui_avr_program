/*
 * usart.h
 *
 * Created: 2017/11/2 11:38:52
 *  Author: HXHZQ
 */ 


#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include "led.h"
#include "beep.h"
#include "send.h"
#include "lcd.h"

//´®¿Ú0µ÷ÊÔÊä³ö
#if 0
#define Usart0 
#endif 

extern unsigned char GPRS_Receive[100];
extern unsigned char GPRS_Receive_Num;	
extern unsigned char GPRS_Receive_Type ; 
extern unsigned int GPRS_Receive_Flag ;
extern unsigned char gprs_flag;

#define READ_FLAG (GPRS_Receive_Num^GPRS_Receive_Num)

//---------------------USARAT0-----------------------
void USART0_Init(void);
void USART0_Transmit(unsigned char data);
void USART0_Transmit_String(unsigned char *Data);
void USART0_Transmit_nChar(unsigned char *Data,unsigned char n);
unsigned char USART0_Receive(void);
unsigned char *USART0_Receive_string(unsigned char *Data);
unsigned char *USART0_Receive_nChar(unsigned char *Data,unsigned char n);

//---------------------USARAT1-----------------------
void USART1_Init(void);
void USART1_Transmit(unsigned char data);
void USART1_Transmit_String(unsigned char *Data);
void USART1_Transmit_nChar(unsigned char *Data,unsigned char n);
unsigned char USART1_Receive(void);
unsigned char *USART1_Receive_string(unsigned char *Data);
unsigned char *USART1_Receive_nChar(unsigned char *Data,unsigned char n);

#endif /* USART_H_ */