/*
 * lcd.h
 *
 * Created: 2017/11/3 16:57:34
 *  Author: HXHZQ
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include "gprs.h"
#include "eeprom.h"
#include "time.h"
//----HT1721----
#define HT1721C_DIO_1 PORTC|=(1<<PC2)  //dio
#define HT1721C_DIO_0 PORTC&=~(1<<PC2)
#define HT1721C_CLK_1 PORTC|=(1<<PC3)   //clk
#define HT1721C_CLK_0 PORTC&=~(1<<PC3)
#define HT1721C_STB_1 PORTC|=(1<<PC4)   //stb
#define HT1721C_STB_0 PORTC&=~(1<<PC4)
#define DISP_CON 0x92     //0x97
#define MODE_SET 0x08
#define HT1721C_BL_1 PORTE|=(1<<PE2)    //����ƿ�
#define HT1721C_BL_0 PORTE&=~(1<<PE2)


#define TM1726_SDA_1 PORTA|=(1<<PA0)  //dio
#define TM1726_SDA_0 PORTA&=~(1<<PA0)
#define TM1726_SCL_1 PORTA|=(1<<PA1)
#define TM1726_SCL_0 PORTA&=~(1<<PA1)


//------�˿ڳ�ʼ��--------
#define TURE   1
#define FALSE 0
#define TINGD 1 //HT1622


//-----�豸ID-------
#define IDDAT1 'B'		  		//�豸��
#define IDDAT2 '1'		  		//�豸��
#define IDDAT3 '1'		  		//�豸��
#define IDDAT4 '1'		  		//�豸��
#define IDDAT5 '1'		  		//�豸��
#define IDDAT6 '1'		  		//�豸��
#define IDDAT7 '3'		  		//�豸��
#define IDDAT8 '6'		  		//�豸��


//extern unsigned char SMG[10];
//extern unsigned char SMG[10];
//extern unsigned char LCDNUM[32];
extern char SCQ ;
extern unsigned char LCD_Ico ;
extern unsigned char drp_num;
extern unsigned char error_ico;//0x1:ȱˮ��0x1<<1:©ˮ��

void LCD_Init(void);
void INIT_1726(void);
void WRITE_DATA_1726(unsigned char wdata);
void WRITE_ALLRAM_1726(unsigned char F_RAM1632_SET);
void WRITE_1DATA_1726(unsigned char addr, unsigned char dat1);
void REFRESH_ALLRAM_1726(void);//ˢ��LCD��


void INIT_1721(void);
void WRITE_DATA_1721(unsigned char wdata);
void WRITE_ALLRAM_1721(unsigned char F_RAM1632_SET);
void WRITE_1DATA_1721(unsigned char addr, unsigned char dat1);
void WRITE_ALLRAM_1721(unsigned char F_RAM1632_SET);
void REFRESH_ALLRAM_1721(void);
void Write_addr_data(unsigned char addr,unsigned char len,unsigned char *data);
void drip(void);

void singal_ico(void);
void hydropenia_ico( void);
void leakage_ico(void);
void pure_water_ico(void);
void time_ico(void);
void flow_ico(void);
void signal(void);
void ico(void);


void time_set(unsigned char *time);//ʱ������
void flow_set(unsigned char *flow);//��������
void pure_water_set(unsigned char *pure_water);//��ˮTDS
void raw_water_set(unsigned char *raw_water);//ԭˮTDS
void reset_lcd(void);

void SET_TIME(unsigned int time);
void SET_FLOW_L(unsigned int flow);
void SET_FLOW(unsigned char *flow);
void SET_TDS1(unsigned char *tds1);
void SET_TDS2(unsigned char *tds1);

#endif /* LCD_H_ */