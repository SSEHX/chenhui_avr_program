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
	init_bc95();
	sei();
}

int main(void)
{
	system_init();
	
	lcd_update_pure_ppm_display(sizeof(query_data_flag.message));

	while(1)
	{
		
	}
    
}

