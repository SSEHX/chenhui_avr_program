/*
 * int.c
 *
 * Created: 2017/10/27 17:11:06
 *  Author: HXHZQ
 */ 
#include <avr/io.h>

#define HT1721C_BL_1 PORTE|=(1<<PE2)    //背光灯开
#define HT1721C_BL_0 PORTE&=~(1<<PE2)
#define LEDRUN PORTC^=(1<<PC0)			//指示灯闪烁


unsigned char T0counter;
unsigned char time1s = 0;   //1秒时间计数
unsigned char count18s = 0;//18s计时
extern unsigned char minbit; //分钟位 1-8:1-20s置位(18s,用作制水定时以及冲洗定时) 2--冲洗动作位
extern unsigned char fastatus; //阀门的状态，1为打开，0为关闭：1-8：1开水阀  2温水阀 3冷水阀 4需要上传状态 5-需要保存热水流量 6-保存温水流量 7-保存冷水流量
unsigned char flushcont = 0;//制水多少冲洗一次 20为1升

extern unsigned char rx0buffer[60];//接收数据0数组

extern unsigned char rx1buffer[60];//接收数据1数组
extern unsigned char rx1count;  //接收数据1计时
extern unsigned char rx1poit;   //接收数据1指针

extern unsigned char DATAlist[200];
extern unsigned char LCDNUM[32];        //LCD的显示数组
unsigned int wificount = 0;//wifi是否有效计数

extern unsigned char sendbit;	//发送标志位  1--01指令，2--02指令 3--03 指令 4--04指令
extern unsigned char needreturn;//需要返回指令号，有返回则清楚，特别是05指令。

unsigned char flatbit = 0x00;//标志位，第一位发送令给普通板,第二位接收到普通板的指令，第三位wifi发送查询指令1有返回，0没返回；
					//第四位表首地址有效位，有效才发送相关数据 第五位，wifi状态，1正常，0断网  第六位 安卓发送指令1需返回，0不用返回。
#define wififlag flatbit&0x10 //wifi是否有返回位

extern unsigned char dealbyte;//需要处理的指令：


//SIGNAL(TIMER0_OVF_vect)//定时器T0中断

void TIME0_hand(void)
{   //uint num1;
	unsigned char w;

	TCNT0 = 0;               //重发初值，下次中断也10MS
	T0counter++;
	//POT0counterRTC^=(1<<LED_LINK);
	if (T0counter > 20)//10-9   100   
	{
		T0counter = 0;
		time1s++;

		if (time1s > 18)    //10-18s  18-1.5s  20-62s   1.5s   时间片检测
		{ // PORTG^=(1<<LED_LINK); 
			time1s = 0;
			LEDRUN;
			count18s++;
			if (count18s > 13)//20s
			{
				minbit |= 0x01;//置位20s
			}

		}	//1.5s		

	//---------流量检测，在微信取水的时候检测--------------

		if (TCNT1 > 30)//开水总量已经达到了要求值，提交记录t1
		{
			fastatus |= 0x10;//需要保存流量了 
			flushcont++;
			if (flushcont > 240)flushcont = 240;//不再增加，等冲洗
		}

	}
	//*****************************中断接收1处理程序*************************************************
	rx1count++;
	if (rx1count > 15)//处理数据 处理完rx1poit=0  &&(havedata==1)
	{
		rx1count = 0;
		// PORTC^=(1<<LED_ALARM); 
		if (rx1poit > 16)
		{
			for (w = 0; w < rx1poit - 3; w++)
			{
				if ((rx1buffer[w] == 0xbb) && (rx1buffer[w + 1] == 0x66) && (rx1buffer[w + 2] == 0x66) && (rx1buffer[w + 3] == 0x80))
				{
					//校验ID码
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
					if ((DATAlist[0x7a] == rx1buffer[w + rx1buffer[w + 5]]) && (DATAlist[0x7b] == rx1buffer[w + rx1buffer[w + 5] + 1]))//CRC校验正确
					{

						//-----------------------串口1接收指令处理------------------------------
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
							flatbit |= 0x04;//有返回置位
							if ((rx1buffer[w + 16] > 0) && (rx1buffer[w + 16] < 0x2F))
							{
								switch (rx1buffer[w + 16])
								{
								case 0x01://开机
									HT1721C_BL_1;
									dealbyte |= 0x01;
									break;
								case 0x02://关机
									HT1721C_BL_0;
									dealbyte |= 0x02;
									break;
								case 0x03://冲洗
									dealbyte |= 0x04;
									break;
								case 0x04://不用

									break;
								case 0x05://BUYONG


									break;
								case 0x06://复位滤芯至额定值
									if ((rx0buffer[w + 17] > 0) && (rx0buffer[w + 17] < 0x07))
									{
										DATAlist[rx1buffer[w + 17] * 2 + 0x22] = 0;
										DATAlist[rx1buffer[w + 17] * 2 + 0x23] = 0;
										DATAlist[rx1buffer[w + 17] + 0x19] = rx1buffer[w + 20];
										dealbyte |= 0x20;//需要保存
									}
									break;
								case 0x09://不用

									break;
								case 0x15://禁止机器
									if ((rx1buffer[w + 17] == 0xaa) || (rx1buffer[w + 17] == 0xbb))
									{
										DATAlist[0x18] = rx1buffer[w + 17];
										dealbyte |= 0x80;
									}
									break;
								case 0x14://设置倒计时时钟
									DATAlist[0x5e] = rx1buffer[w + 17];//
									DATAlist[0x5f] = rx1buffer[w + 18];//
									dealbyte |= 0x10;
									break;
								default:break;
								}
							}
						}
					}// CRC校验正确					
				  //-----------------------------------------------------------------
				  //--------处理完毕
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
