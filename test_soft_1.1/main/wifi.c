/*
 * wifi.c
 *
 * Created: 2017/10/27 16:38:20
 *  Author: HXHZQ
 */ 


#include <util/delay.h>

typedef unsigned char uchar ;
char *ip;
char send3add[] = "+++";
char sendeoff[] = "AT+E=off";// ����STA
char sendsta[] = "AT+WMODE=STA";// ����STA
char sendser[] = "AT+NETP=TCP,CLIENT,7170,wiot.chaction.cn";//���÷�����  119.23.142.140
char sendair[] = "AT+SMTSL=air";
char sendquit[] = "AT+ENTM";//�˳�ATָ��ģʽ
char sendclose[] = "";

extern unsigned char rx1buffer[60];//��������1����
extern unsigned char rx1poit;   //��������1ָ��
void putchar1(unsigned char data1);
//***********************************************************************
//                  ����wifi����
// //------wifiģ���ʼ��--------
//����wifi��ʼ������
//ģʽ�����������˿ڣ�ͨѶ����
//----------------------------
//------------------------------------------------
//***********************************************************************

void wifi_init(void)
{   uchar t;
	for(ip=&send3add[0];ip<(&send3add[0]+3);ip++) //+++
	{
		putchar1(*ip);
	}
	//---wait reply--
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<130;t++)
		{
			if(rx1buffer[0]==0x61) t=131;
			_delay_ms(100);
			//BEEP(100);
		}
	_delay_ms(100);
	//---
	putchar1(0x61); //replay a

	//---wait reply--
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<100;t++)
	{
		if(rx1buffer[1]==0x6f) t=101;
		_delay_ms(100);
		//BEEP(100);
	}
	_delay_ms(100);
	//---
	
	for(ip=&sendeoff[0];ip<(&sendeoff[0]+8);ip++) //AT+E=off
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<100;t++)
	{
		if(rx1buffer[1]==0x6f) t=101;
		_delay_ms(100);
		//BEEP(100);
	}
	_delay_ms(100);
	//---
	
	for(ip=&sendsta[0];ip<(&sendsta[0]+12);ip++) //AT+WMODE=STA
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<100;t++)
	{
		if(rx1buffer[1]==0x6f) t=101;
		_delay_ms(100);
		//BEEP(100);
	}
	_delay_ms(1000);
	//---
	
	for(ip=&sendser[0];ip<(&sendser[0]+40);ip++) //AT+NETP=TCP,CLIENT,7300,wiot.chaction.cn  40
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<100;t++)
	{
		if(rx1buffer[1]==0x6f) t=101;
		_delay_ms(100);
		//BEEP(100);  
	}
	_delay_ms(100);
	//---
	
	for(ip=&sendair[0];ip<(&sendair[0]+12);ip++) //AT+SMTSL=air AT+ENTM
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<100;t++)
	{
		if(rx1buffer[1]==0x6f) t=101;
		_delay_ms(100);
		//BEEP(100);
	}
	_delay_ms(100);
	//---
	
	for(ip=&sendquit[0];ip<(&sendquit[0]+7);ip++) //AT+ENTM
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(200);
	rx1poit=0;
	for(t=0;t<100;t++)
	{
		if(rx1buffer[1]==0x6f) t=101;
		_delay_ms(100);
		//BEEP(100);
	}
	_delay_ms(100);
	//---
	
}