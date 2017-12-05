/*
 * CFile1.c
 *
 * Created: 2017/10/27 16:36:57
 *  Author: HXHZQ
 */ 

#include "time.h"

unsigned char T2Hcount;//T2高位计数器
//-----------------------------------------------------------------------------------------------------
//---------------tc0定时初始化  ，1024分频，定时20ms------
void timer0_init(void)
{
	TCCR0 = 0x00;      //停止定时器
	TCNT0 = 0;         //产生10ms的中断初值,OCR0输出比较寄存器
	OCR0 = 0xff;
	//	TCCR0=0x02;//(1<<CS02)|(1<<CS01)|(1<<CS00);               //1024分频，tc0工作在普通模式 ，开始工作
	TIMSK |= (1 << TOIE0);                                  //中断开
}
//--------------T1计数器初始化----16-----
void T1_init(void)
{
	TCCR1A = 0x00;
	TCCR1B = 0x00;                                        //stop
	TCNT1 = 0;                                         //计数初值0
	OCR1A = 0;
	OCR1B = 0;
	OCR1C = 0;//---test
	ICR1H = 0xff;//test
	ICR1L = 0xff;
	//	TCCR1A=0x00;
	//	TCCR1B=0x07;                                    //上升沿有效，t1工作普通模式，
								  //TIMSK=(1<<TOIE1A);                                  //中断开
}
//--------------T2计数器初始化--8-------
void T2_init(void)
{
	TCCR2 = 0x00;                                        //stop
	TCNT2 = 0;                                         //计数初值0
	OCR2 = 0;
	//	TCCR2=0x07;                                    //上升沿有效，t2工作普通模式，
	TIMSK |= (1 << TOIE2);                                  //中断开
	T2Hcount = 0;
}
//--------------T3定时初始化----16-----
void T3_init(void)
{
	TCCR3A = 0x00;
	TCCR3B = 0x00;                                        //stop
	TCNT3H = 0x00;    //F0                                     //计数初值0
	TCNT3L = 0x00;    //7C
	TCNT3 = 0;
	OCR3A = 0;
	OCR3B = 0;
	OCR3C = 0;
	ICR3H = 0xff;
	ICR3L = 0xff;
	TCCR3A = 0x00;
	TCCR3B = 0x00;    //03                                //上升沿有效，t1工作普通模式，1024分频
	//ETIMSK|=(1<<TOIE3);                                  //中断开
}



unsigned char time_10ms  = 0; 
unsigned int time_1s = 0;
unsigned char flow_ovf_flag = 0;//流量溢出，time1溢出中断
unsigned char time_chongxi = 6;//冲洗默认时间
unsigned int count_chongxi = 0;//冲洗的次数
unsigned int time_zhishui = 0;//制水时间
unsigned char send_flag = 0;
unsigned int send_time = 10;//发送的默认时间

#if 1
unsigned char error_parmeter[3]={0},error_flag = 0x00;//0:缺水,1:漏水,7:发送错误

//			处理的指令		制水的总时间	总流量(L)  		流量值          剩余流量	冲洗周期/5：5s冲洗一次
unsigned int deal_cmd = 0, sum_viuv_time = 0, sum_flow = 0, value_flow = 0, yu_sum_flow = 0,T_chongxi = 5;

unsigned char Filte_CntV[Filte_num * 2] = {0};
//保存滤芯的寿命（时间，流量）
unsigned char Iint_Filte_CntV[Filte_num * 2] = {13,12,13 ,32,12,32 };
//			剩余天数		流量				tds1			tds2				状态
unsigned char time[8] = { 0 }, flow[8] = { 0 }, tds1[4] = { 0 }, tds2[4] = { 0 }, status[4] = { 0 };
#endif


/*
*time0 是8位定时器
*/
void Time0_Init(void)// 10ms
{
	
	TCCR0 = 0x00; //初始化Time0控制寄存器
														//(0x94)
	TCNT0 = 0x94; //Time=PRE*(MAX-TCNT0+1)/F_cpu=1024*(255-148+1)/11.0592MHz=10ms
	//TCNT0 = 0x28;
	
	OCR0  = 0x95; //比较寄存器
	
	TCCR0 |= 0x07; //启动定时器，普通模式，1024分频，不与OC0连接
	
	TIMSK |= (0x01<<TOIE0)|(0x01<<OCIE0);//比较中断和溢出中断允许
	
}
/*
*time1 是16位定时器
*/
void Time1_Port(void)
{
	PORTD |= (0x1<<PD6);
	DDRD  &= ~(0x1<<PD6);
	
	//DDRB |= (0x01<<PB6);
	//PORTB &= ~(0x01<<PB6);
	//PORTB |= (0x01<<PB6);
	
}

void Time1_Config(void)
{
	TCCR1A = 0x00;//通道
	
//	TCCR1B |= 0x05;//时钟(1024分频)
	
//	TCCR1B |= (0x01<<CS12)|(0x01<<CS11)|(0x01<<CS10);//上升沿触发
	
	
 	OCR1A = 0;
 	OCR1B = 0;
 	OCR1C = 0;
	
	//Time=PRE*(MAX-TCNT0+1)/F_cpu=1024*(65535-148+1)/11.0592MHz=10ms
//	TCNT1H = 0xEC;
//	TCNT1L = 0x78;
	
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	
 	ICR1H = 0xFF;
 	ICR1L = 0xFF;
	
	TIMSK |= (0x01<< TOIE1);
	//ETIMSK |= 0x00;
}
void Time1_Init(void)
{
	Time1_Port();
	Time1_Config();
}
void Time1_Start(void)
{
	TCCR1A = 0x00;

	
	TCCR1B |= (0x01<<CS12)|(0x01<<CS11)|(0x01<<CS10);//上升沿触发
}
void Time1_Stop(void)
{
	TCCR1B &= ~((0x01<<CS12)|(0x01<<CS11)|(0x01<<CS10));//上升沿触发
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	flow_ovf_flag = 0;
}
unsigned int  Time1_Get_TCNT(void)
{
	unsigned int tmp = 0;
	tmp = TCNT1H;
	
	tmp <<= 8;
	tmp |= TCNT1L;
	//USART0_Transmit(tmp);
	//USART0_Transmit(tmp>>8);
	return TCNT1;
}
/*
*time2 是8位定时器
*/
void Time2_Init(void)//20ms
{
		
	TCCR2 = 0x00; //初始化Time0控制寄存器
														//(0x94)
	TCNT2 = 0x28; //Time=PRE*(MAX-TCNT0+1)/F_cpu=1024*(255-148+1)/11.0592MHz=10ms
	
	OCR2  = 0xFF; //比较寄存器
	
//	TCCR2 |= 0x05; //启动定时器，普通模式，1024分频，不与OC0连接
	
	//TIMSK |= (0x01<<TOIE2)|(0x01<<OCIE2);//比较中断和溢出中断允许
}

void Time2_Start(void)
{
	TCCR2 |= 0x05; //启动定时器，普通模式，1024分频，不与OC0连接
	TIMSK |= (0x01<<TOIE2)|(0x01<<OCIE2);//比较中断和溢出中断允许
	//LEDON;	
}

unsigned char Get_time2_Status(void)
{
	return (TCCR2 & 0x07);
}
void Time2_Stop(void)
{
	TCCR2 &= ~(0x05); //启动定时器，普通模式，1024分频，不与OC0连接
	TIMSK &= ~((0x01<<TOIE2)|(0x01<<OCIE2));//比较中断和溢出中断允许
	
}
/*
*time3 是16位定时器
*/
void Time3_Init(void)//1s延时
{
	TCCR3A = 0x00;//通道
	
//	TCCR3B |= 0x05;//时钟(1024分频)
	
// 	OCR1A = 0;
// 	OCR1B = 0;
// 	OCR1C = 0;
	
	//Time=PRE*(MAX-TCNT0+1)/F_cpu=1024*(65535-148+1)/11.0592MHz=10ms
//	TCNT3H = 0xFF;
//	TCNT3L = 0x94;
	
	TCNT3H = 0xFB;
	TCNT3L = 0xC8;
	
// 	ICR1H = 0xFF;
// 	ICR1L = 0xFF;
	
	//TIMSK |= (0x01<< TOIE1);
//	ETIMSK |= 0x01 << TOIE3;
}

void Time3_Start(void)
{
	TCCR3B |= 0x05;//时钟(1024分频)
	ETIMSK |= 0x01 << TOIE3;	
}
void Time3_Stop(void)
{
	TCCR3B &= ~(0x05);//时钟(1024分频)
	ETIMSK &= ~(0x01 << TOIE3);
	
}

unsigned char Get_time3_Status(void)
{
	return (TCCR3B & 0x07);
}




int time_main(void)
{
	cli();
	//ASSR |= (1<<AS0);
	//MCUCR  = 0x00;
	//MCUCSR = 0x80;//禁止JTAG
	//USART0_Init();
	LED_Init();
	LCD_Init();
	BEEP_Init();
	//Time0_Init();
	//Time2_Init();
	Time3_Init();
	Time3_Start();
	
	sei();
	//BEEP(20);
	//LEDON;	
	//USART0_Transmit_String("ok\r\n");
	
//	_delay_ms(1000);
	//BEEP(1);
		DDRB |=(0x01<<PB6);
	unsigned int count = 0,tmp = 0;
	unsigned char flaw[4] = {0};
		
	while(1)
	{
		if(!count)
		{
			count = 100;
		}
		_delay_ms(500);
		PORTB ^= (0x01<<PB6);
		tmp = Time1_Get_TCNT();
		//itoa(flaw,Time1_Get_TCNT());
	
	//	USART0_Transmit_nChar(flaw,2);
	itoa(flaw,tmp);
	flow_set(flaw);
	//USART0_Transmit_String(flaw);
	//USART0_Transmit_String("TCN1\r\n");
//	USART0_Transmit(tmp>>8);
//	USART0_Transmit(tmp);
	
		count  --;
 		//USART0_Transmit_String("TCNH\r\n");
   		//tmp =  TCNT1H;
   	//	USART0_Transmit(tmp);
   		//USART0_Transmit(TCNT1L);
   		//tmp = (tmp<<8)|TCNT1L;
			REFRESH_ALLRAM_1726();
		   
	}
	return 0;
}

//02085238246


SIGNAL(TIMER0_OVF_vect)//定时器T0溢出中断
{
	//BEEP(1);
	TCNT0 = 0x94;
	//TCNT0 = 0x28;	
	time_10ms ++;
	static unsigned char refresh_time = 0;
	
	//if(!(time_10ms%50))
			
	if(time_10ms>25)//500ms更新水滴
	{
		time_10ms = 0;
		TC1RUN;		
		if(refresh_time)
		{
			if(!(deal_cmd & 0x01 << 12))			
				drip();		
		}
		else
		{
			refresh_time = 6;
			if(!(deal_cmd & 0x01 << 12))
			{
					ico();
					signal();	
					REFRESH_ALLRAM_1726();
			}
			

		}
		refresh_time --;
		if(send_flag == 0)
			if(!(refresh_time))		//每隔一秒执行一次
			{
				if(!(time_1s % send_time))//设置发送的时间
					send_flag = 1;		  //发送的标志
				time_1s++;			
			}
	}
}

				
unsigned char zhishui_finish_flag = 0;

SIGNAL(TIMER0_COMP_vect)//定时器T0比较中断
{
#if 1
			wdt_reset();//定时500ms喂狗					
			
			//500是漏水灵敏度的设置,越小越灵敏
			if ((ADC_Get(5))< (0xFF60-300))
			{
				LCD_Ico |= 0x01 << 5;
			}
			else
			{
				LCD_Ico &= ~(0x01 << 5);
			}
			//关机的情况漏水会发送信息，可是没有显示
		if (deal_cmd & (0x01 << 12))//关机
			{
				HT1721C_BL_0;
				status[0] &= ~(0x1 << 0);
				close_chongxi();
				return ;
			}
			else
			{
				HT1721C_BL_1;												
				status[0] |= (0x1 << 0);
			}
			
			if (deal_cmd & (0x01 << 13))//停机
			{
				close_chongxi();
				return;
			}

			if (deal_cmd & (0x01 << 15))//租赁时间
			{
				close_chongxi();
				return ;
			}

	if(PINB & (0x01 << PB6))//缺水错误发送
	{
		LCD_Ico |= 0x01 << 4;
		close_chongxi();
	}
	else//	if (!(PINB & 0x01 << PB6))//低压检测
	{
		LCD_Ico &= ~(0x01 << 4);
		if (!(status[1] & (0x01 << 2)))
		{
			if ((deal_cmd & 0x01 << 14))//冲洗
			{
				Time2_Start();
				//冲洗操作
				//chongxi();		//PORTE |= (0x01 << PE7) | (0x01 << PE6);//电磁阀开
				//PORTB |= (0x01 << PB2);//水泵电机开启
				deal_cmd &= ~(0x01 << 14);
			}
		}
		if (Get_time2_Status())//冲洗状态
		{
			chongxi();
			status[1] |= 0x01 << 4;
		}
		else
		{
			status[1] &= ~(0x01 << 4);
			if ((PINB & 0x01 << PB7))//制水 流量机//  高压的检测 //制水状态
			{
			//			LEDON;
				PORTD &= ~((0x1 << PD5) | (0x1 << PD4));
				status[1] |= 0x01 << 2;
			//	time_zhishui = 0;
				Time1_Start();
				Time3_Start();
				zhishui();
				//LEDRUN;
				zhishui_finish_flag = 1;
			}
			else if(zhishui_finish_flag)
			{
				PORTD |= ((0x1 << PD5) | (0x1 << PD4));
				status[1] &= ~(0x01 << 2);//制水待机状态
				close_zhishui();
				//yu_sum_flow = 7;
				//value_flow = 24 + yu_sum_flow ;
				value_flow = Time1_Get_TCNT()+ yu_sum_flow ;
				sum_flow = sum_flow + value_flow / 10;
				yu_sum_flow = value_flow % 10;
			
				Time1_Stop();
				Time3_Stop();
				
							
				sum_viuv_time += time_zhishui;
				time_zhishui = 0;
	
				if (sum_viuv_time > T_chongxi * 2)//设定制水时间后冲洗
				{
					sum_viuv_time = 0;//
					deal_cmd |= (0x01 << 14);
				}
				zhishui_finish_flag = 0;
				
//				Write_VIUV_TIME_EEPROM(sum_viuv_time);
			}
		}
		
	}
#endif	
}


SIGNAL(TIMER1_OVF_vect)
{
	flow_ovf_flag ++;
}
SIGNAL(TIMER1_COMPA_vect)
{
	
}

SIGNAL(TIMER2_OVF_vect)//定时器T2溢出中断20ms
{
	//BEEP(1);
	TCNT2 = 0x28;
	static unsigned int time_20ms = 0;
	time_20ms ++;
	if(time_20ms > 50 * time_chongxi)//设置冲洗的时间
	{
			time_20ms = 0;			
			close_chongxi();
			Time2_Stop();
			count_chongxi ++;
		//	Write_COUNT_CHONGXI_EEPROM(count_chongxi);
			//LEDOFF;
			//deal_cmd &= ~(0x01 << 14);
	}
		//BEEP(1);
}
SIGNAL(TIMER2_COMP_vect)//定时器T2比较中断
{
	
	//USART0_Transmit(a);
	//TCNT0 = 0x00;
	//BEEP(20);
}

//制水的时候开启定时器3计算时间，检测tds1，tds2,制水的时间
SIGNAL(TIMER3_OVF_vect)//计算制水的时间100ms
{
	TCNT3H = 0xFB;
	TCNT3L = 0xC8;
	static unsigned char t_100ms = 0;
		t_100ms ++;
		if(t_100ms > 5)//制水时间————>500ms秒记一次
		{			
			t_100ms = 0;
			time_zhishui ++;
			
		}
		//100ms读取一次
		memset(flow, 0, 8);
		memset(tds1, 0, 4);
		memset(tds2, 0, 4);
		SET_FLOW(flow);//计数一下1ml,1000下为1L
		SET_TDS1(tds1);
		SET_TDS2(tds2);
			//LEDON;
		
		
}
SIGNAL(TIMER3_COMPA_vect)
{
	
}