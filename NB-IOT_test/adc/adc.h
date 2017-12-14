/*
 * adc.h
 *
 * Created: 2017/12/13 14:27:37
 *  Author: 许旗
 */ 


#ifndef ADC_H_
#define ADC_H_


//声明函数
void adc_init();
unsigned int adc_start_return(unsigned char channel);
void adc_start_interrupt(unsigned char channel);

#endif /* ADC_H_ */