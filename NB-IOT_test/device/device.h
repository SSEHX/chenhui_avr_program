/*
 * device.h
 *
 * Created: 2017/12/13 20:11:15
 *  Author: 许旗
 */ 


#ifndef DEVICE_H_
#define DEVICE_H_

#define LED_OPEN 		PORTC &=    ~(1 << PC0)
#define LED_CLOSE		PORTC |=	 (1 << PC0)
#define LED_REVERSE		PORTC ^=	 (1 << PC0)

#define JINSHUI_OPEN	PORTE |=	 (1 << PE7)
#define JINSHUI_CLOSE	PORTE &=	~(1 << PE7)

#define CHUSHUI_OPEN	PORTE |=	 (1 << PE6)
#define CHUSHUI_CLOSE	PORTE &=	~(1 << PE6)

void beep(unsigned char second);
unsigned int get_hight_switch();
unsigned int get_Low_switch();


#endif /* DEVICE_H_ */