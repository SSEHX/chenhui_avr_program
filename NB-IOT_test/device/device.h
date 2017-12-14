/*
 * device.h
 *
 * Created: 2017/12/13 20:11:15
 *  Author: 许旗
 */ 


#ifndef DEVICE_H_
#define DEVICE_H_

#define LED_OPEN 		PORTC &= ~(1 << PC0)
#define LED_CLOSE		PORTC |=  (1 << PC0)
#define LED_REVERSE		PORTC ^=  (1 << PC0)

void beep(unsigned char second);

#endif /* DEVICE_H_ */