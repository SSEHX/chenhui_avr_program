/*
 * lcd.c
 *
 * Created: 2017/10/27 16:47:51
 *  Author: HXHZQ
 */ 

#include "lcd.h"
#include "led.h"

unsigned char SMG[10] = { 0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,0xef,0xcf };//10后
unsigned char SMGL[10] = { 0xfa,0x60,0xbc,0xf4,0x66,0xd6,0xde,0x70,0xfe,0xf6 };//数码管转换 abcd xfge  
unsigned char LCDNUM[32];        //LCD的显示数组

static void TM1726_start(void)
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
static void TM1726_stop(void)
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
	unsigned char i, temp;

	temp = 0x01;  //80
	TM1726_SDA_0;
	TM1726_SCL_0;
	_delay_us(TINGD);
	for (i = 0; i < 8; i++)
	{
		TM1726_SCL_0;
		if ((temp & wdata) == 0)
		{
			TM1726_SDA_0;
		}
		else
		{
			TM1726_SDA_1;
		}
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
	WRITE_DATA_1726(0xc0);//要写的地址
	for (i = 0; i < 18; i++)
	{
		TM1726_SDA_1;
		TM1726_SCL_1;
		if (F_RAM1632_SET)
		{
			WRITE_DATA_1726(0xff);
		}	//all RAM write 1
		else
		{
			WRITE_DATA_1726(0x00);
		}	//all RAM write 0
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
	for (i = 0; i < 18; i++)
	{
		TM1726_SDA_1;
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
void WRITE_1DATA_1726(unsigned char addr, unsigned char dat1)
{
	TM1726_SDA_1;
	TM1726_SCL_1;
	TM1726_start();
	_delay_ms(10);
	WRITE_DATA_1726(0xc0 + addr);//
	TM1726_SDA_1;
	TM1726_SCL_1;
	_delay_ms(10);
	WRITE_DATA_1726(dat1);
	TM1726_stop();
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
功能：填充HT1632C的固定地址和固定数据
*********************************/
void WRITE_1DATA_1721(unsigned char addr, unsigned char dat1)
{
	HT1721C_STB_1;
	//HT1721C_CLK_1;
	HT1721C_DIO_1;
	//WRITE_DATA_1721(0x08);
	HT1721C_STB_1;
	WRITE_DATA_1721(0x44);
	HT1721C_STB_1;
	WRITE_DATA_1721(0xc0 + addr);
	WRITE_DATA_1721(dat1);
	HT1721C_STB_1;
	_delay_us(TINGD);
	WRITE_DATA_1721(0x97);
	HT1721C_STB_1;

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


char SCQ = 0;
unsigned char LCD_Ico = 0;//0:时间,1:流量,2:原水,3:纯水,4:缺水,5:漏水,6:信号,7:附网
static void LCD_GPIO_Init(void)//PA0 
{
	DDRA |= (0x3<<0);
	PORTA |= (0x3<<0);
	
	DDRC |= (0x7<<1);
	PORTA |= (0x7<<1);	
}
void LCD_Init(void)
{
	LCD_GPIO_Init();
	INIT_1726();
	_delay_ms(200);
	HT1721C_BL_1;	
	WRITE_ALLRAM_1726(1);	
	_delay_ms(1000);
	//WRITE_ALLRAM_1726(0);
	
	flow_set(&Device[3]);//显示设备码
	REFRESH_ALLRAM_1726();
	//_delay_ms(3000);
}
void drip(void)
{
	static unsigned char loop = 0;
	switch(loop)
	{
		case 0:
			LCDNUM[16]&= ~(0x10);//大水圈
			WRITE_1DATA_1726(16,LCDNUM[16]);
			LCDNUM[12]|=(0x10);//上水滴
			WRITE_1DATA_1726(12,LCDNUM[12]);
			break;
		case 1:
			LCDNUM[11]|=(0x10);//中水滴
			LCDNUM[12]&=(~0x10);//上水滴
			Write_addr_data(11,2,&LCDNUM[11]);
			break;
		case 2:
			LCDNUM[11]&=(~0x10);//中水滴
			WRITE_1DATA_1726(11,LCDNUM[11]);
			LCDNUM[7]|=(0x01);//下水滴
			WRITE_1DATA_1726(7,LCDNUM[7]);
			break;
		case 3:
			LCDNUM[7]&=(~0x01);//下水滴
			LCDNUM[6]|=(0x01);//小水圈
			Write_addr_data(6,2,&LCDNUM[6]);
			break;
		case 4:
			LCDNUM[6]&=(~0x01);//小水圈
			WRITE_1DATA_1726(6,LCDNUM[6]);
			LCDNUM[15]|=(0x10);//中水圈
			WRITE_1DATA_1726(15,LCDNUM[15]);
			break;
		case 5:
			LCDNUM[15]&=(~0x10);//中水圈
			LCDNUM[16]|=(0x10);//大水圈
			Write_addr_data(15,2,&LCDNUM[15]);
			break;
		default: break;
	}
	loop ++;
	if(loop>5) 
		loop = 0;
}
void signal_ico(void)
{
	if((LCD_Ico & 0x01<<7))
		LCDNUM[1]|=0x01;//信号
	else
		LCDNUM[1]&=(~0x01);
}
void hydropenia_ico( void )
{
	if(LCD_Ico&(0x1<<4))
		LCDNUM[14]|= 0x10;//缺水
	else
		LCDNUM[14]&= (~0x10);
}
void leakage_ico( void )
{
	if(LCD_Ico&(0x1<<5))//漏水
		LCDNUM[13]|=0x10;
	else
		LCDNUM[13]&=(~0x10);
}
void pure_water_ico(void)
{
	if(LCD_Ico&(0x1<<3))
		LCDNUM[10]|= 0x10;//纯水
	else
		LCDNUM[10]&= (~0x10);
}
void raw_water_ico(void)
{
	if(LCD_Ico&(0x1<<2))
		LCDNUM[17]|=0x10;//原水
	else
		LCDNUM[17]&=(~0x10);
}
void flow_ico(void)
{
	if(LCD_Ico&(0x1<<1))
	LCDNUM[8]|=0x01;//流量
	else
	LCDNUM[8]&=(~0x01);
}
void time_ico(void)
{
	if(LCD_Ico&(0x1<<0))
		LCDNUM[0]|=0x01;//time
	else
		LCDNUM[0]&=(~0x01);
}
void null_ico(void)
{
	if(LCD_Ico&(0x1<<7))
		LCDNUM[9]|=0x01;
	else
		LCDNUM[9]&=(~0x01);
}
void signal(void)
{
	//LCDNUM[2]&=(~0x01);
	if(!(LCD_Ico & 0x01<<7))
	SCQ = 0;
	if(SCQ > 0 )
		LCDNUM[2]|=0x01;//信号1
	else 
		LCDNUM[2]&=(~0x01);//信号1
	if(SCQ > 8)
		LCDNUM[3]|=0x01;//信号2
	else 
		LCDNUM[3]&=(~0x01);//信号2
	if(SCQ > 16)
		LCDNUM[4]|=0x01;//信号3
	else 
		LCDNUM[4]&=(~0x01);//信号3
	if(SCQ > 24)
		LCDNUM[5]|=0x01;//信号4
	else 
		LCDNUM[5]&=(~0x01);//信号4
	//Write_addr_data(2,4,&LCDNUM[2]);
}
void ico(void)
{
	signal_ico();
	hydropenia_ico();
	time_ico();
	flow_ico();
	raw_water_ico();
	leakage_ico();
	pure_water_ico();
	null_ico();
}
/************************************************************************/
/* addr:地址，num:数据长度, data:数据地址                                                                    */
/************************************************************************/
void Write_addr_data(unsigned char addr,unsigned char len,unsigned char *data)
{
	unsigned char i;
	TM1726_SDA_1;
	TM1726_SCL_1;
	TM1726_start();
	_delay_us(TINGD);
	WRITE_DATA_1726(0xc0 + addr);
	// all RAM have 96
	for (i = 0; i < len; i++)
	{
		TM1726_SDA_1;
		TM1726_SCL_1;
		WRITE_DATA_1726((*(data + i)));
	}
	TM1726_stop();
}

unsigned char drp_num = 0;
unsigned char error_ico = 0;
inline void reset_lcd(void)
{
	memset(LCDNUM,0,18);
}
void time_set(unsigned char *time)
{
		unsigned char len = strlen(time);
		if(len> 5)
			len = 5;
		//len = 3;
		switch(len)
		{
			case 0:
				LCDNUM[0]=0x00;
			case 1:
				LCDNUM[1]=0x00;
			case 2:
				LCDNUM[2]=0x00;
			case 3:
				LCDNUM[3]=0x00;
			case 4:
				LCDNUM[4]=0x00;
			default: break;
		}
		switch(len)
		{
			case 5:
				LCDNUM[4]=SMGL[*(time+4)-0x30];
			case 4:
				LCDNUM[3]=SMGL[*(time+3)-0x30];
			case 3:
				LCDNUM[2]=SMGL[*(time+2)-0x30];
			case 2:
				LCDNUM[1]=SMGL[*(time+1)-0x30];
			case 1:
				LCDNUM[0]=SMGL[*(time+ 0)-0x30];
			default: break;
		}
}
void flow_set(unsigned char *flow)
{
		unsigned char len = strlen(flow);
		if(len > 5)
			len = 5;
		switch(len)
		{
			case 0:
				LCDNUM[9]=0x00;
			case 1:
				LCDNUM[8]=0x00;
			case 2:
				LCDNUM[7]=0x00;
			case 3:
				LCDNUM[6]=0x00;
			case 4:
				LCDNUM[5]=0x00;
			default: break;
		}
		//len = 3;
		switch(len)
		{
			case 5:
				LCDNUM[5]=SMGL[*(flow+ len - 5)-0x30];
			case 4:
				LCDNUM[6]=SMGL[*(flow+ len - 4)-0x30];
			case 3:
				LCDNUM[7]=SMGL[*(flow+ len - 3)-0x30];
			case 2:
				LCDNUM[8]=SMGL[*(flow+ len - 2)-0x30];
			case 1:
				LCDNUM[9]=SMGL[*(flow+ len - 1)-0x30];
			default: break;
		}
// 		LCDNUM[8] |= 0x01;
// 		if(LCD_Ico&0x1<<0)
// 			if(SCQ>28)
// 				LCDNUM[5] |= 0x01;	
// 		if(drp_num == 3)
// 			LCDNUM[7] |= (0x01);
// 		if(drp_num == 4)
// 			LCDNUM[6] |= (0x01);
// 		else
// 			LCDNUM[6] &= ~(0x01);
}
void pure_water_set(unsigned char *pure_water)
{
		//USART1_Transmit((strlen(pure_water)));
		unsigned char len = strlen(pure_water);
		//len = 3;
		if(len > 4)
			len = 4;
		switch(len)
		{
			case 0:
				LCDNUM[10]=0x00;
			case 1:
				LCDNUM[11]=0x00;
			case 2:
				LCDNUM[12]=0x00;
			case 3:
				LCDNUM[13]=0x00;
			default: break;
		}
		switch(len)
		{
			case 4:
				LCDNUM[13]=SMG[*(pure_water+len-4)-0x30];
			case 3:
				LCDNUM[12]=SMG[*(pure_water+len-3)-0x30];
			case 2:
				LCDNUM[11]=SMG[*(pure_water+len-2)-0x30];
			case 1:
				LCDNUM[10]=SMG[*(pure_water+len-1)-0x30];
			default: break;
		}
// 		LCDNUM[10] |= 0x10;
// 		//LCDNUM[17]|= 0x10;
// 		if(drp_num == 2)
// 			LCDNUM[11] |= (0x10);
// 		if(drp_num == 1)
// 			LCDNUM[12] |= (0x10);
// 		if(error_ico & (0x1<<1))
// 			LCDNUM[13] |= (0x10);
}
void raw_water_set(unsigned char *raw_water)
{
	
		unsigned char len = strlen(raw_water);
		//len = 3;
		if(len > 4)
			len = 4;
			switch(len)
		{
			case 0:
				LCDNUM[17]=0x00;
			case 1:
				LCDNUM[16]=0x00;
			case 2:
				LCDNUM[15]=0x00;
			case 3:
				LCDNUM[14]=0x00;
			default: break;
		}
		switch(len)
		{
			case 4:
				LCDNUM[14]=SMG[*(raw_water+3)-0x30];
			case 3:
				LCDNUM[15]=SMG[*(raw_water+2)-0x30];
			case 2:
				LCDNUM[16]=SMG[*(raw_water+1)-0x30];
			case 1:
				LCDNUM[17]=SMG[*(raw_water+0)-0x30];
			default: break;
		}
// 		
// 		LCDNUM[17] |= 0x10;
// 		if(error_ico&0x1)
// 			LCDNUM[14] |= (0x10);
// 		if(drp_num == 5)
// 			LCDNUM[15] |= (0x10);
// 		if(drp_num == 6)
// 			LCDNUM[16] |= (0x10);
}


inline void SET_TIME(unsigned int time)
{
	unsigned char time_str[8]={0};
	if(time != 0)
	{
		itoa(time_str,time);
		time_set(time_str);
	}
	else
		time_set("0");
}

inline void SET_FLOW_L(unsigned int flow)
{
	unsigned char flow_str[8] = {0};
	if(flow != 0)
	{
		itoa(flow_str,flow);
		flow_set(flow_str);
	}
	else
		flow_set("0");
}
inline void SET_FLOW(unsigned char *flow)
{
	itoa(flow,Time1_Get_TCNT());	
	if(*flow == 0 ) 
		flow_set("0");
//	EEPROM_Write_Buf(0x18,8,flaw);
	else
		flow_set(flow);

}
/************************************************************************/
/* 电压等于 (5.01 *voltage)/1024      
	2;转化系数
	8:导体面积（cm2）
	2000:电源内阻   
	tds = 0.5 * 电导率
	电导率 = I/U (S)
	U = E * ADC/1024;
	I = （E - U)/R0 
	G = I/U
	Q = L/A    ----->L：两极板距离；A：极板面积
	K = G*Q
	tds = K/2; ((uS/cm))                       
							 
									                                     */
/************************************************************************/
#define  Q_cm 357



unsigned int voltage_to_tds(unsigned int adc)
{
// 	unsigned char L_mm  = 3, A_mm2 = 1.2*7;
// 	unsigned int Q_cm = L_mm/A_mm2 * 10;			//单位cm-1
// 	
// 	unsigned int E_mV = 5010;					//5.01v
// 	unsigned int U_mV = E_mV * adc / 1024;
// 	unsigned int I_mA = (E_mV - U_mV) / 2000;	//（R0 = 2000）
// 	unsigned int G_us = I_mA/U_mV ;
// 	unsigned int K = G * Q; //(K单位是uS/cm)
// 	unsigned int tds = K >> 2;
	unsigned int G_us = 0, K = 0;
	unsigned int a = 0,b = 0, n = 0, s = 0;
	a = 1024 - adc;
	for(unsigned char i = 0; i < 3 ; i++)//取3个有效位
	{
		n =	a/ adc;
		b = a% adc;
		a = b*10;
		s = s * 10 + n;
	}
	G_us = s * 5;
//	G_us = (1024 - adc) * 10e6  / (2000 * y);
	K	= G_us * Q_cm/100;
	return (K>>2);
//	return ((1024-adc)*125/(adc));
}
inline void SET_TDS1(unsigned char *tds1)
{
	itoa(tds1,voltage_to_tds(ADC_Get(6)));
	if(*tds1)
	{
	//	EEPROM_Write_Buf(0x1C, 4 ,tds1);
		raw_water_set(tds1);
	
	}
}
inline void SET_TDS2(unsigned char *tds2)
{
	itoa(tds2,voltage_to_tds(ADC_Get(7)));
	if(*tds2 )
	{
		//EEPROM_Write_Buf(0x20, 4 ,tds2);
		pure_water_set(tds2); 
		
	}

}

unsigned char *GET_TIME(void)
{
		LCDNUM[0]=0x00;
		LCDNUM[1]=0x00;
		LCDNUM[2]=0x00;
		LCDNUM[3]=0x00;
		LCDNUM[4]=0x00;
}

int lcd_main(void)
{
	cli();
	USART0_Init();
	LED_Init();
	//printf("lcd init\r\n");
	sei();
	USART0_Transmit_String("lcd init\r\n");
	LCD_Init();
	unsigned char time = 0;

	//time_set("12345");
	//raw_water_set("7845");
	//flow_set("89023");
	//pure_water_set("7853");
	LCD_Ico = 0xFF;
	SCQ = 21;
	//signal();
	ico();
	//LCDNUM[1]|=0x01;//信号
	REFRESH_ALLRAM_1726();
	BEEP(1);
	while(1)
	{
		
		_delay_ms(500);
		drip();
		signal();
	//	time ++;
		LEDRUN;
// 		SCQ ++;
// 		if(SCQ > 32)		
	}
	

	return 0;
}