/*
 * uart.c
 *
 * Created: 2017/10/27 16:38:03
 *  Author: HXHZQ
 */ 

#include "usart.h"

//#define F_CPU 11059200UL


//-------------------USART0--------------
static void USART0_GPIO_Init(void)
{
	
	 |=	0x1<<PE1;//TXD为输出
	DDRE &=	 ~(0x1<<PE0);
	PORTE &= 0xFF;
}
static void USART0_Mode_Init(void)
{
	UCSR0A = 0x00;
	UCSR0B = 0x00;
	UCSR0C = 0x00;
	//波特率设置115200
	UBRR0H = 0;
	UBRR0L = 5;
	//UBRR0L = 6;
	
	UCSR0B |= (1<<RXCIE0)|(1<<TXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	//发送的8个数据位
	UCSR0C |= (1<<UCSZ1)|(1<<UCSZ0);

}
void USART0_Init(void)
{
	USART0_GPIO_Init();
	USART0_Mode_Init();
}
void USART0_Transmit(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
	//LEDRUN;
}
void USART0_Transmit_String(unsigned char *Data)
{
	unsigned char i ;
	for(i = 0;(*(Data+i)) != '\0';i ++)
	{
		USART0_Transmit(*(Data+i));
	}
}
void USART0_Transmit_nChar(unsigned char *Data,unsigned char n)
{
	while(n--)
	{
		USART0_Transmit(*(Data++));
	}
}
unsigned char USART0_Receive(void)
{
	while (!(UCSR0A& (0x1 << RXC0)));
	//LEDRUN;
	return UDR0;	
}
unsigned char *USART0_Receive_string(unsigned char *Data)
{
	
	for(;; Data ++)
	{
		*Data = USART0_Receive();
		if(*Data == '\n')
		break;
	}
	Data ++;
	Data = 0;
	return Data;	
}
unsigned char *USART0_Receive_nChar(unsigned char *Data,unsigned char n)
{
	while(n--)
	{
		*(Data++)=USART0_Receive();
	}	
	return Data;
}
#if 1
SIGNAL(USART0_RX_vect)
{
#if 0
	unsigned char tmp = USART0_Receive() ;
	USART0_Transmit(tmp);
#endif
}
#endif 

SIGNAL(USART0_TX_vect)
{
	
	//BEEP(20);

}

//-------------------USART1--------------
static void USART1_GPIO_Init(void)
{
	DDRD |=		0x1<<PD3;//TXD为输出
	DDRD &=     ~(0x1<<PD2);//RXD为输入
	PORTD &=	0xFF;
}
static void USART1_Mode_Init(void)
{
	UCSR1A = 0x00;
	UCSR1B = 0x00;
	UCSR1C = 0x00;
	//波特率设置(115200)(GPRS模块的初始化波特率是115200)
	UBRR1H = 0;
	UBRR1L = 5;
	//UBRR1L = 6;
	
	UCSR1B |= (1<<RXCIE1)|(1<<TXCIE1)|(1<<RXEN1)|(1<<TXEN1);
	//发送的8个数据位
	UCSR1C |= (1<<UCSZ1)|(1<<UCSZ0);
}
void USART1_Init(void)
{
	USART1_GPIO_Init();
	USART1_Mode_Init();
}
void USART1_Transmit(unsigned char data)
{
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}
void USART1_Transmit_String(unsigned char *Data)
{
	unsigned char i ;
	for(i = 0;(*(Data+i)) != '\0';i ++)
	{
		USART1_Transmit(*(Data+i));
	}
}
void USART1_Transmit_nChar(unsigned char *Data,unsigned char n)
{
	while(n--)
	{
		USART1_Transmit(*(Data++));
	}
}
unsigned char USART1_Receive(void)
{
	while (!(UCSR1A & (1 << RXC1)));
	return UDR1;	
}
unsigned char *USART1_Receive_string(unsigned char *Data)
{
	unsigned char i = 0;
	memset(Data,0,255);
	for(i = 0;(*(Data+i )!= '\n')& (i<255) ;i ++)
	{
		*(Data+i) = USART1_Receive();
	}
	return Data;	
}

unsigned char *USART1_Receive_nChar(unsigned char *Data,unsigned char n)
{
	while(n--)
	{
		*(Data++)=USART1_Receive();
	}	
	return Data;
}

unsigned char GPRS_Receive[100];
unsigned char GPRS_Receive_Num = 0;
unsigned char GPRS_Receive_Type = 0;//数据的类型AT指令，错误信息，Ok信息，数据信息
unsigned int GPRS_Receive_Flag = 1;//0的时候可以操作数据，1的时候不可以操作数据
unsigned char gprs_flag = 0xFF;

SIGNAL(USART1_RX_vect)
{
	//GPRS_Receive_Flag = 0;
	unsigned char tmp =  USART1_Receive();
#ifdef Usart0
	USART0_Transmit(tmp);
#endif 
	if(!GPRS_Receive_Num)
	{
		if(tmp == '>')
			GPRS_Receive_Flag |= 1<<3;	
	}
	GPRS_Receive[GPRS_Receive_Num] = tmp ;
//	USART0_Transmit(GPRS_Receive[GPRS_Receive_Num]);
	GPRS_Receive_Num ++;
#if 1
	 if(GPRS_Receive[GPRS_Receive_Num-1] == '\n')
	{
		GPRS_Receive[GPRS_Receive_Num] = '\0';

		GPRS_Receive_Num = 0;
		
		if(strstr(GPRS_Receive,"+CME ERROR:"))
		{
			//LEDON;
			//LCD_Ico &= ~(0x01 << 7);
			GPRS_Receive_Flag |= 1<<1;
		}
		if(strstr(GPRS_Receive,"+CIPRCV:"))
			GPRS_Receive_Flag |= 1<<2;
		if(strstr(GPRS_Receive,"OK"))
		{
			GPRS_Receive_Flag |= 1<<4;
		//	LCD_Ico |= 0x01 << 7;
			
		}
			
		if(GPRS_Receive[0] == 0 || strstr(GPRS_Receive,"Waiting ..."))
		{
			GPRS_Receive_Flag |= 1<< 5;
			LCD_Ico &= ~(0x01 << 7);
			gprs_flag = 0xFF;
			//LEDON;
		}
		if(strstr(GPRS_Receive,"+CIEV: READY"))
		{
			GPRS_Receive_Flag |= 1<<6;
			if(!(GPRS_Receive_Flag & (0x01<<8)))
				LCD_Ico &= ~(0x01 << 7);
			//LEDOFF;
		}
		if( strstr(GPRS_Receive,"+CME ERROR:50"))
		{
				LCD_Ico &= ~(0x01 << 7);
		}
		if(strstr(GPRS_Receive,"+CTZV:"))
		{
				LCD_Ico &= ~(0x01 << 7);			
		}

		if(strstr(GPRS_Receive,"+CME ERROR:148"))
		{
			//LEDON;
			GPRS_Receive_Flag |= 1<<1;
			
				//LCD_Ico &= ~(0x01 << 7);			
		}
		if(strstr(GPRS_Receive,"+CGATT:1"))
		{
			//LEDRUN;
			LCD_Ico |= 0x01 << 7;
		//	gprs_flag  &= ~(0x01<<3);
		
			//LEDON;
		}
		if( strstr(GPRS_Receive,"+CME ERROR:58"))
		{
				LCD_Ico &= ~(0x01 << 7);
				GPRS_Receive_Flag |= 1<<9;
		}
		
		if(strstr(GPRS_Receive,"+CIEV: service,  1"))
		{
			LCD_Ico |= 0x01 << 7;
			GPRS_Receive_Flag |= 1<<8;
			GPRS_Receive_Flag |= 1<<7;			
			//LEDON;
		}
		if(strstr(GPRS_Receive,"CONNECT OK"))
		{
			LCD_Ico |= 0x01 << 7;
			GPRS_Receive_Flag |= 1<<7;
			//LEDON;
		}
		if(strstr(GPRS_Receive,"+CCID:"))
		{
			LCD_Ico |= 0x01 << 6;
		}
		if(strstr(GPRS_Receive,"+CSQ:"))
		{		
			SCQ = ((GPRS_Receive[6]-0x30)*10+ GPRS_Receive[7] - 0x30);
			if(SCQ)
				LCD_Ico |= 0x01 << 7;
			else
				gprs_flag = 0xFF;
			
		}

	}

#endif
	//GPRS_Receive_Flag = 0;
}

SIGNAL(USART1_TX_vect)
{
	//USART0_Transmit_String("send GPRS cmd:");
	//USART0_Transmit(tmp);
	//BEEP(20);
}



int hfksmain(void)
{
	cli();
	//ASSR |= (1<<AS0);
	LED_Init();
	BEEP_Init();
	USART0_Init();
	USART1_Init();
	sei();
	unsigned char test[100] = {0x00,0x11};
	//BEEP(20);
	
	//USART0_Transmit_nChar(test,2);
	//USART0_Receive_nChar(test,2);
	//USART0_Transmit_nChar(test,2);
	//USART0_Transmit_String(test);
	unsigned char a = 'D';
	USART0_Transmit(a);
	
	USART0_Transmit_String("Hello world!\r\n");
	

	
	//printf("HxH\r\n");
	
	while(1)
	{
		
		 //a = USART1_Receive();
		//USART1_Transmit(a);
		//_delay_ms(500);
		//USART0_Transmit_String("H");
		//USART0_Receive_nChar(test,2);
		//USART0_Transmit_nChar(test,2);
		
		USART0_Receive_string(test);
		//USART0_Transmit_String(test);

	}
	
	return 0;
}