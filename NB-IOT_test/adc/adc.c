/*
 * adc.c
 *
 * Created: 2017/12/13 14:27:13
 *  Author: 许旗
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
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
	
	//					  使能adc		{		 64分频		  }
	ADCSRA=ADCSRA |=	 (1<<ADEN)	|	(1<<ADPS2) | (1<<ADPS1);
	
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
	ADMUX =	(ADMUX&0xe0) | channel;		//不改变其他配置，并设置通道
	
	ADCSRA |= (1 << ADSC);				//启动转换
	while( !(ADCSRA & (1<<ADIF)));		//等待转换结束
	
	ADCSRA|=(1<<ADIF);					//清中断标志位
	
	//将数据保存到adc_channel_data_list[channel]数组内
	adc_channel_data_list[channel] = (ADCL | ADCH << 8);
	
	return adc_channel_data_list[channel];
}

/*------------------------------------------------------------------------------
* 函数名称：adc_start_return	
* 功    能：开始adc转换并打开中断处理		通过中断服务函数将数据存入对应的
												adc_channel_data_list[channel]

* 入口参数：	unsigned char n		adc通道选择
					0 ... 7

* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void adc_start_interrupt(unsigned char channel){
	
	ADCSRA |= (1 << ADIE);					//打开adc中断	
	
	adc_interrupt_channel_cache = channel;	//临时保存通道号,以便在中断服务程序中调用
	
	ADMUX =	(ADMUX&0xe0) | channel;			//不改变其他配置，并设置通道
	ADCSRA |= (1 << ADSC);					//启动转换
}

SIGNAL(ADC_vect){
	adc_channel_data_list[adc_interrupt_channel_cache] = (ADCL | ADCH << 8);
}