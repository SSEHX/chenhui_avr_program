/*
 * adc.c
 *
 * Created: 2017/10/27 16:47:40
 *  Author: HXHZQ
 */ 

#include "adc.h"

extern unsigned char DATAlist[200];

//------AD初始化--------
//8通道输入
//-------------------------
void adc_init(void)
{
	ADCSRA = 0x00;//close
	ADMUX = 0x60;//(1<<REFS0);//CHANAL0  0xC0|(1<<ADLAR);//E0
	ACSR = 0x80;
	ADCSRA = ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);//使能AD转换，64分频
   // ADCSRA&=~(1<<ADIE); //禁止ADC中断

}


//------AD转换函数--------
// 通道 n
//-----------------------
void ad_getdat(unsigned char n)//ADLR=1高位全满，余2位在低字节7、8位
{
	ADMUX = (ADMUX & 0xe0) | n;
	_delay_ms(100);
	ADCSRA |= (1 << ADSC); //START
	while (!(ADCSRA&(1 << ADIF)));//WAIT
	ADCSRA |= (1 << ADIF);
	DATAlist[0x41 + 2 * n + 1] = ADCL;   //first!!!!!!!!  读取的位置首先是ADSL
	DATAlist[0x41 + 2 * n] = ADCH;   //放入相关表内  second!!!!!
}

void ADC_GPIO_Init(void)
{
	//SFIOR |= (0x1<<PUD);
	//SFIOR = 00;
	DDRD |=		(0x1<<PD5)|(0x1<<PD4);
	PORTD |= ((0x1<<PD5)|(0x1<<PD4)); 
	
	PORTF &= ~((0x01<< PF7)|(0x01<<PF6)|(0x01<<PF5));	
	DDRF &= ~((0x01<< PF7) | (0x1<<PF6)|(0x01<<PF5));
	

}
/************************************************************************/
/* ACSR   --->模拟比较器控制和状态寄存器
*  ADMUX ---->ADC多工选择状态寄存器
*  ADCSRA --->ADC控制和状态寄存器
*  ADCL与ADCH --->数据寄存器
*  ADCSRB ---->多工选择状态寄存器
*                                                                     */
/************************************************************************/
void ADC_Mode_Init(void)
{
	ADCSRA = 0x00;		//ADC关闭  ADCRS模拟比较状态寄存器
	//ADMUX = 0x66;		//外部参考源AVCC,左对齐，选择ADC6通道
	ADMUX = 0x60;		//外部参考源AVCC,右对齐，选择ADC0通道
	//ADMUX = 0xE7;		//2.5v的片内基准电压源，左对齐，选择ADC7通道，如果AREF有电压的话不能使用内部基准电影源 
	//ADMUX = 0xE7;		//(1<<REFS0);//CHANAL0  0xC0|(1<<ADLAR);//E0
	ACSR = 0x80;		//模拟比较器控制和状态基准期ACSR的ACD值位，使模拟器比较器禁用
	ADCSRA |= (1 << ADEN) |(1 << ADPS2) | (1 << ADPS1);//使能AD转换，64分频
	//ADCSRA |= (1<< ADATE);//自动触发adc转换使能
	//ADCSRA |= (1<<ADIE); //ADC中断使能
	
   //ADCSR = 0x8B;		//开启ADC,8分频，ADC中断使能
   //ADCSRA |= (1 << ADSC);
}
void ADC_Init(void)
{
	ADC_GPIO_Init();
	ADC_Mode_Init();
}
/************************************************************************/
/* n : 通道                                                             */
/************************************************************************/

unsigned int adc_val = 0;
unsigned char adc_flag = 0;
unsigned int  ADC_Get_Int(unsigned char n)
{
	adc_val = 0;
	ADMUX = (ADMUX & 0xE0) | n;
	//_delay_ms(100);
	ADCSRA |= (1 << ADSC); //START 单次读取

	//_delay_ms(100);
	/*while(!(ADCSRA &(0x1<< ADIF)));*/
	_delay_ms(20);
	
	
	return adc_val;
}

unsigned int  ADC_Get(unsigned char n)
{
	ADCSRA &= ~(0x1<<ADEN);
	_delay_us(2);	
	ADMUX = (ADMUX & 0xE0) | n;
	_delay_us(2);
	ADCSRA |= (0x1<<ADEN)|(1 << ADSC); //START
	
	while(!(ADCSRA &(0x1<< ADIF)));
	//if((ADCSRA &(0x1<< ADIF)))BEEP(1);
	ADCSRA |= (0x1<< ADIF);
	_delay_us(2);	
	
	//LEDON;
	ADCSRA &= ~(0x1<<ADEN);
	//ADCSRA |= (0x1 << ADIF);
	return (ADCL | ADCH<<8);
}

SIGNAL(ADC_vect)
{
	//if((ADCSRA &(0x1<< ADIF)))
	{
		ADCSRA |= (0x1 << ADIF);
		adc_val = ADCL;
		adc_val |= ADCH<<8;
		if(ADMUX&(0x1<< ADLAR))
			adc_val >>= 6;
	}
	//BEEP(1);
	//LEDRUN;
	
	
	//ADCSRA |= (1 << ADSC);
}

int adc_main(void)
{
	cli();
	LED_Init();
	BEEP_Init();
	//SFIOR |= 0x1 << PUD;
// 	while(1)
// 	{
// 		_delay_ms(1000);
// 		DDRF = 0x00;
// 		PORTF = 0x00;
// 		LEDRUN;
// 	}
	//USART0_Init();
	ADC_Init();
	
	LCD_Init();
	sei();
	//WRITE_ALLRAM_1726(1);//全部显示
	//USART0_Transmit_String("ADC Init\r\n");
	//printf("ADC Init Successed!");
	unsigned char a[10];
	drp_num = 0;
	error_ico = 0;
	unsigned char q  = 0;
	BEEP(1);
	
	while(1)
	{
		_delay_ms(1000);
		
		LEDRUN;
	
	
			memset(a,0,10);

			itoa(a,	ADC_Get(5));
	
			pure_water_set(a);
			
			memset(a,0,10);

			itoa(a,	ADC_Get(7));
		
			raw_water_set(a);
		
		REFRESH_ALLRAM_1726();
		//BEEP(1);
	}
	return 0;
}