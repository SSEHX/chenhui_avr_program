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

#define COOLON PORTG|=(1<<PG3)			//�����
#define COOLOFF PORTG&=~(1<<PG3)		//����ر�
#define FA_BUSHUION PORTE|=(1<<PE3)			//��ˮ����
#define FA_BUSHUIOFF PORTE&=~(1<<PE3)		//��ˮ���ر�
#define FA_WARMON PORTE|=(1<<PE4)			//��ˮ����
#define FA_WARMOFF PORTE&=~(1<<PE4)		    //��ˮ���ر�
#define FA_HOTON PORTE|=(1<<PE5)			//��ˮ����
#define FA_HOTOFF PORTE&=~(1<<PE5)		    //��ˮ���ر�

#define BENG1ON PORTB|=(1<<PB1)				//��1��
#define BENG1OFF PORTB&=~(1<<PB1)			//��1�ر�
#define BENG2ON PORTB|=(1<<PB3)				//��2��
#define BENG2OFF PORTB&=~(1<<PB3)			//��2�ر�
//--------------------
#define BENGON PORTB|=(1<<PB2)				//��2��
#define BENGOFF PORTB&=~(1<<PB2)			//��2�ر�

#define FA_INWATERON PORTE|=(1<<PE7)		//��ˮ����
#define FA_INWATEROFF PORTE&=~(1<<PE7)		//��ˮ���ر�
#define FA_FLUSHON PORTE|=(1<<PE6)			//��ϴ����
#define FA_FLUSHOFF PORTE&=~(1<<PE6)		//��ϴ���ر�

#define RELOAD2_1 PORTA|=(1<<PA3)			//POWERKEY
#define RELOAD2_0 PORTA&=~(1<<PA3)			//POWERKEY
#define RESET_0 PORTA|=(1<<PA2)				//RESET
#define RESET_1 PORTA&=~(1<<PA2)			//RESET

#define HT1721C_BL_1 PORTE|=(1<<PE2)    //����ƿ�
#define HT1721C_BL_0 PORTE&=~(1<<PE2)

//-----�豸ID-------
#define IDDAT1 'B'		  		//�豸��
#define IDDAT2 '1'		  		//�豸��
#define IDDAT3 '1'		  		//�豸��
#define IDDAT4 '1'		  		//�豸��
#define IDDAT5 '1'		  		//�豸��
#define IDDAT6 '1'		  		//�豸��
#define IDDAT7 '3'		  		//�豸��
#define IDDAT8 '6'		  		//�豸��

typedef unsigned char uchar;
typedef unsigned char uint;


uchar ICEdownline;
uchar timemode = 0;//��ʱģʽ��Ҫ����
uchar timezhishui;//��ˮ30���ӳ�ϴ��ʱ
uchar timeflush;//��ϴʱ��
uchar enablebit = 0;//������������λ��
uchar swbyte;//����״̬��1-����0-�أ�1λ��ˮ��2λ��ˮ��3λ��ˮ��4λѹ��������λ��  5λ�������λ��0��ɣ�1��ֹ��6Һ����ʾ�л�λ ʱ�� �绰����
uchar fastatus = 0x08; //���ŵ�״̬��1Ϊ�򿪣�0Ϊ�رգ�1-8��1��ˮ��  2��ˮ�� 3��ˮ�� 4��Ҫ�ϴ�״̬ 5-��Ҫ������ˮ���� 6-������ˮ���� 7-������ˮ����
uchar enoughwater;//ȡ�����㹻��ˮ��־λ��1���㹻��0Ϊ-δ�㹻1-��ˮ��2-��ˮ��3-��ˮ
uint hotwaterbk = 0; //�����ټ���ʱ��Ƭ

uchar enbit = 0;//����λbit��1-8��1-��ˮ����λ 2-������� 3-���湤��ģʽ 4-ǿ�м���  5ǿ������ 6--����̽ͷ�쳣����ֹ���� 7--���쳣����ֹ���� 
							  //8-��ͯ����״̬�³�ˮ����λ

uint hotwater; //�·���ˮ����
uint warmwater;//�·���ˮ����
uint icewater; //�·���ˮ����

uchar savebit = 0;//��Ҫ���������λ��1-8:1-�����ϴʱ�䣬2-��ʱģʽ��3-�����������¶ȣ�4-��о�ֵ��5-����ʱ��Σ�6-����ʱ��Σ�7-��������ʱ���
uchar savebit2 = 0;//��Ҫ��������λ��1-8:1-��Ҫ����ʱ��λ��2-��������λ 3-���س�ˮ��ռ��

extern uchar enablebit;//������������λ��
uchar dealbyte = 0;//��Ҫ�����ָ��
uchar needreturn = 0;//��Ҫ����ָ��ţ��з�����������ر���05ָ�
uchar sendbit = 0;	//���ͱ�־λ  1--01ָ�2--02ָ�� 3--03 ָ�� 4--04ָ��
uchar minbit = 0; //����λ 1-8:1-20s��λ(18s,������ˮ��ʱ�Լ���ϴ��ʱ) 2--��ϴ����λ

uchar workmode;//����ģʽ

unsigned int loopnum = 0;//��������
unsigned char DATAlist[200];
extern uint wificount;//wifi�Ƿ���Ч����
extern char *ip;

//uchar SMG[10] = { 0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,0xef,0xcf };//10��
//uchar SMGL[10] = { 0xfa,0x60,0xbc,0xf4,0x66,0xd6,0xde,0x70,0xfe,0xf6 };//�����ת�� abcd xfge  
extern uchar SMG[10];
extern uchar SMGL[10];
uchar LCDNUM[32];        //LCD����ʾ����

extern unsigned char txtdat1[46];  //send
extern unsigned char rx1buffer[60];//��������1����
extern unsigned char rx1count;  //��������1��ʱ
extern unsigned char rx1poit;   //��������1ָ��

//--------GPRS setting------------------

extern char sendtc[];//͸��
extern char sendhx[];//�ػ���
extern char sendnet[];


extern unsigned char flushcont;

unsigned char EEPROM_read(unsigned int uiAddress);
//---
uchar keycode[3];
void WRITE_1DATA_1721(unsigned char addr, unsigned char dat1);//�޸�һ���ֽ�
void REFRESH_ALLRAM_1721(void);//����LCD
void BEEP(unsigned char sencond);//������

void INIT_1721(void);
void WRITE_DATA_1721(unsigned char wdata);
void WRITE_ALLRAM_1721(unsigned char F_RAM1632_SET);
void gprs_init(void);
void INIT_1726(void);
unsigned char EEPROM_read(unsigned int uiAddress);
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
void REFRESH_ALLRAM_1726(void);
void CRC16_rxt11(uchar addr1, uchar wDataLen1);
void send_reply(uchar resultnum);
void send_error(void);
void send_status(void);
void send_tds_temp_speed(void);
void CRC16_txt11(uchar addr, uchar wDataLen);
void WRITE_ALLRAM_1726(unsigned char F_RAM1632_SET);
void send_filter(void);

void init_devices(void);
void ad_getdat(unsigned char n);//ADLR=1��λȫ������2λ�ڵ��ֽ�7��8λ
void putchar1(unsigned char data1);

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
	Port_Init();
	LCD_Init();
	Time1_Init();
	Time2_Init();
	Time3_Init();
	//	sei();			
	//	_delay_ms(5000);		
	//	1Time0_Init();
	//	cli();
	ADC_Init();
	USART1_Init();
		USART0_Init();//usart������time3��ͻ����
			
	LED_Init();
	BEEP_Init();

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
		sum_flow = Read_FLOW_EEPROM();
		SET_FLOW_L(sum_flow);


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
		yu_sum_flow = Read_YU_SUM_FLOW_EEPROM();

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
		yu_sum_flow = 0;
		sum_flow = 0;
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

	unsigned char eeprom_write_flag = 0;
	unsigned char Parameter[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	//unsigned char Ack[22] = { 0 }, Ack_Len = 0;
	//deal_cmd = 0xFFFF;
	Data_Init(status, &sum_viuv_time, &deal_cmd);

	//LEDON;

//	_delay_ms(6000);
	//						�ػ�			ͣ��			��ϴ		����ʱ��
	deal_cmd = 0xFFF | (0x00 << 12) | (0x00 << 13) | (0x00 << 14) | (0x00 << 15);

	//�ܵĵ�ŷ�����ʱ��
//	sum_time = (time_chongxi  + T_chongxi)* count_chongxi - T_chongxi + sum_viuv_time;
	//_delay_ms(5000);
	//LEDON;
//	USART0_Transmit_String("Init end!");
	//BEEP(10);
	//while(1);
	LCD_Ico |= 0x0f;
	//	while(1);		

	while (1)
	{
		//����ķ���
//		//GPRS״̬��
		//_delay_ms(1000);
		//deal_cmd = Read_CMD_EEPROM();
	
		
			GET_GPRS_Signal();
		
		Write_STATUS_EEPROM(status);
		//	if(0)
		if (send_flag)
		{
			LEDRUN;			
			_delay_ms(2000);
							//make_parameter(Parameter, status, tds1, tds2,sum_flow,Filte_CntV);

			unsigned int error = 0;
#if 1
			error = GET_CMD(Parameter);
#endif
			//error = 0xFFFF;
			if ((error && (error != 0xFFFF)))
			{
				deal_cmd = error;
				//						�ػ�			ͣ��			��ϴ		����ʱ��
					//deal_cmd = 0x10 | (0x00 << 12) | (0x00 << 13) | (0x00 << 14) | (0x00 << 15);
					//deal_cmd = 0x10;
				send_flag = 0;
						LEDOFF;
			}
			//deal_cmd = 0x0010;
			if ((LCD_Ico & 0x01 << 4))
			{
				//LEDOFF;
				_delay_ms(200);
				error = ERROR_HYDROPENIA();
				if (error == 0x00)
				{
					//LEDON;
					//BEEP(10);
				}
			}
			//	LEDOFF;				
		}
//		deal_cmd = 0x10;
		{
			//deal_cmd = 0x0010;

			if (deal_cmd & 0x01 << 12)//�ػ�
			{
				WRITE_ALLRAM_1726(0);
				//HT1721C_BL_0;
				//status[0] &= ~(0x1 << 0);
				continue;
			}
			else
			{
				//status[0] |= (0x1 << 0);
			}
			if (deal_cmd & (0x01 << 13))//ͣ��
			{
				continue;
			}
			else
			{

			}
			if (deal_cmd & (0x01 << 15))//����ʱ��
			{
				continue;
			}
			else
			{
				LCD_Ico |= 0x01;
				//	 deal_cmd = 0x0DFE;
				SET_TIME((deal_cmd & 0x0FFF));
				Write_TIME_EEPROM((deal_cmd & 0x0FFF));

			}
			if (LCD_Ico & (0x01 << 5))
			{
				_delay_ms(300);
				ERROR_LEAKAGE();
			}
		}
		Write_CMD_EEPROM(deal_cmd);
		Write_STATUS_EEPROM(status);
#if 1
		if (Get_time3_Status())//����ˮ�����жϵ��޷���������
		{
			eeprom_write_flag = 1;
		}
		else if (eeprom_write_flag)
		{
			Write_YU_SUM_FLOW_EEPROM(yu_sum_flow);
			//sum_flow = 23;
			Write_FLOW_EEPROM(sum_flow);//��������L��

			Write_PURE_EEPROM(tds1);
			Write_RAW_EEPROM(tds2);
			Write_VIUV_TIME_EEPROM(sum_viuv_time);//һ����ˮ��û�г�ϴ��ʱ��(����)

			memset(Filte_CntV, 0, 2 * Filte_num);
			for (unsigned char i = 0; i < Filte_num; i++)
			{
				Filte_CntV[2 * i] += sum_flow << 8;
				Filte_CntV[2 * i + 1] += sum_flow;
				//�����о�����Ƿ���
				if (((Filte_CntV[2 * i] << 8) | Filte_CntV[2 * i + 1]) > ((Iint_Filte_CntV[2 * i] << 8) | Iint_Filte_CntV[2 * i + 1]))
				{
					Filte_CntV[2 * i] = 0;
					Filte_CntV[2 * i + 1] = 0;
				}
			}
			Write_Filte_CntV_EEPROM(Filte_CntV);
			SET_FLOW_L(sum_flow);
			Write_Init_EEPROM();

			eeprom_write_flag = 0;
		}
#endif

	}
	return 0;
}

int main_(void)
{

	unsigned int i;
	uchar temp1;
	uchar temp = 0;
	uint  tempint1;
	uint  tempint2;//��ΪTDS���¶�ֵ�Ĵ�ʹ��
	uchar iceuptemp, icedowntemp;//��ˮ�����¶�
	uchar Bupline;
	uchar uvtime;  //uv��ʱ��
	uchar netofftime = 0;//����������

	uchar LCDmode = 0;          //��ʾģʽ
	//uchar flashaddr;        //Һ����˸λ��
	uchar LCDtemp[18];        //0 Һ����˸λ��ַ 1�ƶ����ݵĳ��� 2��˸���ʱ�� 3��λ��˸�л�,
	uchar Bdownline;//B������������¶�
	//uchar supertemp;//�������¶���
	uchar heatontime, heatofftime;//��Ъ���ȿ����͹ر�ʱ��
	uchar ICEupline;//��ˮ�����������¶�
	uchar voicenum;//�����ε�ѡ��
	//uchar paikonghour,paikongmin;//��ҹ�ſ�ʱ���趨ֵ
	uchar yupulsh, yupulsl;//�����������ֵ
	uint thistimepuls = 0;
	uchar pule1lh, pule1ll;//
	uint pule1l;
	uchar flushontime, flushofftime;//����ϴ�����ر�ʱ��

	uchar keybyte;//ȡˮ�����Ĵ���,1-6bitΪ��ť����λ
	uchar LCD0loop = 0;//LCDģʽΪ0�ڵ�ѭ�������±仯��ʾ��
	uchar JXloop;//��Ъʱ�����
	uchar getwatermode;//ȡˮģʽ
	uchar iceonoff = 4;//��ˮ��ѹ���������ֽ�
	uchar yasuoji = 0;//ѹ���������ֽ�
	uchar yasuojicount = 0;//ѹ�������ش���

	//---------------��ʼ������-----------------��һ����־λ��������ϵ磬����г�ʼ��
	//----2017----
	uchar setkeycount = 0;//���ð�ť����
	uchar settype = 0;//���õ�����  0-Сʱ��1-���ӣ�2-�����¶ȣ�3--��ˮ���¶�
	uchar databit = 0;//��һλ����˸����λ      LCDNUM,16,17,--��ַ 18,19 ����
	uchar flashcount = 0;//��˸Ƶ��
	uchar keybit = 0;//����ȡˮλ��ȡ������
	uchar flashbit = 0;//��˸��־λ���͵���λ1-8��1-wifi  2-����  3-���� 4-���� 5-Ц�� 6-��ͯ����  
	uchar waterbit = 0;//
	uint hot3min = 0;
	uint warm3min = 0;
	//uint cool3min=0;
	//uchar gaoshuiweic=0;//��ˮλ������15s��3��
	uchar coolcount = 0;//ѹ��������Ҫ3���Ӻ����
	uchar setcount = 0;//����˳��ֵ
	//uchar keyheatc=0;//ǿ��ת������
	//uchar keycoolc=0;//ǿ������ת��
	uchar childlock;//��ͯ�����Ĵ���
	uchar childtime = 0;//5s�ı���ʱ��
	uchar enterkeyc = 0;//ȷ�ϰ���3s��ʱ

	uchar timebit = 0x01;//
	uchar bkhottemp = 0; //�����Ƿ�һ���������ϴ�����
	uchar bkcoldtemp = 0;
	uchar tds1num = 0;
	uchar tds2num = 0;
	uchar setloop = 0;//����ѭ������
	uchar sendtime = 0;//����Ƶ������
   //----------------------
	uchar wifibit = 0x01;  //1-8:1--sendsetdat
	uchar loushuibk = 250; //©ˮ����ֵ��С
	uchar sendpuls = 0;//�ػ�״̬�·���״̬����
	uchar resetcount = 0;
	uint bkliul = 0;//�����������ж������ƹ���
	uchar LLJcount = 0;//�����Ƽ���
	uchar tdscount = 10;//����TDS��⣬��ˮ����
	init_devices();
	_delay_ms(1000);
	// init_devices();
	PORTC &= 0xfe;//led
	//BEEP(50);
	LCDmode = 0;           //��ʾģʽ
	BEEP(100);
	RELOAD2_0;
	_delay_ms(500);
	RESET_0;
	_delay_ms(5000);
	RESET_1;
	RELOAD2_1;
	//-----------------------------------------------------------------
	//--��Ҫ���ݺϷ��ԱȽϣ�������ʱҲ��Ҫ�������ݺϷ��ԵıȽ�???
	//--------------------------------------------------------------
	 //-----------------------------------------
	for (i = 0; i < 0x70; i++)  //�����ݳ�ʼ��
	{
		DATAlist[i] = 0x00;
	}
	temp = EEPROM_read(9);  //
	if (temp == 0xaa)//����õ����ݣ����ó�ʼ��
	{
		DATAlist[0x18] = EEPROM_read(25);//��������Ƿ��ֹ����ʹ���ֽ�
		//Adownline=EEPROM_read(26);//A����ȸߵ��¶�
		Bupline = EEPROM_read(27);
		DATAlist[0x3b] = EEPROM_read(28);//��ˮ������λ
		DATAlist[0x3c] = EEPROM_read(29);//
		DATAlist[0x3d] = EEPROM_read(30);//ģʽ3�µ�ʣ��ˮ����
		DATAlist[0x3e] = EEPROM_read(31);//
		ICEdownline = EEPROM_read(33);//��ˮ�����¶ȷ�Χ
		yupulsh = EEPROM_read(35);//ʣ�����������棬����1����������
		yupulsl = EEPROM_read(36);
		pule1lh = EEPROM_read(37);
		pule1ll = EEPROM_read(38);//1���������ĸߵ��ֽ�
		DATAlist[0x5c] = pule1lh;
		DATAlist[0x5d] = pule1ll;
		pule1l = pule1lh * 256 + pule1ll;//����
		flushontime = EEPROM_read(39);

		for (i = 0; i < 6; i++)//�����趨�Ķ����ֵ
		{
			DATAlist[0x1a + i] = EEPROM_read(i + 78);  //menmory to 84
		}
		for (i = 0; i < 12; i++)//������оʹ�õ�ֵ
		{
			DATAlist[0x24 + i] = EEPROM_read(i + 86);  //menmory to 98
		}
		childlock = EEPROM_read(98);
		timemode = EEPROM_read(99);
		timezhishui = EEPROM_read(100);
		timeflush = EEPROM_read(113);  //��ϴʱ��

		DATAlist[0x5e] = EEPROM_read(115);
		DATAlist[0x5f] = EEPROM_read(116);//ʣ��ʱ��
	}
	else//û��ʼ��������
	{

		Bupline = 95; Bdownline = 75;							//�����¶�  Bupline  2017
		DATAlist[0x18] = 0xaa;//�������ʹ��
		EEPROM_write(25, DATAlist[0x18]);
		EEPROM_write(27, Bupline);//EEPROM_write(28,Bdownline);
		DATAlist[0x3b] = 0x00;//EEPROM_read(28);//��ˮ������λ
		DATAlist[0x3c] = 0x00;//EEPROM_read(29);//
		DATAlist[0x3d] = 0x00;//EEPROM_read(30);//ģʽ3�µ�ʣ��ˮ����
		DATAlist[0x3e] = 0x00;//EEPROM_read(31);//
		EEPROM_write(28, DATAlist[0x3b]);
		EEPROM_write(29, DATAlist[0x3c]);
		EEPROM_write(30, DATAlist[0x3d]);
		EEPROM_write(31, DATAlist[0x3e]);
		yupulsh = 0;
		yupulsl = 0;
		EEPROM_write(35, yupulsh);
		EEPROM_write(36, yupulsl);
		flushontime = 10;
		EEPROM_write(39, flushontime);//��ϴʱ����趨

		ICEdownline = 15;
		EEPROM_write(33, ICEdownline);

		pule1lh = 0x05; pule1ll = 20; pule1l = 1300;
		DATAlist[0x5c] = pule1lh;
		DATAlist[0x5d] = pule1ll;
		EEPROM_write(37, pule1lh);
		EEPROM_write(38, pule1ll);

		EEPROM_write(43, 0xaa);
		EEPROM_write(9, 0xaa);//����ɹ���־ 


		DATAlist[0x1a] = 15; EEPROM_write(78, 15);//��ʼ����о�ֵ
		DATAlist[0x1b] = 15; EEPROM_write(79, 15);
		DATAlist[0x1c] = 30; EEPROM_write(80, 30);
		DATAlist[0x1d] = 30; EEPROM_write(81, 30);
		DATAlist[0x1e] = 25; EEPROM_write(82, 25);
		DATAlist[0x1f] = 13; EEPROM_write(83, 13);
		for (i = 0; i < 12; i++)			//menmory to 86+12==98  ��ʼ����о����
		{
			DATAlist[0x24 + i] = 0;
			EEPROM_write((86 + i), DATAlist[0x24 + i]);
		}
		childlock = 0xaa;
		EEPROM_write(98, 0xaa);		//��ͯ������ʼ��
		timemode = 0;					//��ʱ����ģʽ�ر�   menmory to 99
		EEPROM_write(99, 0);
		timezhishui = 30;
		EEPROM_write(100, 30);			//��ˮ30���Ӽ�ʱ  20s-30*60=1800/20=90   menmory to 100
		timeflush = 18;
		EEPROM_write(113, 18);		//��ˮ30���ӳ�ϴ18��
		enablebit = 0;
		EEPROM_write(114, 0);			//ǿ�Ƽ��Ȼ�������λ    menmory to 114
		DATAlist[0x5e] = 0;
		DATAlist[0x5f] = 160;     //ʣ������
		EEPROM_write(115, DATAlist[0x5e]);
		EEPROM_write(116, DATAlist[0x5f]); //menmory to 116
	}
	DATAlist[0x19] = Bupline;

	keybyte = 0xff;//���Խ�ȥ��ť1,-6
	swbyte = 0x00;//����״̬Ϊ�ر�
	JXloop = 0;//��Ъʱ��
	enoughwater = 0;//ȡ����ˮ�Ѿ��㹻���
	hotwater = 0;
	warmwater = 0;
	icewater = 0;
	iceonoff = 04;//��ˮ��ѹ���������ֽ�
	yasuoji = 0;//ѹ����
	yasuojicount = 0;

	//----��ʼ��ʱ��----
	DATAlist[0x80] = EEPROM_read(52);//ʱ����Ƿ�������
	if (DATAlist[0x80] == 0xaa)//�б��棬����ʱ���
	{
		for (i = 0; i < 24; i++)  //�����ݳ�ʼ��
		{
			DATAlist[0x81 + i] = EEPROM_read(53 + i); //memory to 77  53-77    101-112
		}
		for (i = 0; i < 12; i++)  //�����ݳ�ʼ��
		{
			DATAlist[0x99 + i] = EEPROM_read(101 + i); //memory to    101-112
		}
	}
	else   //û������ʱ�γ�ʼֵ
	{
		for (i = 0; i < 18; i++)  //�����ݳ�ʼ��
		{
			DATAlist[0x82 + (2 * i)] = 0;
		}
		DATAlist[0x81] = 7; DATAlist[0x83] = 9;
		DATAlist[0x85] = 0x11; DATAlist[0x87] = 0x13;
		DATAlist[0x89] = 0x18; DATAlist[0x8b] = 0x21;
		DATAlist[0x8d] = 7; DATAlist[0x8f] = 9;
		DATAlist[0x91] = 0x11; DATAlist[0x93] = 0x13;
		DATAlist[0x95] = 0x18; DATAlist[0x97] = 0x21;
		DATAlist[0x99] = 7; DATAlist[0x9B] = 9;
		DATAlist[0x9D] = 0x11; DATAlist[0x9F] = 0x13;
		DATAlist[0xA1] = 0x18; DATAlist[0xA3] = 0x21;
	}


	TCCR0 = 0x05;   //02����TIMER0��ʱ
 //------------------------------------
 //----------��ĳ�ʼ��---------------
	iceuptemp = EEPROM_read(0x04);//
	icedowntemp = EEPROM_read(0x05);//15
	if (iceuptemp > 50)    //�Ϸ���
	{
		iceuptemp = 20;
		EEPROM_write(0x04, iceuptemp);
	}
	if (icedowntemp > 50)
	{
		icedowntemp = 15;
		EEPROM_write(0x05, icedowntemp);
	}
	DATAlist[0x40] = 0xaa;           //�ѱ��׵�ַ��AA
	DATAlist[0] = 0xaa;

	BEEP(50);
	HT1721C_BL_1;
	INIT_1721();
	_delay_ms(200);

	WRITE_ALLRAM_1721(1);
	_delay_ms(100);

	//---test LCD and key-----
	INIT_1726();
	_delay_ms(200);
	WRITE_ALLRAM_1726(1);
	_delay_ms(3000);

	WRITE_ALLRAM_1726(0);
	LCDNUM[5] = SMGL[IDDAT4 - 0x30];
	LCDNUM[6] = SMGL[IDDAT5 - 0x30];
	LCDNUM[7] = SMGL[IDDAT6 - 0x30];
	LCDNUM[8] = SMGL[IDDAT7 - 0x30];
	LCDNUM[9] = SMGL[IDDAT8 - 0x30];
	REFRESH_ALLRAM_1726(); _delay_ms(10000);
	LCDNUM[5] = 0;
	LCDNUM[6] = 0;
	LCDNUM[7] = 0;
	LCDNUM[8] = 0;
	LCDNUM[9] = 0;
	WRITE_ALLRAM_1726(0);
	//--------- 
	_delay_ms(2000);//20000
 //-------wifi initial-----------
	//wifi_init();

 //------------------
 //---�������� �� - ������������о��������
 // read_3key();//use

	LCDNUM[13] = 0x00;
	//-------------
	//------��ʱ����ģʽ��ʾ-------
	timebit = 0x01;
	enterkeyc = 10;

	//------------------------
	LCDmode = 0;
	DATAlist[0x20] = 0x01;
	DATAlist[0x21] = 0;
	fastatus = 0x08;
	if (DATAlist[0x18] == 0xbb)
	{
		LCDmode = 48;
		//HT1721C_BL_0;
		WRITE_ALLRAM_1726(0);
	}
	LCDNUM[0] = 0x00;
	LCDNUM[1] = 0x00;
	LCDNUM[2] = 0x00;
	LCDNUM[3] = 0x00;
	LCDNUM[4] = 0x00;
	//------
	LCDNUM[0] |= 0x01;
	tempint1 = DATAlist[0x5e] * 256 + DATAlist[0x5f];
	temp = tempint1 / 10000;
	if (temp > 0)LCDNUM[0] = (LCDNUM[0] & 0x01) | SMGL[temp];
	tempint1 = tempint1 - (temp * 10000);
	temp = tempint1 / 1000;
	if (temp > 0)LCDNUM[1] = (LCDNUM[1] & 0x01) | SMGL[temp];
	tempint1 = tempint1 - (temp * 1000);
	temp = tempint1 / 100;
	if (temp > 0)LCDNUM[2] = (LCDNUM[2] & 0x01) | SMGL[temp];
	tempint1 = tempint1 - (temp * 100);
	temp = tempint1 / 10;
	if (temp > 0)LCDNUM[3] = (LCDNUM[3] & 0x01) | SMGL[temp];
	tempint1 = tempint1 - (temp * 10);
	temp = tempint1;
	LCDNUM[4] = (LCDNUM[4] & 0x01) | SMGL[temp];
	DATAlist[0x09] = 0x00;
	DATAlist[0x0a] = 0x00;
	DATAlist[0x0b] = 0x00;
	//-----
	TCNT1 = 0;
	TCCR1A = 0x00; //00
	TCCR1B = 0x07;  //07  ����T1����
	flushontime = EEPROM_read(39);
	if (flushontime > 0)
	{
		BENGON;
		FA_INWATERON;
		FA_FLUSHON;
	}
	resetcount = 194;
	tdscount = 10;//TDS���

	while (1)
	{
		_delay_us(302);
		loopnum++;
		if (loopnum > 760)//50000---1000
		{
			loopnum = 0;
			_delay_ms(20);

			//------------------�����·�������-----------------------------
			if (savebit != 0)
			{
				if ((savebit & 0x01) == 0x01)//�����ϴʱ��
				{
					EEPROM_write(100, timezhishui);//����
					EEPROM_write(113, timeflush);//����
					savebit &= 0xfe;
				}
				if ((savebit & 0x08) == 0x08)//������оֵ
				{
					EEPROM_write(78, DATAlist[0x1a]);
					EEPROM_write(79, DATAlist[0x1b]);
					EEPROM_write(80, DATAlist[0x1c]);
					EEPROM_write(81, DATAlist[0x1d]);
					EEPROM_write(82, DATAlist[0x1e]);
					EEPROM_write(83, DATAlist[0x1f]);
					savebit &= 0xf7;
				}
				if ((savebit & 0x80) == 0x80)//���渴λ��оֵ
				{
					for (i = 0; i < 12; i++)  //�����ݳ�ʼ��
					{
						EEPROM_write((86 + i), DATAlist[0x24 + i]);
					}
					savebit &= 0x7f;
				}
				//send0_reply(0);
				_delay_ms(200);
			}
			if ((savebit2 & 0x01) == 0x01)
			{
				//send0_reply(0);
				savebit2 &= 0xfe;
				_delay_ms(200);
			}



			if ((dealbyte & 0x10) == 0x10)//����ʣ��ʱ��
			{
				EEPROM_write(115, DATAlist[0x5e]);
				EEPROM_write(116, DATAlist[0x5f]);
				send_reply(0);
				dealbyte &= 0xef;
				//------
				LCDNUM[0] = LCDNUM[0] & 0x01;
				LCDNUM[1] = LCDNUM[1] & 0x01;
				LCDNUM[2] = LCDNUM[2] & 0x01;
				LCDNUM[3] = LCDNUM[3] & 0x01;
				LCDNUM[4] = LCDNUM[4] & 0x01;
				tempint1 = DATAlist[0x5e] * 256 + DATAlist[0x5f];
				temp = tempint1 / 10000;
				if (temp > 0)LCDNUM[0] = (LCDNUM[0] & 0x01) | SMGL[temp];
				tempint1 = tempint1 - (temp * 10000);
				temp = tempint1 / 1000;
				if (temp > 0)LCDNUM[1] = (LCDNUM[1] & 0x01) | SMGL[temp];
				tempint1 = tempint1 - (temp * 1000);
				temp = tempint1 / 100;
				if (temp > 0)LCDNUM[2] = (LCDNUM[2] & 0x01) | SMGL[temp];
				tempint1 = tempint1 - (temp * 100);
				temp = tempint1 / 10;
				if (temp > 0)LCDNUM[3] = (LCDNUM[3] & 0x01) | SMGL[temp];
				tempint1 = tempint1 - (temp * 10);
				temp = tempint1;
				LCDNUM[4] = (LCDNUM[4] & 0x01) | SMGL[temp];

			}


			//ONE----------------------------------------------------------------------------------
			//----------������ʾ����------------
			//----------------------------------------------------------------------------------
			if (LCDmode == 0)
			{
				_delay_ms(2000);

				LCD0loop++;
				if (LCD0loop > 8)LCD0loop = 1;
				switch (LCD0loop)
				{
				case 1://TDS
				   //------ˮ��-----
					LCDNUM[12] |= 0x10;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;

					if (tdscount > 0) { //TDSֵ������ˮ��ʱ��ż��
						tdscount--;
						PORTD &= ~(1 << PD4);
						PORTD &= ~(1 << PD5);
						_delay_ms(100);
						ad_getdat(6);//TDSֵ
						ad_getdat(7);//TDSֵ  ����
						PORTD |= (1 << PD4);
						PORTD |= (1 << PD5);
						tempint1 = DATAlist[0x41 + 12] * 256 + DATAlist[0x41 + 13];
						tempint1 = (65535 - tempint1) / 100;  //16
						tempint1 = tempint1 / 2;
						DATAlist[0x41 + 12] = tempint1 / 256;
						DATAlist[0x41 + 13] = tempint1 - (DATAlist[0x41 + 12] * 256);
						tempint2 = DATAlist[0x41 + 14] * 256 + DATAlist[0x41 + 15];
						tempint2 = (65535 - tempint2) / 100;
						tempint2 = tempint2 / 2;
						DATAlist[0x41 + 14] = tempint2 / 256;
						DATAlist[0x41 + 15] = tempint2 - (DATAlist[0x41 + 14] * 256);
						if (((bkhottemp != DATAlist[0x44]) || (bkcoldtemp != DATAlist[0x46]) || (tds1num != tempint1) || (tds2num != tempint2)) && (needreturn == 0))//ֵ�����仯���ϴ�
						{	//                                                                                             ���Ѽ�¼�Ѿ��������
							bkhottemp = DATAlist[0x44];
							bkcoldtemp = DATAlist[0x46];
							tds1num = tempint1;
							tds2num = tempint2;
							//if(resetcount<2)send_tds_temp_speed();
							sendtime = 1;//?
						}
						//TDSֵ
						LCDNUM[10] |= 0x10;//
						LCDNUM[17] = 0x10;
						LCDNUM[13] &= 0x10;

						temp = tempint1 / 100;
						if (temp > 0)
						{
							LCDNUM[16] = (LCDNUM[16] & 0x10) | SMG[temp];       //��4λ
						   // LCDNUM[10]=(LCDNUM[10]&0x8F)|((SMG[temp]&0x0f)<<4);//��4λ
						}
						else
						{
							LCDNUM[16] &= 0x10;
						}
						temp1 = tempint1 - (100 * temp);//ȡ����λ
						temp = temp1 / 10;
						LCDNUM[15] = (LCDNUM[15] & 0x10) | SMG[temp];
						temp = temp1 - (temp * 10);
						LCDNUM[14] = (LCDNUM[14] & 0x10) | SMG[temp];

						//��ˮ-----
						temp = tempint2 / 100;
						if (temp > 0)
						{
							LCDNUM[12] = (LCDNUM[12] & 0x10) | SMG[temp];
						}
						else
						{
							LCDNUM[12] &= 0x10;
						}
						temp1 = tempint2 - (100 * temp);//ȡ����λ
						temp = temp1 / 10;
						LCDNUM[11] = (LCDNUM[11] & 0x10) | SMG[temp];
						temp = temp1 - (temp * 10);
						LCDNUM[10] = (LCDNUM[10] & 0x10) | SMG[temp];
					}
					break;
				case 2://����wifi��
					   //------ˮ��-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] |= 0x10;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;
					//---------

					sendtime++;
					if (sendtime > 12) //8-12  18-32  
					{
						sendtime = 0;
					}
					if (sendtime == 2)
					{

						//if(resetcount<2) send_tds_temp_speed();//��������﷢�ͣ����ڲ�ͬ���ﲻ���ٷ���
						// _delay_ms(200);
						if (resetcount < 2)
						{
							if ((sendbit & 0x01) == 0x01)//01
							{
								send_status();
							}
							else if ((sendbit & 0x04) == 0x04)//03
							{
								send_filter();
							}
							else if ((sendbit & 0x08) == 0x08)//04
							{
								send_error();
							}
							else
							{
								send_tds_temp_speed();
							}
						}
						_delay_ms(200);
					}
					if (sendtime == 4)
					{
						_delay_ms(200);
						// send_status();
					}
					_delay_ms(10);
					if (sendtime == 8)
					{
						if (((fastatus & 0x08) == 0x08) && (needreturn == 0) && (resetcount < 2))//����λ����¼�������
						{
							_delay_ms(100);
							fastatus &= 0xf7;                   //���﷢���ܶ�01���ݹ���������
							sendbit |= 0x01;
							send_status();
						}
					}

					wificount++;
					if (wificount == 50)//û�з��أ���ȡ��wifiͼ��
					{

						DATAlist[0x21] &= 0x7f;
						fastatus |= 0x08;//��Ҫ����
						//LCDNUM[0]&=0xf3;//ȡ��wifiͼ��
						LCDNUM[1] &= 0xfe;
						LCDNUM[2] &= 0xfe;
						LCDNUM[3] &= 0xfe;
						LCDNUM[4] &= 0xfe;
						LCDNUM[5] &= 0xfe;
						//BEEP(200);
					}
					if (wificount > 80) //��������wifi��   �ڴ�����wifi����  �������GPRS���˷Ŵ�
					{
						wificount = 0;
						resetcount = 201;
					}
					break;

				case 3:
					//------ˮ��-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] |= 0x01;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;
					//---------


					break;
				case 4://���ʱ���Լ��Ƿ�ǿ��
					  //------ˮ��-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] |= 0x01;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;
					//---------��ʾ����--------------
					LCDNUM[8] |= 0x01;//����

					tempint1 = DATAlist[0x3b] * 256 + DATAlist[0x3c];
					temp = tempint1 / 10000;
					LCDNUM[5] = (LCDNUM[5] & 0x01) | SMGL[temp];
					tempint1 = tempint1 - (temp * 10000);
					temp = tempint1 / 1000;
					LCDNUM[6] = (LCDNUM[6] & 0x01) | SMGL[temp];
					tempint1 = tempint1 - (temp * 1000);
					temp = tempint1 / 100;
					LCDNUM[7] = (LCDNUM[7] & 0x01) | SMGL[temp];
					tempint1 = tempint1 - (temp * 100);
					temp = tempint1 / 10;
					LCDNUM[8] = (LCDNUM[8] & 0x01) | SMGL[temp];
					tempint1 = tempint1 - (temp * 10);
					temp = tempint1;
					LCDNUM[9] = (LCDNUM[9] & 0x01) | SMGL[temp];



					break;
				case 5:// 
					  //------ˮ��-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] |= 0x10;
					LCDNUM[16] &= 0xef;
					//----------���湤��ģʽ----
					if ((enbit & 0x04) == 0x04)//��Ҫ���湤��ģʽ
					{
						EEPROM_write(41, workmode);
						enbit &= 0xfb;
					}
					//---------------------------------------
					//������������͹�����ָ��
					if ((dealbyte & 0x01) == 0x01)//����
					{
						send_reply(0);
						HT1721C_BL_1;
						LCDmode = 0;
						dealbyte &= 0xfe;
						//send_reply(0);
					}
					if ((dealbyte & 0x02) == 0x02)//�ػ�
					{
						send_reply(0);
						HT1721C_BL_0;
						LCDmode = 42;
						dealbyte &= 0xfd;
						//send_reply(0);
						FA_HOTOFF;
						FA_WARMOFF;
						//FA_COOLOFF;
						fastatus &= 0xf8;//3�����ŵ�״̬�ر�
						FA_INWATEROFF;
						waterbit &= 0xfb;//��λ �ر���ˮ
						FA_FLUSHOFF;
						minbit &= 0xfd;//�رճ�ϴ��
						COOLOFF;
						coolcount = 60;
						waterbit &= 0xef;//��0��ѹ�����ѹر�
						//HEATAOFF;
						waterbit &= 0xdf;//��0�������ѹر�
						BENG1OFF;
						BENGOFF;
						fastatus &= 0xf7;
						DATAlist[0x20] = 0x00;
						DATAlist[0x21] = 0x00;
						_delay_ms(1000);
						send_status();
						WRITE_ALLRAM_1726(0);
					}
					if ((dealbyte & 0x04) == 0x04)//��ϴģʽ
					{
						send_reply(0);
						dealbyte &= 0xfb;
						flushontime = EEPROM_read(39);
						BENGON;
						FA_INWATERON;
						FA_FLUSHON;
						DATAlist[0x21] |= 0x10;
						_delay_ms(1000);
						sendbit |= 0x01;
						send_status();
					}
					if (flushontime > 0)
					{
						flushontime--;
						if (flushontime == 0)
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
							DATAlist[0x21] &= 0xef;
							_delay_ms(1000);
							//send_status();
							fastatus |= 0x08;
						}
					}


					if ((dealbyte & 0x20) == 0x20)//��λ��о���趨ֵ
					{
						EEPROM_write(78, DATAlist[0x1a]);
						EEPROM_write(79, DATAlist[0x1b]);
						EEPROM_write(80, DATAlist[0x1c]);
						EEPROM_write(81, DATAlist[0x1d]);
						EEPROM_write(82, DATAlist[0x1e]);
						EEPROM_write(83, DATAlist[0x1f]);
						for (i = 0; i < 12; i++)  //���渴λ��оֵ
						{
							EEPROM_write((86 + i), DATAlist[0x24 + i]);
						}
						send_reply(0);
						dealbyte &= 0xdf;
					}

					if ((dealbyte & 0x80) == 0x80)//��ֹ����
					{
						EEPROM_write(25, DATAlist[0x18]);
						send_reply(0);
						dealbyte &= 0x7f;
						if (DATAlist[0x18] == 0xbb)
						{
							//send_reply(0);
							HT1721C_BL_0;
							LCDmode = 48;
							dealbyte &= 0x7f;
							//send_reply(0);
							FA_HOTOFF;
							FA_WARMOFF;
							//FA_COOLOFF;
							fastatus &= 0xf8;//3�����ŵ�״̬�ر�
							FA_INWATEROFF;
							waterbit &= 0xfb;//��λ �ر���ˮ
							FA_FLUSHOFF;
							minbit &= 0xfd;//�رճ�ϴ��
							COOLOFF;
							coolcount = 60;
							waterbit &= 0xef;//��0��ѹ�����ѹر�
							//HEATAOFF;
							waterbit &= 0xdf;//��0�������ѹر�
							BENG1OFF;
							BENGOFF;
							fastatus &= 0xf7;
							DATAlist[0x20] = 0x00;
							DATAlist[0x21] = 0x00;
							_delay_ms(1000);
							send_status();
							WRITE_ALLRAM_1726(0);
						}
					}
					break;
				case 6:
					//------ˮ��-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] |= 0x10;
					if (((LCDNUM[1] & 0x01) == 0x01) && ((wifibit & 0x01) == 0x01) && (resetcount < 2))
					{
						_delay_ms(200);
						sendbit |= 0x0d;
						wifibit &= 0xfe;
						_delay_ms(2000);
						send_status();
						_delay_ms(2000);
						send_error();
						_delay_ms(2000);
						send_filter();
						_delay_ms(200);

					}
					//---------
					  //------©ˮ-------
					  /*
					  if((PINF&0x20)==0x20)//û©ˮ
					  {
						  LCDNUM[13]&=0xef;
					  }
					  else
					  {
						  LCDNUM[13]|=0x10;
					  }*/
					ad_getdat(5);//
					_delay_ms(100);
					if (DATAlist[0x41 + 10] > 200)//gao,û©ˮ
					{
						LCDNUM[13] &= 0xef;
						DATAlist[0x09] &= 0xef;
					}
					else
					{
						_delay_ms(300);
						ad_getdat(5);
						_delay_ms(100);
						if (DATAlist[0x41 + 10] < 200) //��ȷ��һ��
						{
							LCDNUM[13] |= 0x10;
							DATAlist[0x09] |= 0x10;
							BEEP(50);
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
						}

					}
					if (((DATAlist[0x41 + 10] > 200) && (loushuibk < 201)) || ((DATAlist[0x41 + 10] < 201) && (loushuibk > 200)) && (resetcount < 2))//����©ˮ��Ϣ
					{
						loushuibk = DATAlist[0x41 + 10];
						sendbit |= 0x08;
						send_error();//���ʹ������
						_delay_ms(200);
					}

					break;
				case 7:
					//---------------�����Ƿ�ȱˮ����Ϣ------------------------
					if ((((DATAlist[0x0b] & 0x02) == 0x02) && ((timebit & 0x10) == 0x00)) || (((DATAlist[0x0b] & 0x02) == 0x00) && ((timebit & 0x10) == 0x10)))//��ͬ
					{
						timebit ^= 0x10;
						sendbit |= 0x08;
						send_error();
					}

					break;
				case 8:
					if (resetcount > 0)
					{
						resetcount--;
						switch (resetcount)
						{
						case 199:
							RELOAD2_0;
							//BEEP(10);
							//BEEP(200);
							break;
						case 197:
							RESET_0;
							_delay_ms(10);
							//RESET_0;
							//BEEP(10);
							break;
						case 195:
							RESET_1;
							break;
						case 193:
							RELOAD2_1;
							break;
						case 191:
							RELOAD2_1;
							break;
						case 171://ate0172
							gprs_init();
							rx1poit = 0;
							for (ip = &sendhx[0]; ip < (&sendhx[0] + 4); ip++) //ATE0
							{
								putchar1(*ip);
							}
							putchar1(0x0d);
							putchar1(0x0a);
							//---wait reply--
							rx1buffer[1] = 0x00;
							_delay_ms(200);
							rx1poit = 0;
							for (temp = 0; temp < 100; temp++)
							{
								if (rx1buffer[rx1poit - 3] == 0x4F) temp = 101;
								_delay_ms(150);
								//BEEP(100);
							}
							_delay_ms(100);
							break;
						case 170:
							//-----------------
							rx1poit = 0;
							for (ip = &sendnet[0]; ip < (&sendnet[0] + 37); ip++) //
							{
								putchar1(*ip);
							}
							putchar1(0x0d);
							putchar1(0x0a);
							//---wait reply--
							rx1buffer[1] = 0x00;
							_delay_ms(30000);
							_delay_ms(20000);
							_delay_ms(10000);
							_delay_ms(10000);

							for (temp = 0; temp < 200; temp++)
							{
								if (rx1buffer[rx1poit - 3] == 0x4f) temp = 201;
								_delay_ms(200);
								//BEEP(100);
							}
							_delay_ms(1000);
							//---
							break;
						case 164:
							rx1poit = 0;
							for (ip = &sendtc[0]; ip < (&sendtc[0] + 13); ip++) //
							{
								putchar1(*ip);
							}
							putchar1(0x0d);
							putchar1(0x0a);
							//---wait reply--
							rx1buffer[1] = 0x00;
							_delay_ms(200);

							for (temp = 0; temp < 100; temp++)
							{
								if (rx1buffer[rx1poit - 3] == 0x4f) temp = 101;
								_delay_ms(100);
								//BEEP(100);
							}
							_delay_ms(100);
							//---
							break;
						case 162:
							resetcount = 0;
							sendtime = 1;
							//BEEP(200);
							break;

						default:break;
						}
					}
					break;

				default:break;
				}
				//ONE.2----------------�������ӿ��ź�--------------------------------------------------------
				//-------��ˮ���---------
				if ((flushontime == 0) && (LCDmode == 0))
				{
					if ((PINB & 0x80) == 0x80)//1 1
					{
						LLJcount = 0;
						LCDNUM[14] &= 0xef;
						if ((PINB & 0x40) == 0x00)//ˮ��
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;

							if (flushcont > 200)//��ˮ10���ˣ���ϴһ��
							{
								flushcont = 0;
								flushontime = EEPROM_read(39);
								BENGON;
								FA_INWATERON;
								FA_FLUSHON;
							}
							DATAlist[0x0b] &= 0xfd;//��ˮ
						}
						else//���� 1 0
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
						}
					}
					else   //0
					{
						if ((PINB & 0x40) == 0x00)//��ˮ 0 0 ��û��©ˮ�������
						{
							if ((LCDNUM[13] & 0x10) == 0x00)//���ʱ��Ϊ0����������ˮ��&&(DATAlist[0x5e]!=0x00)&&(DATAlist[0x5f]!=0x00)   û©ˮ
							{
								if ((DATAlist[0x5e] == 0x00) && (DATAlist[0x5f] == 0x00))//ʣ��ʱ�䲻Ϊ0
								{
									_delay_ms(1);
								}
								else
								{//---��ˮ------
									BENGON;
									FA_INWATERON;
									FA_FLUSHOFF;
									tdscount = 6;
									LCDNUM[14] &= 0xef;
									//-----	���� ���ж���λ
									LLJcount++;
									if (LLJcount > 60)
									{
										LLJcount = 0;
										tempint1 = TCNT1;
										if (tempint1 == bkliul)//����û��
										{
											_delay_ms(3000);
											bkliul = TCNT1;
											if (tempint1 == bkliul)//������ȣ�����û�仯�������ƹ���
											{
												if ((DATAlist[0x0b] & 0x40) == 0x00)//û��λ
												{
													DATAlist[0x0b] |= 0x40;
													_delay_ms(300);
													sendbit |= 0x08;
													send_error();
													//_delay_ms(300);
												}
											}
											else//�����
											{
												bkliul = tempint1;
												if ((DATAlist[0x0b] & 0x40) == 0x40)//��λ
												{
													DATAlist[0x0b] &= 0xbf;
													_delay_ms(300);
													sendbit |= 0x08;
													send_error();
													//_delay_ms(300);
												}
											}
										}
										else//�����
										{
											bkliul = tempint1;
											if ((DATAlist[0x0b] & 0x40) == 0x40)//��λ
											{
												DATAlist[0x0b] &= 0xbf;
												_delay_ms(300);
												sendbit |= 0x08;
												send_error();
												//_delay_ms(300);
											}
										}
									}
									//--------------
								}

							}
							DATAlist[0x0b] &= 0xfd;//��ˮ
						}
						else//ȱˮ   01
						{
							LLJcount = 0;
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
							LCDNUM[14] |= 0x10; //ȱˮ
							DATAlist[0x0b] |= 0x02;//ȱˮ
						}
					}
				}//flushontime==0   


			//---------------------��������----------------------ʣ�����Ƚ��Ƿ������------------------------
				if ((fastatus & 0x10) == 0x10)//��Ҫ���汾�γ�ˮ����ֵ
				{
					thistimepuls = TCNT1;
					TCNT1 = 0;
					fastatus &= 0xef;//�������
					thistimepuls = yupulsh * 256 + yupulsl + thistimepuls;//��������
					temp = thistimepuls / pule1l;//����1����������
					yupulsh = (thistimepuls - temp*pule1l) / 256;
					yupulsl = thistimepuls - temp*pule1l - yupulsl * 256;
					EEPROM_write(35, yupulsh);
					EEPROM_write(36, yupulsl);//��������
					if (temp > 0)//��Ҫ������о��ֵ������
					{
						tempint1 = DATAlist[0x24] * 256 + DATAlist[0x25] + temp; //��о1
						DATAlist[0x24] = tempint1 / 256;
						DATAlist[0x25] = tempint1 - (DATAlist[0x24] * 256);
						tempint1 = DATAlist[0x26] * 256 + DATAlist[0x27] + temp; //��о2
						DATAlist[0x26] = tempint1 / 256;
						DATAlist[0x27] = tempint1 - (DATAlist[0x26] * 256);
						tempint1 = DATAlist[0x28] * 256 + DATAlist[0x29] + temp; //��о3
						DATAlist[0x28] = tempint1 / 256;
						DATAlist[0x29] = tempint1 - (DATAlist[0x28] * 256);
						tempint1 = DATAlist[0x2A] * 256 + DATAlist[0x2B] + temp; //��о4
						DATAlist[0x2A] = tempint1 / 256;
						DATAlist[0x2B] = tempint1 - (DATAlist[0x2A] * 256);
						tempint1 = DATAlist[0x2C] * 256 + DATAlist[0x2D] + temp; //��о5
						DATAlist[0x2C] = tempint1 / 256;
						DATAlist[0x2D] = tempint1 - (DATAlist[0x2C] * 256);
						tempint1 = DATAlist[0x2E] * 256 + DATAlist[0x2F] + temp; //��о6
						DATAlist[0x2E] = tempint1 / 256;
						DATAlist[0x2F] = tempint1 - (DATAlist[0x2E] * 256);
						for (i = 0; i < 12; i++)			//menmory to 86+12==98  ��������
						{
							EEPROM_write((86 + i), DATAlist[0x24 + i]);
						}
						tempint1 = DATAlist[0x3b] * 256 + DATAlist[0x3c] + temp;
						DATAlist[0x3b] = tempint1 / 256;
						DATAlist[0x3c] = tempint1 - (DATAlist[0x3b] * 256);

						tempint1 = DATAlist[0x3d] * 256 + DATAlist[0x3e];//+temp; 
						if (tempint1 > temp)
						{
							tempint1 = tempint1 - temp;
						}
						else
						{
							tempint1 = 0;
						}
						DATAlist[0x3d] = tempint1 / 256;
						DATAlist[0x3e] = tempint1 - (DATAlist[0x3d] * 256);
						EEPROM_write(28, DATAlist[0x3b]);
						EEPROM_write(29, DATAlist[0x3c]);
						EEPROM_write(30, DATAlist[0x3d]);
						EEPROM_write(31, DATAlist[0x3e]);
						//----��������-----
						_delay_ms(100);
						sendbit |= 0x04;
						send_filter();
					}
				}


				//ONE.22---
				if (LCDmode == 0) REFRESH_ALLRAM_1726();
				//HT1721C_STB_1;
				//----�Ƿ���Ҫ��˸------
				flashcount++;
				if (flashcount > 6)flashcount = 0;
				if ((databit & 0x01) == 0x01)
				{

					if (flashcount > 3)
					{
						WRITE_1DATA_1721(LCDNUM[16], LCDNUM[18]);
						WRITE_1DATA_1721(LCDNUM[17], LCDNUM[19]);
					}
				}
				if (flashcount > 3)//flash
				{
					if ((flashbit & 0x01) == 0x01)//wifi
					{

					}
					if ((flashbit & 0x02) == 0x02)//����
					{
						WRITE_1DATA_1721(12, LCDNUM[12] & 0xf7);
					}
					if ((flashbit & 0x04) == 0x04)//����
					{
						WRITE_1DATA_1721(13, LCDNUM[13] & 0xf7);
					}
					if ((flashbit & 0x08) == 0x08)//����
					{

					}
					if ((flashbit & 0x10) == 0x10)//Ц��
					{
					}
					if ((flashbit & 0x20) == 0x20)//Ц��+��ͯ����
					{

					}

				}
			} //LCDMODE==0
		   //----------------------------------LCDMODE==0 UP----------------------------------------------18260



			if (LCDmode == 42)
			{
				_delay_ms(1000);
				if ((dealbyte & 0x01) == 0x01)//����
				{
					send_reply(0);
					HT1721C_BL_1;
					LCDmode = 0;
					dealbyte &= 0xfe;

					DATAlist[0x20] = 0x01;
					DATAlist[0x21] = 0x00;
					fastatus |= 0x08;//����һ�ο���״̬
					_delay_ms(1000);
					send_status();
					_delay_ms(1000);
					BEEP(50);
				}
				sendpuls++;
				if (sendpuls > 60)//200-22s
				{
					send_status();
					sendpuls = 0;
					//BEEP(20);
				}
			}

			if (LCDmode == 48)
			{
				_delay_ms(3000);
				if (DATAlist[0x18] == 0xaa)//���ܷ�
				{
					send_reply(0);
					HT1721C_BL_1;
					LCDmode = 0;
					EEPROM_write(25, DATAlist[0x18]);
					dealbyte &= 0x7f;

					DATAlist[0x20] = 0x01;
					DATAlist[0x21] = 0x00;
					fastatus |= 0x08;//����һ�ο���״̬
					temp = 100;
					_delay_ms(3000);
					BEEP(50);
				}

			}


			//SIX--------------------------------------------------------------------------------------------
			//��λ��о
			//---------------------------------------------------------------------------------------
			if (LCDmode == 46)// �жϲ����Ƿ�λ��ȥ !=0    ((LCDmode==200)&&(LCDmode<30))//
			{
				LCDtemp[2]++;
				if (LCDtemp[2] > 8)
				{
					WRITE_1DATA_1721(9, 0x00);
					WRITE_1DATA_1721(8, 0x02);
					LCDtemp[2] = 0;
					if (LCDtemp[3] & 0x80)//---Ϩ��
					{
						LCDtemp[3] &= 0x7f;//
						WRITE_1DATA_1721(6, 0);
						WRITE_1DATA_1721(5, 0);
						WRITE_1DATA_1721(7, 0);
						//WRITE_1DATA_1721(6,LCDtemp[10]&0x10);
						//WRITE_1DATA_1721(5,LCDtemp[11]%0x1f);
						//WRITE_1DATA_1721(7,LCDtemp[13]&0xf0);
					}
					else
					{
						LCDtemp[3] |= 0x80;
						//��ˮ//ȡ����λ
						LCDtemp[10] = 0;
						LCDtemp[11] = 0;
						LCDtemp[13] = 0;
						temp = LCDtemp[4] / 10;
						LCDtemp[10] = (SMGL[temp] & 0xf0) >> 4;
						LCDtemp[11] = (SMGL[temp] & 0x0f) << 4;
						//LCDtemp[10]=(LCDtemp[10]&0xf0)|((SMGL[temp]&0xf0)>>4);
						//LCDtemp[11]=(LCDtemp[11]&0x1F)|((SMGL[temp]&0x0f)<<4);
						temp = LCDtemp[4] - (temp * 10);
						LCDtemp[13] = (SMGL[temp] & 0xf0) >> 4;
						LCDtemp[10] = (LCDtemp[10] & 0x1f) | ((SMGL[temp] & 0x0f) << 4);
						//LCDtemp[13]=(LCDtemp[13]&0xF0)|((SMGL[temp]&0xf0)>>4);
						//LCDtemp[10]=(LCDtemp[10]&0x1f)|((SMGL[temp]&0x0f)<<4);
						WRITE_1DATA_1721(6, LCDtemp[10]);
						WRITE_1DATA_1721(5, LCDtemp[11]);
						WRITE_1DATA_1721(7, LCDtemp[13]);

						switch (LCDtemp[4])
						{
						case 1://0
							WRITE_1DATA_1721(13, 0x20);
							WRITE_1DATA_1721(14, 0x00);
							break;
						case 2://0
							WRITE_1DATA_1721(13, 0x00);
							WRITE_1DATA_1721(14, 0x01);
							break;
						case 3://0
							WRITE_1DATA_1721(13, 0x00);
							WRITE_1DATA_1721(14, 0x02);
							break;
						case 4://0
							WRITE_1DATA_1721(13, 0x40);
							WRITE_1DATA_1721(14, 0x00);
							break;
						case 5://0
							WRITE_1DATA_1721(13, 0x00);
							WRITE_1DATA_1721(14, 0x04);
							break;
						case 6://0
							WRITE_1DATA_1721(13, 0x00);
							WRITE_1DATA_1721(14, 0x08);
							break;
						default:break;
						}
					}
				}
				//-------------------
				//------------��1��ť-------------------
				if (keycode[0] == 0x08)
				{
					BEEP(10);
					_delay_ms(2000);
					LCDtemp[3] &= 0x7f;
					LCDtemp[2] = 9;
					LCDtemp[4]++;
					if (LCDtemp[4] > 6)LCDtemp[4] = 1;
				}
				//--------------------------
				//------------��1��ť-------------------
				if (keycode[0] == 0x80)
				{
					BEEP(10);
					_delay_ms(2000);
					LCDtemp[3] &= 0x7f;
					LCDtemp[2] = 9;
					if (LCDtemp[4] == 1)LCDtemp[4] = 7;
					LCDtemp[4]--;
				}
				//------------�����˳���ť-------------------
				if (keycode[0] == 0x04)
				{
					BEEP(100);
					LCDmode = 0;
				}

				//--------------------------
				//---------ȷ����ť-----�����ϴε����ݣ�������һ��ģʽ----------
				if (keycode[1] == 0x08)//�ı�LCDmode
				{
					BEEP(100);
					_delay_ms(2000);
					setcount++;
					if (setcount > 6) setcount = 0;//16
					LCDtemp[3] &= 0x7f;
					LCDtemp[2] = 9;
					// putchar1(0x55);
					// putchar1(LCDtemp[4]);
					// putchar1(DATAlist[0x1A+setcount]);
					 //WTN3_MUSIC(LCDmode-1);//test
					temp = LCDtemp[4];
					DATAlist[0x22 + 2 * temp] = 0;
					DATAlist[0x23 + 2 * temp] = 0;
					EEPROM_write(2 * temp + 84, 0);
					EEPROM_write(2 * temp + 85, 0);
				}
			}		//36 

		}	//loopnum
	}//while
}//MAIN