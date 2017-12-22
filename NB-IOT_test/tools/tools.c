/*
 * tools.c
 *
 * Created: 2017/12/14 22:51:42
 *  Author: 许旗
 */ 

#include "tools.h"

#define  Q_cm 357



unsigned int voltage_to_tds(unsigned int adc)
{
	// 	unsigned char L_mm  = 3, A_mm2 = 1.2*7;
	// 	unsigned int Q_cm = L_mm/A_mm2 * 10;			//单位cm-1
	//
	// 	unsigned int E_mV = 5010;					//5.01v
	// 	unsigned int U_mV = E_mV * adc / 1024;
	// 	unsigned int I_mA = (E_mV - U_mV) / 2000;	//（R0 = 2000）
	// 	unsigned int G_us = I_mA/U_mV ;
	// 	unsigned int K = G * Q; //(K单位是uS/cm)
	// 	unsigned int tds = K >> 2;
	unsigned int G_us = 0, K = 0;
	unsigned int a = 0,b = 0, n = 0, s = 0;
	a = 1024 - adc;
	for(unsigned char i = 0; i < 3 ; i++)//取3个有效位
	{
		n =	a/ adc;
		b = a% adc;
		a = b*10;
		s = s * 10 + n;
	}
	G_us = s * 5;
	//	G_us = (1024 - adc) * 10e6  / (2000 * y);
	K	= G_us * Q_cm/100;
	return (K>>2);
	//	return ((1024-adc)*125/(adc));
}

