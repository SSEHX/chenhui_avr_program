/*
 * NB-IOT_test.c
 *
 * Created: 2017/12/12 20:55:38
 * Author : 许旗
 */ 

#include "app.h"


int main(void)
{
	port_init();
	lcd_init();
	write_all_ram_tm1726(0);

	
    /* Replace with your application code */
    while (1) 
    {
		
    }
}

