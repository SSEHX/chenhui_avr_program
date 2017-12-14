/*
 * gprs.c
 *
 * Created: 2017/10/27 16:38:41
 *  Author: HXHZQ
 */ 

/*	
*gprs 使用usart1 通讯
*/
#include "gprs.h"

typedef unsigned char uchar;


//--------GPRS setting------------------
char sendat[]="AT\r\n";
char sendhx[]="ATE0\r\n";//关回显   --->发送指令gprs模块没有返回
char sendsim[]="AT+CCID\r\n";//查是否插入SIM卡
char sendcsq[]="AT+CSQ\r\n";//信号检测 
char sendcreg[]="AT+CREG\r\n";//信号检测 
char sendfu[]="AT+CGATT=1\r\n";//附着网络
char sendpdp[]="AT+CGDCONT=1,IP,CMNET\r\n";//设置PDP参数 21
char sendjh[]="AT+CGACT=1,1\r\n";//激活PDP
//char sendnet[]="AT+CIPSTART=TCP,wiot.chaction.cn,7170\r\n";//设置服务器37
char sendnet[]="AT+CIPSTART=TCP,120.24.58.101,7170\r\n";//设置服务器37

char sendtc[] = "AT+CIPTMODE=1";//透传
char sendatmode[] = "+++";//退出透传
char sendtcpclose[] = "AT+CIPCLOSE\r\n";
char sendreset[] = "AT+RST=1\r\n";//重新设置8

//-------------------------------------------
void putchar1(unsigned char data1);
void BEEP(unsigned char sencond);

void init_devices(void);
void uart1_init(void);
//***********************************************************************

unsigned char GPRS_Mode_At(void)
{ 
	unsigned char i = 0;
	for( i = 0; i < sizeof(sendatmode); i ++)
	{
		USART1_Transmit(*(sendatmode + i));
		_delay_ms(200);
	}
}
inline unsigned char GPRS_Send_Cmd(unsigned char *cmd)
{
	unsigned char n = 2;
	unsigned int timeout;//超时操作
	while(n--)
	{
		memset(GPRS_Receive,0,100);
		timeout = 0xFFF;
		GPRS_Receive_Flag = 0;
		USART1_Transmit_String(cmd);
		while((!(GPRS_Receive_Flag &((0x1<<4)|(0x1<<1)|(0x1<<3)|((0x1<<6)))))&& (timeout --))
			_delay_us(50);
		if(GPRS_Receive_Flag&(0x1<<4))
		{
			return 0;
		}
		if(	LCD_Ico &(0x01 << 6) )
		{
			return 0 ;
		}
		if(SCQ != 0)
		{
			return 0;
		}
		if(GPRS_Receive_Flag&(0x1<<3))
		{
			return 0 ;
		}
		_delay_ms(500);
		if(GPRS_Receive_Flag&(0x01<<9))
		{
			//_delay_ms(3000);
		}
		if(GPRS_Receive_Flag&(0x01<<1))
		{
			_delay_ms(300);
		}
	
	}
	return 1;
	
	return (GPRS_Receive_Flag&(0x1<<4));
}


void SHOW_GPRS_Receivce(void)
{
	while(!GPRS_Receive_Flag);
	unsigned char i = 0;
	for(i = 0;i < 4; i++)
		if(GPRS_Receive_Flag&(0x1<<i))
			USART0_Transmit_String(GPRS_Receive[i]);
}

inline void GPRS_RESET(void)
{
	PORTA |= (0x01<<PA2);//高电平 关闭GPRS
	
	_delay_ms(200);
	PORTA &= ~(0x01<<PA2);//低电平 开启GPRS
	
//	DDRA &= ~(0x01<<PA2);

}

void GPRS_PROT(void)
{	
	DDRA |= (0x01<<PA3);//输出高电平
	PORTA|= (0x01<<PA3);
	
	DDRA |= (0x01<<PA2);//输出低电平
	PORTB &= ~(0x01<<PA2);	
	

}

inline void GPRS_Prot_Init(void)
{

	GPRS_Receive_Flag = 0;
//	PORTA &=~(0x01 << PA2);
//	DDRA &= ~(0x01 << PA2);
	unsigned int timeout;//超时操作
	unsigned char i = 10 ;
	_delay_ms(100);
	//GPRS_RESET();
	while(!(PIND&(0x01<<PD2)))
	{	
			GPRS_RESET();
			_delay_ms(3000);
			
	//	PORTD = (1<<PA2);
		
		timeout = 150;
		//GPRS_Receive_Flag &= ~(0x1<<5);
		while(!(GPRS_Receive_Flag & 0x1<<5) && timeout --)
			_delay_ms(20);
			
		if(!(PIND&(0x01<<PD2)))
			break;
		if(!(GPRS_Receive_Flag & 0x1<<5))
		{
			BEEP(1);
			continue;	
		}
		timeout = 500;
		while(!(GPRS_Receive_Flag & 0x1<<6) && timeout --)
			_delay_ms(50);
		if((GPRS_Receive_Flag & 0x1<<6));
			break;
	
	}
	//BEEP(10);
	//USART0_Transmit_String("GPRS end !\r\n ");
	//USART0_Transmit_String(GPRS_Receive[3]);	
}

#define  TIME_OUT  2
#define  COUNT_TIME 100

inline void GPRS_PDP(void)
{
	unsigned char timeout = TIME_OUT;	
	if(gprs_flag & (0x01<<2))
	{
			timeout = TIME_OUT;
		while(GPRS_Send_Cmd(sendpdp)&&timeout--)
			_delay_ms(500);
		if(GPRS_Receive_Flag & (1<<4))
		{
				timeout = TIME_OUT;	
				while(GPRS_Send_Cmd(sendjh)&&timeout--)	
					_delay_ms(500);
 				if(GPRS_Receive_Flag & (1<<4))
				{
 				//gprs_flag  &= ~(0x01<<3);
					LCD_Ico |= 0x01 << 7;
					gprs_flag  &= ~(0x01<<2);		
				 }
		}

	}
}




//GPRS_FLAG 1:hx 2
inline void GPRS_Config(void)
{
	//GPRS_Mode_At();
	//GPRS_Send_Cmd(sendatmode);
	//_delay_ms(100);
	unsigned char timeout = TIME_OUT;
	while(GPRS_Send_Cmd(sendat)&&timeout--)
	_delay_ms(COUNT_TIME);
	//if((gprs_flag & (0x01<<0)))
	if(1)
	{
		timeout = TIME_OUT;
		while(GPRS_Send_Cmd(sendhx)&&timeout--)
		_delay_ms(COUNT_TIME);
		if(GPRS_Receive_Flag & (1<<4))
			gprs_flag &= ~(0x01<<0);
	}
	timeout = TIME_OUT;
	while(GPRS_Send_Cmd(sendsim)&&timeout--)
	_delay_ms(COUNT_TIME);
	timeout = TIME_OUT;
	while(GPRS_Send_Cmd(sendcsq)&&timeout--)
	_delay_ms(COUNT_TIME);
	if(gprs_flag & (0x01<<1))
	{
			timeout = TIME_OUT;
			while(GPRS_Send_Cmd(sendfu)&&timeout--)
			_delay_ms(COUNT_TIME);	
			if(GPRS_Receive_Flag & (1<<4))
				gprs_flag  &= ~(0x01<<1);
	}

	GPRS_PDP();
	
}
inline unsigned char  GPRS_Connect_Tcp(void)
{
//	LEDOFF;	
//	static unsigned char send_tc_flag = 1;
//	return 1;

		
	if(!GPRS_Send_Cmd(sendat))
	{	
		unsigned char timeout = 800;
	//GPRS_Send_Cmd(sendtcpclose);
	_delay_ms(200);
		//USART0_Transmit_String("------------TCPCONET");
	unsigned char n = 2 ,i;
	while(n--)
	{
		memset(GPRS_Receive,0,100);
		timeout = 0xFFF;
		GPRS_Receive_Flag = 0;
			//if(!GPRS_Receive_Flag)
		//BEEP(1);
		_delay_ms(50);
		
//		GPRS_Send_Cmd(sendnet);
#if 1		
		USART1_Transmit_String(sendnet);
		while((!(GPRS_Receive_Flag&((0x1<<4)|(0x1<<1))))&&(timeout --))
		{
			_delay_ms(10);
		}
#endif

		if(GPRS_Receive_Flag&(0x1<<4))break;
			_delay_ms(300);
#if 0
		if(GPRS_Receive_Flag&(0x1<<3))
		{
			break;
		}
		if(	LCD_Ico &(0x01 << 6) )
		{
			break;
		}
		if(SCQ != 0)
		{
			break;
		}
		_delay_ms(3000);
		if(GPRS_Receive_Flag&(0x01<<9))
		{
			_delay_ms(3000);
		}
		if(GPRS_Receive_Flag&(0x01<<1))
		{
			_delay_ms(500);
		}
#endif
//  		if(GPRS_Receive_Flag&(0x1<<1))
//   			GPRS_Send_Cmd(sendtcpclose);
	}
	
	
	//BEEP(10);
	timeout = 800;
	while(!(GPRS_Receive_Flag & 1<<7) && timeout--)
	{
		_delay_ms(10);
	}
	
	if((GPRS_Receive_Flag & 1<<7))
	{
		//	_delay_ms(200);
		//	LEDON;
			BEEP(10);//连接成功响一声10ms
			return 1;			
	}
	}
	return 0;
}

inline void GPRS_Init(void)
{
	GPRS_Prot_Init();
	GPRS_Config();
	//GPRS_Connect_Tcp();
}

inline void GET_GPRS_Signal(void)
{	
	//PORTA &= ~(0x01<<PA2);
	if(!(PINA &(0x01<<PA2)))
	{
		if(GPRS_Receive_Flag & (0x01<< 5))
			_delay_ms(3000);

 		if(GPRS_Send_Cmd(sendat))
		 {
 			GPRS_Prot_Init();
			SCQ = 0;
			 LCD_Ico &= ~(0x1<<6);
						 			
		 }
		else
		{
				_delay_ms(50);
				if(!GPRS_Send_Cmd(sendsim))
				{
						_delay_ms(50);
						if((!GPRS_Send_Cmd(sendcsq)))
						{
							
							_delay_ms(10);
							if(SCQ != 0)
							{			
								_delay_ms(100);
								unsigned char time_out = 0xF;
								
								//while(!(LCD_Ico &(0x1<<7))&&time_out--)
								if(!(LCD_Ico &(0x1<<7)))
								{
//									BEEP(10);								
									GPRS_Config();
								}
//							
							}
						}
		
				}
				
		}
	}
}


void swap(char *a,char *b)
{	
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
}
char *revstr(char *str, size_t len)
{

	char    *start = str;
	char    *end = str + len - 1;
	if (str != NULL)
	{
		while (start < end)
		swap(start++, end --);
	}
	return str;
}
unsigned char * itoc(unsigned char *c,unsigned int i)
{
	unsigned char len = 0;
	while(i)
	{
		*(c+len) = i%10 +0x30;
		i = i/10;
		//c ++;
		len ++;
	}
	//USART0_Transmit_nChar(c,len);
	return c;
	//return revstr(c,len);
}
unsigned char *itoa(unsigned char *c,unsigned int i)
{
	itoc(c,i);
	//USART0_Transmit_String(c);
	revstr(c,strlen(c));
	//USART0_Transmit(strlen(c));
	return c;
}
inline unsigned char GPRS_Send_Data(unsigned char *Send_Data, unsigned int Send_Len,unsigned char *Rece_Data)
{
	//LEDON;	
	//USART0_Transmit_String("GPRS_Send_Data\r\n");
	//USART0_Transmit_nChar(Send_Data,Send_Len);
#if 0	
	if(!Check_CRC16(Send_Data,0,Send_Len-4))
		LEDON;
#endif

	
		unsigned char sendcipsend[15] = "AT+CIPSEND=";
		unsigned char send_len[2]  = {0};
#if 1
		itoc(send_len,Send_Len);
		revstr(send_len,2);
#else
		itoa(send_len,Send_Len);
#endif
		Str_Add_Str(sendcipsend,11,send_len,2);
		Str_Add_Str(sendcipsend,13,"\r\n",2);
	
		//USART0_Transmit_String(sendcipsend);
		unsigned char i = 2;
		while(i--)
		{
			if(!GPRS_Send_Cmd(sendcipsend));
			{
				unsigned char timeout = 500;
				while(!(GPRS_Receive_Flag & 1<<3) & timeout --)
				_delay_ms(10);
		
				USART1_Transmit_nChar(Send_Data,Send_Len);
				timeout	= 1000;
				while (!(GPRS_Receive_Flag & 1<<2) && timeout --)
				_delay_ms(10);

				if((GPRS_Receive_Flag & 1<<2))
				{
				//LEDON;
					break;
				}
			}
		}
		//			数据的总长度	数据长度
		unsigned char len = 0,revice_len = 0;
		unsigned char len_tmp[4] = {0};
			
//2B 43 49 50 52 43 56 3A 32 32 2C BB 66 66 80 00 12 41 31 31 31 31 31 31 33 00 09 00 10 18 35 FC FF 
		Str_Add_Str(len_tmp,0,&GPRS_Receive[8],2);
		len = atoi(len_tmp);
		revice_len = (GPRS_Receive[15]<<8)|GPRS_Receive[16]+ 4 ;
//		if((len  ==  revice_len))
//			LEDON;
// 		if(len != 0)
// 			LEDRUN;
// 		BEEP(10);
		//memset(Rece_Data,0,len);
		if((revice_len == len) > 20)
		{	
			//BEEP(10);
			//LEDON;
		}
		Str_Add_Str(Rece_Data,0,&(GPRS_Receive[11]),len);
		//USART0_Transmit_nChar(Rece_Data,len);
		//while(1);
	return len;
}

/************************************************************************/
/* 返回数据长度:返回0数据错误                                           */
/************************************************************************/

inline unsigned char ShortTurl(unsigned char *Send_Data, unsigned int Send_Len,unsigned char *Rece_Data)
{
	//LEDON;
		
	unsigned char len = 0x0;
#if 1	
	if((LCD_Ico & (0x1 << 7)))
	{
		
			GPRS_Send_Cmd(sendtcpclose);//断开服务器连接，以免连接错误
			_delay_ms(300);
			len = GPRS_Connect_Tcp();
			_delay_ms(300);
			if(len)	
		//	_delay_ms( 300);
			{
				//_delay_ms(200);
				//BEEP(10);
				//GPRS_Send_Cmd(sendat);
				len = GPRS_Send_Data(Send_Data,Send_Len,Rece_Data);
				GPRS_Send_Cmd(sendtcpclose);	
				_delay_ms(300);
				if(len > 20)
				{
				//	USART0_Transmit_nChar(Rece_Data,len);
				//	LEDOFF;				
					if(Check_CRC16(Rece_Data,0,(len-4)))
					{
	
						BEEP(1000);//crc验证失败
						return 0;
					}
					return len;
				}
			//	LEDOFF;
			}
//			GPRS_Send_Cmd(sendtcpclose);
	
	}
#endif 
	return len;
}

int gprs_main(void)
{
	cli();
	USART1_Init();
	USART0_Init();
	
	LED_Init();
	BEEP_Init();
	DDRA |= 0x01<< PA2;
	PORTA &= ~(0x01<<PA2);
	//PORTA |= (0x01<<PA2);
	sei();
	USART0_Transmit_String("USART1 Init!\r\n");
	//GPRS_Init();

// 	DDRA |= ((1<<PA3)|(1<<PA2));
// 	PORTA|= (1<<PA3);
	unsigned char Cmd[2] = {0x00, 0x09};
	unsigned char Parameter[10] = {0x00,0x00,0x00,0x00,0x74,0x00,0x74,0x00,0x00,0x00,0x00,0x00},Ack[24] = {0}, Ack_Len = 0;
	//unsigned char Data[] = {0xab,0x33,0x33,0x80,0x00,0x1a,0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x33,0x00,0x02,0x00,0x00,0x00,0x00,0x74,0x00,0x74,0x00,0x00,0x00,0x0b,0xcd,0xfc,0xff};
	//unsigned char Data[] = {0xab,0x33,0x33,0x80,0x00,0x1a,0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x33,0x00,0x02,0x00,0x00,0x00,0x00,0x74,0x00,0x74,0x00,0x00,0x00,0x0b,0xcd,0xfc,0xff};
	
	unsigned char Data[] = {0xBB,0x66,0x66,0x80,0x00,0x1D,0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x33,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x73,0xfc,0xff};
	//Ack_Len = Send_Status(Parameter,Ack);
	//unsigned int Data_Crc = CRC16(Data,sizeof(Data) - 4);

	//if(!Check_CRC16(Data,0,sizeof(Data)-4))
		//LEDON;
	//USART0_Transmit_nChar(Data,sizeof(Data));
//	GPRS_Connect_Tcp();
//	Ack_Len = GPRS_Send_Data(Data,sizeof(Data),Ack);
	//_delay_ms(500);
	
	//USART0_Transmit_String("服务器返回：AAAAAAAAA\r\n");
	//USART0_Transmit_String(Ack);
	//USART0_Transmit_nChar(Ack,Ack_Len);
	//	LEDOFF;
	//if(!Check_CRC16(Ack,0,Ack_Len-4))
		//LEDON;

	unsigned char cmd[255];
	//USART0_Transmit_String("GPRS Init!\r\n");
	BEEP(10);
	HT1721C_BL_1;

	GPRS_PROT();
	
	
	while(1)
	{
		LEDRUN;
		
		_delay_ms(1000);
		//GPRS_Prot_Init();
		GET_GPRS_Signal();	
		
	//	GPRS_Prot_Init();		
#if 0
//		_delay_ms(1000);
//		Ack_Len = 0;
//		memset(Ack, 0, sizeof(Ack));
//		Ack_Len = ShortTurl(Data,sizeof(Data),Ack);
//		Ack_Len = Send_Cmd_Parameter(Cmd,Parameter,sizeof(Parameter),Ack);

	//	 GPRS_PDP();
	
 		memset(cmd,0,255);
		// LEDOFF;
//  		USART0_Receive_string(cmd);
  //		GPRS_Send_Cmd(cmd);
	//	  _delay_ms(1000)
 #else
			

			//Write_STATUS_EEPROM(status);

		
				//LEDON;
				_delay_ms(500);
						
				//make_parameter(Parameter, status, tds1, tds2,sum_flow,Filte_CntV);
				unsigned int error = GET_CMD(Parameter,9+2*Filte_num);
				if (error != 0x00)
				{
					LEDRUN;
				}

#endif
	}
	return 0;
}