/*
 * port.h
 *
 * Created: 2017/12/13 11:19:13
 *  Author: 许旗
 */ 


#ifndef PORT_H_
#define PORT_H_

#include <avr/io.h>

/*-----------------------------------------------------------------------
							GPRS模块
-----------------------------------------------------------------------*/
#define RELOAD2_1		PORTA |= (1<<PA3)			//POWERKEY
#define RELOAD2_0		PORTA &= ~(1<<PA3)			//POWERKEY
#define RESET_0			PORTA |= (1<<PA2)			//RESET
#define RESET_1			PORTA &= ~(1<<PA2)			//RESET


/*-----------------------------------------------------------------------
							TM1726接口宏定义
-----------------------------------------------------------------------*/
#define TM1726_SDA_1	PORTA|=(1<<PA0)				//串行	SDA拉高
#define TM1726_SDA_0	PORTA&=~(1<<PA0)			//串行	SDA拉低
#define TM1726_SCL_1	PORTA|=(1<<PA1)				//串行	SCL拉高
#define TM1726_SCL_0	PORTA&=~(1<<PA1)			//串行	SCL拉低

/*-----------------------------------------------------------------------
							背光灯宏定义
-----------------------------------------------------------------------*/
#define TM1726_SCL_1 PORTA|=(1<<PA1)
#define TM1726_SCL_0 PORTA&=~(1<<PA1)



void port_init();



#endif /* PORT_H_ */