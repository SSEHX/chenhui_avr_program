/*
 * send.h
 *
 * Created: 2017/11/6 09:00:27
 *  Author: HXHZQ
 */ 


#ifndef SEND_H_
#define SEND_H_


#include <util/delay.h>
#include "usart.h"
#include "gprs.h"


extern const unsigned char  Heart[4];
extern const unsigned char	 Rece_Heart[4] ;
extern const unsigned char	Tail[2];
extern const unsigned char Rece_Tail[2];
extern const unsigned char  Device[] ;

typedef struct Data_Type_Send
{
	unsigned char Cmd[2];
	unsigned char Paramter[10];
	//unsigned char Ack[30];
	//unsigned char (*Send_Fun)(unsigned char *Cmd,unsigned char *Parameter,unsigned char P_Len,unsigned char *Ack);
};



unsigned char Send_Status(unsigned char *Parameter,unsigned char *Ack);
unsigned char *Str_Add_Str(unsigned char *str1,unsigned char n1,unsigned char *str2,unsigned char n2);

unsigned char  Send_Cmd_Parameter(unsigned char *Cmd,unsigned char *Parameter,unsigned char P_Len,unsigned char *Ack);
unsigned char GET_Parameter(unsigned char *Cmd,unsigned char *Parameter,unsigned char P_Len,unsigned char *Ack_Parameter);


unsigned int GET_CMD(unsigned char *Parameter);//»ñÈ¡cmdÃüÁî
unsigned char ERROR_HYDROPENIA(void);//È±Ë®´íÎó
unsigned char ERROR_LEAKAGE(void);//Â©Ë®´íÎó

unsigned char GET_Time(void);
#endif /* SEND_H_ */