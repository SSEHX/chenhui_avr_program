/*
 * eeprom.h
 *
 * Created: 2017/12/12 21:22:05
 *  Author: 许旗
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void eeprom_write_byte(unsigned int Address, unsigned char Data);
void eeprom_write_string(unsigned int Address, unsigned char Len, unsigned char *Data_buff);
unsigned char eeprom_read_byte(unsigned int Address);
void eeprom_read_string(unsigned int Address, unsigned char Len, unsigned char *Data_buff);

#endif /* EEPROM_H_ */