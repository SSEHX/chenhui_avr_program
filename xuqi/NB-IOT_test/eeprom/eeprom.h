/*
 * eeprom.h
 *
 * Created: 2017/11/5 19:19:38
 *  Author: HXHZQ
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_
#include <avr/io.h>
#include <util/delay.h>
#include "beep.h"
#include "led.h"
#include "time.h"

#define		STATUS			 9

#define		Enable_CHARD	25
#define		ADOWNLINE		26
#define		BUPLINE			27

#define		WATER			28
#define		TIME_WATER		98
#define		TIME_MODE		99
#define		TIME_ZHISHUI	100
#define		TIME_FLUSH		113

#define ICEDOWNLINE	33

#define YUPULSH 35

#define PULE1LH 37

#define FLUSONTIME 39



//初始化的验证码
#define Init_str ("S7")


void EEPROM_Write(unsigned int uiAddress, unsigned char ucData);
void EEPROM_Write_Buf(unsigned int addr, unsigned char number, unsigned char *p_buff);
unsigned char EEPROM_Read(unsigned int uiAddress);
void EEPROM_Read_Buf(unsigned int addr, unsigned char number, unsigned char *p_buff);

void Write_Init_EEPROM(void);
unsigned char *Read_Init_EEPROM(unsigned char *Init);
unsigned char *Check_Init(void);

void Write_STATUS_EEPROM(unsigned char *status);
unsigned char *Read_STATUS_EEPROM(unsigned char *status);

void Write_TIME_EEPROM(unsigned int time);
unsigned int Read_TIME_EEPROM(void);

void Write_FLOW_EEPROM(unsigned int flow);
unsigned int Read_FLOW_EEPROM(void);

void Write_RAW_EEPROM(unsigned char *raw);
unsigned char *Read_RAW_EEPROM(unsigned char *raw);
void Write_PURE_EEPROM(unsigned char *pure);
unsigned char *Read_PURE_EEPROM(unsigned char *pure);

void Write_CMD_EEPROM(unsigned int cmd);
unsigned int Read_CMD_EEPROM();

void Write_COUNT_CHONGXI_EEPROM(unsigned int count_chongxi);
unsigned int Read_COUNT_CHONGXI_EEPROM();

void Write_YU_SUM_EEPROM(unsigned int yu_sum_flow);
unsigned int Read_YU_SUM_EEPROM();

void Write_VIUV_TIME_EEPROM(unsigned char viuv_time);
unsigned int Read__VIUV_TIME_EEPROM(void);

void Write_Filte_CntV_EEPROM(unsigned char *filte_cntv);
unsigned char *Read_Filte_CntV_EEPROM(unsigned char *filte_cntv);

void Write_Iint_Filte_CntV_EEPROM(unsigned char *filte_cntv);
unsigned char *Read_Init_Filte_CntV_EEPROM(unsigned char *filte_cntv);


#endif /* EEPROM_H_ */