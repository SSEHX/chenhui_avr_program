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
#include <string.h>
#include "../tools/tools.h"

#define TINGD		1 

#define SHOW		1
#define NO_SHOW		0

#define NO_WATER_ICON		0x10
#define	TIME_ICON			0x01
#define FLOW_ICON			0x01
#define RAW_WATER_ICON		0x10
#define PURE_WATER_ICON		0x10
#define LEAKAGE_ICON		0x10
#define SIGNAL_ICON			0X01

#define YES					1
#define NO					0

struct home_device_status {
	unsigned char no_water;
	unsigned char signal;
	unsigned char leakage;
	unsigned int  raw_water_tds;
	unsigned int  pure_water_tds;
};

struct home_device_status device_status_lcd;

void init_tm1726();
void init_lcd();

void write_byte_tm1726(unsigned char byte);
void write_byte_address_tm1726(unsigned char Address, unsigned char byte);
void write_string_address_tm1726(unsigned char Address,unsigned char len,unsigned char *Data);

void write_all_ram_tm1726(unsigned char write_empty);
void update_all_ram_tm1726();

void lcd_update_flow_display(unsigned long int number);
void lcd_update_time_display(unsigned long int number);
void lcd_update_pure_ppm_display(unsigned int number);
void lcd_update_raw_ppm_display(unsigned int number);

void make_time_number(unsigned long int number, unsigned char *number_array);
void make_flow_number(unsigned long int number, unsigned char *number_array);
void make_raw_ppm_number(unsigned int number, unsigned char *number_array);
void make_pure_ppm_number(unsigned int number, unsigned char *number_array);

#endif /* LCD_H_ */