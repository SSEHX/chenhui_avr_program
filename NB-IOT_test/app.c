/*
 * NB-IOT_test.c
 *
 * Created: 2017/12/12 20:55:38
 * Author : 许旗
 */ 

#include "app.h"

void system_init(){
	cli();
	init_port();
	init_lcd();
	init_uart0();
	init_uart1();
	init_adc();
	sei();
}

int main(void)
{
	system_init();
	init_bc95();
	
	get_all_adc_value();
	
	while(1)
	{
		//bc95接收到数据
		if (device_status_bc95.have_receive)
		{
			
			device_status_bc95.have_receive = 0;
			uart0_send_string("bc95_have_receive");
			/*-----------------------------
					接收数据处理函数
			-----------------------------*/
		}
		//bc95发生重启
		if (device_status_bc95.have_reboot)
		{
			uart1_rx_array_set_empty();
			device_status_bc95.have_reboot = 0;
			uart0_send_string("bc95_have_reboot");
			init_bc95();
			
		}
		
		if (get_Low_switch() == 0)
		{
			device_status_lcd.no_water = YES;
		}else{
			if (get_hight_switch() == 1)
			{
			}
		}
		
		get_all_adc_value();
		_delay_ms(2000);
		
	}
    
}

