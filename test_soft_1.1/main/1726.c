/*
 * _1726.c
 *
 * Created: 2017/10/30 09:37:01
 *  Author: HXHZQ
 */ 

#include <avr/io.h>

//#define	 F_CPU	11059200UL//16000000UL		// 单片机主频为7.3728MHz,用于延时子程序

#include <util/delay.h>
//-----TM1726----
#define TM1726_SDA_1 PORTA|=(1<<PA0)  //dio
#define TM1726_SDA_0 PORTA&=~(1<<PA0)
#define TM1726_SCL_1 PORTA|=(1<<PA1)
#define TM1726_SCL_0 PORTA&=~(1<<PA1)

#define TINGD 1 //HT1622

extern unsigned char LCDNUM[32];        //LCD的显示数组


//*************************************************************************************************
//*******************************TM1726************************************************************
//------------------------------------------------
//START TM1726
//-----------------------------------------------


