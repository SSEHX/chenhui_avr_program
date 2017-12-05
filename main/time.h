/*
 * time.h
 *
 * Created: 2017/11/2 09:43:32
 *  Author: HXHZQ
 */ 


#ifndef TIME_H_
#define TIME_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "led.h"
#include "beep.h"
#include "usart.h"
#include "wdt_use.h"


extern unsigned char time_10ms;
extern unsigned int time_1s;
extern unsigned char send_flag;
extern unsigned int time_zhishui;
extern unsigned char time_chongxi;
extern unsigned int count_chongxi;
extern unsigned char time[8], flow[8], tds1[4], tds2[4], status[4];
extern unsigned int deal_cmd, sum_viuv_time, sum_flow ,yu_sum_flow, T_chongxi;

#define Filte_num  3

extern unsigned char Filte_CntV[2*Filte_num];
extern unsigned char Iint_Filte_CntV[2*Filte_num];

#define TC1RUN  PORTC ^=(0x01<<PC6)

void Time0_Init(void);// 10ms

void Time1_Init(void);
void Time1_Start(void);
void Time1_Stop(void);
unsigned int  Time1_Get_TCNT(void);

void Time2_Init(void);
void Time2_Start(void);
void Time2_Stop(void);
unsigned char Get_time2_Status(void);//1:在运行，0;已停止

void Time3_Init(void);
void Time3_Start(void);
void Time3_Stop(void);
unsigned char Get_time3_Status(void);//:在运行，0;已停止
#endif /* TIME_H_ */