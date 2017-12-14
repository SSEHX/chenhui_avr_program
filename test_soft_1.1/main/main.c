/*
 * main.c
 *
 * Created: 2017/10/27 16:34:06
 *  Author: HXHZQ
 */


 /*
 *������ˮ��
 */


#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
 //#define	 F_CPU	11059200UL
#include <util/delay.h>

#include <stdio.h>
#include "wdt_use.h"
#include "lcd.h"
#include "gprs.h"
#include "time.h"
#include "adc.h"
#include "eeprom.h"

//uchar SMG[10] = { 0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,0xef,0xcf };//10��
//uchar SMGL[10] = { 0xfa,0x60,0xbc,0xf4,0x66,0xd6,0xde,0x70,0xfe,0xf6 };//�����ת�� abcd xfge  

//*********************************************************************************************************************************
//*****************************************************������****************************************************************************
//--------------------

/************************************************************************/
/*
��ϴʱ��,��ϴ���ڣ�����ʱ����time.c�ļ�������
																	 */
 /************************************************************************/

void System_Init(void)
{
	cli();
	GPRS_PROT();//	
	Port_Init();
	LCD_Init();
	Time1_Init();
	Time2_Init();
#if 1
	Time3_Init();
#endif
	ADC_Init();
	USART1_Init();
	
#ifdef Usart0	
		USART0_Init();//usart������time3��ͻ����
#endif 		
	
	LED_Init();
	BEEP_Init();
	
	
	_delay_ms(3000);
	WRITE_DATA_1726(0);
	Time0_Init();//��ʱ��1����,ι���Ź�	
	Time2_Start();//��ʱ��2��������ϴ6��
	
	wdt_enable(WDTO_500MS); //�������Ź�	
	sei();

	//	WRITE_ALLRAM_1726(1);//ȫ����ʾ

}
void  Data_Init(unsigned char *status, unsigned int *viuv_time, unsigned int *cmd)
{
	_delay_ms(300);

	//	LCD_Ico |= 0x0f;		
		//	WRITE_ALLRAM_1726(0);//
	memset(status, 0, 2);//��ʼ��״̬	
	unsigned char read_eeprom_buf[8] = { 0 };

	//LEDON;
	if (Check_Init())
	{
		//flow_set(read_eeprom_buf);

		//ʱ��
		SET_TIME(Read_TIME_EEPROM());
		//����
		flow_l = Read_FLOW_EEPROM();
		SET_FLOW_L(flow_l);


		//tds2 ԭˮ
		memset(read_eeprom_buf, 0, 8);
		Read_RAW_EEPROM(read_eeprom_buf);
		raw_water_set(read_eeprom_buf);
		//tds1 ��ˮ
		memset(read_eeprom_buf, 0, 8);
		pure_water_set(Read_PURE_EEPROM(read_eeprom_buf));
		memset(read_eeprom_buf, 0, 8);

		//��״̬
		Read_STATUS_EEPROM(read_eeprom_buf);

		Str_Add_Str(status, 0, read_eeprom_buf, 2);
		memset(read_eeprom_buf, 0, 8);
		//��������
		Read_CMD_EEPROM(cmd);
		//ʣ�������
		flow_y = Read_YU_SUM_EEPROM();

		Read_Filte_CntV_EEPROM(Filte_CntV);

		*viuv_time = Read__VIUV_TIME_EEPROM();

		count_chongxi = Read_COUNT_CHONGXI_EEPROM();


		//return 0;
		//return (read_eeprom_buf[0]<<8) |(read_eeprom_buf[1]);
	}
	else
	{
		//flow_set("00000");	
		//Write_Iint_Filte_CntV_EEPROM(Iint_Filte_CntV);
	//	Write_Init_EEPROM();
		BEEP(10);
		viuv_time = 0;
		flow_y = 0;
		flow_l = 0;
		raw_water_set("00");
		pure_water_set("00");
		flow_set("0");
		time_set("88888");
		//	Write_TIME_EEPROM(123);
		//	Write_TIME_EEPROM(123);
		//	Write_Init_EEPROM();

				//return 0;	
	}
	//LEDOFF;

}


int main(void)
{

	DDRC |= 0x01 << PC6;
	PORTC |= 0x01 << PC6;
	System_Init();


	//USART0_Transmit_String(" Init END!\r\n");
unsigned char Data[] = {0xBB,0x66,0x66,0x80,0x00,0x1D,0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x33,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x73,0xfc,0xff};
unsigned char Rece_Data[32] = {0};
	//	unsigned char Data[] = {0xab,0x33,0x33,0x80,0x00,0x1a,0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x33,0x00,0x02,0x00,0x00,0x00,0x00,0x74,0x00,0x74,0x00,0x00,0x00,0x0b,0xcd,0xfc,0xff};
		//					BB   66   66   80   00   11   41   31    31   31   31   31   31   33   00   02  00  D3 D0 FC FF
	//	unsigned char A = {0xBB,0x66,0x66,0x80,0x00,0x11,0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x33,0x00,0x02,0x00};
	//unsigned char Cmd[] = { 0x00,0x02 };
	//unsigned char Parameter[] = { 0x00,0x00,0x00,0x00,0x71,0x00,0x74,0x00,0x00,0x00 };

	//unsigned int sum_time = 0;//��ŷ���������ʱ��

	unsigned int error = 0;
	
	unsigned char eeprom_write_flag = 0;
	unsigned char parameter_len = 0;
	unsigned char Parameter[9+2*Filte_num] = { 0x02,0x10,0x00,0x00,0x00,0x01,0x00,0x10,0x20,0x00,0x00,0x00,0x00 };
	//unsigned char Ack[22] = { 0 }, Ack_Len = 0;
	//deal_cmd = 0xFFFF;
	Data_Init(status, &sum_viuv_time, &deal_cmd);

	//LEDON;

//	_delay_ms(6000);
	//						�ػ�			ͣ��			��ϴ		����ʱ��
	deal_cmd = 0xFFF | (0x00 << 12) | (0x00 << 13) | (0x00 << 14) | (0x00 << 15);

	//�ܵĵ�ŷ�����ʱ��
//	sum_time = (time_chongxi  + T_chongxi)* count_chongxi - T_chongxi + sum_viuv_time;

	LCD_Ico |= 0x0f;	
	//GPRS_PROT();//	

	while (1)
	{
		//����ķ���
		//GPRS״̬��
		PORTA &= ~(0x01<<PA2);	//����GPRS	
		_delay_ms(10);
		if(!(PIND & (0x01<<PD2)))//���GPRSģ��
		{
			_delay_ms(100);
 			PORTA |= (0x01<<PA2);	//�ر�GPRS	
		}		
#if 1	
			GET_GPRS_Signal();
#endif				
/*		Write_STATUS_EEPROM(status);*/
		if (send_flag)
		{		
			deal_cmd = Read_CMD_EEPROM();
			
			_delay_ms(2000);
		parameter_len = make_parameter(Parameter, status, tds1, tds2,flow_l,Filte_CntV);

#if 1
			error = GET_CMD(Parameter,parameter_len);
#endif
			if ((error && (error != 0xFFFF)))
			{
				deal_cmd = error;
				//						�ػ�			ͣ��	       ��ϴ		    ����ʱ��
				//deal_cmd = 0x10 | (0x00 << 12) | (0x01 << 13) | (0x00 << 14) | (0x00 << 15);
				send_flag = 0;
			}
			
			if ((LCD_Ico & 0x01 << 4))//ȱˮ����
			{
				_delay_ms(200);
				error = ERROR_HYDROPENIA();
				if (error == 0x00)
				{
	
				}
			}				
		}
		else
		{
			if (deal_cmd & 0x01 << 12)//�ػ�
			{
				WRITE_ALLRAM_1726(0);
				//HT1721C_BL_0;
				//status[0] &= ~(0x1 << 0);
				
				Write_CMD_EEPROM(deal_cmd);		
				Write_STATUS_EEPROM(status);
				continue;
			}
			else
			{
				//status[0] |= (0x1 << 0);
			}
			if (deal_cmd & (0x01 << 13))//ͣ��
			{
				Write_CMD_EEPROM(deal_cmd);		
				Write_STATUS_EEPROM(status);
				continue;
			}
			else
			{

			}
			if(deal_cmd & 0x01<<14)
			{
				
			}
			if (deal_cmd & (0x01 << 15))//����ʱ��
			{
				Write_CMD_EEPROM(deal_cmd);		
				Write_STATUS_EEPROM(status);
				continue;
			}
			else
			{
				LCD_Ico |= 0x01;
				//	 deal_cmd = 0x0DFE;
				SET_TIME((deal_cmd & 0x0FFF));
				Write_TIME_EEPROM((deal_cmd & 0x0FFF));

			}

		}
		if (LCD_Ico & (0x01 << 5))//©ˮ����
		{
			_delay_ms(300);
			ERROR_LEAKAGE();
		}
		Write_CMD_EEPROM(deal_cmd);		
		Write_STATUS_EEPROM(status);
#if 1
		if (Get_time3_Status())//����ˮ�����жϵ��޷���������
		{
			eeprom_write_flag = 1;
			//LEDON;
		}
		else if (eeprom_write_flag)
		{
			Write_YU_SUM_EEPROM(flow_y);
			//sum_flow = 23;
			Write_FLOW_EEPROM(flow_l);//��������L��

			Write_PURE_EEPROM(tds1);
			Write_RAW_EEPROM(tds2);
			Write_VIUV_TIME_EEPROM(sum_viuv_time);//һ����ˮ��û�г�ϴ��ʱ��(����)

			memset(Filte_CntV, 0, 2 * Filte_num);
			for (unsigned char i = 0; i < Filte_num; i++)
			{
				Filte_CntV[2 * i] += flow_l << 8;
				Filte_CntV[2 * i + 1] += flow_l;
				//�����о�����Ƿ���
				if (((Filte_CntV[2 * i] << 8) | Filte_CntV[2 * i + 1]) > ((Iint_Filte_CntV[2 * i] << 8) | Iint_Filte_CntV[2 * i + 1]))
				{
					Filte_CntV[2 * i] = 0;
					Filte_CntV[2 * i + 1] = 0;
				}
			}
			Write_Filte_CntV_EEPROM(Filte_CntV);
			
			SET_FLOW_L(flow_l);
			Write_Init_EEPROM();
			eeprom_write_flag = 0;
		}
#endif

	}
	return 0;
}

