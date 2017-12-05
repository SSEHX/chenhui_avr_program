/*
 * main.c
 *
 * Created: 2017/10/27 16:34:06
 *  Author: HXHZQ
 */


 /*
 *租赁饮水机
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

#define COOLON PORTG|=(1<<PG3)			//制冷打开
#define COOLOFF PORTG&=~(1<<PG3)		//制冷关闭
#define FA_BUSHUION PORTE|=(1<<PE3)			//补水阀打开
#define FA_BUSHUIOFF PORTE&=~(1<<PE3)		//补水阀关闭
#define FA_WARMON PORTE|=(1<<PE4)			//温水阀打开
#define FA_WARMOFF PORTE&=~(1<<PE4)		    //温水阀关闭
#define FA_HOTON PORTE|=(1<<PE5)			//开水阀打开
#define FA_HOTOFF PORTE&=~(1<<PE5)		    //开水阀关闭

#define BENG1ON PORTB|=(1<<PB1)				//泵1打开
#define BENG1OFF PORTB&=~(1<<PB1)			//泵1关闭
#define BENG2ON PORTB|=(1<<PB3)				//泵2打开
#define BENG2OFF PORTB&=~(1<<PB3)			//泵2关闭
//--------------------
#define BENGON PORTB|=(1<<PB2)				//泵2打开
#define BENGOFF PORTB&=~(1<<PB2)			//泵2关闭

#define FA_INWATERON PORTE|=(1<<PE7)		//进水阀打开
#define FA_INWATEROFF PORTE&=~(1<<PE7)		//进水阀关闭
#define FA_FLUSHON PORTE|=(1<<PE6)			//冲洗阀打开
#define FA_FLUSHOFF PORTE&=~(1<<PE6)		//冲洗阀关闭

#define RELOAD2_1 PORTA|=(1<<PA3)			//POWERKEY
#define RELOAD2_0 PORTA&=~(1<<PA3)			//POWERKEY
#define RESET_0 PORTA|=(1<<PA2)				//RESET
#define RESET_1 PORTA&=~(1<<PA2)			//RESET

#define HT1721C_BL_1 PORTE|=(1<<PE2)    //背光灯开
#define HT1721C_BL_0 PORTE&=~(1<<PE2)

//-----设备ID-------
#define IDDAT1 'B'		  		//设备码
#define IDDAT2 '1'		  		//设备码
#define IDDAT3 '1'		  		//设备码
#define IDDAT4 '1'		  		//设备码
#define IDDAT5 '1'		  		//设备码
#define IDDAT6 '1'		  		//设备码
#define IDDAT7 '3'		  		//设备码
#define IDDAT8 '6'		  		//设备码

typedef unsigned char uchar;
typedef unsigned char uint;


uchar ICEdownline;
uchar timemode = 0;//定时模式，要保存
uchar timezhishui;//制水30分钟冲洗计时
uchar timeflush;//冲洗时间
uchar enablebit = 0;//加热制冷许能位：
uchar swbyte;//开关状态，1-开，0-关；1位开水，2位温水，3位冰水，4位压缩机开关位，  5位加热许可位（0许可，1禁止）6液晶显示切换位 时间 电话号码
uchar fastatus = 0x08; //阀门的状态，1为打开，0为关闭：1-8：1开水阀  2温水阀 3冷水阀 4需要上传状态 5-需要保存热水流量 6-保存温水流量 7-保存冷水流量
uchar enoughwater;//取出了足够的水标志位，1是足够，0为-未足够1-开水；2-温水；3-冰水
uint hotwaterbk = 0; //算流速记忆时间片

uchar enbit = 0;//许能位bit：1-8：1-出水许能位 2-响蜂鸣器 3-保存工作模式 4-强行加热  5强行制冷 6--发热探头异常，禁止加热 7--冷异常，禁止制冷 
							  //8-儿童保护状态下出水允许位

uint hotwater; //下发开水总量
uint warmwater;//下发温水总量
uint icewater; //下发冰水总量

uchar savebit = 0;//需要保存的数据位：1-8:1-保存冲洗时间，2-定时模式，3-加热与制冷温度，4-滤芯额定值，5-加热时间段，6-制冷时间段，7-加热制冷时间段
uchar savebit2 = 0;//需要保存数据位：1-8:1-需要保存时间位，2-上送数据位 3-返回出水被占用

extern uchar enablebit;//加热制冷许能位：
uchar dealbyte = 0;//需要处理的指令
uchar needreturn = 0;//需要返回指令号，有返回则清楚，特别是05指令。
uchar sendbit = 0;	//发送标志位  1--01指令，2--02指令 3--03 指令 4--04指令
uchar minbit = 0; //分钟位 1-8:1-20s置位(18s,用作制水定时以及冲洗定时) 2--冲洗动作位

uchar workmode;//工作模式

unsigned int loopnum = 0;//计数周期
unsigned char DATAlist[200];
extern uint wificount;//wifi是否有效计数
extern char *ip;

//uchar SMG[10] = { 0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,0xef,0xcf };//10后
//uchar SMGL[10] = { 0xfa,0x60,0xbc,0xf4,0x66,0xd6,0xde,0x70,0xfe,0xf6 };//数码管转换 abcd xfge  
extern uchar SMG[10];
extern uchar SMGL[10];
uchar LCDNUM[32];        //LCD的显示数组

extern unsigned char txtdat1[46];  //send
extern unsigned char rx1buffer[60];//接收数据1数组
extern unsigned char rx1count;  //接收数据1计时
extern unsigned char rx1poit;   //接收数据1指针

//--------GPRS setting------------------

extern char sendtc[];//透传
extern char sendhx[];//关回显
extern char sendnet[];


extern unsigned char flushcont;

unsigned char EEPROM_read(unsigned int uiAddress);
//---
uchar keycode[3];
void WRITE_1DATA_1721(unsigned char addr, unsigned char dat1);//修改一个字节
void REFRESH_ALLRAM_1721(void);//更新LCD
void BEEP(unsigned char sencond);//蜂鸣器

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
void ad_getdat(unsigned char n);//ADLR=1高位全满，余2位在低字节7、8位
void putchar1(unsigned char data1);

//*********************************************************************************************************************************
//*****************************************************主程序****************************************************************************
//--------------------

/************************************************************************/
/*
冲洗时间,冲洗周期，发送时间在time.c文件中配置
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
		USART0_Init();//usart串口与time3冲突？？
			
	LED_Init();
	BEEP_Init();

	Time0_Init();//定时器1开启,喂看门狗	
	Time2_Start();//定时器2，开机冲洗6秒
	
	wdt_enable(WDTO_500MS); //开启看门狗	
	sei();

	//	WRITE_ALLRAM_1726(1);//全部显示

}
void  Data_Init(unsigned char *status, unsigned int *viuv_time, unsigned int *cmd)
{
	_delay_ms(300);

	//	LCD_Ico |= 0x0f;		
		//	WRITE_ALLRAM_1726(0);//
	memset(status, 0, 2);//初始化状态	
	unsigned char read_eeprom_buf[8] = { 0 };

	//LEDON;
	if (Check_Init())
	{
		//flow_set(read_eeprom_buf);

		//时间
		SET_TIME(Read_TIME_EEPROM());
		//流量
		sum_flow = Read_FLOW_EEPROM();
		SET_FLOW_L(sum_flow);


		//tds2 原水
		memset(read_eeprom_buf, 0, 8);
		Read_RAW_EEPROM(read_eeprom_buf);
		raw_water_set(read_eeprom_buf);
		//tds1 净水
		memset(read_eeprom_buf, 0, 8);
		pure_water_set(Read_PURE_EEPROM(read_eeprom_buf));
		memset(read_eeprom_buf, 0, 8);

		//机状态
		Read_STATUS_EEPROM(read_eeprom_buf);

		Str_Add_Str(status, 0, read_eeprom_buf, 2);
		memset(read_eeprom_buf, 0, 8);
		//处理命令
		Read_CMD_EEPROM(cmd);
		//剩余的流量
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

	//unsigned int sum_time = 0;//电磁阀工作的总时间

	unsigned char eeprom_write_flag = 0;
	unsigned char Parameter[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	//unsigned char Ack[22] = { 0 }, Ack_Len = 0;
	//deal_cmd = 0xFFFF;
	Data_Init(status, &sum_viuv_time, &deal_cmd);

	//LEDON;

//	_delay_ms(6000);
	//						关机			停机			冲洗		租赁时间
	deal_cmd = 0xFFF | (0x00 << 12) | (0x00 << 13) | (0x00 << 14) | (0x00 << 15);

	//总的电磁阀工作时间
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
		//网络的访问
//		//GPRS状态获
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
				//						关机			停机			冲洗		租赁时间
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

			if (deal_cmd & 0x01 << 12)//关机
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
			if (deal_cmd & (0x01 << 13))//停机
			{
				continue;
			}
			else
			{

			}
			if (deal_cmd & (0x01 << 15))//租赁时间
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
		if (Get_time3_Status())//在制水过程中断电无法保存数据
		{
			eeprom_write_flag = 1;
		}
		else if (eeprom_write_flag)
		{
			Write_YU_SUM_FLOW_EEPROM(yu_sum_flow);
			//sum_flow = 23;
			Write_FLOW_EEPROM(sum_flow);//总流量（L）

			Write_PURE_EEPROM(tds1);
			Write_RAW_EEPROM(tds2);
			Write_VIUV_TIME_EEPROM(sum_viuv_time);//一次制水还没有冲洗的时间(分钟)

			memset(Filte_CntV, 0, 2 * Filte_num);
			for (unsigned char i = 0; i < Filte_num; i++)
			{
				Filte_CntV[2 * i] += sum_flow << 8;
				Filte_CntV[2 * i + 1] += sum_flow;
				//检测滤芯寿命是否到期
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
	uint  tempint2;//作为TDS与温度值寄存使用
	uchar iceuptemp, icedowntemp;//冰水上下温度
	uchar Bupline;
	uchar uvtime;  //uv灯时间
	uchar netofftime = 0;//断网检测次数

	uchar LCDmode = 0;          //显示模式
	//uchar flashaddr;        //液晶闪烁位置
	uchar LCDtemp[18];        //0 液晶闪烁位地址 1移动数据的长度 2闪烁间隔时间 3高位闪烁切换,
	uchar Bdownline;//B组加热上下限温度
	//uchar supertemp;//超高温温度线
	uchar heatontime, heatofftime;//间歇加热开启和关闭时间
	uchar ICEupline;//冰水制冷上下限温度
	uchar voicenum;//语音段的选择
	//uchar paikonghour,paikongmin;//隔夜排空时间设定值
	uchar yupulsh, yupulsl;//保存脉冲的余值
	uint thistimepuls = 0;
	uchar pule1lh, pule1ll;//
	uint pule1l;
	uchar flushontime, flushofftime;//反冲洗开启关闭时间

	uchar keybyte;//取水按键寄存器,1-6bit为按钮保存位
	uchar LCD0loop = 0;//LCD模式为0内的循环，更新变化显示用
	uchar JXloop;//间歇时间加热
	uchar getwatermode;//取水模式
	uchar iceonoff = 4;//冰水阀压缩机传送字节
	uchar yasuoji = 0;//压缩机发送字节
	uchar yasuojicount = 0;//压缩机开关次数

	//---------------初始化数据-----------------用一个标志位，如果刚上电，则进行初始化
	//----2017----
	uchar setkeycount = 0;//设置按钮计数
	uchar settype = 0;//设置的类型  0-小时，1-分钟，2-加热温度，3--冷水的温度
	uchar databit = 0;//第一位是闪烁需求位      LCDNUM,16,17,--地址 18,19 内容
	uchar flashcount = 0;//闪烁频率
	uchar keybit = 0;//按键取水位，取反作用
	uchar flashbit = 0;//闪烁标志位：低到高位1-8：1-wifi  2-加热  3-制冷 4-车轮 5-笑脸 6-儿童保护  
	uchar waterbit = 0;//
	uint hot3min = 0;
	uint warm3min = 0;
	//uint cool3min=0;
	//uchar gaoshuiweic=0;//高水位蜂鸣器15s响3声
	uchar coolcount = 0;//压缩机开关要3分钟后才行
	uchar setcount = 0;//设置顺序值
	//uchar keyheatc=0;//强行转换加热
	//uchar keycoolc=0;//强行制冷转换
	uchar childlock;//儿童保护寄存器
	uchar childtime = 0;//5s的保护时间
	uchar enterkeyc = 0;//确认按键3s计时

	uchar timebit = 0x01;//
	uchar bkhottemp = 0; //备份是否一样，重新上传数据
	uchar bkcoldtemp = 0;
	uchar tds1num = 0;
	uchar tds2num = 0;
	uchar setloop = 0;//设置循环计数
	uchar sendtime = 0;//发送频率设置
   //----------------------
	uchar wifibit = 0x01;  //1-8:1--sendsetdat
	uchar loushuibk = 250; //漏水了数值变小
	uchar sendpuls = 0;//关机状态下发送状态脉冲
	uchar resetcount = 0;
	uint bkliul = 0;//备份流量，判断流量计故障
	uchar LLJcount = 0;//流量计计数
	uchar tdscount = 10;//启动TDS检测，制水后检测
	init_devices();
	_delay_ms(1000);
	// init_devices();
	PORTC &= 0xfe;//led
	//BEEP(50);
	LCDmode = 0;           //显示模式
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
	for (i = 0; i < 0x70; i++)  //将数据初始化
	{
		DATAlist[i] = 0x00;
	}
	temp = EEPROM_read(9);  //
	if (temp == 0xaa)//保存好的数据，不用初始化
	{
		DATAlist[0x18] = EEPROM_read(25);//保存的是是否禁止机器使用字节
		//Adownline=EEPROM_read(26);//A组加热高低温度
		Bupline = EEPROM_read(27);
		DATAlist[0x3b] = EEPROM_read(28);//制水总量高位
		DATAlist[0x3c] = EEPROM_read(29);//
		DATAlist[0x3d] = EEPROM_read(30);//模式3下的剩余水总量
		DATAlist[0x3e] = EEPROM_read(31);//
		ICEdownline = EEPROM_read(33);//冰水上下温度范围
		yupulsh = EEPROM_read(35);//剩余脉冲数保存，少于1升的脉冲数
		yupulsl = EEPROM_read(36);
		pule1lh = EEPROM_read(37);
		pule1ll = EEPROM_read(38);//1升脉冲数的高低字节
		DATAlist[0x5c] = pule1lh;
		DATAlist[0x5d] = pule1ll;
		pule1l = pule1lh * 256 + pule1ll;//总量
		flushontime = EEPROM_read(39);

		for (i = 0; i < 6; i++)//读出设定的额定寿命值
		{
			DATAlist[0x1a + i] = EEPROM_read(i + 78);  //menmory to 84
		}
		for (i = 0; i < 12; i++)//读出滤芯使用的值
		{
			DATAlist[0x24 + i] = EEPROM_read(i + 86);  //menmory to 98
		}
		childlock = EEPROM_read(98);
		timemode = EEPROM_read(99);
		timezhishui = EEPROM_read(100);
		timeflush = EEPROM_read(113);  //冲洗时间

		DATAlist[0x5e] = EEPROM_read(115);
		DATAlist[0x5f] = EEPROM_read(116);//剩余时间
	}
	else//没初始化过数据
	{

		Bupline = 95; Bdownline = 75;							//上限温度  Bupline  2017
		DATAlist[0x18] = 0xaa;//允许机器使用
		EEPROM_write(25, DATAlist[0x18]);
		EEPROM_write(27, Bupline);//EEPROM_write(28,Bdownline);
		DATAlist[0x3b] = 0x00;//EEPROM_read(28);//制水总量高位
		DATAlist[0x3c] = 0x00;//EEPROM_read(29);//
		DATAlist[0x3d] = 0x00;//EEPROM_read(30);//模式3下的剩余水总量
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
		EEPROM_write(39, flushontime);//冲洗时间的设定

		ICEdownline = 15;
		EEPROM_write(33, ICEdownline);

		pule1lh = 0x05; pule1ll = 20; pule1l = 1300;
		DATAlist[0x5c] = pule1lh;
		DATAlist[0x5d] = pule1ll;
		EEPROM_write(37, pule1lh);
		EEPROM_write(38, pule1ll);

		EEPROM_write(43, 0xaa);
		EEPROM_write(9, 0xaa);//保存成功标志 


		DATAlist[0x1a] = 15; EEPROM_write(78, 15);//初始化滤芯额定值
		DATAlist[0x1b] = 15; EEPROM_write(79, 15);
		DATAlist[0x1c] = 30; EEPROM_write(80, 30);
		DATAlist[0x1d] = 30; EEPROM_write(81, 30);
		DATAlist[0x1e] = 25; EEPROM_write(82, 25);
		DATAlist[0x1f] = 13; EEPROM_write(83, 13);
		for (i = 0; i < 12; i++)			//menmory to 86+12==98  初始化滤芯计数
		{
			DATAlist[0x24 + i] = 0;
			EEPROM_write((86 + i), DATAlist[0x24 + i]);
		}
		childlock = 0xaa;
		EEPROM_write(98, 0xaa);		//儿童保护初始化
		timemode = 0;					//定时工作模式关闭   menmory to 99
		EEPROM_write(99, 0);
		timezhishui = 30;
		EEPROM_write(100, 30);			//制水30分钟计时  20s-30*60=1800/20=90   menmory to 100
		timeflush = 18;
		EEPROM_write(113, 18);		//制水30分钟冲洗18秒
		enablebit = 0;
		EEPROM_write(114, 0);			//强制加热或者制冷位    menmory to 114
		DATAlist[0x5e] = 0;
		DATAlist[0x5f] = 160;     //剩余天数
		EEPROM_write(115, DATAlist[0x5e]);
		EEPROM_write(116, DATAlist[0x5f]); //menmory to 116
	}
	DATAlist[0x19] = Bupline;

	keybyte = 0xff;//可以进去按钮1,-6
	swbyte = 0x00;//开关状态为关闭
	JXloop = 0;//间歇时间
	enoughwater = 0;//取出的水已经足够额度
	hotwater = 0;
	warmwater = 0;
	icewater = 0;
	iceonoff = 04;//冰水阀压缩机传送字节
	yasuoji = 0;//压缩机
	yasuojicount = 0;

	//----初始化时段----
	DATAlist[0x80] = EEPROM_read(52);//时间段是否设置了
	if (DATAlist[0x80] == 0xaa)//有保存，读出时间段
	{
		for (i = 0; i < 24; i++)  //将数据初始化
		{
			DATAlist[0x81 + i] = EEPROM_read(53 + i); //memory to 77  53-77    101-112
		}
		for (i = 0; i < 12; i++)  //将数据初始化
		{
			DATAlist[0x99 + i] = EEPROM_read(101 + i); //memory to    101-112
		}
	}
	else   //没有设置时段初始值
	{
		for (i = 0; i < 18; i++)  //将数据初始化
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


	TCCR0 = 0x05;   //02开启TIMER0定时
 //------------------------------------
 //----------表的初始化---------------
	iceuptemp = EEPROM_read(0x04);//
	icedowntemp = EEPROM_read(0x05);//15
	if (iceuptemp > 50)    //合法性
	{
		iceuptemp = 20;
		EEPROM_write(0x04, iceuptemp);
	}
	if (icedowntemp > 50)
	{
		icedowntemp = 15;
		EEPROM_write(0x05, icedowntemp);
	}
	DATAlist[0x40] = 0xaa;           //把表首地址置AA
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
 //---按下设置 和 - 按键，进入滤芯寿命设置
 // read_3key();//use

	LCDNUM[13] = 0x00;
	//-------------
	//------定时工作模式显示-------
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
	TCCR1B = 0x07;  //07  启动T1计数
	flushontime = EEPROM_read(39);
	if (flushontime > 0)
	{
		BENGON;
		FA_INWATERON;
		FA_FLUSHON;
	}
	resetcount = 194;
	tdscount = 10;//TDS检测

	while (1)
	{
		_delay_us(302);
		loopnum++;
		if (loopnum > 760)//50000---1000
		{
			loopnum = 0;
			_delay_ms(20);

			//------------------保存下发的数据-----------------------------
			if (savebit != 0)
			{
				if ((savebit & 0x01) == 0x01)//保存冲洗时间
				{
					EEPROM_write(100, timezhishui);//保存
					EEPROM_write(113, timeflush);//保存
					savebit &= 0xfe;
				}
				if ((savebit & 0x08) == 0x08)//保存额定滤芯值
				{
					EEPROM_write(78, DATAlist[0x1a]);
					EEPROM_write(79, DATAlist[0x1b]);
					EEPROM_write(80, DATAlist[0x1c]);
					EEPROM_write(81, DATAlist[0x1d]);
					EEPROM_write(82, DATAlist[0x1e]);
					EEPROM_write(83, DATAlist[0x1f]);
					savebit &= 0xf7;
				}
				if ((savebit & 0x80) == 0x80)//保存复位滤芯值
				{
					for (i = 0; i < 12; i++)  //将数据初始化
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



			if ((dealbyte & 0x10) == 0x10)//设置剩余时间
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
			//----------更新显示内容------------
			//----------------------------------------------------------------------------------
			if (LCDmode == 0)
			{
				_delay_ms(2000);

				LCD0loop++;
				if (LCD0loop > 8)LCD0loop = 1;
				switch (LCD0loop)
				{
				case 1://TDS
				   //------水滴-----
					LCDNUM[12] |= 0x10;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;

					if (tdscount > 0) { //TDS值，在制水的时候才检测
						tdscount--;
						PORTD &= ~(1 << PD4);
						PORTD &= ~(1 << PD5);
						_delay_ms(100);
						ad_getdat(6);//TDS值
						ad_getdat(7);//TDS值  运算
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
						if (((bkhottemp != DATAlist[0x44]) || (bkcoldtemp != DATAlist[0x46]) || (tds1num != tempint1) || (tds2num != tempint2)) && (needreturn == 0))//值发生变化，上传
						{	//                                                                                             消费记录已经发送完毕
							bkhottemp = DATAlist[0x44];
							bkcoldtemp = DATAlist[0x46];
							tds1num = tempint1;
							tds2num = tempint2;
							//if(resetcount<2)send_tds_temp_speed();
							sendtime = 1;//?
						}
						//TDS值
						LCDNUM[10] |= 0x10;//
						LCDNUM[17] = 0x10;
						LCDNUM[13] &= 0x10;

						temp = tempint1 / 100;
						if (temp > 0)
						{
							LCDNUM[16] = (LCDNUM[16] & 0x10) | SMG[temp];       //高4位
						   // LCDNUM[10]=(LCDNUM[10]&0x8F)|((SMG[temp]&0x0f)<<4);//低4位
						}
						else
						{
							LCDNUM[16] &= 0x10;
						}
						temp1 = tempint1 - (100 * temp);//取后两位
						temp = temp1 / 10;
						LCDNUM[15] = (LCDNUM[15] & 0x10) | SMG[temp];
						temp = temp1 - (temp * 10);
						LCDNUM[14] = (LCDNUM[14] & 0x10) | SMG[temp];

						//纯水-----
						temp = tempint2 / 100;
						if (temp > 0)
						{
							LCDNUM[12] = (LCDNUM[12] & 0x10) | SMG[temp];
						}
						else
						{
							LCDNUM[12] &= 0x10;
						}
						temp1 = tempint2 - (100 * temp);//取后两位
						temp = temp1 / 10;
						LCDNUM[11] = (LCDNUM[11] & 0x10) | SMG[temp];
						temp = temp1 - (temp * 10);
						LCDNUM[10] = (LCDNUM[10] & 0x10) | SMG[temp];
					}
					break;
				case 2://设置wifi？
					   //------水滴-----
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

						//if(resetcount<2) send_tds_temp_speed();//如果在这里发送，则在不同那里不必再发送
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
						if (((fastatus & 0x08) == 0x08) && (needreturn == 0) && (resetcount < 2))//有置位，记录发送完毕
						{
							_delay_ms(100);
							fastatus &= 0xf7;                   //哪里发生很多01数据过来？？？
							sendbit |= 0x01;
							send_status();
						}
					}

					wificount++;
					if (wificount == 50)//没有返回，则取消wifi图标
					{

						DATAlist[0x21] &= 0x7f;
						fastatus |= 0x08;//需要发送
						//LCDNUM[0]&=0xf3;//取消wifi图标
						LCDNUM[1] &= 0xfe;
						LCDNUM[2] &= 0xfe;
						LCDNUM[3] &= 0xfe;
						LCDNUM[4] &= 0xfe;
						LCDNUM[5] &= 0xfe;
						//BEEP(200);
					}
					if (wificount > 80) //重新设置wifi？   在此设置wifi重启  如果不是GPRS将此放大
					{
						wificount = 0;
						resetcount = 201;
					}
					break;

				case 3:
					//------水滴-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] |= 0x01;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;
					//---------


					break;
				case 4://检查时间以及是否强制
					  //------水滴-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] |= 0x01;
					LCDNUM[15] &= 0xef;
					LCDNUM[16] &= 0xef;
					//---------显示流量--------------
					LCDNUM[8] |= 0x01;//流量

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
					  //------水滴-----
					LCDNUM[12] &= 0xef;
					LCDNUM[11] &= 0xef;
					LCDNUM[7] &= 0xfe;
					LCDNUM[6] &= 0xfe;
					LCDNUM[15] |= 0x10;
					LCDNUM[16] &= 0xef;
					//----------保存工作模式----
					if ((enbit & 0x04) == 0x04)//需要保存工作模式
					{
						EEPROM_write(41, workmode);
						enbit &= 0xfb;
					}
					//---------------------------------------
					//处理服务器发送过来的指令
					if ((dealbyte & 0x01) == 0x01)//开机
					{
						send_reply(0);
						HT1721C_BL_1;
						LCDmode = 0;
						dealbyte &= 0xfe;
						//send_reply(0);
					}
					if ((dealbyte & 0x02) == 0x02)//关机
					{
						send_reply(0);
						HT1721C_BL_0;
						LCDmode = 42;
						dealbyte &= 0xfd;
						//send_reply(0);
						FA_HOTOFF;
						FA_WARMOFF;
						//FA_COOLOFF;
						fastatus &= 0xf8;//3个阀门的状态关闭
						FA_INWATEROFF;
						waterbit &= 0xfb;//置位 关闭制水
						FA_FLUSHOFF;
						minbit &= 0xfd;//关闭冲洗阀
						COOLOFF;
						coolcount = 60;
						waterbit &= 0xef;//置0，压缩机已关闭
						//HEATAOFF;
						waterbit &= 0xdf;//置0，加热已关闭
						BENG1OFF;
						BENGOFF;
						fastatus &= 0xf7;
						DATAlist[0x20] = 0x00;
						DATAlist[0x21] = 0x00;
						_delay_ms(1000);
						send_status();
						WRITE_ALLRAM_1726(0);
					}
					if ((dealbyte & 0x04) == 0x04)//冲洗模式
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


					if ((dealbyte & 0x20) == 0x20)//复位滤芯到设定值
					{
						EEPROM_write(78, DATAlist[0x1a]);
						EEPROM_write(79, DATAlist[0x1b]);
						EEPROM_write(80, DATAlist[0x1c]);
						EEPROM_write(81, DATAlist[0x1d]);
						EEPROM_write(82, DATAlist[0x1e]);
						EEPROM_write(83, DATAlist[0x1f]);
						for (i = 0; i < 12; i++)  //保存复位滤芯值
						{
							EEPROM_write((86 + i), DATAlist[0x24 + i]);
						}
						send_reply(0);
						dealbyte &= 0xdf;
					}

					if ((dealbyte & 0x80) == 0x80)//禁止机器
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
							fastatus &= 0xf8;//3个阀门的状态关闭
							FA_INWATEROFF;
							waterbit &= 0xfb;//置位 关闭制水
							FA_FLUSHOFF;
							minbit &= 0xfd;//关闭冲洗阀
							COOLOFF;
							coolcount = 60;
							waterbit &= 0xef;//置0，压缩机已关闭
							//HEATAOFF;
							waterbit &= 0xdf;//置0，加热已关闭
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
					//------水滴-----
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
					if (DATAlist[0x41 + 10] > 200)//gao,没漏水
					{
						LCDNUM[13] &= 0xef;
						DATAlist[0x09] &= 0xef;
					}
					else
					{
						_delay_ms(300);
						ad_getdat(5);
						_delay_ms(100);
						if (DATAlist[0x41 + 10] < 200) //再确认一下
						{
							LCDNUM[13] |= 0x10;
							DATAlist[0x09] |= 0x10;
							BEEP(50);
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
						}

					}
					if (((DATAlist[0x41 + 10] > 200) && (loushuibk < 201)) || ((DATAlist[0x41 + 10] < 201) && (loushuibk > 200)) && (resetcount < 2))//发送漏水信息
					{
						loushuibk = DATAlist[0x41 + 10];
						sendbit |= 0x08;
						send_error();//发送错误代码
						_delay_ms(200);
					}

					break;
				case 7:
					//---------------发生是否缺水的信息------------------------
					if ((((DATAlist[0x0b] & 0x02) == 0x02) && ((timebit & 0x10) == 0x00)) || (((DATAlist[0x0b] & 0x02) == 0x00) && ((timebit & 0x10) == 0x10)))//不同
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
				//ONE.2----------------检测各个接口信号--------------------------------------------------------
				//-------制水检测---------
				if ((flushontime == 0) && (LCDmode == 0))
				{
					if ((PINB & 0x80) == 0x80)//1 1
					{
						LLJcount = 0;
						LCDNUM[14] &= 0xef;
						if ((PINB & 0x40) == 0x00)//水满
						{
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;

							if (flushcont > 200)//制水10升了，冲洗一次
							{
								flushcont = 0;
								flushontime = EEPROM_read(39);
								BENGON;
								FA_INWATERON;
								FA_FLUSHON;
							}
							DATAlist[0x0b] &= 0xfd;//有水
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
						if ((PINB & 0x40) == 0x00)//制水 0 0 在没有漏水的情况下
						{
							if ((LCDNUM[13] & 0x10) == 0x00)//如果时间为0，则不运行制水了&&(DATAlist[0x5e]!=0x00)&&(DATAlist[0x5f]!=0x00)   没漏水
							{
								if ((DATAlist[0x5e] == 0x00) && (DATAlist[0x5f] == 0x00))//剩余时间不为0
								{
									_delay_ms(1);
								}
								else
								{//---制水------
									BENGON;
									FA_INWATERON;
									FA_FLUSHOFF;
									tdscount = 6;
									LCDNUM[14] &= 0xef;
									//-----	计数 后判断置位
									LLJcount++;
									if (LLJcount > 60)
									{
										LLJcount = 0;
										tempint1 = TCNT1;
										if (tempint1 == bkliul)//流量没变
										{
											_delay_ms(3000);
											bkliul = TCNT1;
											if (tempint1 == bkliul)//还是相等，流量没变化，流量计故障
											{
												if ((DATAlist[0x0b] & 0x40) == 0x00)//没置位
												{
													DATAlist[0x0b] |= 0x40;
													_delay_ms(300);
													sendbit |= 0x08;
													send_error();
													//_delay_ms(300);
												}
											}
											else//不相等
											{
												bkliul = tempint1;
												if ((DATAlist[0x0b] & 0x40) == 0x40)//置位
												{
													DATAlist[0x0b] &= 0xbf;
													_delay_ms(300);
													sendbit |= 0x08;
													send_error();
													//_delay_ms(300);
												}
											}
										}
										else//不相等
										{
											bkliul = tempint1;
											if ((DATAlist[0x0b] & 0x40) == 0x40)//置位
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
							DATAlist[0x0b] &= 0xfd;//有水
						}
						else//缺水   01
						{
							LLJcount = 0;
							BENGOFF;
							FA_INWATEROFF;
							FA_FLUSHOFF;
							LCDNUM[14] |= 0x10; //缺水
							DATAlist[0x0b] |= 0x02;//缺水
						}
					}
				}//flushontime==0   


			//---------------------保存流量----------------------剩余量比较是否过量？------------------------
				if ((fastatus & 0x10) == 0x10)//需要保存本次出水流量值
				{
					thistimepuls = TCNT1;
					TCNT1 = 0;
					fastatus &= 0xef;//本次完成
					thistimepuls = yupulsh * 256 + yupulsl + thistimepuls;//加上余量
					temp = thistimepuls / pule1l;//除以1升的脉冲数
					yupulsh = (thistimepuls - temp*pule1l) / 256;
					yupulsl = thistimepuls - temp*pule1l - yupulsl * 256;
					EEPROM_write(35, yupulsh);
					EEPROM_write(36, yupulsl);//保存余量
					if (temp > 0)//需要保存滤芯数值，总量
					{
						tempint1 = DATAlist[0x24] * 256 + DATAlist[0x25] + temp; //滤芯1
						DATAlist[0x24] = tempint1 / 256;
						DATAlist[0x25] = tempint1 - (DATAlist[0x24] * 256);
						tempint1 = DATAlist[0x26] * 256 + DATAlist[0x27] + temp; //滤芯2
						DATAlist[0x26] = tempint1 / 256;
						DATAlist[0x27] = tempint1 - (DATAlist[0x26] * 256);
						tempint1 = DATAlist[0x28] * 256 + DATAlist[0x29] + temp; //滤芯3
						DATAlist[0x28] = tempint1 / 256;
						DATAlist[0x29] = tempint1 - (DATAlist[0x28] * 256);
						tempint1 = DATAlist[0x2A] * 256 + DATAlist[0x2B] + temp; //滤芯4
						DATAlist[0x2A] = tempint1 / 256;
						DATAlist[0x2B] = tempint1 - (DATAlist[0x2A] * 256);
						tempint1 = DATAlist[0x2C] * 256 + DATAlist[0x2D] + temp; //滤芯5
						DATAlist[0x2C] = tempint1 / 256;
						DATAlist[0x2D] = tempint1 - (DATAlist[0x2C] * 256);
						tempint1 = DATAlist[0x2E] * 256 + DATAlist[0x2F] + temp; //滤芯6
						DATAlist[0x2E] = tempint1 / 256;
						DATAlist[0x2F] = tempint1 - (DATAlist[0x2E] * 256);
						for (i = 0; i < 12; i++)			//menmory to 86+12==98  保存数据
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
						//----发送数据-----
						_delay_ms(100);
						sendbit |= 0x04;
						send_filter();
					}
				}


				//ONE.22---
				if (LCDmode == 0) REFRESH_ALLRAM_1726();
				//HT1721C_STB_1;
				//----是否需要闪烁------
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
					if ((flashbit & 0x02) == 0x02)//加热
					{
						WRITE_1DATA_1721(12, LCDNUM[12] & 0xf7);
					}
					if ((flashbit & 0x04) == 0x04)//制冷
					{
						WRITE_1DATA_1721(13, LCDNUM[13] & 0xf7);
					}
					if ((flashbit & 0x08) == 0x08)//车轮
					{

					}
					if ((flashbit & 0x10) == 0x10)//笑脸
					{
					}
					if ((flashbit & 0x20) == 0x20)//笑脸+儿童保护
					{

					}

				}
			} //LCDMODE==0
		   //----------------------------------LCDMODE==0 UP----------------------------------------------18260



			if (LCDmode == 42)
			{
				_delay_ms(1000);
				if ((dealbyte & 0x01) == 0x01)//开机
				{
					send_reply(0);
					HT1721C_BL_1;
					LCDmode = 0;
					dealbyte &= 0xfe;

					DATAlist[0x20] = 0x01;
					DATAlist[0x21] = 0x00;
					fastatus |= 0x08;//发送一次开机状态
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
				if (DATAlist[0x18] == 0xaa)//许能否？
				{
					send_reply(0);
					HT1721C_BL_1;
					LCDmode = 0;
					EEPROM_write(25, DATAlist[0x18]);
					dealbyte &= 0x7f;

					DATAlist[0x20] = 0x01;
					DATAlist[0x21] = 0x00;
					fastatus |= 0x08;//发送一次开机状态
					temp = 100;
					_delay_ms(3000);
					BEEP(50);
				}

			}


			//SIX--------------------------------------------------------------------------------------------
			//复位滤芯
			//---------------------------------------------------------------------------------------
			if (LCDmode == 46)// 判断拨码是否复位回去 !=0    ((LCDmode==200)&&(LCDmode<30))//
			{
				LCDtemp[2]++;
				if (LCDtemp[2] > 8)
				{
					WRITE_1DATA_1721(9, 0x00);
					WRITE_1DATA_1721(8, 0x02);
					LCDtemp[2] = 0;
					if (LCDtemp[3] & 0x80)//---熄灭
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
						//冷水//取后两位
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
				//------------加1按钮-------------------
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
				//------------减1按钮-------------------
				if (keycode[0] == 0x80)
				{
					BEEP(10);
					_delay_ms(2000);
					LCDtemp[3] &= 0x7f;
					LCDtemp[2] = 9;
					if (LCDtemp[4] == 1)LCDtemp[4] = 7;
					LCDtemp[4]--;
				}
				//------------设置退出按钮-------------------
				if (keycode[0] == 0x04)
				{
					BEEP(100);
					LCDmode = 0;
				}

				//--------------------------
				//---------确定按钮-----保存上次的数据，进入下一次模式----------
				if (keycode[1] == 0x08)//改变LCDmode
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