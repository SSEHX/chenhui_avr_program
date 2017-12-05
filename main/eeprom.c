/*
 * eeprom.c
 *
 * Created: 2017/10/27 17:01:34
 *  Author: HXHZQ
 */ 
#include "eeprom.h"


//*****************��EEPROM����д������*****************
//����������ַ������
//uiAddress : EEPROM �ĵ�ַ
//ucData : д��EEPROM������
//***************************************************
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	while (EECR&(1 << EEWE));    //�ȴ���һ��д��������
	EEAR = uiAddress;     //���õ�ַ�Ĵ���
	EEDR = ucData;      //�������ݼĴ���
	EECR |= (1 << EEMWE);     //��λEEMWE,����дʹ��
	EECR |= (1 << EEWE);     //��λEEWE,дʹ�ܣ�����д����
}
//***********************************************************************
//              EEPROMд���ַ���
//          addr����ַ��number�����ȣ�p_buff��д�����ݴ��ָ��
//addr : ����EEPROM�ĵ�ַ
//number: �������ݵĳ���
//p_buff: �������ݵ��׵�ַ
//***********************************************************************
void eprom_write(unsigned int addr, unsigned char number, unsigned char *p_buff)
{
	EEARH = 0x00;
	while (number--) {
		while (EECR & (1 << EEWE));       //�ȴ�ǰ���д���
		EEARL = addr++;                 //д��ַ
		EEDR = *p_buff++;               //д���ݵ�EEDR
		EECR |= (1 << EEMWE);            //����д������λ
		EECR &= ~(1 << EEWE);
		EECR |= (1 << EEWE);             //EEWEΪ1��ִ��д����
	}
}
//***********************************************************************
//                EEPROM��ȡ�����ַ�
//           uiAddress����ַ
//***********************************************************************

unsigned char EEPROM_read(unsigned int uiAddress)
{
	while (EECR &(1 << EEWE));  //�ȴ���һ��д��������
	EEAR = uiAddress;       //���õ�ַ�Ĵ���
	EECR |= (1 << EERE);       //��ʹ��
	return EEDR;            //���ض���EEDR���������
}

//***********************************************************************
//                EEPROM��ȡ����*/
//            addr����ַ��number�����ȣ�p_buff���������ݴ��ָ��
//addr : ��EEPROM���ݵĵ�ַ
//number: ��EEPROM���ݵĳ���
//p_buff: ��EEPROM���ݵ��ڴ���׵�ַ
//***********************************************************************
void eprom_read(unsigned int addr, unsigned char number, unsigned char *p_buff)
{
	while (EECR & (1 << EEWE));               //�ȴ�ǰ���д���
	EEARH = 0x00;                           //д���ֽڵ�ַ
	while (number--) {
		EEARL = addr++;                 //д��ַ���ֽ�
		EECR |= (1 << EERE);            //������λ��1
		*p_buff++ = EEDR;               //����EEDR�е�����
	}
}




void EEPROM_Write(unsigned int uiAddress, unsigned char ucData)
{
	while (EECR&(1 << EEWE));    //�ȴ���һ��д��������
	EEAR = uiAddress;     //���õ�ַ�Ĵ���
	EEDR = ucData;      //�������ݼĴ���
	EECR |= (1 << EEMWE);     //��λEEMWE,����дʹ��
	EECR |= (1 << EEWE);     //��λEEWE,дʹ�ܣ�����д����
}
void EEPROM_Write_Buf(unsigned int addr, unsigned char number, unsigned char *p_buff)
{
	EEARH = 0x00;
	while (number--) {
		while (EECR & (1 << EEWE));       //�ȴ�ǰ���д���
		EEARL = addr++;                 //д��ַ
		EEDR = *p_buff++;               //д���ݵ�EEDR
		EECR |= (1 << EEMWE);            //����д������λ
		EECR &= ~(1 << EEWE);
		EECR |= (1 << EEWE);             //EEWEΪ1��ִ��д����
	}
}
unsigned char EEPROM_Read(unsigned int uiAddress)
{
	while (EECR &(1 << EEWE));  //�ȴ���һ��д��������
	EEAR = uiAddress;       //���õ�ַ�Ĵ���
	EECR |= (1 << EERE);       //��ʹ��
	return EEDR;            //���ض���EEDR���������
}
void EEPROM_Read_Buf(unsigned int addr, unsigned char number, unsigned char *p_buff)
{
	while (EECR & (1 << EEWE));               //�ȴ�ǰ���д���
	EEARH = 0x00;                           //д���ֽڵ�ַ
	while (number--) {
		EEARL = addr++;                 //д��ַ���ֽ�
		EECR |= (1 << EERE);            //������λ��1
		*p_buff++ = EEDR;               //����EEDR�е�����
	}
}



void Write_Init_EEPROM(void)
{
	EEPROM_Write_Buf(0,2,Init_str);
}
unsigned char *Read_Init_EEPROM(unsigned char *Init)
{
	EEPROM_Read_Buf(0,2,Init);
	return Init;
}
unsigned char *Check_Init(void)
{
	unsigned char Init[4] = {0};
	Read_Init_EEPROM(Init);
	return strstr(Init,Init_str);
}

void Write_STATUS_EEPROM(unsigned char *status)
{
	EEPROM_Write_Buf(4,2,status);
}
unsigned char *Read_STATUS_EEPROM(unsigned char *status)
{
	EEPROM_Read_Buf(4,2,status);
	return status;
}


void Write_TIME_EEPROM(unsigned int time)
{
	unsigned char tmp[2] = {0};
	tmp[0] = time >> 8;
	tmp[1] = time ;
	EEPROM_Write_Buf(8,2,tmp);
}
unsigned int Read_TIME_EEPROM(void)
{
	unsigned char time[4] = {0};
	EEPROM_Read_Buf(8,2, time);
	return ((time[0]<<8)|time[1]);
}

void Write_FLOW_EEPROM(unsigned int flow)
{
	unsigned char tmp[2] = {0};
	tmp[0] = flow >> 8;
	tmp[1] = flow ;
	EEPROM_Write_Buf(16,2,tmp);
}
unsigned int Read_FLOW_EEPROM(void)
{
	unsigned char flow[4] = {0};
	EEPROM_Read_Buf(16,2, flow);
	return ((flow[0]<<8)|flow[1]);
}

void Write_RAW_EEPROM(unsigned char *raw)
{
	EEPROM_Write_Buf(24,4,raw);
}
unsigned char *Read_RAW_EEPROM(unsigned char *raw)
{
	EEPROM_Read_Buf(24,4,raw);
	return raw;
}

void Write_PURE_EEPROM(unsigned char *pure)
{
	EEPROM_Write_Buf(32,4,pure);
}
unsigned char *Read_PURE_EEPROM(unsigned char *pure)
{
	EEPROM_Read_Buf(32,4,pure);
	return pure;
}


void Write_CMD_EEPROM(unsigned int cmd)
{
	unsigned char *cmd_tmp[2]={0};
	cmd_tmp[0] = cmd >> 8;
	cmd_tmp[1] = cmd;
	EEPROM_Write_Buf(40,2,cmd_tmp);
}
unsigned int Read_CMD_EEPROM()
{
	unsigned char cmd_tmp[2] = {0};
	EEPROM_Read_Buf(40,2,cmd_tmp);
	return ((cmd_tmp[0]<<8) | cmd_tmp[1]);
}

void Write_COUNT_CHONGXI_EEPROM(unsigned int count_chongxi)
{
	unsigned char *tmp[2]={0};
	tmp[0] = count_chongxi >> 8;
	tmp[1] = count_chongxi;
	EEPROM_Write_Buf(44,2,tmp);
}
unsigned int Read_COUNT_CHONGXI_EEPROM()
{
	unsigned char tmp[2] = {0};
	EEPROM_Read_Buf(44,2,tmp);
	return ((tmp[0]<<8) | tmp[1]);

}

void Write_YU_SUM_FLOW_EEPROM(unsigned int yu_sum_flow)
{
	unsigned char tmp[2]={0};
	tmp[0] = yu_sum_flow >> 8;
	tmp[1] = yu_sum_flow;
	EEPROM_Write_Buf(48,2,tmp);
}
unsigned int Read_YU_SUM_FLOW_EEPROM()
{
	unsigned char tmp[2] = {0};
	EEPROM_Read_Buf(48,2,tmp);
	return ((tmp[0]<<8) | tmp[1]);

}





void Write_VIUV_TIME_EEPROM(unsigned char viuv_time)
{
	unsigned char viuv_time_tmp[8] = {0};
	itoa(viuv_time_tmp,viuv_time); 	
	EEPROM_Write_Buf(52,4,viuv_time_tmp);
}

unsigned int Read__VIUV_TIME_EEPROM(void)
{
	unsigned char viuv_time_tmp[8] = {0};
	EEPROM_Read_Buf(52,4,viuv_time_tmp);
	return atoi(viuv_time_tmp);
}
#if 1
//60-72
void Write_Filte_CntV_EEPROM(unsigned char *filte_cntv)
{
	EEPROM_Write_Buf(60,2*Filte_num,filte_cntv);
}
unsigned char *Read_Filte_CntV_EEPROM(unsigned char *filte_cntv)
{
	EEPROM_Read_Buf(60,8,filte_cntv);
	return filte_cntv;
}
//76-88
void Write_Iint_Filte_CntV_EEPROM(unsigned char *filte_cntv)
{
	EEPROM_Write_Buf(76,2*Filte_num,filte_cntv);
}
unsigned char *Read_Init_Filte_CntV_EEPROM(unsigned char *filte_cntv)
{
	EEPROM_Read_Buf(76,8,filte_cntv);
	return filte_cntv;
}
#endif

int eeprom_main(void)
{
	LED_Init();
//	LEDON;
#if 0
	//_delay_ms(1000);
	unsigned char wtest[] = "hello world", rteset,rteset1[15];
	EEPROM_write(89,'h');
	rteset = EEPROM_read(89);
	eprom_write(89,sizeof(wtest), wtest);
	eprom_read(89,sizeof(wtest),rteset1);
	if('h' == rteset)
	{
		BEEP(1);
		_delay_ms(100);
		int i = 0;
		while(1)
		{
			if(*(wtest+i) == *(rteset1+i))
			{
				BEEP(1);
				_delay_ms(100);
			}
			i++;
			
		}
		
	}
	else
		BEEP(3);
#endif

	unsigned int i , a = 0;
	for(i = 0;i < 500; i ++)
	{
		LEDRUN;
		_delay_ms(1000);
		Write_TIME_EEPROM(i);
		a = Read_TIME_EEPROM();
		if(a != i)
			LEDOFF;
		else
			BEEP(10);
	}
	while(1);
	return 0;
}