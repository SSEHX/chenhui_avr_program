/*
 * bc95.c
 *
 * Created: 2017/12/15 17:20:41
 *  Author: 许旗
 */
#include "bc95.h"


/*---------------------------------------------------
				bc95 用于返回比较的数据
---------------------------------------------------*/
const char bc95_response_ok[]	= "\r\nOK\r\n";
const char bc95_response_error[] = "ERROR";

unsigned char bc95_reboot_command[] = "AT+NRB\r\n";
unsigned char bc95_close_socket_command[] = "AT+NSOCL=0\r\n";


/*---------------------------------------------------
				网络连接配置
---------------------------------------------------*/
unsigned char ip[] =				"119.23.25.195";
unsigned char server_port[] = 		"3000";
unsigned char local_port[] =		"3100";


/*---------------------------------------------------
				网络连接配置
---------------------------------------------------*/
struct bc95_socket_config socket_config= {									//服务器配置信息
	ip,					//ip
	local_port,			//本地端口
	server_port,		//远程端口
	1,					//启用接收
	0					//socket编号
};


/*-----------------------------------------------
发送AT指令判断模块是否存在

正常返回：	0D 0A 4F 4B 0D 0A		HEX
            \r \n O  K  \r \n 		TXET
exemple
            AT

            OK

end
-----------------------------------------------*/
unsigned  char at_check_device_command[] = "AT\r\n";

/*-----------------------------------------------
是否报告移动终端错误			目前bc95不支持:2
    0	禁用报告
    1	启用报告，结果代码并使用数字值
    2	启动报告，结果代码并使用详细的错误值
正常返回：	0D 0A 4F 4B 0D 0A	HEX
            \r \n O  K  \r \n 	TXET
exemple
            AT+CMEE=1

            OK

end
-----------------------------------------------*/
unsigned  char at_set_back_command[] = "AT+CMEE=0\r\n";

/*-----------------------------------------------
获取模块制造商信息

正常返回：	0D 0A 模块制造商 0D 0A   0D 0A 4F 4B 0D 0A	HEX
            \r \n 模块制造商 \r \n   \r \n O  K  \r \n 	TXET
exemple
            AT+CGMI

            Quectel

            OK

end  					此返回只针对bc95模块
-----------------------------------------------*/
//unsigned char command_2[] = "AT+CGMI\r\n";


/*-----------------------------------------------
获取模块设备的模块信息

正常返回：	0D 0A 模块信息 0D 0A   0D 0A 4F 4B 0D 0A		HEX
            \r \n 模块信息 \r \n   \r \n O  K  \r \n 		TXET
exemple
            AT+CGMM

            BC95HB-02-STD_850

            OK

end
-----------------------------------------------*/
//unsigned char command_3[] = "AT+CGMM\r\n";


/*-----------------------------------------------
查询模块支持的频段			目前仅支持5,8,20和28频段

正常返回：	0D 0A 2B 4E 42 41 4E 44 3A 35 0D 0A   0D 0A 4F 4B 0D 0A		HEX
            \r \n +  N  B  A  N  D  :  5  \r \n   \r \n O  K  \r \n 	TXET
exemple
            AT+NBAND?

            +NBAND:5

            OK

end
-----------------------------------------------*/
unsigned char at_get_band_command[] = "AT+NBAND?\r\n";

/*-----------------------------------------------
查询IMEI码

正常返回：	0D 0A 2B 43 47 53 4E 3A IMEI码 0D 0A   0D 0A 4F 4B 0D 0A		HEX
            \r \n +  C 	G  S  N  :  IMEI码 \r \n   \r \n O  K  \r \n 	TXET
exemple
            AT+CGSN=1

            +CGSN:863703030636570

            OK

end
-----------------------------------------------*/
unsigned char at_get_imei_command[] = "AT+CGSN=1\r\n";

/*-----------------------------------------------
获取模块设备的IMSI

正常返回：	0D 0A 模块信息 0D 0A   0D 0A 4F 4B 0D 0A		HEX
            \r \n 模块信息 \r \n   \r \n O  K  \r \n 		TXET
exemple
            AT+CIMI

            460111176388046

            OK

end
-----------------------------------------------*/
unsigned char at_get_imsi_command[] = "AT+CIMI\r\n";

/*-----------------------------------------------
开启PDP上下文支持，用于获取一个PDP地址，否则无法上网

正常返回：	0D 0A 4F 4B 0D 0A		HEX
            \r \n O  K  \r \n 		TXET
exemple
            AT+CGATT=1

            OK

end
-----------------------------------------------*/
unsigned  char at_set_pdp_command[] = "AT+CGATT=1\r\n";

/*-----------------------------------------------
查询上下文配置的状态，可能需要几秒钟				如果不成功则重复查询
    0		不成功
    1		成功

正常返回：	0D 0A 2B 43 47 41 54 54 3A 31 0D 0A 0D 0A 4F 4B 0D 0A		HEX
            \r \n +  C 	G  A  T  T  :  1  \r \n \r \n O  K  \r \n 		TXET
exemple
            AT+CGATT?

            +CGATT:1

            OK

end
-----------------------------------------------*/
unsigned  char at_get_profile_status_command[] = "AT+CGATT?\r\n";

/*-----------------------------------------------
查询信号质量
    第一返回值
        0 			-113dBm				最差
        1 			-111dBm
        2...30		-109dBm...-53dBm
        31 			-51dBm 				最好
        99			未知或不可检测
    第二返回值
        0...7		通道误码率
        99 			未知或不可检测


正常返回：	0D 0A 2B 43 53 51 3A 34 2C 39 39 0D 0A 0D 0A 4F 4B 0D 0A		HEX
            \r \n +  C  S  Q  :  4  ,  9  9  \r \n \r \n O  K  \r \n 		TXET
exemple
            AT+CSQ

            +CSQ:4,99

            OK

end
-----------------------------------------------*/
unsigned  char at_get_csq_command[] = "AT+CSQ\r\n";


/*-----------------------------------------------
PLMN选择查询						其他参数查询手册
    第一返回值
        0		自动
        2		从网络注销
    第二返回值
        0		长格式字母数字
        1		短格式的字母数字
        2		数字
    第三返回值
        GSM位置区域识别号码

正常返回：	0D 0A 2B 43 4F 50 53 3A 30 2C 32 2C 22 34 36 30 31 31 22 0D 0A 0D 0A 4F 4B 0D 0A		HEX
            \r \n +  C  O  P  S  :  0  ,  2  ,  "  4  6  0  1  1  "  \r \n \r \n O  K  \r \n 		TXET
exemple
            AT+COPS?

            +COPS:0,2,"46011"

            OK

end
-----------------------------------------------*/
unsigned  char at_get_plmn_command[] = "AT+COPS?\r\n";


/*-----------------------------------------------
EPS网络注册状态							如果不成功则重复查询

    第一个参数
        0		禁用网络注册未经请求的结果代码
        1		启用网络注册未经请求的结果代码
    第二个参数
        0		未注册，MT目前没有搜索要注册的运营商
        1		注册，家庭网络
        2		未注册，但MT正在尝试附加或搜索运营商注册


正常返回：	0D 0A 2B 43 45 52 45 47 3A 31 2C 31 0D 0A 0D 0A 4F 4B 0D 0A		HEX
            \r \n +  C  E  R  E  G  :  1  ,  1  \r \n \r \n O  K  \r \n 	TXET
exemple
            AT+CEREG?

            +CEREG:1,1

            OK

end
-----------------------------------------------*/
unsigned  char at_get_eps_status_command[] = "AT+CEREG?\r\n";

/*-------------------------------------------------
		bc95 初始化 命令设置结构体
-------------------------------------------------*/
struct at_init_command init_command[12] = {
	{
		SET,
		LOOP,
		at_check_device_command,
		NULL
	},
	{
		SET,
		UNLOOP,
		at_set_back_command,
		NULL
	},
	/*
	{
		INFO,
		UNLOOP,
		command_2,
		NULL
	},
	{
		INFO,
		UNLOOP,
		command_3,
		NULL
	},
	*/
	{
		QUERY,
		LOOP,
		at_get_band_command,
		callback_get_band
	},
	{
		QUERY,
		LOOP,
		at_get_imei_command,
		callback_get_imei
	},
	{
		INFO,
		UNLOOP,
		at_get_imsi_command,
		callback_get_imsi
	},
	{
		SET,
		LOOP,
		at_set_pdp_command,
		NULL
	},
	{
		QUERY,
		LOOP,
		at_get_profile_status_command,
		callback_get_profile_status
	},
	{		//7
		QUERY,
		UNLOOP,
		at_get_csq_command,
		callback_get_csq
	},
	{
		QUERY,
		UNLOOP,
		at_get_plmn_command,
		callback_get_plmn
	},
	{
		QUERY,
		LOOP,
		at_get_eps_status_command,
		callback_get_eps_status
	}
};


/*------------------------------------------------------------------------------
* 函数名称：init_bc95
* 功    能：初始化bc95模块，通过一系列存储在init_command结构体内的AT指令及配置
			向NB-IOT模块发送一系列AT指令获取状态，设置参数判断是否正常，并显示
			在lcd上
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void init_bc95(){
    for (unsigned char i = 0; i < 10; i++)
    {
		bc95_send_string(init_command[i].command);		
        unsigned char	resend_count = RESEND_NUMBER;       //初始化循环检查次数
		_delay_ms(BC95_COMMAND_DELAY);

        //根据发送的数据类型处理bc95返回的数据
        switch (init_command[i].type){
            case SET:
                //如果检测执行不成功则判断是否需要重新发送
                if (type_set_process() != 1){
					//如果不成功且命令需要循环发送就循环发送最大循环次数并判断是否成功
                    while(((resend_count--) > 0) && (init_command[i].loop_config == LOOP)){
						bc95_send_string(init_command[i].command);

						_delay_ms(BC95_COMMAND_DELAY);
						//如果判断成功则退出循环发送下一条指令
                        if (type_set_process() == 1){
                            break;
                        }
                    }
                }

                break;

            case QUERY:
                //如果检测执行不成功则判断是否需要重新发送
                if (type_query_process(i) != 1){
					set_bc95_query_data_flag_empty();			//清空数据标志结构体准备下一次接收
					//如果不成功且命令需要循环发送就循环发送最大循环次数并判断是否成功
                    while(((resend_count--) > 0) && (init_command[i].loop_config == LOOP)){
						bc95_send_string(init_command[i].command);

						_delay_ms(BC95_COMMAND_DELAY);
                        if (type_query_process(i) == 1){
							set_bc95_query_data_flag_empty();	//清空数据标志结构体准备下一次接收
                            break;
                        }
					set_bc95_query_data_flag_empty();			//清空数据标志结构体准备下一次接收
                    }
                }
				set_bc95_query_data_flag_empty();				//清空数据标志结构体准备下一次接收
                break;

            case INFO:
                //如果检测执行不成功则判断是否需要重新发送
                if (type_info_process(i) != 1){
					//如果不成功且命令需要循环发送就循环发送最大循环次数并判断是否成功
                    while(((resend_count--) > 0) && (init_command[i].loop_config == LOOP)){
						bc95_send_string(init_command[i].command);
						_delay_ms(BC95_COMMAND_DELAY);			//延时一段时间再发送
                        if (type_info_process(i) == 1){
							set_bc95_query_data_flag_empty();
                            break;
                        }
                    }
                }
				set_bc95_query_data_flag_empty();
                break;
            default:
                break;
        }
    }
	//创建socket连接
	device_status_bc95.socket_status = bc95_create_socket();
	if (device_status_bc95.socket_status == 'e')
	{
		//尝试关闭连接
		bc95_send_string(bc95_close_socket_command);
		_delay_ms(BC95_COMMAND_DELAY);
/*--------------------------------------
			调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("bc95 socket create error try to close socket ... \r\n");
#endif
		//重新创建socket连接，如果错误则标记socket创建失败
		device_status_bc95.socket_status = bc95_create_socket();
		if (device_status_bc95.socket_status != 'e')
		{
			uart0_send_string("bc95 socket try reconnect success ...\r\n");
			device_status_bc95.socket_status = 1;
		}else{
/*--------------------------------------
			调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("bc95 rebooting ... \r\n");
#endif
			bc95_reboot();
		}
	}
}

/*------------------------------------------------------------------------------
* 函数名称：type_set_process
* 功    能：类型为set的数据处理
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
unsigned char type_set_process(){
	unsigned char uart1_rx_array1[] = "\r\nOK\r\n";
	strcpy(query_data_flag.message, uart1_rx_data.message);
	uart1_rx_array_set_empty();
	query_data_flag.message_length = strlen(query_data_flag.message);
/*--------------------------------------
			调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("bc95 set process is run ... \r\n");
#endif
	
	if (strstr(query_data_flag.message, bc95_response_ok) != NULL)
	{
		
		uart1_rx_array_set_empty();
		return 1;
	}else if (strstr(query_data_flag.message, bc95_response_error) != NULL)
	{
		
		return 0;
	}
	uart1_rx_array_set_empty();
	return 0;
}

/*------------------------------------------------------------------------------
* 函数名称：type_info_process
* 功    能：类型为info的数据处理
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
unsigned char type_info_process(unsigned char init_command_number){
	strcpy(query_data_flag.message, uart1_rx_data.message);
	uart1_rx_array_set_empty();
	query_data_flag.message_length = strlen(query_data_flag.message);
	unsigned char cache_count = 0;
	/*--------------------------------------
					调试输出
    --------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("bc95 info process is run ... \r\n");
#endif
	
	if (strstr(query_data_flag.message, bc95_response_ok) != NULL)
	{
		//循环遍历信息
		for (unsigned int i = 0 ; i < query_data_flag.message_length ; i ++){
			if (query_data_flag.message[i] == '\r')
			{
				//如果已经找到数据开头，则再次检测到”\r“为结束
				if (query_data_flag.info_offset_start != 0)		
				{
					query_data_flag.info_offset_stop = i;
					return init_command[init_command_number].callback_function();
				}
				//如果检测到”\r“则直接看下一位是否为”\n“如果是则直接跳过/n到数据位
				if (query_data_flag.message[i+1] == '\n')		
				{
					i++;
					//设置信息开始位置偏移量
					query_data_flag.info_offset_start = i+1;
				}
			}
		}
	}	//判断是否出错，
	else if (strstr(query_data_flag.message, bc95_response_error) != NULL)
	{
		return 0;
	}else{
		return 0;
	}
	return 0;
}

/*------------------------------------------------------------------------------
* 函数名称：type_query_process
* 功    能：类型为query的数据处理

* 入口参数：unsigned char callback_function_number		初始化命令编号
	
* 出口参数：无

* 返 回 值：unsigned char						1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/

unsigned char type_query_process(unsigned char callback_function_number){
	//测试数据
	//unsigned char uart1_rx_array1[] = "\r\n+NBAND:5\r\n\r\nOK\r\n";
	//unsigned char uart1_rx_array1[] = "\r\n+CSQ:12,99\r\n\r\nOK\r\n";
	//unsigned char uart1_rx_array1[] = "\r\n+CGSN:863703030636570\r\n\r\nOK\r\n";
	//unsigned char uart1_rx_array1[] = "\r\n+CGATT:1\r\n\r\nOK\r\n";
	strcpy(query_data_flag.message, uart1_rx_data.message);	//复制串口数据
	uart1_rx_array_set_empty();							//清空串口数据用于接收
	
	uart0_send_string(query_data_flag.message);
													
	query_data_flag.comma_offset_number = 0;									//初始化逗号记录，记录有多少个逗号，就有+1个数据
	
	if (strstr(query_data_flag.message, bc95_response_ok)!= NULL && strstr(query_data_flag.message, "+") != NULL && strstr(query_data_flag.message, ":") != NULL)
	{
		query_data_flag.message_length = strlen(query_data_flag.message);				//如果接收完成，则计算字符串长度
		for (unsigned char i = 0 ; i < query_data_flag.message_length ; i++)	//循环处理字符串，将标志与信息记录在query_data_flag结构体里
		{
			if (query_data_flag.message[i] == '+')								//判断如果是”+“则记录”+“位置
			{
				query_data_flag.add_offset = i;
			}else if (query_data_flag.message[i] == ':')						//判断如果是”：“则记录”：“位置
			{
				query_data_flag.colon_offset = i;
			}else if (query_data_flag.message[i] == ',')						//判断如果是”，“则记录”，“位置，并存储在数组内
			{
				query_data_flag.comma_offset[query_data_flag.comma_offset_number] = i;
				query_data_flag.comma_offset_number++;
			}
		}		
		return init_command[callback_function_number].callback_function();		//调用回调函数，并返回处理结果
	}	//判断是否出错，
	else if (strstr(query_data_flag.message, bc95_response_error) != NULL)				//如果检测到错误标志则返回0
	{
		return 0;
	}else{
		return 0;
	}
	return 0;
}

/*------------------------------------------------------------------------------
* 函数名称：query_process
* 功    能：发送命令之后解析bc95返回的数据

* 入口参数：unsigned char (*callback)()			回调函数	用于验证解析后的数据
	
* 出口参数：无

* 返 回 值：unsigned char						1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char query_process(unsigned char (*callback)()){
	//测试数据
	//unsigned char uart1_rx_array1[] = "\r\n+NBAND:5\r\n\r\nOK\r\n";
	strcpy(query_data_flag.message, uart1_rx_data.message);							//复制串口数据
	
	uart1_rx_array_set_empty();													//清空串口数据用于接收
	query_data_flag.comma_offset_number = 0;									//初始化逗号记录，记录有多少个逗号，就有+1个数据
	
	if (strstr(query_data_flag.message, bc95_response_ok)!= NULL && strstr(query_data_flag.message, "+") != NULL && strstr(query_data_flag.message, ":") != NULL)
	{
		query_data_flag.message_length = strlen(query_data_flag.message);				//如果接收完成，则计算字符串长度
		for (unsigned char i = 0 ; i < query_data_flag.message_length ; i++)	//循环处理字符串，将标志与信息记录在query_data_flag结构体里
		{
			if (query_data_flag.message[i] == '+')								//判断如果是”+“则记录”+“位置
			{
				query_data_flag.add_offset = i;
			}else if (query_data_flag.message[i] == ':')						//判断如果是”：“则记录”：“位置
			{
				query_data_flag.colon_offset = i;
			}else if (query_data_flag.message[i] == ',')						//判断如果是”，“则记录”，“位置，并存储在数组内
			{
				query_data_flag.comma_offset[query_data_flag.comma_offset_number] = i;
				query_data_flag.comma_offset_number++;
			}
		}
		return callback();		//调用回调函数，并返回处理结果
	}	//判断是否出错，
	else if (strstr(query_data_flag.message, bc95_response_error) != NULL)
	{
		return 0;
		}else{
		return 0;
	}
	return 0;
}

/*------------------------------------------------------------------------------
* 函数名称：bc95_reboot
* 功    能：重启bc95模块
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
void bc95_reboot(){
   bc95_send_string(bc95_reboot_command);
}


/*------------------------------------------------------------------------------
* 函数名称：bc95_create_socket
* 功    能：创建一个socket连接，并将socket编号传递给socket_config.socket_number
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
unsigned char bc95_create_socket(){
	
	/*--------------------------------------
					调试输出
    --------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 start create socket ... \r\n");
#endif

    unsigned char command[30] = {0};
    unsigned char command_basic[] = "AT+NSOCR=DGRAM,17,";

    /*--------------------------------------
                    构建字符串
    --------------------------------------*/
    strcat(command, command_basic);
    strcat(command, socket_config.local_port);
    strcat(command, ',');
    strcat(command, socket_config.receive_control);
	strcat(command, "\r\n");

	uart1_rx_array_set_empty();
	//发送给bc95
	bc95_send_string(command);
	
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("bc95 create socket command ---> ");
	uart0_send_string(command);
	uart0_send_string("\r\n");
#endif
	
	strcpy(query_data_flag.message, uart1_rx_data.message);
	query_data_flag.message_length = strlen(uart1_rx_data.message);
	
	if (strstr(query_data_flag.message, bc95_response_ok) != NULL)
	{
		//循环遍历信息
		for (unsigned int i = 0 ; i < query_data_flag.message_length ; i ++){
			if (query_data_flag.message[i] == '\r')
			{
				//如果已经找到数据开头，则再次检测到”\r“为结束
				if (query_data_flag.info_offset_start != 0)
				{

					socket_config.socket_number = query_data_flag.message[query_data_flag.info_offset_start];
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
					uart0_send_string("bc95 get socket number ---> ");
					uart0_send_byte(socket_config.socket_number);
					uart0_send_string("\r\n");
#endif					
					return 1;
				}
				//如果检测到”\r“则直接看下一位是否为”\n“如果是则直接跳过/n到数据位
				if (query_data_flag.message[i+1] == '\n')
				{
					i++;
					//设置信息开始位置偏移量
					query_data_flag.info_offset_start = i+1;
				}
			}
		}
		
	}//没有成功标识，表示失败
	else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
					uart0_send_string("bc95 get socket number ---> ERROR \r\n");
#endif	
		
		return 'e';
	}
}

/*------------------------------------------------------------------------------
* 函数名称：bc95_send_socket
* 功    能：通过bc95发送一串数据到指定服务器，用于构建发送命令，并发送

* 入口参数：struct bc95_send send_data

* 出口参数：无

* 返 回 值：unsigned char 发送是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char bc95_send_socket(struct bc95_send send_data){
	
/*--------------------------------------
			调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 start send socket ... \r\n");
#endif

	unsigned char command[100] = {0};
	unsigned char command_basic[] = "AT+NSOST=";
	
	strcat(command, command_basic);
	strcat(command, socket_config.socket_number);
	strcat(command, ',');
	strcat(command, socket_config.ip);
	strcat(command, ',');
	strcat(command, socket_config.server_port);
	strcat(command, ',');
	strcat(command, send_data.length);
	strcat(command,	',');
	strcat(command, send_data.message);
	
	bc95_send_string(command);
	
	
}


/*------------------------------------------------------------------------------
* 函数名称：set_bc95_query_data_flag_empty
* 功    能：清空 query_data_flage	结构体
* 入口参数：无
* 出口参数：无
* 返 回 值：无
*-----------------------------------------------------------------------------*/
inline void set_bc95_query_data_flag_empty(){
	memset(&query_data_flag , 0, sizeof(struct bc95_query_data_flag));
}


/*------------------------------------------------------------------------------
* 函数名称：callback_get_imei
* 功    能：解析bc95返回的数据，读取imei
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_imei(){
	unsigned char name[] = "CGSN";
	unsigned char cache_count = 0;
	
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 get imei is run ... \r\n");
#endif

	if (strstr(query_data_flag.message, name) != NULL)			//判断是否是需要的返回值
	{
		//从”：“开始检查并存储数据到device_status_bc95.band	直到碰见“\r”数据结束
		for (unsigned char i = query_data_flag.colon_offset+1 ; i < query_data_flag.message_length ; i++)
		{
			if (query_data_flag.message[i] != '\r')
			{
				device_status_bc95.imei[cache_count] = query_data_flag.message[i];
				cache_count++;
			}else{
				if ((i - query_data_flag.colon_offset) != 16)
				{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get imei ----> NUMBER ERROR \r\n");
#endif
					return 0;
				}
/*--------------------------------------
			调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get imei ----> ");
				uart0_send_string(device_status_bc95.imei);
				uart0_send_string(" \r\n");
#endif
				
				return 1;
			}
		}
	}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get imei ----> ERROR \r\n");
#endif
		return 0;
	}
}


/*------------------------------------------------------------------------------
* 函数名称：callback_get_band
* 功    能：解析bc95返回的数据，读取band
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_band(){
	unsigned char name[] = "NBAND";
	unsigned char cache_count = 0;
	
/*--------------------------------------
调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 get band is run ... \r\n");
#endif

	if (strstr(query_data_flag.message, name) != NULL)			//判断是否是需要的返回值
	{
		//从”：“开始检查并存储数据到device_status_bc95.band	直到碰见“\r”数据结束
		for (unsigned char i = query_data_flag.colon_offset+1 ; i < query_data_flag.message_length ; i++)
		{
			if (query_data_flag.message[i] != '\r')
			{
				device_status_bc95.band[cache_count] = query_data_flag.message[i];
				cache_count++;
			}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get band ----> ");
				uart0_send_string(device_status_bc95.band);
				uart0_send_string(" \r\n");
#endif
				
				return 1;
			}
		}
	}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get band ----> ERROR  \r\n");

#endif
		return 0;
	}
}


/*------------------------------------------------------------------------------
* 函数名称：callback_get_csq
* 功    能：解析bc95返回的数据，读取csq信号强度
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_csq(){
	unsigned char name[] = "CSQ";
	unsigned char cache_count = 0;
	
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("--------------------------------------\r\n");
		uart0_send_string("bc95 get csq is run ... \r\n");
#endif

	if (strstr(query_data_flag.message, name) != NULL)	//判断是否存在返回的需要的相关数据
	{
		//循环存储数据，从“：”开始为数据，碰到“，”即为结束
		for (unsigned char i = query_data_flag.colon_offset+1 ; i < query_data_flag.message_length ; i++)
		{
			if (query_data_flag.message[i] != ',')		//如果碰不到“，”则继续接收
			{
				device_status_bc95.csq[cache_count] = query_data_flag.message[i];
				cache_count++;						
			}else{										//碰到“，”则返回1			
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get csq ----> ");
				uart0_send_string(device_status_bc95.csq);
				uart0_send_string(" \r\n");
#endif
				return 1;
			}
		}
	}else{												//如果没有检测到相关的返回数据，则返回0
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get csq ----> ERROR \r\n");
#endif
		return 0;
	}
		return 0;
}

/*------------------------------------------------------------------------------
* 函数名称：callback_get_imsi
* 功    能：解析bc95返回的数据，获取imsi号
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_imsi(){
	unsigned char cache_count = 0;
	
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 get imsi is run ... \r\n");
#endif
	//判断获取到的imsi是否为15位如果不是则返回0
	if ((query_data_flag.info_offset_stop - query_data_flag.info_offset_start) != 15)
	{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get imsi ---> NUMBER ERROR \r\n");
#endif
		return 0;
	}
	//从起始偏移量开始复制到当前位置，即为imsi的数据,并返回
	for (unsigned int i = query_data_flag.info_offset_start ; i < query_data_flag.info_offset_stop ; i++)
	{
		device_status_bc95.imsi[cache_count] = query_data_flag.message[i];
		cache_count++;
	}

/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("bc95 get imsi ---> ");
	uart0_send_string(device_status_bc95.imsi);
	uart0_send_string(" \r\n");
#endif
	return 1;
}

/*------------------------------------------------------------------------------
* 函数名称：callback_get_eps_status
* 功    能：解析bc95返回的数据，获取网络注册状态
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_eps_status(){
	unsigned char name[] = "CEREG";
	
/*--------------------------------------
调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 get eps is run ... \r\n");
#endif
	//判断是否是需要的返回值
	if (strstr(query_data_flag.message, name) != NULL)			
	{
		//判断第一位是否位1个字符，如果是则继续判断第二个参数是否位1，如果不是则返回0 注册失败重新查询
		if ((query_data_flag.comma_offset[0] - query_data_flag.colon_offset) == 2)
		{
			if (query_data_flag.message[query_data_flag.comma_offset[0]+1] != '1')
			{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get eps ----> NUMBER ERROR \r\n");
#endif
				return 0;
			}else{
				//从”：“开始存储数据到device_status_bc95.eps_status	只拷贝1位
				device_status_bc95.eps_status = query_data_flag.message[query_data_flag.colon_offset+1];
				//从”，“开始存储数据到device_status_bc95.eps_registered_status	只拷贝1位
				device_status_bc95.eps_registered_status = query_data_flag.message[query_data_flag.comma_offset[0]+1];
				
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get eps ----> ");
				uart0_send_byte(device_status_bc95.eps_status);
				uart0_send_byte(',');
				uart0_send_byte(device_status_bc95.eps_registered_status);
				uart0_send_string(" \r\n");
#endif
				return 1;
			}
		}
		
		
	}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get eps ----> ERROR \r\n");
#endif
		return 0;
	}
	return 0;
}

/*------------------------------------------------------------------------------
* 函数名称：callback_get_profile_status
* 功    能：解析bc95返回的数据，获取上下文配置状态
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_profile_status(){
	unsigned char name[] = "CGATT";
	
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 get profile status is run ... \r\n");
#endif

	if (strstr(query_data_flag.message, name) != NULL)			//判断是否是需要的返回值
	{
		if (query_data_flag.message[query_data_flag.colon_offset+1] == '1' || query_data_flag.message[query_data_flag.colon_offset+1] == 1)
		{
			device_status_bc95.profile_status = query_data_flag.message[query_data_flag.colon_offset+1];
			
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get profile status ----> ");
				uart0_send_byte(device_status_bc95.profile_status);
				uart0_send_string(" \r\n");
#endif
			return 1;
		}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
				uart0_send_string("bc95 get profile status ----> ERROR \r\n");
#endif
			return 0;
		}		
	}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get profile status ----> ");
		uart0_send_string("ERROR");
		uart0_send_string(" \r\n");
#endif
		return 0;
	}
}

/*------------------------------------------------------------------------------
* 函数名称：callback_get_plmn
* 功    能：解析bc95返回的数据，获取plmn状态和gsm位置区域识别号码
* 入口参数：无
* 出口参数：无
* 返 回 值：unsigned char 获取是否成功，	1--->成功	0--->不成功
*-----------------------------------------------------------------------------*/
unsigned char callback_get_plmn(){
		unsigned char name[] = "COPS";
		unsigned char cache_count = 0;
	
/*--------------------------------------
调试输出
--------------------------------------*/
#ifdef	DEBUG
	uart0_send_string("--------------------------------------\r\n");
	uart0_send_string("bc95 get plmn is run ... \r\n");
#endif
	//判断是否是需要的返回值
	if (strstr(query_data_flag.message, name) != NULL)			
	{
		if (query_data_flag.comma_offset_number != 2)
		{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
			uart0_send_string("bc95 get plmn ----> ERROR \r\n");
#endif
			return 0;
		}
		//判断第一位是否位1个字符，如果是则继续判断第二个参数是否为1个字符，如果不是则返回0 注册失败重新查询
		if ((query_data_flag.comma_offset[0] - query_data_flag.colon_offset) == 2)
		{
			if ((query_data_flag.comma_offset[1] - query_data_flag.comma_offset[0]) == 2)
			{
				//存储plmn状态
				device_status_bc95.plmn_status = query_data_flag.message[query_data_flag.colon_offset+1];
				
				//循环存储gsm位置区域识别号，碰到‘\r’即为结束，并返回
				for (unsigned int i = query_data_flag.comma_offset[1]+1; i < query_data_flag.message_length ; i++)
				{
					if (query_data_flag.message[i] != '\r')
					{
						device_status_bc95.gsm_location_number[cache_count] = query_data_flag.message[i];
						cache_count++;
					}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
			uart0_send_string("bc95 get plmn ----> ");
			uart0_send_byte(device_status_bc95.plmn_status);
			uart0_send_byte(',');
			uart0_send_string(device_status_bc95.gsm_location_number);
			uart0_send_string(" \r\n");
#endif
						return 1;
					}
				}
			}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
			uart0_send_string("bc95 get plmn ----> ERROR \r\n");
#endif
			}
		}
	}else{
/*--------------------------------------
				调试输出
--------------------------------------*/
#ifdef	DEBUG
		uart0_send_string("bc95 get plmn ----> ERROR \r\n");
#endif
		return 0;
	}
	return 0;
}


/*
[2017-12-19 20:52:31:560_S:] AT+NSOST=0,119.23.26.195,9100,1,48
[2017-12-19 20:52:31:604_R:]
[2017-12-19 20:52:31:613_R:] 0,1

[2017-12-19 20:52:31:616_R:] OK
[2017-12-19 20:52:32:297_R:]
[2017-12-19 20:52:32:303_R:] +NSONMI:0,3
[2017-12-19 20:52:45:891_S:] AT+NSORF=0,3

[2017-12-19 20:52:45:913_S:] AT+NSORF=0,3
[2017-12-19 20:52:45:916_R:] 0,119.23.26.195,9100,3,616263,0

[2017-12-19 20:52:45:956_R:] OK
*/








