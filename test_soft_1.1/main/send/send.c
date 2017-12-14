/*
 * send.c
 *
 * Created: 2017/10/30 09:44:18
 *  Author: HXHZQ
 */ 

//#define	 F_CPU	11059200UL//16000000UL		// ��Ƭ����ƵΪ7.3728MHz,������ʱ�ӳ���

#include "send.h"




//const unsigned char  Heart[4]= {0xAC,0x33,0x33,0x80};
//const unsigned char  Heart[4]= {0xAB,0x33,0x33,0x80};
const unsigned char  Heart[4]= {0xBB, 0x66, 0x66, 0x80};
//const unsigned char	 Rece_Heart[4] = {0xBB, 0x66, 0x66, 0x80};

const unsigned char	Tail[2] = {0xFC,0xFF};
//const unsigned char Rece_Tail[2] = {0x3F, 0x3F};

const unsigned char  Device[] = "A1111378";


unsigned char *Str_Add_Str(unsigned char *str1,unsigned char n1,unsigned char *str2,unsigned char n2)
{
	unsigned char i;
	for(i = 0; i < n2; i++)
	{
			str1[n1+i] = str2[i];
	}
		return str1;
}

void make_status(unsigned char *status,unsigned int status_num)
{
	
	status[0] |= (status_num & (0x01<< 0 ));//�豸����״̬��0:�ػ� 1:����
	status[0] |= (status_num & (0x01<< 1 ));//��濪��״̬��0:�ػ� 1:����
	status[0] |= (status_num & (0x01<< 2 ));//���俪��״̬��0:�ػ� 1:����
	status[0] |= (status_num & (0x01<< 3 ));//���ȿ���״̬��0:�ػ� 1:����
// 	����
//	status[0] |= (status_num & (0x01<< 4));
//	status[0] |= (status_num & (0x01<< 5));
//	status[0] |= (status_num & (0x01<< 6));
//	status[0] |= (status_num & (0x01<< 7));
// 	
	status_num >>= 8;
	
	status[1] |= (status_num & (0x01<< 0 ));//�������״̬��0:���� 1:����
	status[1] |= (status_num & (0x01<< 1 ));//�������״̬��0:���� 1:����
	status[1] |= (status_num & (0x01<< 2 ));//��ˮ���״̬��0:���� 1:����
	status[1] |= (status_num & (0x01<< 3 ));//��ˮ���״̬��0:ֹͣ 1:���
	
	status[1] |= (status_num & (0x01<< 4 ));//��ϴ���״̬��0:ֹͣ 1:���
	status[1] |= (status_num & (0x01<< 5 ));//�������״̬��0:���� 1:����
	status[1] |= (status_num & (0x01<< 6 ));//
	status[1] |= (status_num & (0x01<< 7 ));//
}
//	Parameter:���͵Ĳ�����Status:״̬��Tds1��Tds2��WaterCnt:��ˮ����,Filte_num:��о�ĸ���,
unsigned char  make_parameter(unsigned char *Parameter,unsigned char *Status,unsigned char *Tds1,unsigned char *Tds2,unsigned int WaterCnt,unsigned char *Filte_CntV)
{
	unsigned char tmp[2] = {0};
	Str_Add_Str(Parameter,0,Status,2);
	tmp[0] = atoi(Tds1)>>8;
	tmp[1] = atoi(Tds1);
	Str_Add_Str(Parameter,2,tmp,2);
	tmp[0] = atoi(Tds2)>>8;
	tmp[1] = atoi(Tds2);
	Str_Add_Str(Parameter,4,tmp,2);
	tmp[0] = (WaterCnt)>>8;
	tmp[1] = (WaterCnt);
	Str_Add_Str(Parameter,6,tmp,2);
	//Str_Add_Str(Parameter,8,LimitWaterCnt,2);
	tmp[0] = Filte_num;	
	Str_Add_Str(Parameter,8,tmp,1);
	Str_Add_Str(Parameter,9,Filte_CntV,2*Filte_num);
	return (9+2*Filte_num);
}


/*   4     2        8      2       len    2         2       */
//����ͷ+���ݳ���+�豸��+������+��������+RCR��֤��+������
static unsigned char * send_data(unsigned char *SendData,unsigned char *Cmd,unsigned char *Parameter,unsigned char len)
{
	unsigned char len_tmp[2],rcr_tmp[2];
	unsigned int rcr = 0;
	len_tmp[1] = (16 + len);
	len_tmp[0] = (16 + len) >> 8;

	Str_Add_Str(SendData, 0, Heart, 4);
	
	//strncat(SendData , len_tmp , 1);
	Str_Add_Str(SendData, 4,len_tmp, 2);
	//USART0_Transmit_nChar(SendData,6);
	//strncat(SendData , Device, 8);
	Str_Add_Str(SendData, 6,Device, 8);
	//USART0_Transmit_nChar(SendData,14);
	//strncat(SendData , Cmd , 2);
	Str_Add_Str(SendData, 14,Cmd, 2);
	//USART0_Transmit_nChar(SendData,16);
	//strncat(SendData , Parameter, len);
	Str_Add_Str(SendData, 16,Parameter, len);
	//USART0_Transmit_nChar(SendData,16+len);
	//USART0_Transmit_String(Parameter);
	//rcr��֤�ĳ�����16+len
	rcr = CRC16( SendData,0, 16 + len);
	rcr_tmp[0] = rcr  ;//0�ǵ�λ
	rcr_tmp[1] = rcr >> 8;//1�Ǹ�λ
	//USART0_Transmit_nChar(rcr_tmp,2);
	//USART0_Transmit_String("send_data\r\nAAAAAA");
	//USART0_Transmit_nChar(rcr_tmp,2);
	//USART0_Transmit_nChar(SendData,16 + len);
	Str_Add_Str(SendData, 16+len,rcr_tmp, 2);

	Str_Add_Str(SendData ,18+len, Tail , 2);
 //	if(!Check_CRC16(SendData,0,16+len))
 //			BEEP(10);
	//USART0_Transmit_nChar(SendData,20+len);
	//USART0_Transmit_nChar(SendData,20 + len);
	return SendData;
}
/*
*Parameter : �豸��״̬
*/
unsigned char  Send_Status(unsigned char *Parameter,unsigned char *Ack)
{	
	//SendData �ĳ��ȵ���20+len
	unsigned char SendData[22] = {0}; 
	unsigned char Cmd[2] = {0x00,0x01};
	send_data(SendData,Cmd,Parameter,2);
	//USART1_Transmit_String(SendData);
	//USART0_Transmit_String(SendData);
	//USART0_Transmit_nChar(SendData,22);
	return (GPRS_Send_Data(SendData,22,Ack));
}

unsigned char  Send_Cmd_Parameter(unsigned char *Cmd,unsigned char *Parameter,unsigned char P_Len,unsigned char *Ack)
{
	//LEDON;
		//SendData �ĳ��ȵ���20+len
		unsigned char SendData[100] = {0};
		unsigned char len = 0;
		send_data(SendData,Cmd,Parameter,P_Len);
		
		len = (ShortTurl(SendData,20+P_Len,Ack));
		//return (GPRS_Send_Data(SendData,20+P_Len,Ack));
		//memset(Ack,0,sizeof(Ack));
// 		if(len)
// 			USART0_Transmit_nChar(Ack,len);
		return len;
//		return 1;
}

unsigned char GET_Parameter(unsigned char *Cmd,unsigned char *Parameter,unsigned char P_Len,unsigned char *Ack_Parameter)
{
	unsigned char Ack[100],Ack_len = 0;
	Ack_len = Send_Cmd_Parameter(Cmd,Parameter,P_Len,Ack);
	//LEDON;
	
	if(Ack_len > 20)
	{
		//LEDOFF;
		//USART0_Transmit_nChar(Ack,Ack_len);
		Str_Add_Str(Ack_Parameter,0,&Ack[16],Ack_len-20);
		return (Ack_len-20);
	}
	//while(1);
	
	return 0;
}

unsigned char  GET_Time(void)
{
	unsigned char Cmd[] = {0x00,0x02};
	unsigned char Parameter[]={0x00,0x00,0x00,0x00,0x71,0x00,0x74,0x00,0x00,0x00};
	unsigned char time[5] = {0};				
	if(GET_Parameter(Cmd,Parameter,sizeof(Parameter),time))
	{
		//time_set(time);
		//Write_TIME_EEPROM(time);
		return 0;
	}
	return 1;
}

unsigned int GET_CMD(unsigned char *Parameter,unsigned char Parameter_len)
{
	
	unsigned char Cmd[2] = {0x00,0x09};
	unsigned char GET_cmd[4] = {0};	
	unsigned int deal_cmd = 0xFFFF;	
//	memset(GET_cmd,1,4);
	if(GET_Parameter(Cmd,Parameter,Parameter_len,GET_cmd))
	{	
//		LEDON ;
		//USART0_Transmit_nChar(GET_cmd,2);
// 		USART0_Transmit_String("----------------------");
// 		USART0_Transmit(GET_cmd[0]);
// 		USART0_Transmit(GET_cmd[1]);
		deal_cmd = (GET_cmd[0]<<8)|GET_cmd[1];		
	}
	return deal_cmd;
}

unsigned char ERROR_HYDROPENIA(void)//ȱˮ����
{
	unsigned char Cmd[2] = {0x00,0x04};
	unsigned char Parameter[3]={0x00,0x00,0x02};
	unsigned char error = 0xFF;				
	GET_Parameter(Cmd,Parameter,sizeof(Parameter),&error);
	return error;
}

unsigned char ERROR_SEND(unsigned char *Parameter)
{
	unsigned char Cmd[] = {0x00,0x04};
	//unsigned char Parameter[]={0x00,0x00,0x00};
	unsigned char error = 0xFF;				
	GET_Parameter(Cmd,Parameter,sizeof(Parameter),&error);
	return error;
}

unsigned char ERROR_LEAKAGE(void)//©ˮ����
{
	unsigned char Parameter[]={0x10,0x00,0x00};
	unsigned char Cmd[] = {0x00,0x04};	
	unsigned char error = 0xFF;				
	(GET_Parameter(Cmd,Parameter,sizeof(Parameter),&error));
	return error;

	//return ERROR_SEND();
}

void REPLY_SEVICE(unsigned char Parameter)
{
	unsigned char Cmd[] = {0x00,0x16};
	//unsigned char Parameter[]={0x00,0x00,0x00};
	unsigned char error = 0xFF;				
	GET_Parameter(Cmd,&Parameter,1,&error);
	return error;
}
int send_main(void)
{
	
	cli();
	//ASSR |= (1<<AS0);
	USART0_Init();
	USART1_Init();
	sei();
	
	unsigned char Parameter[2]={0x00,0x01};
	//	USART0_Transmit_nChar(Parameter,2);
	unsigned char Cmd[4] = {0x00, 0x09};
	unsigned char Ack[22] = {0};
	//Str_Add_Str(Cmd,2,Parameter,2);
	//USART0_Transmit_nChar(Cmd,4);
	Send_Status(Parameter,Ack);
	
	while(1);
	
	
	return 0;
}