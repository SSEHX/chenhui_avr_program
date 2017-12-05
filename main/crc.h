/*
 * rcr.h
 *
 * Created: 2017/11/10 16:19:52
 *  Author: HXHZQ
 */ 


#ifndef CRC_H_
#define CRC_H_

#include <util/delay.h>
#include "led.h"
#include "beep.h"
typedef unsigned char uchar;
typedef unsigned int uint;

unsigned int  CRC16(unsigned char *data,uchar addr, uchar wDataLen);
unsigned int Check_CRC16(unsigned char *data,unsigned char addr, unsigned char wDataLen);


#endif /* CRC_H_ */