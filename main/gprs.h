
/*
 * gprs.h
 *
 * Created: 2017/11/2 16:54:32
 *  Author: HXHZQ
 */ 


#ifndef GPRS_H_
#define GPRS_H_

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "led.h"
#include "beep.h"
#include "usart.h"


#define RELOAD2_1 PORTA|=(1<<PA3)			//POWERKEY
#define RELOAD2_0 PORTA&=~(1<<PA3)			//POWERKEY
#define RESET_0 PORTA|=(1<<PA2)				//RESET
#define RESET_1 PORTA&=~(1<<PA2)			//RESET


extern char sendsim[];//查是否插入SIM卡
extern char sendcsq[];//信号检测 
extern char sendtcpclose[];

void GPRS_Init(void);			//POWERKEY
void GPRS_Prot_Init(void);
void GPRS_Config(void);
unsigned char GPRS_Connect_Tcp(void);
unsigned char GPRS_Send_Cmd(unsigned char *cmd);
unsigned char GPRS_Send_Data(unsigned char *Send_Data, unsigned int Send_Len,unsigned char *Rece_Data);
unsigned char ShortTurl(unsigned char *Send_Data, unsigned int Send_Len,unsigned char *Rece_Data);
void SHOW_GPRS_Receivce(void);

void GET_GPRS_Signal(void);

unsigned char *itoa(unsigned char *c,unsigned int i);


#endif /* GPRS_H_ */