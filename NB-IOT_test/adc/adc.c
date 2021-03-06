﻿/*
 * adc.c
 *
 * Created: 2017/12/13 14:27:13
 *  Author: 许旗
 */ 
#include "adc.h"


//定义变量
unsigned int		adc_channel_data_list[8] = {0};
unsigned char		adc_interrupt_channel_cache = 0;

/*------------------------------------------------------------------------------
* 函数名称：init_adc
* 功    能：初始化adc
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void init_adc()
{
	ADCSRA=0x00;
	
	/*----------------------------------------------------
	ADMUX	BIT	7 ：6				参考源
				0	1				带有外部电容的AVCC
				1	1				内部2.56V参考电压映射在连接了外部电容的AREF引脚
				
			BIT	5					ADC数据对齐控制
				0					右对齐
				1					左对齐
				
			BIT	4 ：0				模拟通道和增益选择位
				00000		0		单通道	ADC0
				00001		1		单通道	ADC1
				...		   ...
				00111		7		单通道	ADC7
				。。。	参考数据手册P248
	----------------------------------------------------*/
	ADMUX=0x40;		
	
	ACSR=0x80;			//使能模拟比较器
	
	//		 使能adc		{		 64分频		  }
	ADCSRA |= (1<<ADEN)	|	(1<<ADPS2) | (1<<ADPS1);
	
}

/*------------------------------------------------------------------------------
* 函数名称：adc_start_return	
* 功    能：开始adc转换		并将数据存入对应的
							adc_channel_data_list[channel]

* 入口参数：	unsigned char n		adc通道选择
					0 ... 7

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
unsigned int adc_start_return(unsigned char channel)
{

	ADMUX =	(ADMUX & 0xE0) | channel;		//不改变其他配置，并设置通道
	
	ADCSRA |= (1 << ADEN)|(1 << ADSC);		//启动转换
	while( !(ADCSRA & (1<<ADIF)));			//等待转换结束
	
	ADCSRA |= (1 << ADIF);					//清中断标志位
	
	_delay_us(2);
	

	//将数据保存到adc_channel_data_list[channel]数组内
	adc_channel_data_list[channel] = (ADCL | ADCH << 8);
	
	return adc_channel_data_list[channel];
	
}

/*------------------------------------------------------------------------------
* 函数名称：adc_start	
* 功    能：开始adc转换并打开中断处理		通过中断服务函数将数据存入对应的
												adc_channel_data_list[channel]

* 入口参数：	unsigned char n		adc通道选择
					0 ... 7

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void adc_start(unsigned char channel){

/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("adc start ... \r\n");
#endif

	ADCSRA |= (1 << ADIE);						//打开adc中断	
	
	adc_interrupt_channel_cache = channel;		//临时保存通道号,以便在中断服务程序中调用
	
	ADMUX =	(ADMUX&0xe0) | channel;				//不改变其他配置，并设置通道
	ADCSRA |= (1 << ADSC);						//启动转换
}

/*------------------------------------------------------------------------------
* 函数名称：ADC_vect
* 功    能：adc中断
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
SIGNAL(ADC_vect){
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("adc ok ... \r\n");
#endif
	adc_channel_data_list[adc_interrupt_channel_cache] = (ADCL | ADCH << 8);
	
	unsigned char cache[10] = {0}; 
	
	uart0_send_string(itoa(cache, adc_channel_data_list[adc_interrupt_channel_cache], 10));
	
	switch(adc_interrupt_channel_cache){
		case (PURE_WATER) :
			adc_data.pure_water = adc_channel_data_list[adc_interrupt_channel_cache];
		break;
		case (RAW_WATER) :
			adc_data.raw_water = adc_channel_data_list[adc_interrupt_channel_cache];
		break;
		case (LEAKAGE) :
			adc_data.leakage = adc_channel_data_list[adc_interrupt_channel_cache];
		break;
	}
}

/*------------------------------------------------------------------------------
* 函数名称：adc_get_pure_water
* 功    能：获取pure_water端口adc值		通过中断服务函数将数据存入对应的
										adc_data.pure_water
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void adc_get_pure_water(){
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("start get pure water tds ... \r\n");
#endif	
	
	adc_data.pure_water = adc_start_return(PURE_WATER);
}

/*------------------------------------------------------------------------------
* 函数名称：adc_get_raw_water
* 功    能：获取raw_water端口adc值		通过中断服务函数将数据存入对应的
										adc_data.raw_water
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/

void adc_get_raw_water(){
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("start get raw water tds ... \r\n");
#endif	
	
	adc_data.raw_water = adc_start_return(RAW_WATER);
}

/*------------------------------------------------------------------------------
* 函数名称：adc_check_leakage
* 功    能：获取leakage端口adc值		通过中断服务函数将数据存入对应的
										adc_data.leakage
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void adc_check_leakage(){
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("start get leakage ... \r\n");
#endif	
	
	adc_data.leakage = adc_start_return(LEAKAGE);
	
}


