/*
 * bc95.h
 *
 * Created: 2017/12/15 17:20:51
 *  Author: 许旗
 */


#ifndef BC95_H_
#define BC95_H_

#include <string.h>
#include <util/delay.h>
#include <stdlib.h>;
#include "../uart/uart.h"


#define RESEND_NUMBER	10;				//重发次数

#define SET     1       //命令类型为设置，只返回   OK     或者ERROR
#define QUERY   2       //命令类型为询问，返回多个或一个参数
#define INFO    3       //命令类型为信息，返回一个不重要的信息

#define LOOP    1       //循环发送
#define UNLOOP  0       //不循环发送

#define BC95_COMMAND_DELAY 10


/*--------------------------------------------------
		bc95 socket 设置结构体
--------------------------------------------------*/
struct bc95_socket_config
{
	unsigned char *ip;						//服务器IP地址
	unsigned char *local_port;				//本地端口
	unsigned char *server_port;				//远程端口
	unsigned char receive_control;			//是否开启接收	1接收	0不接收
	unsigned char socket_number;			//socket编号
};

/*--------------------------------------------------
		初始化bc95结构体
--------------------------------------------------*/
struct at_init_command
{
	unsigned char type;						//命令类型
	unsigned char loop_config;				//如果发送错误是否循环发送
	unsigned char *command;					//存储命令地址
	unsigned char (*callback_function)();	//回调函数用于处理query回来的数据
};

/*-------------------------------------------------
		bc95 socket 数据发送结构体
-------------------------------------------------*/
struct bc95_send
{
	unsigned char length;					//要发送信息的长度
	unsigned char message[255];				//要发送的信息长度
};

struct bc95_send	bc95_send_socket_data;
	
/*-------------------------------------------------
		bc95 socket 数据发送结构体
-------------------------------------------------*/
struct bc95_receive
{
	unsigned char 	socket_number;			//socket编号
	unsigned char 	receive_length;			//模块接收到的数据长度
	unsigned char 	complete_length;		//单片机接收成功的长度
	unsigned char 	undone_length;			//单片机未读取信息的长度
	unsigned char 	message[255];			//接收的信息长度
};

struct bc95_receive	bc95_receive_socket_data;

struct bc95_query_data{
	unsigned char *query_name;				//询问的名字
	unsigned int   query_number;			//有多少个数据
	unsigned int  *query_list[10];			//存储数据的地址，最多可存十组数据
};

struct bc95_query_data query_data;

/*-------------------------------------------------
		bc95 的返回数据解析后存储结构体
-------------------------------------------------*/
struct bc95_query_data_flag{
	unsigned char message[255];				//存储bc95发来的临时数据
	unsigned char message_length;			//数据长度
	unsigned char add_offset;				//加号偏移位置
	unsigned char colon_offset;				//冒号偏移位置
	unsigned char comma_offset_number;		//逗号数量
	unsigned char comma_offset[10];			//逗号偏移位置数组（可能有多个逗号）
};

struct bc95_query_data_flag query_data_flag;

struct bc95_device_status{
	unsigned char band[3];		//频段
	unsigned char csq[3];		//信号强度
};

struct bc95_device_status device_status_bc95;

void init_bc95();
void bc95_reboot();
void bc95_update_display_csq();
unsigned char bc95_create_socket();

unsigned char type_set_process();
unsigned char type_info_process();
unsigned char type_query_process(unsigned char callback_function_number);


/*-------------------------------------------------
		bc95 query类型数据回调函数
-------------------------------------------------*/
unsigned char callback_get_band();
unsigned char callback_get_imei();
unsigned char callback_get_profile_status();
unsigned char callback_get_csq();
unsigned char callback_get_plmn();
unsigned char callback_get_eps_status();


/*-------------------------------------------------
		bc95 info类型数据回调函数
-------------------------------------------------*/
unsigned char callback_get_imsi();

unsigned char callback_create_socket();
#endif /* BC95_H_ */