/*
 * int.c
 *
 * Created: 2017/10/27 17:11:06
 *  Author: HXHZQ
 */ 
#include <avr/io.h>

#define HT1721C_BL_1 PORTE|=(1<<PE2)    //����ƿ�
#define HT1721C_BL_0 PORTE&=~(1<<PE2)
#define LEDRUN PORTC^=(1<<PC0)			//ָʾ����˸


unsigned char T0counter;
unsigned char time1s = 0;   //1��ʱ�����
unsigned char count18s = 0;//18s��ʱ
extern unsigned char minbit; //����λ 1-8:1-20s��λ(18s,������ˮ��ʱ�Լ���ϴ��ʱ) 2--��ϴ����λ
extern unsigned char fastatus; //���ŵ�״̬��1Ϊ�򿪣�0Ϊ�رգ�1-8��1��ˮ��  2��ˮ�� 3��ˮ�� 4��Ҫ�ϴ�״̬ 5-��Ҫ������ˮ���� 6-������ˮ���� 7-������ˮ����
unsigned char flushcont = 0;//��ˮ���ٳ�ϴһ�� 20Ϊ1��

extern unsigned char rx0buffer[60];//��������0����

extern unsigned char rx1buffer[60];//��������1����
extern unsigned char rx1count;  //��������1��ʱ
extern unsigned char rx1poit;   //��������1ָ��

extern unsigned char DATAlist[200];
extern unsigned char LCDNUM[32];        //LCD����ʾ����
unsigned int wificount = 0;//wifi�Ƿ���Ч����

extern unsigned char sendbit;	//���ͱ�־λ  1--01ָ�2--02ָ�� 3--03 ָ�� 4--04ָ��
extern unsigned char needreturn;//��Ҫ����ָ��ţ��з�����������ر���05ָ�

unsigned char flatbit = 0x00;//��־λ����һλ���������ͨ��,�ڶ�λ���յ���ͨ���ָ�����λwifi���Ͳ�ѯָ��1�з��أ�0û���أ�
					//����λ���׵�ַ��Чλ����Ч�ŷ���������� ����λ��wifi״̬��1������0����  ����λ ��׿����ָ��1�践�أ�0���÷��ء�
#define wififlag flatbit&0x10 //wifi�Ƿ��з���λ

extern unsigned char dealbyte;//��Ҫ�����ָ�


//SIGNAL(TIMER0_OVF_vect)//��ʱ��T0�ж�

void TIME0_hand(void)
{   //uint num1;
	unsigned char w;

	TCNT0 = 0;               //�ط���ֵ���´��ж�Ҳ10MS
	T0counter++;
	//POT0counterRTC^=(1<<LED_LINK);
	if (T0counter > 20)//10-9   100   
	{
		T0counter = 0;
		time1s++;

		if (time1s > 18)    //10-18s  18-1.5s  20-62s   1.5s   ʱ��Ƭ���
		{ // PORTG^=(1<<LED_LINK); 
			time1s = 0;
			LEDRUN;
			count18s++;
			if (count18s > 13)//20s
			{
				minbit |= 0x01;//��λ20s
			}

		}	//1.5s		

	//---------������⣬��΢��ȡˮ��ʱ����--------------

		if (TCNT1 > 30)//��ˮ�����Ѿ��ﵽ��Ҫ��ֵ���ύ��¼t1
		{
			fastatus |= 0x10;//��Ҫ���������� 
			flushcont++;
			if (flushcont > 240)flushcont = 240;//�������ӣ��ȳ�ϴ
		}

	}
	//*****************************�жϽ���1�������*************************************************
	rx1count++;
	if (rx1count > 15)//�������� ������rx1poit=0  &&(havedata==1)
	{
		rx1count = 0;
		// PORTC^=(1<<LED_ALARM); 
		if (rx1poit > 16)
		{
			for (w = 0; w < rx1poit - 3; w++)
			{
				if ((rx1buffer[w] == 0xbb) && (rx1buffer[w + 1] == 0x66) && (rx1buffer[w + 2] == 0x66) && (rx1buffer[w + 3] == 0x80))
				{
					//У��ID��
					wificount = 0;
					LCDNUM[1] |= 0x01;
					LCDNUM[2] |= 0x01;
					LCDNUM[3] |= 0x01;
					LCDNUM[4] |= 0x01;
					LCDNUM[5] |= 0x01;

					if ((DATAlist[0x21] & 0x80) == 0x00)
					{
						DATAlist[0x21] |= 0x80;
						fastatus |= 0x08;
					}

					//-----CRC ---
					CRC16_rxt11(w, rx1buffer[w + 5]);
					if ((DATAlist[0x7a] == rx1buffer[w + rx1buffer[w + 5]]) && (DATAlist[0x7b] == rx1buffer[w + rx1buffer[w + 5] + 1]))//CRCУ����ȷ
					{

						//-----------------------����1����ָ���------------------------------
						if (rx1buffer[w + 15] == 0x01)sendbit &= 0x01;
						if (rx1buffer[w + 15] == 0x02)sendbit &= 0x02;
						if (rx1buffer[w + 15] == 0x03)sendbit &= 0x04;
						if (rx1buffer[w + 15] == 0x04)sendbit &= 0x08;
						if (rx1buffer[w + 15] == 0x05)
						{
							needreturn = 0x00;
						}
						if (rx1buffer[w + 15] == 0x06)
						{ //BEEP(20);//wrong
							flatbit |= 0x04;//�з�����λ
							if ((rx1buffer[w + 16] > 0) && (rx1buffer[w + 16] < 0x2F))
							{
								switch (rx1buffer[w + 16])
								{
								case 0x01://����
									HT1721C_BL_1;
									dealbyte |= 0x01;
									break;
								case 0x02://�ػ�
									HT1721C_BL_0;
									dealbyte |= 0x02;
									break;
								case 0x03://��ϴ
									dealbyte |= 0x04;
									break;
								case 0x04://����

									break;
								case 0x05://BUYONG


									break;
								case 0x06://��λ��о���ֵ
									if ((rx0buffer[w + 17] > 0) && (rx0buffer[w + 17] < 0x07))
									{
										DATAlist[rx1buffer[w + 17] * 2 + 0x22] = 0;
										DATAlist[rx1buffer[w + 17] * 2 + 0x23] = 0;
										DATAlist[rx1buffer[w + 17] + 0x19] = rx1buffer[w + 20];
										dealbyte |= 0x20;//��Ҫ����
									}
									break;
								case 0x09://����

									break;
								case 0x15://��ֹ����
									if ((rx1buffer[w + 17] == 0xaa) || (rx1buffer[w + 17] == 0xbb))
									{
										DATAlist[0x18] = rx1buffer[w + 17];
										dealbyte |= 0x80;
									}
									break;
								case 0x14://���õ���ʱʱ��
									DATAlist[0x5e] = rx1buffer[w + 17];//
									DATAlist[0x5f] = rx1buffer[w + 18];//
									dealbyte |= 0x10;
									break;
								default:break;
								}
							}
						}
					}// CRCУ����ȷ					
				  //-----------------------------------------------------------------
				  //--------�������
					w = rx1poit;
					rx1poit = 0;
				}	//if			 
			}	//for

			w = rx1poit;
			rx1poit = 0;


		} //rx1poit>6
		else
		{
			rx1poit = 0;
		}


	}//rx1count>15
//**********************************************************************************
//********************************************************************************** 
}
