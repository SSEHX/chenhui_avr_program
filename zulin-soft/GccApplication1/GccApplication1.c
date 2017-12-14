/*
租赁饮水机


加200升 断网 限制
制水30分钟冲洗18s
1:2   1:2  1:2    1:1   1:1
5700 12000 12000 46000 12000
pp                      后缀
复位gprs 测试
TDS 测试
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/wdt.h>
#define	 F_CPU	11059200UL//16000000UL		// 单片机主频为7.3728MHz,用于延时子程序
#define baud 115200//38400//
#define uchar unsigned char
#define uint unsigned int
/*-----------------------------------------------------------------------
						端口初始化
-----------------------------------------------------------------------*/
#define TURE   1
#define FALSE 0
#define TINGD 1										//HT1622
/*-----------------------------------------------------------------------
						HT1721接口定义
-----------------------------------------------------------------------*/
#define HT1721C_DIO_1 PORTC|=(1<<PC2)				//dio
#define HT1721C_DIO_0 PORTC&=~(1<<PC2)
#define HT1721C_CLK_1 PORTC|=(1<<PC3)				//clk
#define HT1721C_CLK_0 PORTC&=~(1<<PC3)
#define HT1721C_STB_1 PORTC|=(1<<PC4)				//stb
#define HT1721C_STB_0 PORTC&=~(1<<PC4)
#define DISP_CON 0x92								//0x97
#define MODE_SET 0x08
#define HT1721C_BL_1 PORTE|=(1<<PE2)				//背光灯开
#define HT1721C_BL_0 PORTE&=~(1<<PE2)
#define LEDRUN PORTC^=(1<<PC0)						//指示灯闪烁

/*-----------------------------------------------------------------------
						TM1726接口定义
-----------------------------------------------------------------------*/
#define TM1726_SDA_1 PORTA|=(1<<PA0)				//dio
#define TM1726_SDA_0 PORTA&=~(1<<PA0)
#define TM1726_SCL_1 PORTA|=(1<<PA1)
#define TM1726_SCL_0 PORTA&=~(1<<PA1)

/*-----------------------------------------------------------------------
						开关设备接口定义
-----------------------------------------------------------------------*/
//#define UVON PORTB|=(1<<PB0)						//UV灯打开
//#define UVOFF PORTB&=~(1<<PB0)					//UV灯关闭

#define HEATBON PORTB|=(1<<PB7)						//A加热打开
#define HEATBOFF PORTB&=~(1<<PB7)					//A加热关闭

//#define HEATAON PORTB|=(1<<PB6)					//B加热打开
//#define HEATAOFF PORTB&=~(1<<PB6)					//B加热关闭

#define COOLON PORTG|=(1<<PG3)						//制冷打开
#define COOLOFF PORTG&=~(1<<PG3)					//制冷关闭

#define FA_BUSHUION PORTE|=(1<<PE3)					//补水阀打开
#define FA_BUSHUIOFF PORTE&=~(1<<PE3)				//补水阀关闭

#define FA_WARMON PORTE|=(1<<PE4)					//温水阀打开
#define FA_WARMOFF PORTE&=~(1<<PE4)					//温水阀关闭

#define FA_HOTON PORTE|=(1<<PE5)					//开水阀打开
#define FA_HOTOFF PORTE&=~(1<<PE5)					//开水阀关闭

#define FA_INWATERON PORTE|=(1<<PE7)				//进水阀打开
#define FA_INWATEROFF PORTE&=~(1<<PE7)				//进水阀关闭

#define FA_FLUSHON PORTE|=(1<<PE6)					//冲洗阀打开
#define FA_FLUSHOFF PORTE&=~(1<<PE6)				//冲洗阀关闭


/*-----------------------------------------------------------------------
							泵接口定义
-----------------------------------------------------------------------*/
#define BENG1ON PORTB|=(1<<PB1)						//泵1打开
#define BENG1OFF PORTB&=~(1<<PB1)					//泵1关闭
#define BENG2ON PORTB|=(1<<PB3)						//泵2打开
#define BENG2OFF PORTB&=~(1<<PB3)					//泵2关闭
//--------------------
#define BENGON PORTB|=(1<<PB2)						//泵2打开
#define BENGOFF PORTB&=~(1<<PB2)					//泵2关闭


/*-----------------------------------------------------------------------
							GPRS模块
-----------------------------------------------------------------------*/
#define RELOAD2_1 PORTA|=(1<<PA3)					//POWERKEY
#define RELOAD2_0 PORTA&=~(1<<PA3)					//POWERKEY
#define RESET_0 PORTA|=(1<<PA2)						//RESET
#define RESET_1 PORTA&=~(1<<PA2)					//RESET


#define	C_COMMAND1632_SYSEN	    0x01				//SYSTEM ENABLE
#define	C_COMMAND1632_SYSDIS    0x00				//SYSTEM DISABLE
#define	C_COMMAND1632_LEDON     0x03				//LED ON
#define	C_COMMAND1632_LEDOFF	0x02				//LED OFF
#define C_COMMAND1632_TIMEEN    0x06				//TIME BASE EN

#define BOMA1 (PINF&0x10)==0x10						//拨码第1位
//#define BOMA2 (PINF&0x20)==0x20					//拨码第2位
//#define BOMA3 (PINF&0x40)==0x40					//拨码第3位
#define KEY_SHIFT (PINB&0x01)						//切换按键
#define KEY_JIAN (PINB&0x02)						//切换按键
#define KEY_ENTER (PINB&0x04)						//切换按键
#define KEY_JIA (PINB&0x08)							//切换按键

//------------阀门输出-----PORTA------
#define SW_WATERIN			0						//进水阀
#define SW_FLUSH			1						//冲洗阀
#define SW_ICE				2						//冰水阀
#define SW_BANG				3
#define SW_COLD				4
#define SW_ADV				5
#define SW_UVLIGHT			6

//-------LED---PORTC--
#define LED_RUN				0
#define LED_COM				1
#define LED_LINK			2
#define LED_ALARM			3

/*-----------------------------------------------------------------------
							设备ID
-----------------------------------------------------------------------*/
#define IDDAT1				'B'		  		//设备码
#define IDDAT2				'1'		 		//设备码
#define IDDAT3				'1'				//设备码
#define IDDAT4				'1'		  		//设备码
#define IDDAT5				'1'		  		//设备码
#define IDDAT6				'1'		  		//设备码
#define IDDAT7				'3'		  		//设备码
#define IDDAT8				'6'		  		//设备码


/*-----------------------------------------------------------------------
							UART数据
-----------------------------------------------------------------------*/
unsigned char txtdat1[46];					//send
unsigned char rx1buffer[60];				//接收数据数组
unsigned char rx1count;						//接收数据1计时
unsigned char rx1poit=0;					//接收数据1指针
//----
unsigned char txtdat0[46];					//send
unsigned char rx0buffer[60];				//接收数据数组
unsigned char rx0count;						//接收数据1计时
unsigned char rx0poit=0;					//接收数据1指针

/*-----------------------------------------------------------------------
							LCD显示数组
-----------------------------------------------------------------------*/
uchar LCDNUM[32];


/*-----------------------------------------------------------------------
							数据初始化定义
-----------------------------------------------------------------------*/
unsigned char DATAlist[200];			//主从机通讯数据数组，刚开始需要初始化,到99
uchar headcount;						//发送数据头计数
uchar bklength;							//备份数据长度

uchar rechead[4];						//接收数据头
uchar reccount;							//接收计数
uchar sendcount;						//发送计数
uchar testtwi;							//测试TWI

/*-----------------------------------------------------------------------
							数码管段选？
-----------------------------------------------------------------------*/
uchar SMG[10]={0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,0xef,0xcf};		//10后
uchar SMGL[10]={0xfa,0x60,0xbc,0xf4,0x66,0xd6,0xde,0x70,0xfe,0xf6};		//数码管转换 abcd xfge

//----T0 TDS
uchar T1count;			//T1计数值
uchar T2count;			//T2计数值
uchar T0counter;
uchar T2Hcount;			//T2高位计数器
uchar ICETEMP;			//冰水温度
uint hotwater;			//下发开水总量
uint warmwater;			//下发温水总量
uint icewater;			//下发冰水总量

/*-----------------------------------------------------------------------
开关状态				1-开				0-关	
						1位开水				2位温水			3位冰水
						4位压缩机开关位		5位加热许可位（0许可，1禁止）
						6液晶显示切换位 时间 电话号码
-----------------------------------------------------------------------*/
uchar swbyte;		
	
/*-----------------------------------------------------------------------
取出了足够的水标志位	1是足够				0为-未足够
						1-开水				2-温水				3-冰水
-----------------------------------------------------------------------*/
uchar enoughwater;		//

/*-----------------------------------------------------------------------
							WiFi设置
-----------------------------------------------------------------------*/
char *ip;
char send3add[]="+++";
char sendeoff[]="AT+E=off";				// 设置STA
char sendsta[]="AT+WMODE=STA";			// 设置STA

//设置服务器  119.23.142.140
char sendser[]="AT+NETP=TCP,CLIENT,7170,wiot.chaction.cn";

char sendair[]="AT+SMTSL=air";
char sendquit[]="AT+ENTM";				//退出指令模式

/*-----------------------------------------------------------------------
							GPRS设置
-----------------------------------------------------------------------*/
char sendat[]="AT";
char sendhx[]="ATE0";					//关回显
char sendsim[]="AT+CCID";				//查是否插入SIM卡
char sendfu[]="AT+CGATT=1";				//附着网络
char sendpdp[]="AT+CGDCONT=1,IP,CMNET";	//设置PDP参数 21
char sendjh[]="AT+CGACT=1,1";			//激活PDP
char sendnet[]="AT+CIPSTART=TCP,wiot.chaction.cn,7170";
char sendtc[]="AT+CIPTMODE=1";			//透传
char sendreset[]="AT+RST=1";			//透传8

/*-----------------------------------------------------------------------
							标志位
-----------------------------------------------------------------------*/
uchar enbit=0;							//许能位bit：1-8：1-出水许能位 2-响蜂鸣器 3-保存工作模式 4-强行加热  5强行制冷 6--发热探头异常，禁止加热 7--冷异常，禁止制冷
//8-儿童保护状态下出水允许位
uchar waterstyle=0;						//出水种类
uchar workmode;							//工作模式
uint wificount=0;						//wifi是否有效计数
uchar minbit=0;							//分钟位 1-8:1-20s置位(18s,用作制水定时以及冲洗定时) 2--冲洗动作位
uchar count18s=0;						//18s计时
uchar timemode=0;						//定时模式，要保存
uchar timezhishui;						//制水30分钟冲洗计时
uchar timeflush;						//冲洗时间
//uchar Bupline;						//设定的加热温度与制冷温度
uchar ICEdownline;
/*-----------------------------------------------------------------------
							其他
-----------------------------------------------------------------------*/
uchar flatbit=0x00;//标志位，第一位发送令给普通板,第二位接收到普通板的指令，第三位wifi发送查询指令1有返回，0没返回；
//第四位表首地址有效位，有效才发送相关数据 第五位，wifi状态，1正常，0断网  第六位 安卓发送指令1需返回，0不用返回。
#define wififlag flatbit&0x10			//wifi是否有返回位
unsigned int loopnum=0;					//计数周期
//uchar step=0;							//步伐
uchar flushtime=0;						//冲洗时间
uchar uvstatus=0;						//uv灯状态  初始化
uint hotwaterbk=0;						//算流速记忆时间片
uint warmwaterbk=0;
uint icewaterbk=0;
uchar time1s=0;							//1秒时间计数
//uint liusu=0;							//流速数值
uint liuliang=0;						//流量数值
//uint liusubk1=0;
//uint liusubk2=0;
uchar keybit2=0;//按键出水位：1-8-：1-出热水 2-关热水 3-出温水 4-关热水 5-出冰水 6-关冰水 7-更新时钟位
uchar savebit=0;//需要保存的数据位：1-8:1-保存冲洗时间，2-定时模式，3-加热与制冷温度，4-滤芯额定值，5-加热时间段，6-制冷时间段，7-加热制冷时间段
//		8-复位滤芯
uchar savebit2=0;						//需要保存数据位：1-8:1-需要保存时间位，2-上送数据位 3-返回出水被占用
uchar enablebit=0;						//加热制冷许能位：
uchar dealbyte=0;						//需要处理的指令：
uchar timebyte[6];						//修改时钟用
uchar keytime=0;						//按键关屏时间
uchar delaytime=0;						//下发按键不取水延时处理时间
uchar needreturn=0;						//需要返回指令号，有返回则清楚，特别是05指令。
uchar waterid[12];						//12水单号
uchar fastatus=0x08;					//阀门的状态，1为打开，0为关闭：1-8：1开水阀  2温水阀 3冷水阀 4需要上传状态 5-需要保存热水流量 6-保存温水流量 7-保存冷水流量
uchar flushcont=0;						//制水多少冲洗一次 20为1升
uchar gprsstatus=0;						//GPRS状态
uchar sendbit=0;						//发送标志位  1--01指令，2--02指令 3--03 指令 4--04指令

//-------------------------------------------------------
unsigned char EEPROM_read(unsigned int uiAddress);

//---
uchar keycode[3];
void WRITE_1DATA_1721(unsigned char addr,unsigned char dat1);//修改一个字节
void REFRESH_ALLRAM_1721(void);//更新LCD
void BEEP(unsigned char sencond);//蜂鸣器

void INIT_1721(void);
void WRITE_DATA_1721(unsigned char wdata);
void WRITE_ALLRAM_1721(unsigned char F_RAM1632_SET);
void gprs_init(void);
void INIT_1726(void);
unsigned char EEPROM_read(unsigned int uiAddress);
void EEPROM_write(unsigned int uiAddress,unsigned char ucData);
void REFRESH_ALLRAM_1726(void);
void CRC16_rxt11(uchar addr1, uchar wDataLen1);
void send_reply(uchar resultnum);
void send_error(void);
void send_status(void);
void send_tds_temp_speed(void);
void CRC16_txt11(uchar addr, uchar wDataLen);
void WRITE_ALLRAM_1726(unsigned char F_RAM1632_SET);
void send_filter(void);
//-------------------------------------------------------
void port_init(void) //DDR 1-OUT,0-IN
{
	
	PORTC=0x01;		//数据			0000 0001	C1置高
	DDRC=0xbf;		//输入			1011 1111	C6输入
	
	PORTE=0xc4;		//				1000 0100
	DDRE=0xfe;		//				1111 1110
	
	
	PORTD=0xf3;  //6,7为T1,T2.  c3		1111 0011
	DDRD|=(1<<PD3)|0x03|0x30;		//	0011 1011
	//串口输出,TWI端口PD3--com1  0x03--iic   0x30--TDS
	
	
	PORTF=0x00;//PINF
	DDRF=0x00;
	
	PORTA=0xf3;//输出低电平 1c			1111 0011
	DDRA=0xef;//输出					1110 1111
	
	PORTB=0xf4;				//		1111 0100
	DDRB=0x3f;//输入触摸按钮			0011 1111
	
	PORTG=0x03;				//		0000 0011
	DDRG=0x08;				//		0000 1000
}
//------串口0发生数据--------
void putchar0(unsigned char data1)
{while(!(UCSR0A&(1<<UDRE0)));
	UDR0=data1;
}
//------串口1发送数据--------
void putchar1(unsigned char data1)
{while(!(UCSR1A&(1<<UDRE1)));
	UDR1=data1;
}
//--------------------
//------串口0初始化--------
//波特率9600 8，N,1
//-------------------------
void uart0_init(void)
{
	UCSR0B=0x00;
	UCSR0A=0x00;
	UCSR0C=0x06;				//						0000 0110
	UBRR0L=5;//(F_CPU/16/(baud+1)%256);
	UBRR0H=0;//(F_CPU/16/(baud+1)/256);
	UCSR0B=(1<<RXCIE0)|(1<<TXEN0)|(1<<RXEN0);//0x98
}
//------串口1初始化--------
//波特率9600 8，N,1
//-------------------------
void uart1_init(void)
{UCSR1B=0x00;
	UCSR1A=0x00;//倍速02
	UCSR1C|=0x06;//0x06
	UBRR1L=5;//(F_CPU/(16*baud)-1)%256; ok //(F_CPU/16/(baud+1)%256);8   5-11.0592
	UBRR1H=0;//(F_CPU/(16*baud)-1)/256; ok //(F_CPU/16/(baud+1)/256);0
	UCSR1B|=(1<<RXCIE1)|(1<<TXEN1)|(1<<RXEN1);//0x98
}
//------AD初始化--------
//8通道输入
//-------------------------
void adc_init(void)
{ADCSRA=0x00;//close
	ADMUX=0x60;//(1<<REFS0);//CHANAL0  0xC0|(1<<ADLAR);//E0
	ACSR=0x80;
	ADCSRA=ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);//使能AD转换，64分频
	// ADCSRA&=~(1<<ADIE); //禁止ADC中断
	
}
//------AD转换函数--------
//通道
//-----------------------
void ad_getdat(unsigned char n)//ADLR=1高位全满，余2位在低字节7、8位
{
	ADMUX=(ADMUX&0xe0)| n;
	_delay_ms(100);
	ADCSRA|=(1<<ADSC); //START
	while(!(ADCSRA&(1<<ADIF)));//WAIT
	ADCSRA|=(1<<ADIF);
	DATAlist[0x41+2*n+1]=  ADCL;   //first!!!!!!!!  读取的位置首先是ADSL
	DATAlist[0x41+2*n]=  ADCH;   //放入相关表内  second!!!!!
}

//--------------------
//------串口0接收完成中断--------
//接收数据存储，比较
//------------------------------
SIGNAL(SIG_UART0_RECV)
{	/*
unsigned char temp;
temp=UDR0; //存储接收数据
if(temp==0x88)
{putchar0(0x99);
}
*/
while( !(UCSR0A & (1<<RXC0)) );
rx0buffer[rx0poit]=UDR0;
rx0poit++;
rx0count=0;//接收计时
if(rx0poit>100)rx0poit=0;
}
//***********************************************************串口1接收中断start******************************************************************
//------串口1接收完成中断--------
//接收数据存储，比较
//------------------------------
SIGNAL(SIG_UART1_RECV)
{//unsigned char temp;
	//unsigned int i;
	
	while( !(UCSR1A & (1<<RXC1)) );
	rx1buffer[rx1poit]=UDR1;
	rx1poit++;
	rx1count=0;//接收计时
	if(rx1poit>100)rx1poit=0;
}

//发送数据
//---------------------------
void uart0_send0(unsigned char n)
{unsigned char i;
	for(i=0;i<n;i++)
	{
		putchar0(txtdat0[i]);
	}
}
//------串口0发送数据TXTDAT1的数据--------
//发送数据
//---------------------------
void uart0_send1(unsigned char n)
{unsigned char i;
	for(i=0;i<n;i++)
	{
		putchar0(txtdat1[i]);
	}
}

//------串口1发送数据--------
//发送数据
//---------------------------
void uart1_send(unsigned char n)
{unsigned char i;
	for(i=0;i<n;i++)
	{
		putchar1(txtdat1[i]);
	}
}

//***************************************************************************************************************
//  T0定时中断 比较流量是否已经够了  中断处理程序
//----------------------------------
SIGNAL(TIMER0_OVF_vect)
{   //uint num1;
	unsigned char w,k;
	
	TCNT0=0;               //重发初值，下次中断也10MS
	T0counter++;
	//PORTC^=(1<<LED_LINK);
	if(T0counter>20)//10-9   100
	{T0counter=0;
		time1s++;
		
		if(time1s>18)    //10-18s  18-1.5s  20-62s   1.5s   时间片检测
		{ // PORTG^=(1<<LED_LINK);
			time1s=0;
			LEDRUN;
			count18s++;
			if(count18s>13)//20s
			{
				minbit|=0x01;//置位20s
			}

		}	//1.5s
		
		//---------流量检测，在微信取水的时候检测--------------
		
		if(TCNT1>30)//开水总量已经达到了要求值，提交记录t1
		{
			fastatus|=0x10;//需要保存流量了
			flushcont++;
			if(flushcont>240)flushcont=240;//不再增加，等冲洗
		}

	}
	//*****************************中断接收1处理程序*************************************************
	rx1count++;
	if(rx1count>15)//处理数据 处理完rx1poit=0  &&(havedata==1)
	{rx1count=0;
		// PORTC^=(1<<LED_ALARM);
		if(rx1poit>16)
		{
			for(w=0;w<rx1poit-3;w++)
			{if((rx1buffer[w]==0xbb)&&(rx1buffer[w+1]==0x66)&&(rx1buffer[w+2]==0x66)&&(rx1buffer[w+3]==0x80))
				{
					//校验ID码
					wificount=0;
					LCDNUM[1]|=0x01;
					LCDNUM[2]|=0x01;
					LCDNUM[3]|=0x01;
					LCDNUM[4]|=0x01;
					LCDNUM[5]|=0x01;

					if((DATAlist[0x21]&0x80)==0x00)
					{
						DATAlist[0x21]|=0x80;
						fastatus|=0x08;
					}
					
					//-----CRC ---
					CRC16_rxt11(w,rx1buffer[w+5]);
					if((DATAlist[0x7a]==rx1buffer[w+rx1buffer[w+5]])&&(DATAlist[0x7b]==rx1buffer[w+rx1buffer[w+5]+1]))//CRC校验正确
					{
						
						//-----------------------串口1接收指令处理------------------------------
						if(rx1buffer[w+15]==0x01)sendbit&=0x01;
						if(rx1buffer[w+15]==0x02)sendbit&=0x02;
						if(rx1buffer[w+15]==0x03)sendbit&=0x04;
						if(rx1buffer[w+15]==0x04)sendbit&=0x08;
						if(rx1buffer[w+15]==0x05)
						{
							needreturn=0x00;
						}
						if(rx1buffer[w+15]==0x06)
						{ //BEEP(20);//wrong
							flatbit|=0x04;//有返回置位
							if((rx1buffer[w+16]>0)&&(rx1buffer[w+16]<0x2F))
							{ 	switch(rx1buffer[w+16])
								{
									case 0x01://开机
									HT1721C_BL_1;
									dealbyte|=0x01;
									break;
									case 0x02://关机
									HT1721C_BL_0;
									dealbyte|=0x02;
									break;
									case 0x03://冲洗
									dealbyte|=0x04;
									break;
									case 0x04://不用
									
									break;
									case 0x05://BUYONG

									
									break;
									case 0x06://复位滤芯至额定值
									if((rx0buffer[w+17]>0)&&(rx0buffer[w+17]<0x07))
									{
										DATAlist[rx1buffer[w+17]*2+0x22]=0;
										DATAlist[rx1buffer[w+17]*2+0x23]=0;
										DATAlist[rx1buffer[w+17]+0x19]=rx1buffer[w+20];
										dealbyte|=0x20;//需要保存
									}
									break;
									case 0x09://不用
									
									break;
									case 0x15://禁止机器
									if((rx1buffer[w+17]==0xaa)||(rx1buffer[w+17]==0xbb))
									{DATAlist[0x18]=rx1buffer[w+17];
										dealbyte|=0x80;
									}
									break;
									case 0x14://设置倒计时时钟
									DATAlist[0x5e]=rx1buffer[w+17];//
									DATAlist[0x5f]=rx1buffer[w+18];//
									dealbyte|=0x10;
									break;
									default:break;
								}
							}
						}
					}// CRC校验正确
					//-----------------------------------------------------------------
					//--------处理完毕
					w=rx1poit;
					rx1poit=0;
				}	//if
			}	//for
			
			w=rx1poit;
			rx1poit=0;
			
			
		} //rx1poit>6
		else
		{rx1poit=0;
		}
		
		
	}//rx1count>20
	//**********************************************************************************
	//**********************************************************************************
	
}

//-----------------------------------------------------------------------------------------------------
//---------------tc0定时初始化  ，1024分频，定时20ms------
void timer0_init(void)
{ TCCR0=0x00;                                        //stop
	TCNT0=0;                                         //产生10ms的中断初值
	OCR0=0xff;
	//	TCCR0=0x02;//(1<<CS02)|(1<<CS01)|(1<<CS00);               //1024分频，tc0工作在普通模式 ，开始工作
	TIMSK|=(1<<TOIE0);                                  //中断开
}
//--------------T1计数器初始化----16-----
void T1_init(void)
{ TCCR1A=0x00;
	TCCR1B=0x00;                                        //stop
	TCNT1=0;                                         //计数初值0
	OCR1A=0;
	OCR1B=0;
	OCR1C=0;//---test
	ICR1H=0xff;//test
	ICR1L=0xff;
	//	TCCR1A=0x00;
	//	TCCR1B=0x07;                                    //上升沿有效，t1工作普通模式，
	//TIMSK=(1<<TOIE1A);                                  //中断开
}
//--------------T2计数器初始化--8-------
void T2_init(void)
{
	TCCR2=0x00;                                        //stop
	TCNT2=0;                                         //计数初值0
	OCR2=0;
	//	TCCR2=0x07;                                    //上升沿有效，t2工作普通模式，
	TIMSK|=(1<<TOIE2);                                  //中断开
	T2Hcount=0;
}
//--------------T3定时初始化----16-----
void T3_init(void)
{   TCCR3A=0x00;
	TCCR3B=0x00;                                        //stop
	TCNT3H=0x00;    //F0                                     //计数初值0
	TCNT3L=0x00;    //7C
	TCNT3=0;
	OCR3A=0;
	OCR3B=0;
	OCR3C=0;
	ICR3H=0xff;
	ICR3L=0xff;
	TCCR3A=0x00;
	TCCR3B=0x00;    //03                                //上升沿有效，t1工作普通模式，1024分频
	//ETIMSK|=(1<<TOIE3);                                  //中断开
}

//-----------------------------------------------------------------------------------------------
//***********************************************************定时器3中断处理程序end**********************************************************************
//--------------------
//------系统初始化--------
void init_devices(void)
{
	cli();					//关中断
	XDIV=0x00;				//全速运行
	XMCRA=0x00;				//外部存储器控制
	port_init();
	uart0_init();
	uart1_init();
	adc_init();
	MCUCR=0x00;
	EICRA=0x00;
	EICRB=0x00;
	EIMSK=0x00;
	TIMSK=0x00;//中断T0
	ETIMSK=0x00;
	timer0_init();//时钟0初始化，定时
	T1_init();
	T2_init();
	T3_init();
	sei();
	
}
//*********************************************************************************************************************************
//*****************************************************主程序****************************************************************************
//--------------------
int main(void)
{
	unsigned int i;
	uchar temp1;
	uchar temp=0;
	uint  tempint1;
	uint  tempint2;//作为TDS与温度值寄存使用
	uchar iceuptemp,icedowntemp;//冰水上下温度
	uchar Bupline;
	uchar uvtime;  //uv灯时间
	uchar netofftime=0;//断网检测次数

	uchar LCDmode=0;          //显示模式
	//uchar flashaddr;        //液晶闪烁位置
	uchar LCDtemp[18];        //0 液晶闪烁位地址 1移动数据的长度 2闪烁间隔时间 3高位闪烁切换,
	uchar Bdownline;//B组加热上下限温度
	//uchar supertemp;//超高温温度线
	uchar heatontime,heatofftime;//间歇加热开启和关闭时间
	uchar ICEupline;//冰水制冷上下限温度
	uchar voicenum;//语音段的选择
	//uchar paikonghour,paikongmin;//隔夜排空时间设定值
	uchar yupulsh,yupulsl;//保存脉冲的余值
	uint thistimepuls=0;
	uchar pule1lh,pule1ll;//
	uint pule1l;
	uchar flushontime,flushofftime;//反冲洗开启关闭时间
	
	uchar keybyte;//取水按键寄存器,1-6bit为按钮保存位
	uchar LCD0loop=0;//LCD模式为0内的循环，更新变化显示用
	uchar JXloop;//间歇时间加热
	uchar getwatermode;//取水模式
	uchar iceonoff=4;//冰水阀压缩机传送字节
	uchar yasuoji=0;//压缩机发送字节
	uchar yasuojicount=0;//压缩机开关次数

	//---------------初始化数据-----------------用一个标志位，如果刚上电，则进行初始化
	//----2017----
	uchar setkeycount=0;//设置按钮计数
	uchar settype=0;//设置的类型  0-小时，1-分钟，2-加热温度，3--冷水的温度
	uchar databit=0;//第一位是闪烁需求位      LCDNUM,16,17,--地址 18,19 内容
	uchar flashcount=0;//闪烁频率
	uchar keybit=0;//按键取水位，取反作用
	uchar flashbit=0;//闪烁标志位：低到高位1-8：1-wifi  2-加热  3-制冷 4-车轮 5-笑脸 6-儿童保护
	uchar waterbit=0;//
	uint hot3min=0;
	uint warm3min=0;
	//uint cool3min=0;
	//uchar gaoshuiweic=0;//高水位蜂鸣器15s响3声
	uchar coolcount=0;//压缩机开关要3分钟后才行
	uchar setcount=0;//设置顺序值
	//uchar keyheatc=0;//强行转换加热
	//uchar keycoolc=0;//强行制冷转换
	uchar childlock;//儿童保护寄存器
	uchar childtime=0;//5s的保护时间
	uchar enterkeyc=0;//确认按键3s计时
	
	uchar timebit=0x01;//
	uchar bkhottemp=0; //备份是否一样，重新上传数据
	uchar bkcoldtemp=0;
	uchar tds1num=0;
	uchar tds2num=0;
	uchar setloop=0;//设置循环计数
	uchar sendtime=0;//发送频率设置
	//----------------------
	uchar wifibit=0x01;  //1-8:1--sendsetdat
	uchar loushuibk=250; //漏水了数值变小
	uchar sendpuls=0;//关机状态下发送状态脉冲
	uchar resetcount=0;
	uint bkliul=0;//备份流量，判断流量计故障
	uchar LLJcount=0;//流量计计数
	uchar tdscount=10;//启动TDS检测，制水后检测
	init_devices();
	_delay_ms(1000);
	// init_devices();
	PORTC&=0xfe;//led
	//BEEP(50);
	LCDmode=0;           //显示模式
	BEEP(100);
	RELOAD2_0;
	_delay_ms(500);
	RESET_0;
	_delay_ms(5000);
	RESET_1;
	RELOAD2_1;
	//-----------------------------------------------------------------
	//--主要数据合法性比较，在设置时也需要增加数据合法性的比较???
	//--------------------------------------------------------------
	//-----------------------------------------
	for(i=0;i<0x70;i++)  //将数据初始化
	{
		DATAlist[i]=0x00;
	}
	temp=EEPROM_read(9);  //
	if(temp==0xaa)//保存好的数据，不用初始化
	{
		DATAlist[0x18]=EEPROM_read(25);//保存的是是否禁止机器使用字节
		//Adownline=EEPROM_read(26);//A组加热高低温度
		Bupline=EEPROM_read(27);
		DATAlist[0x3b]=EEPROM_read(28);//制水总量高位
		DATAlist[0x3c]=EEPROM_read(29);//
		DATAlist[0x3d]=EEPROM_read(30);//模式3下的剩余水总量
		DATAlist[0x3e]=EEPROM_read(31);//
		ICEdownline=EEPROM_read(33);//冰水上下温度范围
		yupulsh=EEPROM_read(35);//剩余脉冲数保存，少于1升的脉冲数
		yupulsl=EEPROM_read(36);
		pule1lh=EEPROM_read(37);
		pule1ll=EEPROM_read(38);//1升脉冲数的高低字节
		DATAlist[0x5c]=pule1lh;
		DATAlist[0x5d]=pule1ll;
		pule1l=pule1lh*256+pule1ll;//总量
		flushontime=EEPROM_read(39);
		
		for(i=0;i<6;i++)//读出设定的额定寿命值
		{
			DATAlist[0x1a+i]=EEPROM_read(i+78);  //menmory to 84
		}
		for(i=0;i<12;i++)//读出滤芯使用的值
		{
			DATAlist[0x24+i]=EEPROM_read(i+86);  //menmory to 98
		}
		childlock=EEPROM_read(98);
		timemode=EEPROM_read(99);
		timezhishui=EEPROM_read(100);
		timeflush=EEPROM_read(113);  //冲洗时间

		DATAlist[0x5e]=EEPROM_read(115);
		DATAlist[0x5f]=EEPROM_read(116);//剩余时间
	}
	else//没初始化过数据
	{
		
		Bupline=95;Bdownline=75;							//上限温度  Bupline  2017
		DATAlist[0x18]=0xaa;//允许机器使用
		EEPROM_write(25,DATAlist[0x18]);
		EEPROM_write(27,Bupline);//EEPROM_write(28,Bdownline);
		DATAlist[0x3b]=0x00;//EEPROM_read(28);//制水总量高位
		DATAlist[0x3c]=0x00;//EEPROM_read(29);//
		DATAlist[0x3d]=0x00;//EEPROM_read(30);//模式3下的剩余水总量
		DATAlist[0x3e]=0x00;//EEPROM_read(31);//
		EEPROM_write(28,DATAlist[0x3b]);
		EEPROM_write(29,DATAlist[0x3c]);
		EEPROM_write(30,DATAlist[0x3d]);
		EEPROM_write(31,DATAlist[0x3e]);
		yupulsh=0;
		yupulsl=0;
		EEPROM_write(35,yupulsh);
		EEPROM_write(36,yupulsl);
		flushontime=10;
		EEPROM_write(39,flushontime);//冲洗时间的设定

		ICEdownline=15;
		EEPROM_write(33,ICEdownline);

		pule1lh=0x05;pule1ll=20;pule1l=1300;
		DATAlist[0x5c]=pule1lh;
		DATAlist[0x5d]=pule1ll;
		EEPROM_write(37,pule1lh);
		EEPROM_write(38,pule1ll);

		EEPROM_write(43,0xaa);
		EEPROM_write(9,0xaa);//保存成功标志

		
		DATAlist[0x1a]=15;EEPROM_write(78,15);//初始化滤芯额定值
		DATAlist[0x1b]=15;EEPROM_write(79,15);
		DATAlist[0x1c]=30;EEPROM_write(80,30);
		DATAlist[0x1d]=30;EEPROM_write(81,30);
		DATAlist[0x1e]=25;EEPROM_write(82,25);
		DATAlist[0x1f]=13;EEPROM_write(83,13);
		for(i=0;i<12;i++)			//menmory to 86+12==98  初始化滤芯计数
		{
			DATAlist[0x24+i]=0;
			EEPROM_write((86+i),DATAlist[0x24+i]);
		}
		childlock=0xaa;
		EEPROM_write(98,0xaa);		//儿童保护初始化
		timemode=0;					//定时工作模式关闭   menmory to 99
		EEPROM_write(99,0);
		timezhishui=30;
		EEPROM_write(100,30);			//制水30分钟计时  20s-30*60=1800/20=90   menmory to 100
		timeflush=18;
		EEPROM_write(113,18);		//制水30分钟冲洗18秒
		enablebit=0;
		EEPROM_write(114,0);			//强制加热或者制冷位    menmory to 114
		DATAlist[0x5e]=0;
		DATAlist[0x5f]=160;     //剩余天数
		EEPROM_write(115,DATAlist[0x5e]);
		EEPROM_write(116,DATAlist[0x5f]); //menmory to 116
	}
	DATAlist[0x19]=Bupline;
	
	keybyte=0xff;//可以进去按钮1,-6
	swbyte=0x00;//开关状态为关闭
	JXloop=0;//间歇时间
	enoughwater=0;//取出的水已经足够额度
	hotwater=0;
	warmwater=0;
	icewater=0;
	iceonoff=04;//冰水阀压缩机传送字节
	yasuoji=0;//压缩机
	yasuojicount=0;

	//----初始化时段----
	DATAlist[0x80]=EEPROM_read(52);//时间段是否设置了
	if(DATAlist[0x80]==0xaa)//有保存，读出时间段
	{
		for(i=0;i<24;i++)  //将数据初始化
		{
			DATAlist[0x81+i]=EEPROM_read(53+i); //memory to 77  53-77    101-112
		}
		for(i=0;i<12;i++)  //将数据初始化
		{
			DATAlist[0x99+i]=EEPROM_read(101+i); //memory to    101-112
		}
	}
	else   //没有设置时段初始值
	{
		for(i=0;i<18;i++)  //将数据初始化
		{
			DATAlist[0x82+(2*i)]=0;
		}
		DATAlist[0x81]=7;DATAlist[0x83]=9;
		DATAlist[0x85]=0x11;DATAlist[0x87]=0x13;
		DATAlist[0x89]=0x18;DATAlist[0x8b]=0x21;
		DATAlist[0x8d]=7;DATAlist[0x8f]=9;
		DATAlist[0x91]=0x11;DATAlist[0x93]=0x13;
		DATAlist[0x95]=0x18;DATAlist[0x97]=0x21;
		DATAlist[0x99]=7; DATAlist[0x9B]=9;
		DATAlist[0x9D]=0x11;DATAlist[0x9F]=0x13;
		DATAlist[0xA1]=0x18;DATAlist[0xA3]=0x21;
	}


	TCCR0=0x05;   //02开启TIMER0定时
	//------------------------------------
	//----------表的初始化---------------
	iceuptemp=EEPROM_read(0x04);//
	icedowntemp=EEPROM_read(0x05);//15
	if(iceuptemp>50)    //合法性
	{
		iceuptemp=20;
		EEPROM_write(0x04,iceuptemp);
	}
	if(icedowntemp>50)
	{
		icedowntemp=15;
		EEPROM_write(0x05,icedowntemp);
	}
	DATAlist[0x40]=0xaa;           //把表首地址置AA
	DATAlist[0]=0xaa;

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
	LCDNUM[5]=SMGL[IDDAT4-0x30];
	LCDNUM[6]=SMGL[IDDAT5-0x30];
	LCDNUM[7]=SMGL[IDDAT6-0x30];
	LCDNUM[8]=SMGL[IDDAT7-0x30];
	LCDNUM[9]=SMGL[IDDAT8-0x30];
	REFRESH_ALLRAM_1726();_delay_ms(10000);
	LCDNUM[5]=0;
	LCDNUM[6]=0;
	LCDNUM[7]=0;
	LCDNUM[8]=0;
	LCDNUM[9]=0;
	WRITE_ALLRAM_1726(0);
	//---------
	_delay_ms(2000);//20000
	//-------wifi initial-----------
	//wifi_init();
	
	//------------------
	//---按下设置 和 - 按键，进入滤芯寿命设置
	// read_3key();//use

	LCDNUM[13]=0x00;
	//-------------
	//------定时工作模式显示-------
	timebit=0x01;
	enterkeyc=10;

	//------------------------
	LCDmode=0;
	DATAlist[0x20]=0x01;
	DATAlist[0x21]=0;
	fastatus=0x08;
	if(DATAlist[0x18]==0xbb)
	{
		LCDmode=48;
		//HT1721C_BL_0;
		WRITE_ALLRAM_1726(0);
	}
	LCDNUM[0]=0x00;
	LCDNUM[1]=0x00;
	LCDNUM[2]=0x00;
	LCDNUM[3]=0x00;
	LCDNUM[4]=0x00;
	//------
	LCDNUM[0]|=0x01;
	tempint1=DATAlist[0x5e]*256+DATAlist[0x5f];
	temp=tempint1/10000;
	if(temp>0)LCDNUM[0]=(LCDNUM[0]&0x01)|SMGL[temp];
	tempint1=tempint1-(temp*10000);
	temp=tempint1/1000;
	if(temp>0)LCDNUM[1]=(LCDNUM[1]&0x01)|SMGL[temp];
	tempint1=tempint1-(temp*1000);
	temp=tempint1/100;
	if(temp>0)LCDNUM[2]=(LCDNUM[2]&0x01)|SMGL[temp];
	tempint1=tempint1-(temp*100);
	temp=tempint1/10;
	if(temp>0)LCDNUM[3]=(LCDNUM[3]&0x01)|SMGL[temp];
	tempint1=tempint1-(temp*10);
	temp=tempint1;
	LCDNUM[4]=(LCDNUM[4]&0x01)|SMGL[temp];
	DATAlist[0x09]=0x00;
	DATAlist[0x0a]=0x00;
	DATAlist[0x0b]=0x00;
	//-----
	TCNT1=0;
	TCCR1A=0x00; //00
	TCCR1B=0x07;  //07  启动T1计数
	flushontime=EEPROM_read(39);
	if(flushontime>0)
	{	BENGON;
		FA_INWATERON;
		FA_FLUSHON;
	}
	resetcount=194;
	tdscount=10;//TDS检测

	while(1)
	{
		_delay_us(302);
		loopnum++;
		if(loopnum>760)//50000---1000
		{loopnum=0;
			_delay_ms(20);

			//------------------保存下发的数据-----------------------------
			if(savebit!=0)
			{
				if((savebit&0x01)==0x01)//保存冲洗时间
				{
					EEPROM_write(100,timezhishui);//保存
					EEPROM_write(113,timeflush);//保存
					savebit&=0xfe;
				}
				if((savebit&0x08)==0x08)//保存额定滤芯值
				{
					EEPROM_write(78,DATAlist[0x1a]);
					EEPROM_write(79,DATAlist[0x1b]);
					EEPROM_write(80,DATAlist[0x1c]);
					EEPROM_write(81,DATAlist[0x1d]);
					EEPROM_write(82,DATAlist[0x1e]);
					EEPROM_write(83,DATAlist[0x1f]);
					savebit&=0xf7;
				}
				if((savebit&0x80)==0x80)//保存复位滤芯值
				{
					for(i=0;i<12;i++)  //将数据初始化
					{
						EEPROM_write((86+i),DATAlist[0x24+i]);
					}
					savebit&=0x7f;
				}
				//send0_reply(0);
				_delay_ms(200);
			}
			if((savebit2&0x01)==0x01)
			{
				//send0_reply(0);
				savebit2&=0xfe;
				_delay_ms(200);
			}

			

			if((dealbyte&0x10)==0x10)//设置剩余时间
			{   EEPROM_write(115,DATAlist[0x5e]);
				EEPROM_write(116,DATAlist[0x5f]);
				send_reply(0);
				dealbyte&=0xef;
				//------
				LCDNUM[0]=LCDNUM[0]&0x01;
				LCDNUM[1]=LCDNUM[1]&0x01;
				LCDNUM[2]=LCDNUM[2]&0x01;
				LCDNUM[3]=LCDNUM[3]&0x01;
				LCDNUM[4]=LCDNUM[4]&0x01;
				tempint1=DATAlist[0x5e]*256+DATAlist[0x5f];
				temp=tempint1/10000;
				if(temp>0)LCDNUM[0]=(LCDNUM[0]&0x01)|SMGL[temp];
				tempint1=tempint1-(temp*10000);
				temp=tempint1/1000;
				if(temp>0)LCDNUM[1]=(LCDNUM[1]&0x01)|SMGL[temp];
				tempint1=tempint1-(temp*1000);
				temp=tempint1/100;
				if(temp>0)LCDNUM[2]=(LCDNUM[2]&0x01)|SMGL[temp];
				tempint1=tempint1-(temp*100);
				temp=tempint1/10;
				if(temp>0)LCDNUM[3]=(LCDNUM[3]&0x01)|SMGL[temp];
				tempint1=tempint1-(temp*10);
				temp=tempint1;
				LCDNUM[4]=(LCDNUM[4]&0x01)|SMGL[temp];
				
			}
			

			//ONE----------------------------------------------------------------------------------
			//----------更新显示内容------------
			//----------------------------------------------------------------------------------
			if(LCDmode==0)
			{
				_delay_ms(2000);

				LCD0loop++;
				if(LCD0loop>8)LCD0loop=1;
				switch(LCD0loop)
				{   case 1://TDS
					//------水滴-----
					LCDNUM[12]|=0x10;
					LCDNUM[11]&=0xef;
					LCDNUM[7]&=0xfe;
					LCDNUM[6]&=0xfe;
					LCDNUM[15]&=0xef;
					LCDNUM[16]&=0xef;
					
					if(tdscount>0){ //TDS值，在制水的时候才检测
						tdscount--;
						PORTD&=~(1<<PD4);
						PORTD&=~(1<<PD5);
						_delay_ms(100);
						ad_getdat(6);//TDS值
						ad_getdat(7);//TDS值  运算
						PORTD|=(1<<PD4);
						PORTD|=(1<<PD5);
						tempint1=DATAlist[0x41+12]*256+DATAlist[0x41+13];
						tempint1=(65535-tempint1)/100;  //16
						tempint1=tempint1/2;
						DATAlist[0x41+12]=tempint1/256;
						DATAlist[0x41+13]=tempint1-(DATAlist[0x41+12]*256);
						tempint2=DATAlist[0x41+14]*256+DATAlist[0x41+15];
						tempint2=(65535-tempint2)/100;
						tempint2=tempint2/2;
						DATAlist[0x41+14]=tempint2/256;
						DATAlist[0x41+15]=tempint2-(DATAlist[0x41+14]*256);
						if(((bkhottemp!=DATAlist[0x44])||(bkcoldtemp!=DATAlist[0x46])||(tds1num!=tempint1)||(tds2num!=tempint2))&&(needreturn==0))//值发生变化，上传
						{	//                                                                                             消费记录已经发送完毕
							bkhottemp=DATAlist[0x44];
							bkcoldtemp=DATAlist[0x46];
							tds1num=tempint1;
							tds2num=tempint2;
							//if(resetcount<2)send_tds_temp_speed();
							sendtime=1;//?
						}
						//TDS值
						LCDNUM[10]|=0x10;//
						LCDNUM[17]=0x10;
						LCDNUM[13]&=0x10;
						
						temp=tempint1/100;
						if(temp>0)
						{
							LCDNUM[16]=(LCDNUM[16]&0x10)|SMG[temp];       //高4位
							// LCDNUM[10]=(LCDNUM[10]&0x8F)|((SMG[temp]&0x0f)<<4);//低4位
						}
						else
						{
							LCDNUM[16]&=0x10;
						}
						temp1=tempint1-(100*temp);//取后两位
						temp=temp1/10;
						LCDNUM[15]=(LCDNUM[15]&0x10) | SMG[temp];
						temp=temp1-(temp*10);
						LCDNUM[14]=(LCDNUM[14]&0x10) | SMG[temp];
						
						//纯水-----
						temp=tempint2/100;
						if(temp>0)
						{
							LCDNUM[12]=(LCDNUM[12]&0x10)|SMG[temp];
						}
						else
						{
							LCDNUM[12]&=0x10;
						}
						temp1=tempint2-(100*temp);//取后两位
						temp=temp1/10;
						LCDNUM[11]=(LCDNUM[11]&0x10)|SMG[temp];
						temp=temp1-(temp*10);
						LCDNUM[10]=(LCDNUM[10]&0x10)|SMG[temp];
					}
					break;
					case 2://设置wifi？
					//------水滴-----
					LCDNUM[12]&=0xef;
					LCDNUM[11]|=0x10;
					LCDNUM[7]&=0xfe;
					LCDNUM[6]&=0xfe;
					LCDNUM[15]&=0xef;
					LCDNUM[16]&=0xef;
					//---------
					
					sendtime++;
					if(sendtime>12) //8-12  18-32
					{sendtime=0;
					}
					if(sendtime==2)
					{

						//if(resetcount<2) send_tds_temp_speed();//如果在这里发送，则在不同那里不必再发送
						// _delay_ms(200);
						if(resetcount<2)
						{
							if((sendbit&0x01)==0x01)//01
							{
								send_status();
							}
							else if((sendbit&0x04)==0x04)//03
							{
								send_filter();
							}
							else if((sendbit&0x08)==0x08)//04
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
					if(sendtime==4)
					{
						_delay_ms(200);
						// send_status();
					}
					_delay_ms(10);
					if(sendtime==8)
					{
						if(((fastatus&0x08)==0x08)&&(needreturn==0)&&(resetcount<2))//有置位，记录发送完毕
						{  _delay_ms(100);
							fastatus&=0xf7;                   //哪里发生很多01数据过来？？？
							sendbit|=0x01;
							send_status();
						}
					}
					
					wificount++;
					if(wificount==50)//没有返回，则取消wifi图标
					{
						
						DATAlist[0x21]&=0x7f;
						fastatus|=0x08;//需要发送
						//LCDNUM[0]&=0xf3;//取消wifi图标
						LCDNUM[1]&=0xfe;
						LCDNUM[2]&=0xfe;
						LCDNUM[3]&=0xfe;
						LCDNUM[4]&=0xfe;
						LCDNUM[5]&=0xfe;
						//BEEP(200);
					}
					if(wificount>80) //重新设置wifi？   在此设置wifi重启  如果不是GPRS将此放大
					{
						wificount=0;
						resetcount=201;
					}
					break;
					
					case 3:
					//------水滴-----
					LCDNUM[12]&=0xef;
					LCDNUM[11]&=0xef;
					LCDNUM[7]|=0x01;
					LCDNUM[6]&=0xfe;
					LCDNUM[15]&=0xef;
					LCDNUM[16]&=0xef;
					//---------

					
					break;
					case 4://检查时间以及是否强制
					//------水滴-----
					LCDNUM[12]&=0xef;
					LCDNUM[11]&=0xef;
					LCDNUM[7]&=0xfe;
					LCDNUM[6]|=0x01;
					LCDNUM[15]&=0xef;
					LCDNUM[16]&=0xef;
					//---------显示流量--------------
					LCDNUM[8]|=0x01;//流量
					
					tempint1=DATAlist[0x3b]*256+DATAlist[0x3c];
					temp=tempint1/10000;
					LCDNUM[5]=(LCDNUM[5]&0x01)|SMGL[temp];
					tempint1=tempint1-(temp*10000);
					temp=tempint1/1000;
					LCDNUM[6]=(LCDNUM[6]&0x01)|SMGL[temp];
					tempint1=tempint1-(temp*1000);
					temp=tempint1/100;
					LCDNUM[7]=(LCDNUM[7]&0x01)|SMGL[temp];
					tempint1=tempint1-(temp*100);
					temp=tempint1/10;
					LCDNUM[8]=(LCDNUM[8]&0x01)|SMGL[temp];
					tempint1=tempint1-(temp*10);
					temp=tempint1;
					LCDNUM[9]=(LCDNUM[9]&0x01)|SMGL[temp];
					

					
					break;
					case 5://
					//------水滴-----
					LCDNUM[12]&=0xef;
					LCDNUM[11]&=0xef;
					LCDNUM[7]&=0xfe;
					LCDNUM[6]&=0xfe;
					LCDNUM[15]|=0x10;
					LCDNUM[16]&=0xef;
					//----------保存工作模式----
					if((enbit&0x04)==0x04)//需要保存工作模式
					{
						EEPROM_write(41,workmode);
						enbit&=0xfb;
					}
					//---------------------------------------
					//处理服务器发送过来的指令
					if((dealbyte&0x01)==0x01)//开机
					{
						send_reply(0);
						HT1721C_BL_1;
						LCDmode=0;
						dealbyte&=0xfe;
						//send_reply(0);
					}
					if((dealbyte&0x02)==0x02)//关机
					{
						send_reply(0);
						HT1721C_BL_0;
						LCDmode=42;
						dealbyte&=0xfd;
						//send_reply(0);
						FA_HOTOFF;
						FA_WARMOFF;
						//FA_COOLOFF;
						fastatus&=0xf8;//3个阀门的状态关闭
						FA_INWATEROFF;
						waterbit&=0xfb;//置位 关闭制水
						FA_FLUSHOFF;
						minbit&=0xfd;//关闭冲洗阀
						COOLOFF;
						coolcount=60;
						waterbit&=0xef;//置0，压缩机已关闭
						//HEATAOFF;
						waterbit&=0xdf;//置0，加热已关闭
						BENG1OFF;
						BENGOFF;
						fastatus&=0xf7;
						DATAlist[0x20]=0x00;
						DATAlist[0x21]=0x00;
						_delay_ms(1000);
						send_status();
						WRITE_ALLRAM_1726(0);
					}
					if((dealbyte&0x04)==0x04)//冲洗模式
					{
						send_reply(0);
						dealbyte&=0xfb;
						flushontime=EEPROM_read(39);
						BENGON;
						FA_INWATERON;
						FA_FLUSHON;
						DATAlist[0x21]|=0x10;
						_delay_ms(1000);
						sendbit|=0x01;
						send_status();
					}
					if(flushontime>0)
					{
						flushontime--;
						if(flushontime==0)
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
							DATAlist[0x21]&=0xef;
							_delay_ms(1000);
							//send_status();
							fastatus|=0x08;
						}
					}
					

					if((dealbyte&0x20)==0x20)//复位滤芯到设定值
					{	EEPROM_write(78,DATAlist[0x1a]);
						EEPROM_write(79,DATAlist[0x1b]);
						EEPROM_write(80,DATAlist[0x1c]);
						EEPROM_write(81,DATAlist[0x1d]);
						EEPROM_write(82,DATAlist[0x1e]);
						EEPROM_write(83,DATAlist[0x1f]);
						for(i=0;i<12;i++)  //保存复位滤芯值
						{
							EEPROM_write((86+i),DATAlist[0x24+i]);
						}
						send_reply(0);
						dealbyte&=0xdf;
					}

					if((dealbyte&0x80)==0x80)//禁止机器
					{   EEPROM_write(25,DATAlist[0x18]);
						send_reply(0);
						dealbyte&=0x7f;
						if(DATAlist[0x18]==0xbb)
						{
							//send_reply(0);
							HT1721C_BL_0;
							LCDmode=48;
							dealbyte&=0x7f;
							//send_reply(0);
							FA_HOTOFF;
							FA_WARMOFF;
							//FA_COOLOFF;
							fastatus&=0xf8;//3个阀门的状态关闭
							FA_INWATEROFF;
							waterbit&=0xfb;//置位 关闭制水
							FA_FLUSHOFF;
							minbit&=0xfd;//关闭冲洗阀
							COOLOFF;
							coolcount=60;
							waterbit&=0xef;//置0，压缩机已关闭
							//HEATAOFF;
							waterbit&=0xdf;//置0，加热已关闭
							BENG1OFF;
							BENGOFF;
							fastatus&=0xf7;
							DATAlist[0x20]=0x00;
							DATAlist[0x21]=0x00;
							_delay_ms(1000);
							send_status();
							WRITE_ALLRAM_1726(0);
						}
					}
					break;
					case 6:
					//------水滴-----
					LCDNUM[12]&=0xef;
					LCDNUM[11]&=0xef;
					LCDNUM[7]&=0xfe;
					LCDNUM[6]&=0xfe;
					LCDNUM[15]&=0xef;
					LCDNUM[16]|=0x10;
					if(((LCDNUM[1]&0x01)==0x01)&&((wifibit&0x01)==0x01)&&(resetcount<2))
					{_delay_ms(200);
						sendbit|=0x0d;
						wifibit&=0xfe;
						_delay_ms(2000);
						send_status();
						_delay_ms(2000);
						send_error();
						_delay_ms(2000);
						send_filter();
						_delay_ms(200);
						
					}
					//---------
					//------漏水-------
					/*
					if((PINF&0x20)==0x20)//没漏水
					{
					LCDNUM[13]&=0xef;
					}
					else
					{
					LCDNUM[13]|=0x10;
					}*/
					ad_getdat(5);//
					_delay_ms(100);
					if(DATAlist[0x41+10]>200)//gao,没漏水
					{
						LCDNUM[13]&=0xef;
						DATAlist[0x09]&=0xef;
					}
					else
					{    _delay_ms(300);
						ad_getdat(5);
						_delay_ms(100);
						if(DATAlist[0x41+10]<200) //再确认一下
						{
							LCDNUM[13]|=0x10;
							DATAlist[0x09]|=0x10;
							BEEP(50);
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
						}
						
					}
					if(((DATAlist[0x41+10]>200)&&(loushuibk<201))||((DATAlist[0x41+10]<201)&&(loushuibk>200))&&(resetcount<2))//发送漏水信息
					{
						loushuibk=DATAlist[0x41+10];
						sendbit|=0x08;
						send_error();//发送错误代码
						_delay_ms(200);
					}
					
					break;
					case 7:
					//---------------发生是否缺水的信息------------------------
					if((((DATAlist[0x0b]&0x02)==0x02)&&((timebit&0x10)==0x00))||(((DATAlist[0x0b]&0x02)==0x00)&&((timebit&0x10)==0x10)))//不同
					{   timebit^=0x10;
						sendbit|=0x08;
						send_error();
					}
					
					break;
					case 8:
					if(resetcount>0)
					{resetcount--;
						switch(resetcount)
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
							//gprs_init();
							rx1poit=0;
							for(ip=&sendhx[0];ip<(&sendhx[0]+4);ip++) //ATE0
							{
								putchar1(*ip);
							}
							putchar1(0x0d);
							putchar1(0x0a);
							//---wait reply--
							rx1buffer[1]=0x00;
							_delay_ms(200);
							rx1poit=0;
							for(temp=0;temp<100;temp++)
							{
								if(rx1buffer[rx1poit-3]==0x4F) temp=101;
								_delay_ms(150);
								//BEEP(100);
							}
							_delay_ms(100);
							break;
							case 170:
							//-----------------
							rx1poit=0;
							for(ip=&sendnet[0];ip<(&sendnet[0]+37);ip++) //
							{
								putchar1(*ip);
							}
							putchar1(0x0d);
							putchar1(0x0a);
							//---wait reply--
							rx1buffer[1]=0x00;
							_delay_ms(30000);
							_delay_ms(20000);
							_delay_ms(10000);
							_delay_ms(10000);
							
							for(temp=0;temp<200;temp++)
							{
								if(rx1buffer[rx1poit-3]==0x4f) temp=201;
								_delay_ms(200);
								//BEEP(100);
							}
							_delay_ms(1000);
							//---
							break;
							case 164:
							rx1poit=0;
							for(ip=&sendtc[0];ip<(&sendtc[0]+13);ip++) //
							{
								putchar1(*ip);
							}
							putchar1(0x0d);
							putchar1(0x0a);
							//---wait reply--
							rx1buffer[1]=0x00;
							_delay_ms(200);
							
							for(temp=0;temp<100;temp++)
							{
								if(rx1buffer[rx1poit-3]==0x4f) temp=101;
								_delay_ms(100);
								//BEEP(100);
							}
							_delay_ms(100);
							//---
							break;
							case 162:
							resetcount=0;
							sendtime=1;
							//BEEP(200);
							break;

							default:break;
						}
					}
					break;

					default:break;
				}
				//ONE.2----------------检测各个接口信号--------------------------------------------------------
				//-------制水检测---------
				if((flushontime==0)&&(LCDmode==0))
				{
					if((PINB&0x80)==0x80)//1 1
					{   LLJcount=0;
						LCDNUM[14]&=0xef;
						if((PINB&0x40)==0x00)//水满
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
							
							if(flushcont>200)//制水10升了，冲洗一次
							{
								flushcont=0;
								flushontime=EEPROM_read(39);
								BENGON;
								FA_INWATERON;
								FA_FLUSHON;
							}
							DATAlist[0x0b]&=0xfd;//有水
						}
						else//检修 1 0
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
						}
					}
					else   //0
					{
						if((PINB&0x40)==0x00)//制水 0 0 在没有漏水的情况下
						{   if((LCDNUM[13]&0x10)==0x00)//如果时间为0，则不运行制水了&&(DATAlist[0x5e]!=0x00)&&(DATAlist[0x5f]!=0x00)   没漏水
							{  if((DATAlist[0x5e]==0x00)&&(DATAlist[0x5f]==0x00))//剩余时间不为0
								{	_delay_ms(1);
								}
								else
								{//---制水------
									BENGON;
									FA_INWATERON;
									FA_FLUSHOFF;
									tdscount=6;
									LCDNUM[14]&=0xef;
									//-----	计数 后判断置位
									LLJcount++;
									if(LLJcount>60)
									{   LLJcount=0;
										tempint1=TCNT1;
										if(tempint1==bkliul)//流量没变
										{
											_delay_ms(3000);
											bkliul=TCNT1;
											if(tempint1==bkliul)//还是相等，流量没变化，流量计故障
											{   if((DATAlist[0x0b]&0x40)==0x00)//没置位
												{   DATAlist[0x0b]|=0x40;
													_delay_ms(300);
													sendbit|=0x08;
													send_error();
													//_delay_ms(300);
												}
											}
											else//不相等
											{
												bkliul=tempint1;
												if((DATAlist[0x0b]&0x40)==0x40)//置位
												{   DATAlist[0x0b]&=0xbf;
													_delay_ms(300);
													sendbit|=0x08;
													send_error();
													//_delay_ms(300);
												}
											}
										}
										else//不相等
										{
											bkliul=tempint1;
											if((DATAlist[0x0b]&0x40)==0x40)//置位
											{   DATAlist[0x0b]&=0xbf;
												_delay_ms(300);
												sendbit|=0x08;
												send_error();
												//_delay_ms(300);
											}
										}
									}
									//--------------
								}
								
							}
							DATAlist[0x0b]&=0xfd;//有水
						}
						else//缺水   01
						{   LLJcount=0;
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
							LCDNUM[14]|=0x10; //缺水
							DATAlist[0x0b]|=0x02;//缺水
						}
					}
				}//flushontime==0


				//---------------------保存流量----------------------剩余量比较是否过量？------------------------
				if((fastatus&0x10)==0x10)//需要保存本次出水流量值
				{   thistimepuls=TCNT1;
					TCNT1=0;
					fastatus&=0xef;//本次完成
					thistimepuls=yupulsh*256+yupulsl+thistimepuls;//加上余量
					temp=thistimepuls/pule1l;//除以1升的脉冲数
					yupulsh=(thistimepuls-temp*pule1l)/256;
					yupulsl=thistimepuls-temp*pule1l-yupulsl*256;
					EEPROM_write(35,yupulsh);
					EEPROM_write(36,yupulsl);//保存余量
					if(temp>0)//需要保存滤芯数值，总量
					{
						tempint1=DATAlist[0x24]*256+DATAlist[0x25]+temp; //滤芯1
						DATAlist[0x24]=tempint1/256;
						DATAlist[0x25]=tempint1-(DATAlist[0x24]*256);
						tempint1=DATAlist[0x26]*256+DATAlist[0x27]+temp; //滤芯2
						DATAlist[0x26]=tempint1/256;
						DATAlist[0x27]=tempint1-(DATAlist[0x26]*256);
						tempint1=DATAlist[0x28]*256+DATAlist[0x29]+temp; //滤芯3
						DATAlist[0x28]=tempint1/256;
						DATAlist[0x29]=tempint1-(DATAlist[0x28]*256);
						tempint1=DATAlist[0x2A]*256+DATAlist[0x2B]+temp; //滤芯4
						DATAlist[0x2A]=tempint1/256;
						DATAlist[0x2B]=tempint1-(DATAlist[0x2A]*256);
						tempint1=DATAlist[0x2C]*256+DATAlist[0x2D]+temp; //滤芯5
						DATAlist[0x2C]=tempint1/256;
						DATAlist[0x2D]=tempint1-(DATAlist[0x2C]*256);
						tempint1=DATAlist[0x2E]*256+DATAlist[0x2F]+temp; //滤芯6
						DATAlist[0x2E]=tempint1/256;
						DATAlist[0x2F]=tempint1-(DATAlist[0x2E]*256);
						for(i=0;i<12;i++)			//menmory to 86+12==98  保存数据
						{
							EEPROM_write((86+i),DATAlist[0x24+i]);
						}
						tempint1=DATAlist[0x3b]*256+DATAlist[0x3c]+temp;
						DATAlist[0x3b]=tempint1/256;
						DATAlist[0x3c]=tempint1-(DATAlist[0x3b]*256);

						tempint1=DATAlist[0x3d]*256+DATAlist[0x3e];//+temp;
						if(tempint1>temp)
						{
							tempint1=tempint1-temp;
						}
						else
						{
							tempint1=0;
						}
						DATAlist[0x3d]=tempint1/256;
						DATAlist[0x3e]=tempint1-(DATAlist[0x3d]*256);
						EEPROM_write(28,DATAlist[0x3b]);
						EEPROM_write(29,DATAlist[0x3c]);
						EEPROM_write(30,DATAlist[0x3d]);
						EEPROM_write(31,DATAlist[0x3e]);
						//----发送数据-----
						_delay_ms(100);
						sendbit|=0x04;
						send_filter();
					}
				}


				//ONE.22---
				if(LCDmode==0) REFRESH_ALLRAM_1726();
				//HT1721C_STB_1;
				//----是否需要闪烁------
				flashcount++;
				if(flashcount>6)flashcount=0;
				if((databit&0x01)==0x01)
				{
					
					if(flashcount>3)
					{
						WRITE_1DATA_1721(LCDNUM[16],LCDNUM[18]);
						WRITE_1DATA_1721(LCDNUM[17],LCDNUM[19]);
					}
				}
				if(flashcount>3)//flash
				{
					if((flashbit&0x01)==0x01)//wifi
					{
						
					}
					if((flashbit&0x02)==0x02)//加热
					{
						WRITE_1DATA_1721(12,LCDNUM[12]&0xf7);
					}
					if((flashbit&0x04)==0x04)//制冷
					{
						WRITE_1DATA_1721(13,LCDNUM[13]&0xf7);
					}
					if((flashbit&0x08)==0x08)//车轮
					{
						
					}
					if((flashbit&0x10)==0x10)//笑脸
					{
					}
					if((flashbit&0x20)==0x20)//笑脸+儿童保护
					{
						
					}
					
				}
			} //LCDMODE==0
			//----------------------------------LCDMODE==0 UP----------------------------------------------18260



			if(LCDmode==42)
			{
				_delay_ms(1000);
				if((dealbyte&0x01)==0x01)//开机
				{
					send_reply(0);
					HT1721C_BL_1;
					LCDmode=0;
					dealbyte&=0xfe;
					
					DATAlist[0x20]=0x01;
					DATAlist[0x21]=0x00;
					fastatus|=0x08;//发送一次开机状态
					_delay_ms(1000);
					send_status();
					_delay_ms(1000);
					BEEP(50);
				}
				sendpuls++;
				if(sendpuls>60)//200-22s
				{
					send_status();
					sendpuls=0;
					//BEEP(20);
				}
			}

			if(LCDmode==48)
			{
				_delay_ms(3000);
				if(DATAlist[0x18]==0xaa)//许能否？
				{
					send_reply(0);
					HT1721C_BL_1;
					LCDmode=0;
					EEPROM_write(25,DATAlist[0x18]);
					dealbyte&=0x7f;
					
					DATAlist[0x20]=0x01;
					DATAlist[0x21]=0x00;
					fastatus|=0x08;//发送一次开机状态
					temp=100;
					_delay_ms(3000);
					BEEP(50);
				}

			}

			
			//SIX--------------------------------------------------------------------------------------------
			//复位滤芯
			//---------------------------------------------------------------------------------------
			if(LCDmode==46)// 判断拨码是否复位回去 !=0    ((LCDmode==200)&&(LCDmode<30))//
			{
				LCDtemp[2]++;
				if(LCDtemp[2]>8)
				{   WRITE_1DATA_1721(9,0x00);
					WRITE_1DATA_1721(8,0x02);
					LCDtemp[2]=0;
					if(LCDtemp[3]&0x80)//---熄灭
					{
						LCDtemp[3]&=0x7f;//
						WRITE_1DATA_1721(6,0);
						WRITE_1DATA_1721(5,0);
						WRITE_1DATA_1721(7,0);
						//WRITE_1DATA_1721(6,LCDtemp[10]&0x10);
						//WRITE_1DATA_1721(5,LCDtemp[11]%0x1f);
						//WRITE_1DATA_1721(7,LCDtemp[13]&0xf0);
					}
					else
					{
						LCDtemp[3]|=0x80;
						//冷水//取后两位
						LCDtemp[10]=0;
						LCDtemp[11]=0;
						LCDtemp[13]=0;
						temp=LCDtemp[4]/10;
						LCDtemp[10]=(SMGL[temp]&0xf0)>>4;
						LCDtemp[11]=(SMGL[temp]&0x0f)<<4;
						//LCDtemp[10]=(LCDtemp[10]&0xf0)|((SMGL[temp]&0xf0)>>4);
						//LCDtemp[11]=(LCDtemp[11]&0x1F)|((SMGL[temp]&0x0f)<<4);
						temp=LCDtemp[4]-(temp*10);
						LCDtemp[13]=(SMGL[temp]&0xf0)>>4;
						LCDtemp[10]=(LCDtemp[10]&0x1f)|((SMGL[temp]&0x0f)<<4);
						//LCDtemp[13]=(LCDtemp[13]&0xF0)|((SMGL[temp]&0xf0)>>4);
						//LCDtemp[10]=(LCDtemp[10]&0x1f)|((SMGL[temp]&0x0f)<<4);
						WRITE_1DATA_1721(6,LCDtemp[10]);
						WRITE_1DATA_1721(5,LCDtemp[11]);
						WRITE_1DATA_1721(7,LCDtemp[13]);
						
						switch(LCDtemp[4])
						{   case 1://0
							WRITE_1DATA_1721(13,0x20);
							WRITE_1DATA_1721(14,0x00);
							break;
							case 2://0
							WRITE_1DATA_1721(13,0x00);
							WRITE_1DATA_1721(14,0x01);
							break;
							case 3://0
							WRITE_1DATA_1721(13,0x00);
							WRITE_1DATA_1721(14,0x02);
							break;
							case 4://0
							WRITE_1DATA_1721(13,0x40);
							WRITE_1DATA_1721(14,0x00);
							break;
							case 5://0
							WRITE_1DATA_1721(13,0x00);
							WRITE_1DATA_1721(14,0x04);
							break;
							case 6://0
							WRITE_1DATA_1721(13,0x00);
							WRITE_1DATA_1721(14,0x08);
							break;
							default:break;
						}
					}
				}
				//-------------------
				//------------加1按钮-------------------
				if(keycode[0]==0x08)
				{   BEEP(10);
					_delay_ms(2000);
					LCDtemp[3]&=0x7f;
					LCDtemp[2]=9;
					LCDtemp[4]++;
					if(LCDtemp[4]>6)LCDtemp[4]=1;
				}
				//--------------------------
				//------------减1按钮-------------------
				if(keycode[0]==0x80)
				{   BEEP(10);
					_delay_ms(2000);
					LCDtemp[3]&=0x7f;
					LCDtemp[2]=9;
					if(LCDtemp[4]==1)LCDtemp[4]=7;
					LCDtemp[4]--;
				}
				//------------设置退出按钮-------------------
				if(keycode[0]==0x04)
				{   BEEP(100);
					LCDmode=0;
				}

				//--------------------------
				//---------确定按钮-----保存上次的数据，进入下一次模式----------
				if(keycode[1]==0x08)//改变LCDmode
				{  BEEP(100);
					_delay_ms(2000);
					setcount++;
					if(setcount>6) setcount=0;//16
					LCDtemp[3]&=0x7f;
					LCDtemp[2]=9;
					// putchar1(0x55);
					// putchar1(LCDtemp[4]);
					// putchar1(DATAlist[0x1A+setcount]);
					//WTN3_MUSIC(LCDmode-1);//test
					temp=LCDtemp[4];
					DATAlist[0x22+2*temp]=0;
					DATAlist[0x23+2*temp]=0;
					EEPROM_write(2*temp+84,0);
					EEPROM_write(2*temp+85,0);
				}
			}		//36
			
		}	//loopnum
	}//while
}//


//*****************向EEPROM里面写入数据*****************
//输入量：地址，数据
//***************************************************
void EEPROM_write(unsigned int uiAddress,unsigned char ucData)
{
	while(EECR&(1<<EEWE));    //等待上一次写操作结束
	EEAR = uiAddress;     //地址
	EEDR = ucData;      //数据
	EECR |=(1<<EEMWE);     //置位EEMWE,主机写使能
	EECR |=(1<<EEWE);     //置位EEWE,写使能，启动写操作
}
//***********************************************************************
//              EEPROM写入字符串
//          addr：地址；number：长度；p_buff：写入数据存放指针
//***********************************************************************
void eprom_write(unsigned int addr, unsigned char number, unsigned char *p_buff)
{
	EEARH = 0x00;
	while(number --) {
		while(EECR & (1 << EEWE));       //等待前面的写完成
		EEARL = addr ++;                 //写地址
		EEDR = *p_buff ++;               //写数据到EEDR
		EECR |= (1 << EEMWE);            //主机写入允许位
		EECR&=~(1<<EEWE);
		EECR |= (1 << EEWE);             //EEWE为1，执行写操作
	}
}
//***********************************************************************
//                EEPROM读取单个字符
//           uiAddress：地址
//***********************************************************************

unsigned char EEPROM_read(unsigned int uiAddress)
{
	while(EECR&(1<<EEWE));  //等待上一次写操作结束
	EEAR = uiAddress;       //设置地址寄存器
	EECR |=(1<<EERE);       //读使能
	return EEDR;            //返回读入EEDR里面的数据
}

//***********************************************************************
//                EEPROM读取函数*/
//            addr：地址；number：长度；p_buff：读出数据存放指针
//***********************************************************************
void eprom_read(unsigned int addr, unsigned char number, unsigned char *p_buff)
{
	while(EECR & (1 << EEWE));               //等待前面的写完成
	EEARH = 0x00;                           //写高字节地址
	while(number --) {
		EEARL = addr ++;                 //写地址低字节
		EECR |= (1 << EERE);            //读允许位置1
		*p_buff++ = EEDR;               //读出EEDR中的数据
	}
}
//***********************************************************************
//                  发送设备状态函数
// 没返回
//***********************************************************************
void send_status(void)
{//uint crcresult;
	txtdat1[0]=0xAC;txtdat1[1]=0x33;txtdat1[2]=0x33;txtdat1[3]=0x80;
	txtdat1[4]=0x00;txtdat1[5]=0x12;
	txtdat1[6]=IDDAT1;txtdat1[7]=IDDAT2;txtdat1[8]=IDDAT3;txtdat1[9]=IDDAT4;
	txtdat1[10]=IDDAT5;txtdat1[11]=IDDAT6;txtdat1[12]=IDDAT7;txtdat1[13]=IDDAT8;
	txtdat1[14]=0x00;txtdat1[15]=0x01;txtdat1[16]=DATAlist[0x20];
	txtdat1[17]=DATAlist[0x21];
	//crcresult=CRC16_txt1(0,22);
	CRC16_txt11(0,18);
	txtdat1[18]=DATAlist[0x78];txtdat1[19]=DATAlist[0x79];
	txtdat1[20]=0xFC;txtdat1[21]=0xFF;
	uart1_send(22);
	_delay_ms(1000);

}

//-------------------------------------------------------------------------
//***********************************************************************
//                  发送设备TDS和冰水、温水、开水温度和流速函数
// 没返回
//***********************************************************************
void send_tds_temp_speed(void)
{txtdat1[0]=0xAC;txtdat1[1]=0x33;txtdat1[2]=0x33;txtdat1[3]=0x80;
	txtdat1[4]=0x00;txtdat1[5]=0x1A;
	txtdat1[6]=IDDAT1;txtdat1[7]=IDDAT2;txtdat1[8]=IDDAT3;txtdat1[9]=IDDAT4;
	txtdat1[10]=IDDAT5;txtdat1[11]=IDDAT6;txtdat1[12]=IDDAT7;txtdat1[13]=IDDAT8;
	txtdat1[14]=0x00;txtdat1[15]=0x02;
	txtdat1[16]=DATAlist[0x41+12];//DATAlist[0x51]; TDS1
	txtdat1[17]=DATAlist[0x41+13];//DATAlist[0x52];
	txtdat1[18]=DATAlist[0x41+14];//DATAlist[0x53]; TDS2
	txtdat1[19]=DATAlist[0x41+15];//DATAlist[0x54];
	txtdat1[20]=DATAlist[0x46];txtdat1[21]=0;txtdat1[22]=DATAlist[0x44];//冰水  温水  开水
	txtdat1[23]=0;txtdat1[24]=0;txtdat1[25]=0;//txtdat1[23]=DATAlist[7];txtdat1[24]=DATAlist[6];txtdat1[25]=DATAlist[5];
	CRC16_txt11(0,26);
	txtdat1[26]=DATAlist[0x78];txtdat1[27]=DATAlist[0x79];
	txtdat1[28]=0xFC;txtdat1[29]=0xFF;
	uart1_send(30);
	_delay_ms(100);
	uart0_send1(30);
}
//***********************************************************************
//                  发送滤芯数值
// 没返回
//***********************************************************************
void send_filter(void)
{txtdat1[0]=0xAC;txtdat1[1]=0x33;txtdat1[2]=0x33;txtdat1[3]=0x80;
	txtdat1[4]=0x00;txtdat1[5]=0x1A;
	txtdat1[6]=IDDAT1;txtdat1[7]=IDDAT2;txtdat1[8]=IDDAT3;txtdat1[9]=IDDAT4;
	txtdat1[10]=IDDAT5;txtdat1[11]=IDDAT6;txtdat1[12]=IDDAT7;txtdat1[13]=IDDAT8;
	txtdat1[14]=0x00;txtdat1[15]=0x03;
	txtdat1[16]=DATAlist[0x3b]; txtdat1[17]=DATAlist[0x3c];//制水总量
	txtdat1[18]=DATAlist[0x3d]; txtdat1[19]=DATAlist[0x3e];//限制制水总量
	txtdat1[20]=0x06;//滤芯数
	txtdat1[21]=DATAlist[0x24];txtdat1[22]=DATAlist[0x25];//1
	txtdat1[23]=DATAlist[0x26];txtdat1[24]=DATAlist[0x27];//2
	txtdat1[25]=DATAlist[0x28];txtdat1[26]=DATAlist[0x29];//3
	txtdat1[27]=DATAlist[0x2A];txtdat1[28]=DATAlist[0x2B];//4
	txtdat1[29]=DATAlist[0x2C];txtdat1[30]=DATAlist[0x2D];//5
	txtdat1[30]=DATAlist[0x2E];txtdat1[31]=DATAlist[0x2F];//6
	CRC16_txt11(0,32);
	txtdat1[32]=DATAlist[0x78];txtdat1[33]=DATAlist[0x79];
	txtdat1[34]=0xFC;txtdat1[35]=0xFF;
	uart1_send(36);
	_delay_ms(100);
	uart0_send1(36);
}
//-------------------------------------------------------------------------
//***********************************************************************
//                  发送设备故障代码函数   判别返回状态是否返回，返回
// 有返回指令 没指定
//***********************************************************************
void send_error(void)
{txtdat1[0]=0xAC;txtdat1[1]=0x33;txtdat1[2]=0x33;txtdat1[3]=0x80;
	txtdat1[4]=0x00;txtdat1[5]=0x0d;
	txtdat1[6]=IDDAT1;txtdat1[7]=IDDAT2;txtdat1[8]=IDDAT3;txtdat1[9]=IDDAT4;
	txtdat1[10]=IDDAT5;txtdat1[11]=IDDAT6;txtdat1[12]=IDDAT7;txtdat1[13]=IDDAT8;
	txtdat1[14]=0x00;txtdat1[15]=0x04;
	
	if(DATAlist[0x22]&0x04) //缺源水
	{
		DATAlist[0x64]|=0x08;//置1
	}
	else
	{
		DATAlist[0x64]&=0xf7; //置0
	}
	
	txtdat1[16]=DATAlist[0x09];
	txtdat1[17]=DATAlist[0x0a];
	txtdat1[18]=DATAlist[0x0b];
	CRC16_txt11(0,19);
	txtdat1[19]=DATAlist[0x78];txtdat1[20]=DATAlist[0x79];
	txtdat1[21]=0xFC;txtdat1[22]=0xFF;
	uart1_send(23);
	_delay_ms(100);
	//uart0_send1(23);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//***********************************************************************
//                  发送取设备指令函数
// 有返回指令
//***********************************************************************
void send_com(void)
{txtdat1[0]=0xAC;txtdat1[1]=0x33;txtdat1[2]=0x33;txtdat1[3]=0x80;
	txtdat1[4]=0x00;txtdat1[5]=0x0d;
	txtdat1[6]=IDDAT1;txtdat1[7]=IDDAT2;txtdat1[8]=IDDAT3;txtdat1[9]=IDDAT4;
	txtdat1[10]=IDDAT5;txtdat1[11]=IDDAT6;txtdat1[12]=IDDAT7;txtdat1[13]=IDDAT8;
	txtdat1[14]=0x00;txtdat1[15]=0x06;//txtdat1[16]=DATAlist[1];txtdat1[17]=DATAlist[2];
	CRC16_txt11(0,16);
	txtdat1[16]=DATAlist[0x78];txtdat1[17]=DATAlist[0x79];
	txtdat1[18]=0xFC;txtdat1[19]=0xFF;
	uart1_send(20);
}


//-------------------------------------------------------------------------
//***********************************************************************
//                  发送执行成功失败返回函数
//
//***********************************************************************
void send_reply(uchar resultnum)
{txtdat1[0]=0xAC;txtdat1[1]=0x33;txtdat1[2]=0x33;txtdat1[3]=0x80;
	txtdat1[4]=0x00;txtdat1[5]=0x0d;
	txtdat1[6]=IDDAT1;txtdat1[7]=IDDAT2;txtdat1[8]=IDDAT3;txtdat1[9]=IDDAT4;
	txtdat1[10]=IDDAT5;txtdat1[11]=IDDAT6;txtdat1[12]=IDDAT7;txtdat1[13]=IDDAT8;
	txtdat1[14]=0x00;txtdat1[15]=0x16;txtdat1[16]=resultnum;
	CRC16_txt11(0,17);
	txtdat1[17]=DATAlist[0x78];txtdat1[18]=DATAlist[0x79];
	txtdat1[19]=0xFC;txtdat1[20]=0xFF;
	uart1_send(21);
}

//-------------------------------------------------------------------------
/*********************************
功能：写一字节数据到HT1721C
*********************************/
void WRITE_DATA_1721(unsigned char wdata)
{
	unsigned char i,temp;
	
	temp = 0x01;  //80
	HT1721C_STB_0;
	_delay_us(TINGD);
	for(i=0 ; i<8 ; i++)
	{
		HT1721C_CLK_0;				//clk = 0 for data ready
		//_delay_us(TINGD);
		if((temp & wdata) == 0)
		{HT1721C_DIO_0;}
		else
		{HT1721C_DIO_1;}
		//		_nop_();
		_delay_us(TINGD);
		HT1721C_CLK_1;				//clk = 1 for data write into 1632
		wdata >>= 1;   //>>
		//_delay_us(TINGD);    //0317
	}
	//HT1721C_STB_1;//???
}
/*********************************
功能：填充HT1632C的RAM数据
*********************************/
void WRITE_ALLRAM_1721(unsigned char F_RAM1632_SET)
{
	unsigned char i;
	HT1721C_STB_1;
	// HT1721C_CLK_1;
	HT1721C_DIO_1;
	//WRITE_DATA_1721(0x08);
	HT1721C_STB_1;
	WRITE_DATA_1721(0x40);
	HT1721C_STB_1;
	WRITE_DATA_1721(0xc0);
	//HT1721C_STB_1;
	// all RAM have 96
	for(i=0;i<16;i++)
	{
		if(F_RAM1632_SET)
		{WRITE_DATA_1721(0xff);}	//all RAM write 1
		else
		{WRITE_DATA_1721(0x00);}	//all RAM write 0
	}
	HT1721C_STB_1;
	_delay_us(TINGD);
	WRITE_DATA_1721(0x97);
	HT1721C_STB_1;
}
/*********************************
功能：更新HT1632C的RAM数据
*********************************/
void REFRESH_ALLRAM_1721(void)
{
	unsigned char i;
	HT1721C_STB_1;
	//HT1721C_CLK_1;
	HT1721C_DIO_1;
	//WRITE_DATA_1721(0x08);
	HT1721C_STB_1;
	WRITE_DATA_1721(0x40);
	HT1721C_STB_1;
	WRITE_DATA_1721(0xc0);
	
	// all RAM have 96
	for(i=0;i<16;i++)
	{
		WRITE_DATA_1721(LCDNUM[i]);
	}
	HT1721C_STB_1;
	_delay_us(TINGD);
	WRITE_DATA_1721(0x97);
	HT1721C_STB_1;
	_delay_us(TINGD);
}
//*********************************
/*********************************
功能：初始化HT1632C
*********************************/
void INIT_1721(void)
{
	_delay_us(TINGD);
	HT1721C_STB_1;
	//HT1721C_CLK_1;
	HT1721C_DIO_1;
	
	WRITE_DATA_1721(0x08);//dis mode  0x08	MODE_SET
	HT1721C_STB_1;
	_delay_us(TINGD);
	WRITE_DATA_1721(0x97);//dis  0x97   0x92 DISP_CON
	HT1721C_STB_1;
	_delay_us(TINGD);

	
	//TM1721_Write_String(0,clear,11);

}

/*********************************
功能：填充HT1632C的固定地址和固定数据
*********************************/
void WRITE_1DATA_1721(unsigned char addr,unsigned char dat1)
{	HT1721C_STB_1;
	//HT1721C_CLK_1;
	HT1721C_DIO_1;
	//WRITE_DATA_1721(0x08);
	HT1721C_STB_1;
	WRITE_DATA_1721(0x44);
	HT1721C_STB_1;
	WRITE_DATA_1721(0xc0+addr);
	WRITE_DATA_1721(dat1);
	HT1721C_STB_1;
	_delay_us(TINGD);
	WRITE_DATA_1721(0x97);
	HT1721C_STB_1;

}



/*********************************
功能：蜂鸣器响声
*********************************/
void BEEP(unsigned char sencond)
{   uchar i;
	PORTB|=(1<<PB0);
	for(i=0;i<sencond;i++)
	{
		_delay_ms(20);
	}
	PORTB&=~(1<<PB0);
}

//***********************************************************************
//                  设置wifi联网
// //------wifi模块初始化--------
//设置wifi初始化参数
//模式，服务器，端口，通讯参数
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
unsigned int const TABLE1021[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108,
	0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231,
	0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339,
	0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462,
	0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a,
	0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653,
	0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b,
	0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4,
	0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc,
	0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5,
	0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd,
	0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6,
	0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae,
	0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97,
	0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f,
	0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188,
	0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080,
	0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83b9,
	0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1,
	0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea,
	0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2,
	0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db,
	0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3,
	0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 0xd94c,
	0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844,
	0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d,
	0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75,
	0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e,
	0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26,
	0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f,
	0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17,
};
void CRC16_txt11(uchar addr, uchar wDataLen)
{
	uchar chCRCHi = 0; // 高CRC字节初始化
	uint chCRC=0;
	uchar wIndex;            // CRC循环中的索引
	uchar n;
	for(n=0;n<wDataLen;n++)//while (wDataLen--)     // 计算CRC
	{
		chCRCHi=chCRC/256;
		wIndex = chCRCHi ^ txtdat1[n];
		chCRC<<=8;
		chCRC^=TABLE1021[wIndex];
	}
	//return ((chCRCHi << 8) | chCRCLo); chCRCLo;//chCRCHi;//
	DATAlist[0x79]=chCRC/256;
	DATAlist[0x78]=chCRC-(DATAlist[0x78]*256);
}
//*************************************************************************************************
void CRC16_rxt11(uchar addr1, uchar wDataLen1)
{
	uchar chCRCHi = 0; // 高CRC字节初始化
	uint chCRC=0;
	uchar wIndex;            // CRC循环中的索引
	uchar n;
	for(n=0;n<wDataLen1;n++)//while (wDataLen--)
	{ // putchar0(rx1buffer[n+addr1]);
		chCRCHi=chCRC/256;
		wIndex = chCRCHi ^ rx1buffer[n+addr1];
		chCRC<<=8;
		chCRC^=TABLE1021[wIndex];
	}
	//return ((chCRCHi << 8) | chCRCLo); chCRCLo;//chCRCHi;//
	DATAlist[0x7b]=chCRC/256;
	DATAlist[0x7a]=chCRC-(DATAlist[0x7b]*256);
}

//*************************************************************************************************
//*******************************TM1726************************************************************
//------------------------------------------------
//START TM1726
//-----------------------------------------------
void TM1726_start(void)
{
	TM1726_SDA_1;
	TM1726_SCL_1;
	_delay_us(10);
	TM1726_SDA_0;
	_delay_us(10);
	TM1726_SCL_0;
	_delay_us(10);
}
//------------------------------------------------
//STOP TM1726
//-----------------------------------------------
void TM1726_stop(void)
{
	TM1726_SDA_0;
	TM1726_SCL_1;
	_delay_us(10);
	TM1726_SDA_1;
	_delay_us(10);
	TM1726_SCL_0;
	_delay_us(10);
}
//-----------------------------------------------
//功能：写一字节数据到TM1726
//-----------------------------------------------
void WRITE_DATA_1726(unsigned char wdata)
{
	unsigned char i,temp;
	
	temp = 0x01;  //80
	TM1726_SDA_0;
	TM1726_SCL_0;
	_delay_us(TINGD);
	for(i=0 ; i<8 ; i++)
	{
		TM1726_SCL_0;
		if((temp & wdata) == 0)
		{TM1726_SDA_0;}
		else
		{TM1726_SDA_1;}
		_delay_us(TINGD);
		TM1726_SCL_1;				//clk = 1 for data write into 1632
		wdata >>= 1;   //>>
	}
	TM1726_SCL_0;
	TM1726_SDA_0;
}
//-----------------------------------------------
//功能：填充TM1726的RAM数据
//-----------------------------------------------
void WRITE_ALLRAM_1726(unsigned char F_RAM1632_SET)
{
	unsigned char i;
	TM1726_SDA_1;
	TM1726_SCL_1;
	TM1726_start();
	_delay_us(TINGD);
	WRITE_DATA_1726(0xc0);
	for(i=0;i<18;i++)
	{	TM1726_SDA_1;
		TM1726_SCL_1;
		if(F_RAM1632_SET)
		{WRITE_DATA_1726(0xff);}	//all RAM write 1
		else
		{WRITE_DATA_1726(0x00);}	//all RAM write 0
	}
	TM1726_stop();
}
//-----------------------------------------------
//功能：更新HT1632C的RAM数据
//-----------------------------------------------
void REFRESH_ALLRAM_1726(void)
{
	unsigned char i;
	TM1726_SDA_1;
	TM1726_SCL_1;
	TM1726_start();
	_delay_us(TINGD);
	WRITE_DATA_1726(0xc0);
	// all RAM have 96
	for(i=0;i<18;i++)
	{   TM1726_SDA_1;
		TM1726_SCL_1;
		WRITE_DATA_1726(LCDNUM[i]);
	}
	TM1726_stop();
}
//*********************************
//-----------------------------------------------
//功能：初始化TM1726
//-----------------------------------------------
void INIT_1726(void)
{
	TM1726_SDA_1;
	TM1726_SCL_1;
	TM1726_start();
	_delay_us(TINGD);
	WRITE_DATA_1726(0x37);//27
	TM1726_stop();
}
//-----------------------------------------------
//功能：填充TM1726的固定地址和固定数据
//-----------------------------------------------
void WRITE_1DATA_1726(unsigned char addr,unsigned char dat1)
{	TM1726_SDA_1;
	TM1726_SCL_1;
	TM1726_start();
	_delay_ms(10);
	WRITE_DATA_1726(0xc0+addr);//
	TM1726_SDA_1;
	TM1726_SCL_1;
	_delay_ms(10);
	WRITE_DATA_1726(dat1);
	TM1726_stop();

}
//-------------------------------------------
//***********************************************************************
void gprs_init(void)
{   uchar t;

	//-----

	rx1poit=0;
	for(ip=&sendhx[0];ip<(&sendhx[0]+4);ip++) //ATE0
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
		if(rx1buffer[rx1poit-3]==0x4F) t=101;
		_delay_ms(150);
		//BEEP(100);
	}
	_delay_ms(100);

	//-----------------
	rx1poit=0;
	for(ip=&sendnet[0];ip<(&sendnet[0]+37);ip++)
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(30000);
	_delay_ms(30000);
	_delay_ms(10000);
	_delay_ms(10000);
	
	for(t=0;t<200;t++)
	{
		if(rx1buffer[rx1poit-3]==0x4f) t=201;
		_delay_ms(1000);
		//BEEP(100);
	}
	_delay_ms(1000);
	//---

	rx1poit=0;
	for(ip=&sendtc[0];ip<(&sendtc[0]+13);ip++) //
	{
		putchar1(*ip);
	}
	putchar1(0x0d);
	putchar1(0x0a);
	//---wait reply--
	rx1buffer[1]=0x00;
	_delay_ms(200);
	
	for(t=0;t<100;t++)
	{
		if(rx1buffer[rx1poit-3]==0x4f) t=101;
		_delay_ms(100);
		//BEEP(100);
	}
	_delay_ms(100);
	//---
}