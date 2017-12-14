/*
 * lcd.h
 *
 * Created: 2017/12/14 9:40:43
 *  Author: 许旗
 */ 


#ifndef LCD_H_
#define LCD_H_

#include "../port/port.h"
#include <util/delay.h>

#define TINGD 1 

void lcd_init();
void write_string_address_tm1726(unsigned char Address,unsigned char len,unsigned char *Data);
void write_byte_address_tm1726(unsigned char Address, unsigned char byte);
void init_tm1726();
void update_all_ram_tm1726();
void write_all_ram_tm1726(unsigned char write_empty);
void write_byte_tm1726(unsigned char byte);



#endif /* LCD_H_ */