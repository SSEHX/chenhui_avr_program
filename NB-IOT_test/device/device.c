/*
 * device.c
 *
 * Created: 2017/12/13 11:19:33
 *  Author: 许旗
 */ 

#include "device.h"

/*------------------------------------------------------------------------------
* 函数名称：beep
* 功    能：蜂鸣器蜂鸣

* 入口参数：	unsigned char second	蜂鸣时间长短
				
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void beep(unsigned char second)
{
	unsigned char i;
	PORTB |= (1 << PB0);
	for (i = 0; i < second; i++)
	{
		_delay_ms(20);
	}
	PORTB &= ~(1 << PB0);
}

/*------------------------------------------------------------------------------
* 函数名称：get_low_switch
* 功    能：获取低压开关
* 入口参数：无
* 出口参数：无

* 返 回 值：	0		无压力	无水
				1		有压力	有水
*-----------------------------------------------------------------------------*/
unsigned int get_Low_switch(){
	if ((PINB & (1 << PB6)))
	{
		return 1;		//有压力断开
	}else{
		return 0;		//无压力短接到地，读取端口为0
	}
	
}

/*------------------------------------------------------------------------------
* 函数名称：get_low_switch
* 功    能：获取低压开关
* 入口参数：无
* 出口参数：无

* 返 回 值：	0		无压力	出水开关打开
				1		有压力	出水开关关闭
*-----------------------------------------------------------------------------*/
unsigned int get_hight_switch(){
	if ((PINB & (1 << PB7)))
	{
		return 0;		//无压力短接	出水开关打开，或者缺水
	}else{
		return 1;		//有压力断开	出水开关关闭，有水
	}
}

/*------------------------------------------------------------------------------
* 函数名称：start_create_water
* 功    能：开始制水		打开水泵，并打开进水电磁阀
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void start_create_water(){
	PORTB |= (1 << PB2);		//打开水泵
	PORTE |= (1 << PE7);		//打开进水电磁阀
}

/*------------------------------------------------------------------------------
* 函数名称：stop_create_water
* 功    能：停止制水		关闭进水电磁阀并关闭水泵
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void stop_create_water(){
	PORTE &= ~(1 << PE7);		//关闭进水电磁阀
	PORTB &= ~(1 << PB2);		//关闭水泵
}

/*------------------------------------------------------------------------------
* 函数名称：start_chongxi
* 功    能：开始冲洗		开启浓水电磁阀并制水
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void start_chongxi(){
	start_create_water();		//开始制水
	PORTE |= (1 << PE6);		//打开浓水电磁阀
}

/*------------------------------------------------------------------------------
* 函数名称：stop_chongxi
* 功    能：停止冲洗		关闭浓水电磁阀并停止制水
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void stop_chongxi(){
	stop_create_water();		//停止制水
	PORTE &= ~(1 << PE6);		//关闭浓水电磁阀
}

void get_all_adc_value(){
	memset(&adc_data, 0, sizeof(struct adc_data_list));
	
	PORTD	&= ~((1 << PD4) | (1 << PD5));
	
	adc_get_pure_water();
	adc_get_raw_water();
	adc_check_leakage();
	
	PORTD	|= ((1 << PD4) | (1 << PD5));
	
	device_status_lcd.raw_water_tds = voltage_to_tds(adc_data.raw_water);
	device_status_lcd.pure_water_tds = voltage_to_tds(adc_data.pure_water);

#ifdef	DEBUG
	unsigned char cache[6] = {0};
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("get all adc is run ... \r\n");
	uart0_send_string("leakage ---> ");
	itoa(adc_data.leakage, cache, 10);
	uart0_send_string(cache);
	uart0_send_string("\r\n");
	uart0_send_string("raw_water ---> ");
	itoa(device_status_lcd.raw_water_tds, cache, 10);
	uart0_send_string(cache);
	uart0_send_string("\r\n");
	uart0_send_string("pure_warter ---> ");
	itoa(device_status_lcd.pure_water_tds, cache, 10);
	uart0_send_string(cache);
	uart0_send_string("\r\n");
#endif
}