;这是为广东世纪丰源饮水设备公司开发的智能饮水机PT板源程序
;单片机型号:ATMEGA64_TQFP64
;晶振:16MHZ,单指令周期约为0.0625US
;最新版本=V01.00 
;各版本说明如下：



;*****************************************
;程序前定义区
;*****************************************
;************************************
.include "m64def.inc"
;************************************
;口地址定义
;************************************
;PA口位定义:全口为输出
.EQU   BVOPUL    =0       ;语音脉冲冲输出位 H
.EQU   BVORET    =1       ;语音复位输出位   H
.EQU   BDCS      =2       ;显示屏片选位     L
.EQU   BDWR      =3       ;显示屏写使能位   L
.EQU   BDDATA    =4       ;显示屏数据位     L/H
.EQU   BBUZZ     =5       ;蜂鸣器输出位     H
;.EQU  B         =6       ;未用
;.EQU  B         =7       ;示用
;------------------------------------
;PB口位定义:全口为输入
;.EQU   BKRS     =0       ;右移键      H
;.EQU   BKDown   =1       ;下移键      H
;.EQU   BKSet    =2       ;设置键      H
;.EQU   BKUp     =3       ;上移键      H
.EQU   BKKS      =4       ;开水键      H
.EQU   BKWS      =5       ;温水键      H
.EQU   BKBS      =6       ;冰水键      H
.EQU   BKBY1     =7       ;备用键1     H
;------------------------------------
;PC口位定义:全口为输入
.EQU   BKBY2     =0       ;备用键2     H
.EQU   BKBY3     =1       ;备用键3     H
.EQU   B90DI     =2       ;90度开关入,超温H
.EQU   B85DI     =3       ;85度开关入,超温H
.EQU   BYSKI     =4       ;源水位开关入,缺水H
.EQU   BRSKI     =5       ;热水位开关入,缺水H
.EQU   BSSKI     =6       ;上水位开关入,缺水H
.EQU   BXSKI     =7       ;下水位开关入,缺水H
;------------------------------------
;PD口位定义:输入口
;.EQU   SCL      =0       ;I2C
;.EQU   SDA      =1       ;I2C
.EQU   BRKKS     =2       ;读卡（机）开水 H  
.EQU   BRKWS     =3       ;读卡（机）温水2 H
.EQU   BRKBS     =4       ;读卡（机）冰水3 H
.EQU   CUPIN     =5       ;水杯检测输入    L
;.EQU   T/C1     =6       ;流量计数1
;.EQU   T/C21    =7       ;流量计数2
;------------------------------------
;PE口位定义:输出口,6/7为输入
.EQU   BWSFO     =0       ;温水阀输出  H
.EQU   BKSFO     =1       ;开水阀输出  H
.EQU   BJSFO     =2       ;进水阀输出  H
.EQU   BPSFO     =3       ;排水阀输出  H
.EQU   BAZJRO    =4       ;A组加热输出 H
.EQU   BBZJRO    =5       ;B组加热输出 H
;.EQU   T/C3     =6       ;流量计数3
.EQU   BLEAKI    =7       ;漏水输入    H
;------------------------------------
;PF口位定义:全口为输入
;       ADC0     =0       ;环境NTC
;       ADC1     =1       ;开水NTC
;       ADC2     =2       ;温水NTC
;       ADC3     =3       ;超高温NTC
.EQU   BBMKI1    =4       ;拨码开关输入1
.EQU   BBMKI2    =5       ;拨码开关输入2
.EQU   BBMKI3    =6       ;拨码开关输入3 
.EQU   BVOEFI    =7       ;语音结束反馈输入
;------------------------------------
;PG口定义：全口输出
.EQU   BLEDYX    =0       ;LED运行
.EQU   BLEDQS    =1       ;LED取水
.EQU   BLEDLJ    =2       ;LED联机
.EQU   BLEDBJ    =3       ;LED报警
;                =4       ;未用
;********************************************
;********************************************
;单片机前16个通用寄存器(R0~R15)定义:
;-----------------------------------
;R0~R7为主程序专用!! 
.DEF   R0M       =R0 
.DEF   R1M       =R1        
.DEF   R2M       =R2
.DEF   R3M       =R3
.DEF   R4M       =R4       
.DEF   R5M       =R5       
.DEF   KeyRE     =R6       ;键上升沿值  @@ 
;------------------
 .EQU   BKRS     =0        ;右移键
 .EQU   BKDown   =1        ;下移键
 .EQU   BKSet    =2        ;设置键
 .EQU   BKUp     =3        ;上移键
;---------------------
.DEF    FlagM    =R7       ;主程序专用标志
 .EQU   BITKS    =0        ;初次用开水
 .EQU   BLCDD    =1        ;LCD显示
 .EQU   B1LTO    =2        ;1升到了
 .EQU   BKeyS    =3        ;在键盘状态标志 
 .EQU   BNkOv    =4        ;无键超时
 .EQU   WSSBIT   =5        ;
 .EQU   Sale     =6        ;(售水)取水状态
;.EQU            =7        ;
;----------------------------------
;R8~R15为中断程序专用!!
.DEF    R8I      =R8        
.DEF    R9I      =R9
.DEF    R10I     =R10

.DEF    RAL1     =R11       ;报警值1 
.DEF    RAL2     =R12       ;报警值2
.DEF    RCNT     =R13       ;接收计数
.DEF    SREG_B   =R14
;--------------------
.DEF    FlagI    =R15       ;中断专用标志
 .EQU   HostM    =0         ;TWI为主机模式
 .EQU   BTZR     =1         ;TWI发转收
 .EQU   BCLKS    =2         ;在时钟态
;.EQU            =3        ;
;.EQU            =4        ;
;.EQU            =5        ;
;.EQU            =6        ;
;.EQU            =7        ;
;-----------------------------------
;单片机后16个通用寄存器(R16~R31)定义:
.DEF   BUFM      =R16      ;主程序专用BUF
.DEF   CNTM      =R17      ;主程序专用CNT
.DEF   BUFMS     =R18      ;主程序的子程专用BUF
.DEF   CNTMS     =R19      ;主程序的子程专用CNT
.DEF   BUFI      =R20      ;中断程序专用BUF
.DEF   CNTI      =R21      ;中断程序专用CNT
.DEF   BUFIS     =R22      ;中断程序的子程专用BUF
.DEF   CNTIS     =R23      ;中断程序的子程专用CNT
;--------------------
.DEF   WLI       =R24      ;W和Y指针为中断专用
.DEF   WHI       =R25
;.DEF  XL        =R26      ;X和Z指针为主程序专用
;.DEF  XH        =R27
.DEF   YLI       =R28
.DEF   YHI       =R29
;.DEF  ZL        =R30
;.DEF  ZH        =R31
;************************************
;MEGA64单片机内部SRAM定义:
;0x0100~0x0FFF=3840 为内部一般用SRAM
;0x1000~0x10FF=256  为内部堆栈用SRAM
.EQU    SramStart =0x0100
;************************************
;1、程序最常用SRAM（0x0100~0x011F=32）
;------------------------------------
.EQU   Message1  =SramStart  ;消息1
  .EQU   B20MST    =0        ;20MS处理任务标志
  .EQU   BI2CMRT   =1        ;I2C主机收数处理任务标志
  .EQU   BI2CSRT   =2        ;I2C从机收数处理任务标志
  .EQU   B100MST   =3        ;100MS处理任务标志
  .EQU   B300MST   =4        ;300MS处理任务标志
  .EQU   B500MST   =5        ;500MS处理任务标志
  .EQU   B1ST      =6        ;1秒处理任务标志
; .EQU             =7        ;
.EQU   Message2  =0x0101   ;消息2
.EQU   Message3  =0x0102   ;消息3
;--------------
.EQU   POT100MS  =0x0103   ;程序对象定时100mS
.EQU   POT300MS  =0x0104   ;程序对象定时300mS
.EQU   POT500MS  =0x0105   ;程序对象定时500mS
.EQU   POT1S     =0x0106   ;程序对象定时1S
;.EQU            =0x0107   ;程序对象定时



   
;END:0x011F   
;-----------------------
;2、I/O口专用SRAM：0120~013F=32
;-----------------------
.EQU   PortTIVA  =0x0120   ;A口临时输入值
.EQU   PortIVA   =0x0121   ;A口输入值
.EQU   PortTOVA  =0x0122   ;A口临时输出值
.EQU   PortOVA   =0x0123   ;A口输出值
.EQU   PortTIVB  =0x0124   ;B口临时输入值
.EQU   PortIVB   =0x0125   ;B口输入值
.EQU   PortTOVB  =0x0126   ;B口临时输出值
.EQU   PortOVB   =0x0127   ;B口输出值
.EQU   PortTIVC  =0x0128   ;C口临时输入值
.EQU   PortIVC   =0x0129   ;C口输入值
.EQU   PortTOVC  =0x012A   ;C口临时输出值
.EQU   PortOVC   =0x012B   ;C口输出值
.EQU   PortTIVD  =0x012C   ;D口临时输入值
.EQU   PortIVD   =0x012D   ;D口输入值
.EQU   PortTOVD  =0x012E   ;D口临时输出值
.EQU   PortOVD   =0x012F   ;D口输出值
.EQU   PortTIVE  =0x0130   ;E口临时输入值
.EQU   PortIVE   =0x0131   ;E口输入值
.EQU   PortTOVE  =0x0132   ;E口临时输出值
.EQU   PortOVE   =0x0133   ;E口输出值
.EQU   PortTIVF  =0x0134   ;F口临时输入值
.EQU   PortIVF   =0x0135   ;F口输入值
.EQU   PortTOVF  =0x0136   ;F口临时输出值
.EQU   PortOVF   =0x0137   ;F口输出值
.EQU   PortTIVG  =0x0138   ;G口临时输入值
.EQU   PortIVG   =0x0139   ;G口输入值
.EQU   PortTOVG  =0x013A   ;G口临时输出值
.EQU   PortOVG   =0x013B   ;G口输出值
;-----------------------------


;END:0x013F
;-----------------------------
;3、键盘专用SRAM：0140~014F=16
;-----------------------------
.EQU   KeyIV     =0x0140    ;键盘输入值  
.EQU   KPCnt     =0x0141    ;键盘设置参数计数
.EQU   KP2Cnt    =0x0142    ;键盘设置2级参数计数
.EQU   KRSCnt    =0x0143    ;键盘设置右移计数
.EQU   KDelay    =0x0144    ;键盘设置延时关
.EQU   KFTCnt    =0x0145    ;键盘设置闪烁时间计数
.EQU   KFMCnt    =0x0146    ;键盘设置闪烁暂存
.EQU   PSETDH    =0x0147    ;参数设置十进制数H
.EQU   PSETDL    =0x0148    ;参数设置十进制数L

;END:0x014F
;-----------------------------
;4、ADC专用SRAM：0150~016F=32
;-----------------------------
.EQU   ADV16SH   =0x0150   ;16次ADC值存储H
.EQU   ADV16SL   =0x0151   ;16次ADC值存储L
.EQU   ADV16CNT  =0x0152   ;16次计数
;------
.EQU   ADVSH0    =0x0153   ;AD值存储H0
.EQU   ADVSL0    =0x0154   ;AD值存储L0
.EQU   ADVSH1    =0x0155   ;AD值存储H1
.EQU   ADVSL1    =0x0156   ;AD值存储L1
.EQU   ADVSH2    =0x0157   ;AD值存储H2
.EQU   ADVSL2    =0x0158   ;AD值存储L2
.EQU   ADVSH3    =0x0159   ;AD值存储H3
.EQU   ADVSL3    =0x015A   ;AD值存储L3

;END:0x017F
;-----------------------
;5、报警（含语音、闪灯）专用SRAM：0180~018F
;-----------------------
.EQU    VODS      =0x0180   ;语音段数
.EQU    VOCNT     =0x0181   ;语音段数
.EQU    VOTCNTKS  =0x0182   ;语音计数（开水）
.EQU    VOTCNTWS  =0x0183   ;语音计数（温水）
.EQU    VOLine1   =0x0184   ;语音排队1
.EQU    VOLine2   =0x0185   ;语音排队2
.EQU    VOLine3   =0x0186   ;语音排队3


.EQU    MLEDFFCNT =0x018A   ;LED快闪计数
.EQU    MLEDFFEN  =0x018B   ;LED快闪使能


;END:0x018F
;-----------------------
;6、水温控管理专用SRAM：0190~01AF=32
;-----------------------
.EQU    CNT3SKS   =0x0190    ;计数3秒（开水）
.EQU    WRequest  =0x0191    ;水请求
;-----------
.EQU    FlowAPH   =0x0195    ;流量总脉冲H
.EQU    FlowAPL   =0x0196    ;流量总脉冲L
.EQU    FlowSP1H  =0x0197    ;1路流量求和脉冲H
.EQU    FlowSP1L  =0x0198    ;1路流量求和脉冲L
.EQU    FlowSP2H  =0x0199    ;2路流量求和脉冲H
.EQU    FlowSP2L  =0x019A    ;2路流量求和脉冲L
.EQU    FlowSP3H  =0x019B    ;3路流量求和脉冲H
.EQU    FlowSP3L  =0x019C    ;3路流量求和脉冲L
;------------------------
.EQU    FlowCPSH  =0x019D    ;流量当次脉冲和H
.EQU    FlowCPSL  =0x019E    ;流量当次脉冲和L
.EQU    FlowCBD1  =0x019F    ;流量当次基准1
.EQU    FlowCBD0  =0x01A0    ;流量当次基准0
.EQU    FlowCD1   =0x01A1    ;流量当次十进制数1
.EQU    FlowCD0   =0x01A2    ;流量当次十进制数0
;-------------------------
.EQU    FlowD0    =0x01A3    ;水流量十进制数0(小数)
.EQU    FlowD1    =0x01A4    ;水流量十进制数1   
.EQU    FlowD2    =0x01A5    ;水流量十进制数2
.EQU    FlowD3    =0x01A6    ;水流量十进制数3
.EQU    FlowD4    =0x01A7    ;水流量十进制数4
.EQU    FlowD5    =0x01A8    ;水流量十进制数5
;-----------------------


.EQU    TXCOMMTC  =0x01AE    ;发送命令时间控制  
.EQU    SFMode =0x01AF    ;收费控制.AA为收费
;END:0x01AF
;-----------------------
;7、液晶专用SRAM：01B0~01DF=48
;-----------------------
.EQU    LCDDB0    =0x01B0    ;LCD数据字节0         
.EQU    LCDDB1    =0x01B1    ;LCD数据字节1
.EQU    LCDDB2    =0x01B2    ;LCD数据字节2
.EQU    LCDDB3    =0x01B3    ;LCD数据字节3
.EQU    LCDDB4    =0x01B4    ;LCD数据字节4
.EQU    LCDDB5    =0x01B5    ;LCD数据字节5
.EQU    LCDDB6    =0x01B6    ;LCD数据字节6
.EQU    LCDDB7    =0x01B7    ;LCD数据字节7
.EQU    LCDDB8    =0x01B8    ;LCD数据字节8         
.EQU    LCDDB9    =0x01B9    ;LCD数据字节9
.EQU    LCDDB10   =0x01BA    ;LCD数据字节10
.EQU    LCDDB11   =0x01BB    ;LCD数据字节11
.EQU    LCDDB12   =0x01BC    ;LCD数据字节12
.EQU    LCDDB13   =0x01BD    ;LCD数据字节13
.EQU    LCDDB14   =0x01BE    ;LCD数据字节14
.EQU    LCDDB15   =0x01BF    ;LCD数据字节15
.EQU    LCDDB16   =0x01C0    ;LCD数据字节16        
.EQU    LCDDB17   =0x01C1    ;LCD数据字节17
.EQU    LCDDB18   =0x01C2    ;LCD数据字节18
.EQU    LCDDB19   =0x01C3    ;LCD数据字节19
.EQU    LCDDB20   =0x01C4    ;LCD数据字节20
.EQU    LCDDB21   =0x01C5    ;LCD数据字节21
;---------
.EQU    LCDDB22   =0x01C6    ;LCD数据字节22
.EQU    LCDDB23   =0x01C7    ;LCD数据字节23
;------------------------
.EQU    LCDCB0    =0x01C8    ;LCD命令字节0
.EQU    LCDCB1    =0x01C9    ;LCD命令字节1
.EQU    LCDCB2    =0x01CA    ;LCD命令字节2
;-----------
.EQU    TNLDTim   =0x01CB    ;温度轮流显示时间
.EQU    ZZDHDTim  =0x01CC    ;装载电话显示间
.EQU    H1NLDTim  =0x01CD    ;第一行数字轮流显示时间



;.......
;END:0x01DF
;------------------------
;-----------------------
;8、时钟专用SRAM：01E0~01FF=32
;-----------------------
;当前运行时间
.EQU    CLKSS     =0x01E0    ;时钟 秒
.EQU    CLKMM     =0x01E1    ;时钟 分  
.EQU    CLKHH     =0x01E2    ;时钟 时
.EQU    CLKDD     =0x01E3    ;时钟 日
.EQU    CLKWW     =0x01E4    ;时钟 星期
.EQU    CLKMO     =0x01E5    ;时钟 月
.EQU    CLKYY     =0x01E6    ;时钟 年
;------------
;后备操作时间
.EQU    CLKSSB    =0x01E7    ;时钟后备 秒
.EQU    CLKMMB    =0x01E8    ;时钟后备 分  
.EQU    CLKHHB    =0x01E9    ;时钟后备 时
.EQU    CLKDDB    =0x01EA    ;时钟后备 日
.EQU    CLKWWB    =0x01EB    ;时钟后备 星期
.EQU    CLKMOB    =0x01EC    ;时钟后备 月
.EQU    CLKYYB    =0x01ED    ;时钟后备 年
;-------------


.EQU    CLKSCNT   =0x01FE    ;时钟秒计数
;.......
;END:0x01FF
;-----------------------
;9、通讯专用SRAM：0200~020F=16
;-----------------------
.EQU    SLADDW    =0x0200    ;放从机写地址      
.EQU    MTXAB     =0x0201    ;主机发送总字节
.EQU    MRXAB     =0x0202    ;主机接收总字节
.EQU    MRTPOIN   =0x0203    ;主机收发指针
.EQU    MTCode    =0X0204    ;主机发送代码定义
.EQU    SRTCNT    =0x0205    ;从机收发计数   @1
.EQU    SReadFA   =0X0206    ;从机接收(相对)首地址
.EQU    TabPoinH  =0x0207    ;表指针H
.EQU    TabPoinL  =0x0208    ;表指针L
.EQU    MOENDFI   =0X0209    ;主机操作后返回信息 
.EQU    SOENDFI   =0X020A    ;从机操作后返回信息
.EQU    STOPCECK  =0X020B    ;停止位验证

;-----------------------
;10、售水专用SRAM：0210~021F=16
;-----------------------
.EQU    WatENB    =0X0210    ;使能位
.EQU    WatKSPH   =0X0211    ;开水脉冲数量H
.EQU    WatKSPL   =0X0212    ;开水脉冲数量L
.EQU    WatWSPH   =0X0213    ;温水脉冲数量H
.EQU    WatWSPL   =0X0214    ;温水脉冲数量L
.EQU    WatBSPH   =0X0215    ;冰水脉冲数量H
.EQU    WatBSPL   =0X0216    ;冰水脉冲数量L
.EQU    FlowCP1H  =0X0217    ;1路当前流量脉冲数H
.EQU    FlowCP1L  =0X0218    ;1路当前流量脉冲数L
.EQU    FlowCP2H  =0X0219    ;2路当前流量脉冲数H
.EQU    FlowCP2L  =0X021A    ;2路当前流量脉冲数L
.EQU    FlowCP3H  =0X021B    ;1路当前流量脉冲数H
.EQU    FlowCP3L  =0X021C    ;1路当前流量脉冲数L
.EQU    WSwitQ    =0X021D    ;出水开关请求
.EQU    WSwitQ1   =0X021E    ;出水开关请求(旧值)
.EQU    WWorkS    =0X021F    ;出水工作状态
.EQU    QWType    =0X0220    ;取水类型
.EQU    OFFWSB    =0X0221    ;关水工作状态位
;---------------
.EQU    PFTIMEH   =0X0222    ;排放时间H
.EQU    PFTIMEL   =0X0223    ;排放时间L
.EQU    PFFLAG    =0X0224    ;排放标志
		

;.......
;END:0x022F
;------------------------
;EEPRAM映射区专用RAM:0230~023F=16
;------------------------
.EQU    KSCAVH    =0x0230    ;开水控制模拟量值H
.EQU    KSCAVL    =0x0231    ;开水控制模拟量值L
.EQU    WSCAVH    =0x0232    ;温水控制模拟量值H
.EQU    WSCAVL    =0x0233    ;温水控制模拟量值L
.EQU    BSCAVH    =0x0234    ;冰水控制模拟量值H
.EQU    BSCAVL    =0x0235    ;冰水控制模拟量值L
;------------------------

;.......
;END:0x023F
;------------------------





;.......
;END:0x0D6F
;-----------------------
;、临时通用SRAM：0D70~0D7F=16
;-----------------------
.EQU    TempM1    =0x0D70   ;临时RAM1
.EQU    TempM2    =0x0D71   ;临时RAM2
.EQU    TempM3    =0x0D72   ;临时RAM3
.EQU    TempM4    =0x0D73   ;临时RAM4
.EQU    TempM5    =0x0D74   ;临时RAM5



;.......
;END:0x0D7F 
;**************************************
;7、PT机关联参表地址表:0x0D80~0x0DBF=64                @1
;-------------------------------
.EQU   PTPFADD  =0x0D80     ;PT板参数首地址 0XAA有效 
.EQU   THEXD0   =0x0D81     ;进水温度(度)
.EQU   THEXD1   =0x0D82     ;开水温度(度)
.EQU   THEXD2   =0x0D83     ;温水温度(度)
.EQU   THEXD3   =0x0D84     ;超高温温度(度)
;------------
.EQU   SpeedOP1 =0x0D85     ;1路开水流速（0.1升/分）
.EQU   SpeedOP2 =0x0D86     ;2路温水流速（0.1升/分）
.EQU   SpeedOP3 =0x0D87     ;3路冰水流速（0.1升/分）
;----------------------
.EQU   RUNCOD   =0x0D88     ;设备运行状态码
.EQU   ERRCOD1  =0x0D89     ;故障代码1
.EQU   ERRCOD2  =0x0D8A     ;故障代码2
.EQU   ERRCOD3  =0x0D8B     ;故障代码3
;----------------------
.EQU   Flow1H0  =0x0D8C     ;1路流量总和（十六进制，单位升）
.EQU   Flow1H1  =0x0D8D
.EQU   Flow1H2  =0x0D8E
.EQU   Flow1H3  =0x0D8F
.EQU   Flow2H0  =0x0D90     ;2路流量总和（十六进制，单位升）
.EQU   Flow2H1  =0x0D91
.EQU   Flow2H2  =0x0D92
.EQU   Flow2H3  =0x0D93
.EQU   Flow3H0  =0x0D94     ;3路流量总和（十六进制，单位升）
.EQU   Flow3H1  =0x0D95
.EQU   Flow3H2  =0x0D96
.EQU   Flow3H3  =0x0D97
;---------------
;.EQU           =0x0D98     
;.EQU           =0x0D99
;.EQU           =0x0D9A
;.EQU           =0x0D9B
;.EQU           =0x0D9C
;.EQU           =0x0D9D
;.EQU           =0x0D9E
;.EQU           =0x0D9F
;-----------------------------
;开关量参数区（1有效，0无效）
.EQU   CARDOUT   =0x0DA0     ;3个刷卡器工作状态
.EQU   WK6Out    =0x0DA1     ;6个取水按钮状态
.EQU   EK6OUT    =0x0DA2     ;6个外部开关量状态
.EQU   POOUT     =0x0DA3     ;PT功率口输出状态
.EQU   LWBMOUT   =0x0DA4     ;漏水及拨码开关状态（拨码占后半字节）
;.EQU           =0x0DA5     ;
;.EQU           =0x0DA6     ;
;.EQU           =0x0DA7     ;
;.EQU           =0x0DA8     ;
;.EQU           =0x0DA9     ;
;.EQU           =0x0DAA     ;
;.EQU           =0x0DAB     ;
;.EQU           =0x0DAC     ;
;.EQU           =0x0DAD     ;
;.EQU           =0x0DAE     ;
;.EQU           =0x0DAF     ;
;---------------------------------
.EQU   PCommand =0x0DB0     ;命令字节
.EQU   PPByte0  =0x0DB1     ;普通机参数0
.EQU   PPByte1  =0x0DB2     ;
.EQU   PPByte2  =0x0DB3     ;
.EQU   PPByte3  =0x0DB4     ;
.EQU   PPByte4  =0x0DB5     ;
.EQU   PPByte5  =0x0DB6     ;
.EQU   PPByte6  =0x0DB7     ;
.EQU   PPByte7  =0x0DB8     ;
.EQU   PPByte8  =0x0DB9     ;
.EQU   PPByte9  =0x0DBA     ;
.EQU   PPByte10 =0x0DBB     ;
.EQU   PPByte11 =0x0DBC     ;
.EQU   PPByte12 =0x0DBD     ;
.EQU   PPByte13 =0x0DBE     ;
.EQU   PPByte14 =0x0DBF     ;普通机参数14
;-----------------------




;END:0x0DBF
;*************************************
;8、ZN机关联参表地址表:0x0DC0~0x0DFF=64
.EQU   ZNPFADD    =0x0DD0    ;有效确认
;.EQU            =0x0DE1     ;电导值H0
;.EQU            =0x0DE2     ;电导值L0
;.EQU            =0x0DE3     ;电导值H1
;.EQU            =0x0DE4     ;电导值L1
;.EQU            =0x0DE5     ;指定播放语音段
;.EQU            =0x0DE6     ;显示数据1
;.EQU            =0x0DE7     ;显示数据2
;.EQU            =0x0DE8     ;显示数据3
;.EQU            =0x0DE9     ;显示数据4
;.EQU            =0x0DEA     ;显示数据5
;.EQU            =0x0DEB     ;显示数据6
;.EQU            =0x0DEC     ;显示数据7
;.EQU            =0x0DFD     ;风扇/UV灯开关状态
;.EQU            =0x0DFE     ;ZN功率接口状态
;.EQU            =0x0DFF     ;拨码开关状态



;----------------------
.EQU   ZCommand =0x0DF0     ;智能机命令专用字节(相对表地址：70H)
.EQU   ZPByte0  =0x0DF1     ;智能机参数0
.EQU   ZPByte1  =0x0DF2     ;
.EQU   ZPByte2  =0x0DF3     ;
.EQU   ZPByte3  =0x0DF4     ;
.EQU   ZPByte4  =0x0DF5     ;
.EQU   ZPByte5  =0x0DF6     ;
.EQU   ZPByte6  =0x0DF7     ;
.EQU   ZPByte7  =0x0DF8     ;
.EQU   ZPByte8  =0x0DF9     ;
.EQU   ZPByte9  =0x0DFA     ;
.EQU   ZPByte10 =0x0DFB     ;
.EQU   ZPByte11 =0x0DFC     ;
.EQU   ZPByte12 =0x0DFD     ;
.EQU   ZPByte13 =0x0DFE     ;
.EQU   ZPByte14 =0x0DFF     ;智能机参数14
;END:0x0DFF 
;***********************************
;9、通信数据存储区1:0x0E00~0x0EFF=256
.EQU   MRTDFA    =0x0E00   ;主收发数据区首地址
.EQU   MRTDA1    =0x0E01
.EQU   MRTDA2    =0x0E02
.EQU   MRTDA3    =0x0E03
.EQU   MRTDA4    =0x0E04
.EQU   MRTDA5    =0x0E05
.EQU   MRTDA6    =0x0E06
.EQU   MRTDA7    =0x0E07
.EQU   MRTDA8    =0x0E08
.EQU   MRTDA9    =0x0E09
.EQU   MRTDA10   =0x0E0A
.EQU   MRTDA11   =0x0E0B


;....
;END:0x0EFF
;***********************************
;10、通信数据存储区2:0x0F00~0x0FFF=256
.EQU   SRTDFA    =0x0F00   ;从收发数据区首地址
                          
;.....
;END：0x0FFF
;***********************************
;11、堆栈工作区（0x1000~0x10FF=256）

;.......
.EQU   SPFAH     =0x10     ;ATMEGA64内部SRAM最高地址：0x10FF  !!!
.EQU   SPFAL     =0xFD
;************************************
;单片机内部EEPRAM定义:
;************************************


;--------------------------
;EEROM定义区
;--------------------------
;EE设置参数区(24组参数)
;-------------------------
.EQU   EESP1     =0x0010      ;密码       @5
.EQU   EESP2     =0x0012      ;P2~P4: 手机号           
.EQU   EESP3     =0x0014      
.EQU   EESP4     =0x0016      
.EQU   EESP5     =0x0018      ;P5~P7: 400专号
.EQU   EESP6     =0x001A
.EQU   EESP7     =0x001C
.EQU   EESP8     =0x001E      ;开水温控上限和下限
.EQU   EESP9     =0x0020      ;温水温控上限和下限
.EQU   EESP10    =0x0022      ;冰水温控上限和下限
.EQU   EESP11    =0x0024      ;加热断通电时间
.EQU   EESP12    =0x0026      ;自动排水开始时间（两位时、两位分）
.EQU   EESP13    =0x0028      ;自动排水时间长度（两位分、两位秒）
.EQU   EESP14    =0x002A      ;设置25度时标准加热功率（单位为瓦）
.EQU   EESP15    =0x002C      ;设置加热功率调整系数（单位为瓦/度）
.EQU   EESP16    =0x002E      ;设置流量计每升水的脉冲个数
.EQU   EESP17    =0x0030      ;设置更换虑芯报警的总用水量（单位为升）
.EQU   EESP18    =0x0032      ;设置可选择的语音段
.EQU   EESP19    =0x0034
.EQU   EESP20    =0x0036
.EQU   EESP21    =0x0038
.EQU   EESP22    =0x003A
.EQU   EESP23    =0x003C
.EQU   EESP24    =0x003E
;-------------------------
.EQU   EEFS1H0  =0x0040       ;1路流量总和H0
.EQU   EEFS1H1  =0x0041       ;1路流量总和H1
.EQU   EEFS1H2  =0x0042       ;1路流量总和H2
.EQU   EEFS1H3  =0x0043       ;1路流量总和H3
.EQU   EEFS2H0  =0x0044       ;2路流量总和H0
.EQU   EEFS2H1  =0x0045       ;2路流量总和H1
.EQU   EEFS2H2  =0x0046       ;2路流量总和H2
.EQU   EEFS2H3  =0x0047       ;2路流量总和H3
.EQU   EEFS3H0  =0x0048       ;3路流量总和H0
.EQU   EEFS3H1  =0x0049       ;3路流量总和H1
.EQU   EEFS3H2  =0x004A       ;3路流量总和H2
.EQU   EEFS3H3  =0x004B       ;3路流量总和H3
;--------------------------
;.EQU  EEFASD0  =0x004C 
.EQU   EEFASD1  =0x004D       ;全路流量总和D1 
.EQU   EEFASD2  =0x004E       ;全路流量总和D2
.EQU   EEFASD3  =0x004F       ;全路流量总和D3
.EQU   EEFASD4  =0x0050       ;全路流量总和D4
.EQU   EEFASD5  =0x0051       ;全路流量总和D5


;---------------------------------------
.EQU   EESFMode =0x0060       ;收费模式，AA为收费
.EQU   EEONOFF  =0x0061       ;开关机，AA为收费关机
.EQU   EELXSV0  =0x0062       ;滤芯设置值0
.EQU   EELXSV1  =0x0063       ;滤芯设置值1
.EQU   EELXSV2  =0x0064       ;滤芯设置值2
.EQU   EEZREN   =0x0065       ;制热使能，0X55为不制热，其它则为制热



;*****************************************
;本程序常量定义区:
;*****************************************
;TWI相关常量
;-----------------------------------------
.EQU   PTSADD    =0x98        ;PT板从机地址
.EQU   CLKSAW    =0xA2        ;时钟从机地址写
.EQU   CLKSAR    =0xA3        ;时钟从机地址读
.EQU   ZNBSAW    =0x68        ;ZN板从机地址写
.EQU   ZNBSAR    =0x69        ;ZN板从机地址读 

;----------------------
.EQU   TWCR_STA  =0b11100101  ;发启动位
.EQU   TWCR_STO  =0b11010101  ;发停止位
.EQU   TWCR_CIF  =0b11000101  ;清中断标志位后继续
.EQU   TWCR_CIFN =0b10000101  ;清中断标志位后继续(非应答)
;----------------------



;-----------------------------------------
;语音段数常量
;VOAMS2   =2        ;1段："初始化测试正常"
;VOAMS3   =3        ;2段："欢迎使用世纪丰源智能饮水设备"
;VOAMS4   =4        ;3段："欢迎使用高温开水"
;VOAMS5   =5        ;4段："本温开水为开水降温，安全卫生，请放心饮用"
;VOAMS6   =6        ;5段："达标净水请放心饮用"
;VOAMS7   =7        ;6段："请放好水杯"
;VOAMS8   =8        ;7段："通讯故障，请检查"
;VOAMS9   =9        ;8段："请注意，现在缺水"
;VOAMS10  =10       ;9段："开水电磁阀故障"
;VOAMS11  =11       ;10段："温水电磁阀故障"
;VOAMS12  =12       ;11段："冰水电磁阀故障"
;VOAMS13  =13       ;12段："进水电磁阀故障"
;VOAMS14  =14       ;13段："制冷系统故障"
;VOAMS15  =15       ;14段："加热管烧坏请检查"
;VOAMS16  =16       ;15段："高压泵故障"
;VOAMS17  =17       ;16段："冲洗阀故障"
;VOAMS18  =18       ;17段："刷卡水控故障"
;VOAMS19  =19       ;18段："密码无效"
;VOAMS20  =10       ;19段："烟感灵敏度过低"
;VOAMS21  =21       ;20段："有火灾险情"
;VOAMS22  =22       ;21段："高温超标"
;----------------------------------------------

;----------------------------------------------
;数字温度与ADC值对应表
;---------------------
.EQU   DTN10    =208
.EQU   DTN9     =206
.EQU   DTN8     =204
.EQU   DTN7     =202
.EQU   DTN6     =200
.EQU   DTN5     =198
.EQU   DTN4     =196
.EQU   DTN3     =194
.EQU   DTN2     =192
.EQU   DTN1     =190
.EQU   DT0      =188
.EQU   DT1      =186
.EQU   DT2      =184
.EQU   DT3      =181
.EQU   DT4      =179
.EQU   DT5      =177
.EQU   DT6      =174
.EQU   DT7      =172
.EQU   DT8      =170
.EQU   DT9      =167
.EQU   DT10     =165
.EQU   DT11     =162
.EQU   DT12     =160
.EQU   DT13     =157
.EQU   DT14     =155
.EQU   DT15     =153
.EQU   DT16     =150
.EQU   DT17     =148
.EQU   DT18     =145
.EQU   DT19     =143
.EQU   DT20     =142
.EQU   DT21     =138   
.EQU   DT22     =135
.EQU   DT23     =133
.EQU   DT24     =130
.EQU   DT25     =128
.EQU   DT26     =126
.EQU   DT27     =123
.EQU   DT28     =121
.EQU   DT29     =118
.EQU   DT30     =116
.EQU   DT31     =113
.EQU   DT32     =112
.EQU   DT33     =110
.EQU   DT34     =107
.EQU   DT35     =105
.EQU   DT36     =103
.EQU   DT37     =100
.EQU   DT38     =98
.EQU   DT39     =96
.EQU   DT40     =94
.EQU   DT41     =92
.EQU   DT42     =90
.EQU   DT43     =88
.EQU   DT44     =86
.EQU   DT45     =84
.EQU   DT46     =82
.EQU   DT47     =80
.EQU   DT48     =79
.EQU   DT49     =77
.EQU   DT50     =75
.EQU   DT51     =73
.EQU   DT52     =72
.EQU   DT53     =70
.EQU   DT54     =68
.EQU   DT55     =67
.EQU   DT56     =65
.EQU   DT57     =64
.EQU   DT58     =62
.EQU   DT59     =61
.EQU   DT60     =59
.EQU   DT61     =58
.EQU   DT62     =56
.EQU   DT63     =55
.EQU   DT64     =54
.EQU   DT65     =53
.EQU   DT66     =51
.EQU   DT67     =50
.EQU   DT68     =49
.EQU   DT69     =48
.EQU   DT70     =47
.EQU   DT71     =45
.EQU   DT72     =44
.EQU   DT73     =43
.EQU   DT74     =42
.EQU   DT75     =41
.EQU   DT76     =40
.EQU   DT77     =39
.EQU   DT78     =38
.EQU   DT79     =37
.EQU   DT80     =37
.EQU   DT81     =36
.EQU   DT82     =35
.EQU   DT83     =34
.EQU   DT84     =33
.EQU   DT85     =32
.EQU   DT86     =32
.EQU   DT87     =31
.EQU   DT88     =30
.EQU   DT89     =29
.EQU   DT90     =29
.EQU   DT91     =28
.EQU   DT92     =27
.EQU   DT93     =27
.EQU   DT94     =26
.EQU   DT95     =26
.EQU   DT96     =25
.EQU   DT97     =24
.EQU   DT98     =24
.EQU   DT99     =23
.EQU   DT100    =23
.EQU   DT101    =22
.EQU   DT102    =22
.EQU   DT103    =21
.EQU   DT104    =21
.EQU   DT105    =20
.EQU   DT106    =20
.EQU   DT107    =19
.EQU   DT108    =19
.EQU   DT109    =18
;-----------------------------------------


;*****************************************
;语句间距统一为:12-18-36,从今以后均以此为准!!!!
;*****************************************
;00地址开始：代码程序区
;*****************************************
.ORG  0x0000
;*****************************************
;ATmega64复位和中断向量：
;------------------------------------
           JMP   RESET             ;复位地址             1
           JMP INTNE;   EXT_INT0          ;外部中断请求0        2
		   JMP INTNE;   EXT_INT1          ;外部中断请求1        3
		   JMP INTNE;   EXT_INT2          ;外部中断请求2        4
		   JMP INTNE;   EXT_INT3          ;外部中断请求3        5
		   JMP INTNE;   EXT_INT4          ;外部中断请求4        6
		   JMP INTNE;   EXT_INT5          ;外部中断请求5        7
		   JMP INTNE;   EXT_INT6          ;外部中断请求6        8
		   JMP INTNE;   EXT_INT7          ;外部中断请求7        9
           JMP INTNE;   TIM2_COMP         ;定时/计数器2比较匹配 10
           JMP INTNE;   TIM2_OVF          ;定时/计数器2溢出     11
           JMP INTNE;   TIM1_CAPT         ;定时/计数器1捕捉     12
           JMP INTNE;   TIM1_COMPA        ;定时/计数器1比较A    13
           JMP INTNE;   TIM1_COMPB        ;定时/计数器1比较B    14
           JMP INTNE;   TIM1_OVF          ;定时/计数器1溢出     15
           JMP INTNE;   TIM0_COMP         ;定时/计数器0比较     16
           JMP   TIM0_OVF          ;定时/计数器0溢出     17
           JMP INTNE;   SPI_STC           ;SPI串行传输结束      18
           JMP INTNE;   USART0_RX         ;USART0，Rx结束       19
           JMP INTNE;   USART0_UDRE       ;USART0数据寄存器空   20
           JMP INTNE;   USART0_TX         ;USART0,Tx结束        21
           JMP   INT_ADC           ;ADC转换结束          22
		   JMP INTNE;   EE_READY          ;EEPROM 就绪          23
           JMP INTNE;   ANALOG_COMP       ;模拟比较器           24
           JMP INTNE;   TIM1_COMPC        ;定时/计数器1比较C    25
           JMP INTNE;   ITM3_CAPT         ;定时/计数3捕捉事件   26
           JMP INTNE;   ITM3_COMPA        ;定时/计数3比较A      27
           JMP INTNE;   ITM3_COMPB        ;定时/计数3比较B      28
           JMP INTNE;   ITM3_COMPC        ;定时/计数3比较C      29
           JMP INTNE;   ITM3_OVF          ;定时/计数3溢出       30
           JMP INTNE;   USART1_RX         ;USART1,Rx结束        31
           JMP INTNE;   USART1_UDRE       ;USART1,数据寄存器空  32
		   JMP INTNE;   USART1_TX         ;USART1,Tx结束        33
           JMP   TWI_IPRO          ;两线串行接口                 34
           JMP INTNE;   SPM_READY         ;保存程序存储器内容就绪35
           NOP                        ;ATmega64中断结束地址为0X0045
		   NOP
           NOP

.ORG 0x0050
INTNE:     NOP                      ;未设中断，统一中断返回
           NOP
           RETI
;*****************************************
;一、初始化程序区
;*****************************************
;(一)、初始化子程序区
;*****************************************
;1、初始化CPU子程序
;------------------------------------------
INIT_CPU:  LDI   BUFMS,0b00000000 ;清除休眠
           OUT   MCUCR,BUFMS
           LDI   BUFMS,0b00000011 ;T0~T3预分频复位
           OUT   SFIOR,BUFMS
		   IN    BUFMS,WDTCR
		   SBR   BUFMS,0B00011000  ;WDT修改使能,WDCE必先置1  !!!!
		   OUT   WDTCR,BUFMS
		   LDI   BUFMS,0b00011111  ;开WDT,溢出周期=2秒
           OUT   WDTCR,BUFMS
		   NOP
           NOP
		   NOP
		   RET
;------------------------------------------
;SUB :初始化数据方向寄存器子程序
;------------------------------------------
INIT_DDR:  LDI   BUFMS,0b00111111  ;A口方向
           OUT   DDRA,BUFMS
           LDI   BUFMS,0b00000000  ;B口方向
           OUT   DDRB,BUFMS
		   LDI   BUFMS,0b00000000  ;C口方向
           OUT   DDRC,BUFMS
		   LDI   BUFMS,0b00000001  ;D口方向
           OUT   DDRD,BUFMS
		   LDI   BUFMS,0b00111111  ;E口方向
           OUT   DDRE,BUFMS
		   LDI   BUFMS,0b00000000  ;F口方向
           STS   DDRF,BUFMS
		   LDI   BUFMS,0b00001111  ;G口方向
           STS   DDRG,BUFMS
           RET
;------------------------------------------
;SUB :初始化端口数据寄存器子程序
;------------------------------------------
INIT_PORT: LDI   BUFMS,0b00011100  ;A口初值
		   OUT   PORTA,BUFMS
           LDI   BUFMS,0b00000000  ;B口初值
		   OUT   PORTB,BUFMS
		   LDI   BUFMS,0b10000000  ;C口初值
		   OUT   PORTC,BUFMS
		   LDI   BUFMS,0b00100000  ;D口初值    
		   OUT   PORTD,BUFMS
           LDI   BUFMS,0b00000000  ;E口初值
		   OUT   PORTE,BUFMS
		   LDI   BUFMS,0b11110000  ;F口初值    
		   STS   PORTF,BUFMS
		   LDI   BUFMS,0b00001111  ;G口初值
		   STS   PORTG,BUFMS
           RET
;------------------------------------------
;SUB :初始化寄存器子程序
;------------------------------------------
INIT_REGIS:CLR   FlagM
           CLR   FlagI           

;------------------------------------------
;SUB :初始化数据存储器子程序
;------------------------------------------
INIT_SRAM: CLR   FlagM             ;清寄存器标志
           CLR   BUFMS
           LDI   XH,HIGH(SramStart) ;清SRAM的0100~0FFF
           LDI   XL,LOW(SramStart)
           ST    X+,BUFMS
		   CPI   XH,0X10           ;4K SRAM才用此!!
		   BRNE  PC-2
;------------------------------
;要置初值的SRAM
           LDI   BUFMS,04
		   STS   MLEDFFCNT,BUFMS
		   LDI   BUFMS,0B00000001
		   STS   MLEDFFEN,BUFMS
		   		  		  
;------------------------------
		   RET
;-------------------------------------------
;SUB :初始化T/C0中断(10MS中断)
;-------------------------------------------
INIT_TC0:  LDI   BUFMS,0b00000111   ;时钟1024分频后:64US
           OUT   TCCR0,BUFMS
           LDI   BUFMS,100          ;置初值100
           OUT   TCNT0,BUFMS
           IN    BUFMS,TIMSK
		   NOP
		   ORI   BUFMS,0b00000001   ;T/C0溢出中断(TOIE0)使能
		   OUT   TIMSK,BUFMS
           NOP  
           RET
;-------------------------------------------
;SUB :初始化T/C1为计数模式
;-------------------------------------------
INIT_TC1:  IN    BUFMS,TIMSK
           CLT
		   BLD   BUFMS,TOIE1       ;先屏蔽TC1中断
           OUT   TIMSK,BUFMS
		   CLR   BUFMS             ;设为普通模式
           OUT   TCCR1A,BUFMS
           LDI   BUFMS,0B00000111  ;T1外引脚下升沿计数
           OUT   TCCR1B,BUFMS
           CLR   BUFMS
		   OUT   TCNT1L,BUFMS
		   OUT   TCNT1H,BUFMS
           RET
;-------------------------------------------
;SUB :初始化T/C2为计数模式
;-------------------------------------------
INIT_TC2:  IN    BUFMS,TIMSK
           CLT
		   BLD   BUFMS,TOIE2       ;先屏蔽TC2中断
           OUT   TIMSK,BUFMS
           LDI   BUFMS,0B00000111  ;T2外引脚下升沿计数
           OUT   TCCR2,BUFMS
           CLR   BUFMS
		   OUT   TCNT2,BUFMS
           RET
;-------------------------------------------
;SUB :初始化T/C3为计数模式
;-------------------------------------------
INIT_TC3:  IN    BUFMS,TIMSK
           CLT
		   BLD   BUFMS,TOIE3       ;先屏蔽TC3中断
           OUT   TIMSK,BUFMS
		   CLR   BUFMS             ;设为普通模式
           STS   TCCR3A,BUFMS
           LDI   BUFMS,0B00000111  ;T1外引脚下升沿计数
           STS   TCCR3B,BUFMS
           CLR   BUFMS
		   OUT   TCNT1L,BUFMS
		   OUT   TCNT1H,BUFMS
           RET
;-------------------------------------------
;SUB :初始化TWI为中断
;-------------------------------------------
INIT_TWI: CLR    BUFMS
          STS    TWCR,BUFMS
          LDI    BUFMS,98          ;比特率设为20KHZ
          STS    TWBR,BUFMS
		  LDI    BUFMS,0B11111000  ;预分频值设为1
		  STS    TWSR,BUFMS
		  LDI    BUFMS,PTSADD      ;本机从地址
		  STS    TWAR,BUFMS
		  LDI    BUFMS,0B11000101  ;使能TWEA TWEN TWIE
          STS    TWCR,BUFMS
		  RET
;-------------------------------------------
;SUB :初始化ADC中断
;-------------------------------------------
INIT_ADC: LDI    BUFMS,0b01000000  ;VREF=AVCC,右对齐.初次选通ADC0
          OUT    ADMUX,BUFMS
          LDI    BUFMS,0B00000100  ;T/C0溢出触发ADC
		  STS    ADCSRB,BUFMS
		  LDI    BUFMS,0b10101111  ;ADEN=1,ADATE=1,ADIE=1,128分频
          OUT    ADCSRA,BUFMS
          RET
;--------------------------------------------
;初始化液晶模块子程序
;--------------------------------------------
INIT_LCD: LDI    BUFMS,1        ;开系统振荡
          CALL   SendDCMS
	      LDI    BUFMS,3        ;开LCD BIAS发生器
          CALL   SendDCMS
	      LDI    BUFMS,5        ;WDT DIS
          CALL   SendDCMS
	      LDI    BUFMS,0X0F     ;清除 WDT
          CALL   SendDCMS
	      NOP
          RET
;--------------------------------------------
;SUB:初始化调用延时子程序组(Y=8MHZ时)
;--------------------------------------------
;占用资源：CNTMS
INDEL100US:LDI   CNTMS,99   ;延时100US 
DEL100USLP:NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   DEC   CNTMS
		   BRNE  DEL100USLP
           RET
;--------------------------------------------
;占用BUMS和CNTMS
INDEL10MS: LDI   BUFMS,100     ;延时10MS
		   RCALL INDEL100US
		   DEC   BUFMS    
		   BRNE  PC-2
           RET
;--------------------------------------------
;占用BUMS和CNTMS
INDEL25MS: LDI   BUFMS,250     ;延时25MS
		   RCALL INDEL100US
		   DEC   BUFMS    
		   BRNE  PC-2
           RET
;-------------------------------------------
;占用BUFMS/CNTMS/R2M
INDEL500MS:WDR
           LDI   BUFMS,50     ;50*10MS=500MS  
           MOV   R2M,BUFMS
		   RCALL INDEL10MS
           DEC   R2M
		   BRNE  PC-2
		   RET
;------------------------------------------
;开温冰三水温控数字转模拟量子程序
;------------------------------------------
;入口条件:数字放EESP8/EESP9/EESP10中
;出口条件:KSCAVH/L WSCAVH/L BSCAVH/L
;占用资源:
W3CDTASUB: LDI   XH,HIGH(EESP8)    ;取开水设置值转换
           LDI   XL,LOW(EESP8)
		   CALL  REE2BSUB
		   NOP
		   CALL  HexDecMS
		   MOV   BUFMS,XH
		   CALL  Dec1BHexMS        ;1字节十进制转为十六进制
           NOP
		   CALL  QweryTAdcMS       ;转为模拟量
		   STS   KSCAVH,BUFMS
;--------
           MOV   BUFMS,XL
		   CALL  Dec1BHexMS
           NOP
		   CALL  QweryTAdcMS
		   STS   KSCAVL,BUFMS
;------------------
		   LDI   XH,HIGH(EESP9)    ;取温水设置值转换
           LDI   XL,LOW(EESP9)
		   CALL  REE2BSUB
		   NOP
		   CALL  HexDecMS
		   MOV   BUFMS,XH
		   CALL  Dec1BHexMS
           NOP
		   CALL  QweryTAdcMS
		   STS   WSCAVH,BUFMS
;--------
           MOV   BUFMS,XL
		   CALL  Dec1BHexMS
           NOP
		   CALL  QweryTAdcMS
		   STS   WSCAVL,BUFMS
;------------------
		   LDI   XH,HIGH(EESP10)    ;取冰水设置值转换
           LDI   XL,LOW(EESP10)
		   CALL  REE2BSUB
		   NOP
		   CALL  HexDecMS
		   MOV   BUFMS,XH
		   CALL  Dec1BHexMS
           NOP
		   CALL  QweryTAdcMS
		   STS   BSCAVH,BUFMS
;--------
           MOV   BUFMS,XL
		   CALL  Dec1BHexMS
           NOP
		   CALL  QweryTAdcMS
		   STS   BSCAVL,BUFMS
		   RET
;---------------------------------


;*****************************************
;(二)、初始化头程序区
;*****************************************
RESET:     NOP
           CLI
           LDI   BUFIS,SPFAH      ;置堆栈指针为内SRAM的顶部
           OUT   SPH,BUFIS        
           LDI   BUFIS,SPFAL
           OUT   SPL,BUFIS
		   RCALL INIT_CPU         ;初始化CPU
		   NOP
           RCALL INIT_DDR         ;初始化数据方向寄存器
		   NOP
           RCALL INIT_PORT        ;初始化端口数据寄存器
;---------
           NOP
		   RCALL INIT_SRAM        ;初始化数据存储器
;-------------------
;初始化CPU外围功能模块
           RCALL INIT_TC0         ;初始化定时计数器0中断
           NOP
		   RCALL INIT_TC1         ;初始化定时计数器1为计数
           NOP
		   RCALL INIT_TC2         ;初始化定时计数器2为计数
		   NOP
		   RCALL INIT_TC3         ;初始化定时计数器3为计数
       	   NOP
		   RCALL INIT_TWI         ;初始化TWI
		   NOP                      
	       RCALL INIT_ADC         ;初始化ADC中断
;--------------------
;初始化外部功能模块
           RCALL INIT_LCD
;;-------------------
;外部件测试
           LDI   CNTM,3           ;测试LED灯
CheckExt:  CLR   BUFM
		   STS   PORTG,BUFM
;------
           CALL  DDAllOn           ;测试LCD
           NOP
		   CALL  D22BMS	     
;------          
           SBI   PORTA,BBUZZ       ;测试蜂鸣器
		   RCALL INDEL500MS
		   NOP
		   RCALL INDEL500MS
;-----------
;测试取反
           LDI   BUFM,0X0F
		   STS   PORTG,BUFM
;------
           CALL  DDAllOff
		   NOP
		   CALL  D22BMS	     
;------          
           CBI   PORTA,BBUZZ
		   RCALL INDEL500MS
;-----------
		   DEC   CNTM
		   BRNE  CheckExt
;---------------------
           CALL  DDAllOff          ;
;---------------------
;播放初始化语音
           WDR
           CALL  VOAMS2            ;2段： "始化测试正常"
           NOP
		   NOP
           CALL  VOAMS3            ;"欢迎使用世纪丰源智能饮水设备" 
		   NOP  
;----------------------
;从EEPRAM中加载数据
           LDI   XH,HIGH(EEFASD1)
		   LDI   XL,LOW(EEFASD1)
		   LDI   ZH,HIGH(FLOWD1)
		   LDI   ZL,LOW(FLOWD1) 
		   LDI   CNTM,5 
		   CALL  REENBSUB
;-------------
           CALL  W3CDTASUB         ;三水温控数字值转模拟量备用
;-----------------------------
		   LDI   BUFM,250
		   STS   FLOWAPL,BUFM
		   CLR   BUFM
		   STS   FLOWAPH,BUFM
           LDI   BUFM,01
		   STS   CLKSCNT,BUFM
		   LDI   BUFM,0XAA         ;设为收费式
		   STS   SFMode,BUFM
		   LDI   BUFM,03
		   STS   TXCOMMTC,BUFM
		   LDI   BUFM,0XAA
		   STS   PTPFADD,BUFM
;---------------------------
           CALL  DDAllOn
           NOP
		   CALL  D22BMS	  
;--------------------------
    	   SEI
;******************************************
;二、据消息面向对象处理程序区
;******************************************
;Message1位定义：
;D0: B20MST   20MS处理任务
;D1: BI2CMRT   对I2C主机收数据处理任务
;D2: BI2CSRT   对I2C从机收数据处理任务
;D3: B100MST  100MS处理任务
;D4: B300MST  300MS处理任务
;D5: B500MST  500MS处理任务
;D6: B1ST     1秒处理任务
;D7:
;       LDI   BUFM,0XFF
;       STS   MLEDFFEN,BUFM
;******************************************
MainPro:   NOP
           LDS   BUFM,Message1
		   NOP
		   TST   BUFM
		   BRNE  TestDM1
           LDS   BUFM,Message2
		   NOP
		   TST   BUFM
		   BREQ  PC+2
		   RJMP  TestDM2
;-------------
;元任何处理对则等待
           NOP
		   NOP
		   NOP
		   NOP
           NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
		   NOP
           RJMP  MainPro
;*********************************
TestDM1:   SBRC  BUFM,0
           JMP   ObjectP0
           SBRC  BUFM,1
           JMP   ObjectP1
           SBRC  BUFM,2
           JMP   ObjectP2
           SBRC  BUFM,3
           JMP   ObjectP3
		   SBRC  BUFM,4
           JMP   ObjectP4
           SBRC  BUFM,5
           JMP   ObjectP5
           SBRC  BUFM,6
           JMP   ObjectP6
           SBRC  BUFM,7
           JMP   ObjectP7
		   NOP
           RJMP  MainPro           ;正常是不会至此的
;------------------------------------
TestDM2:   SBRC  BUFM,0
           JMP   ObjectP8
           SBRC  BUFM,1
           JMP   ObjectP9
           SBRC  BUFM,2
           JMP   ObjectP10
           SBRC  BUFM,3
           JMP   ObjectP11
		   SBRC  BUFM,4
           JMP   ObjectP12
           SBRC  BUFM,5
           JMP   ObjectP13
           SBRC  BUFM,6
           JMP   ObjectP13
           SBRC  BUFM,7
           JMP   ObjectP15
		   NOP
           RJMP  MainPro           ;正常是不会至此的
;****************************************
;对象0处理程序：每20MS对象
;****************************************
ObjectP0:  ANDI  BUFM,0B11111110
           STS   Message1,BUFM
;========================================

;=======================================       
ObjectP0END:NOP
           JMP   MainPro
;****************************************
;对象1处理程序：TWI主机操作后续处理
;****************************************
;BI2CMRT
ObjectP1:  ANDI  BUFM,0B11111101
           STS   Message1,BUFM
;========================================
;信息入口:SLADD
;MOENDFI=A0：主机发正确  =A1：主机发收正确 
;       =50：从机无应答  =51：作为主机失败
;-----------------------------
;判正确否
		   LDS   BUFM,MOENDFI
		   CPI   BUFM,0XA1
		   BRNE  PC+2
		   RJMP  MRDOKPRO          ;主机接收数据正确  
           CPI   BUFM,0XA0
		   BRNE  PC+2
		   RJMP  MTDOKPRO          ;主机发送数据正确
		   CPI   BUFM,0X50
		   BRNE  PC+2
		   RJMP  MTSNACK           ;从机无应答
           CPI   BUFM,0X51
		   BRNE  PC+2
		   RJMP  MTRDATAF          ;主发收数据失败
		   RJMP  MRDTEND
;------------------------------------
;主机接收数据正确(A1),判主机发送代码
MRDOKPRO: 
		   LDS   BUFM,MTCODE
		   CPI   BUFM,2
		   BRNE  PC+2
		   RJMP  MTA1P2
		   CPI   BUFM,3
		   BRNE  PC+2
		   RJMP  MTA1P3
		   CPI   BUFM,4
		   BRNE  PC+2
		   RJMP  MTA1P4
		   CPI   BUFM,5
		   BRNE  PC+2
		   RJMP  MTA1P5
		   CPI   BUFM,6
		   BRNE  PC+2
		   RJMP  MTA1P6
;-----------------------------
;是读ZN全表
MTA1P2:    nop
         

           RJMP  MRDTEND
;-----------------------------
;
MTA1P3:    nop

           RJMP  MRDTEND
;-----------------------------
;
MTA1P4:    nop

           RJMP  MRDTEND
;-----------------------------
;
MTA1P5:    nop

           RJMP  MRDTEND
;-----------------------------
;
MTA1P6:   nop

          RJMP  MRDTEND
;--------------------------------------
;主机发送数正确(A0)，判主机发送代码   
MTDOKPRO:  LDS   BUFM,MTCODE
           CPI   BUFM,00
		   BRNE  PC+3      
		   NOP
		   NOP
		                         ;暂不处理
		   RJMP  MRDTEND
;---------------------------------------
;从机未应答(51H)
MTSNACK:   NOP
           LDI   BUFM,0B10000000   ;释放总线
		   STS   TWCR,BUFM 
           CALL  INDEL10MS
		   NOP 
		   CALL  INIT_TWI          ;再初始化
           NOP
           RJMP  MRDTEND
;---------------------------------------
;主发收数据失败
MTRDATAF:  NOP
           LDI   BUFM,0B10000000   ;释放总线
		   STS   TWCR,BUFM 
           CALL  INDEL10MS
		   NOP 
		   CALL  INIT_TWI          ;再初始化
           NOP
           RJMP  MRDTEND
;---------------------------------------
MRDTEND:   NOP
;========================================
ObjectP1END:NOP
           JMP   MainPro
;****************************************
;对象2处理程序：TWI从机操作后续处理   @8
;****************************************
ObjectP2:  ANDI  BUFM,0B11111011
           STS   Message1,BUFM
;========================================  
;信息入口:SLADD
;SOENDFI=A0：从机发数正确  =A1：从机收数正确 
;       =50：错误0    =51：错误1
;--------------------------------
;判正确否
		   LDS   BUFM,SOENDFI
		   CPI   BUFM,0XA1
		   BRNE  PC+2
		   RJMP  SRDOKPRO          ;从机收数正确  
           CPI   BUFM,0XA0
		   BRNE  PC+2  	
		   RJMP  STDOKPRO          ;从机发数正确
		   CPI   BUFM,0X50
		   BRNE  PC+2
		   RJMP  SRTERRP           ;从机收错误0
           CPI   BUFM,0X51
		   BRNE  PC+2
		   RJMP  SRTERRP           ;从机收错误1
		   RJMP  MRDTEND
;---------------------------------
;从机接收数据正确(A1),对数据进行处理
;入口消息：SReadFA  SRTCNT 
SRDOKPRO:  LDS   CNTM,SReadFA
           CPI   CNTM,0X70         ;70H是命令相对地址!
		   BREQ  PC+2
		   RJMP  SRDTEND           ;对非命令不处理
;------------------------------
           CALL  TESTSY
;对命令字节解释      
           LDS   CNTM,ZCommand
		   CPI   CNTM,0X0B         
		   BRNE  PC+2
		   RJMP  COMMPX0B          ;命令0B：按流量取水
		   CPI   CNTM,0X10         
		   BRNE  PC+2
		   RJMP  COMMPX10          ;命令10：收费
           CPI   CNTM,0X01         
		   BRNE  PC+2
		   RJMP  COMMPX01          ;命令01：开机
		   CPI   CNTM,0X02         
		   BRNE  PC+2
		   RJMP  COMMPX02          ;命令02：关机
		   CPI   CNTM,0X05         
		   BRNE  PC+2
		   RJMP  COMMPX05          ;设备初始化
           CPI   CNTM,0X06         
		   BRNE  PC+2
		   RJMP  COMMPX06          ;复位滤芯至设定值
		   CPI   CNTM,0X0A         
		   BRNE  PC+2
		   RJMP  COMMPX0A          ;开加热
		   CPI   CNTM,0X09         
		   BRNE  PC+2
		   RJMP  COMMPX09          ;关加热
 
 
 
 
 ;待写命令         
		   RJMP  SRDTEND
;========================================
COMMPX0B:  CLR   BUFM
           STS   ZCommand,BUFM
;按流量售水命令
           LDS   BUFM,ZPBYTE0
           TST   BUFM
		   BRNE  WATENPRO1
           LDS   BUFM,ZPBYTE1
           TST   BUFM
		   BRNE  WATENPRO1
           LDS   BUFM,ZPBYTE2
           TST   BUFM
		   BRNE  WATENPRO2
           LDS   BUFM,ZPBYTE3
           TST   BUFM
		   BRNE  WATENPRO2
           LDS   BUFM,ZPBYTE4
           TST   BUFM
		   BRNE  WATENPRO3
           LDS   BUFM,ZPBYTE5
           TST   BUFM
		   BRNE  WATENPRO3
;-------------
           RJMP  SRDTEND          ;三组全为0，非法，不处理
;----------------------------
WATENPRO1: LDS   BUFM,WatENB
           SET                    ;AA 使能开水        
           BLD   BUFM,0
		   STS   WatENB,BUFM
           LDS   CNTM,ZPBYTE0     ;装入温水脉冲
		   STS   WatKSPH,CNTM
		   LDS   CNTM,ZPBYTE1
		   STS   WatKSPL,CNTM
		   RJMP  SRDTEND
;-------------- 
WATENPRO2: LDS   BUFM,WatENB
           SET                    ;AA 使能温水        
           BLD   BUFM,1
		   STS   WatENB,BUFM
           LDS   CNTM,ZPBYTE2     ;装入温水脉冲
		   STS   WatWSPH,CNTM
		   LDS   CNTM,ZPBYTE3
		   STS   WatWSPL,CNTM
		   RJMP  SRDTEND
;-------------- 
WATENPRO3: LDS   BUFM,WatENB
           SET                    ;AA 使能冰水        
           BLD   BUFM,2
		   STS   WatENB,BUFM
           LDS   CNTM,ZPBYTE4     ;装入冰水脉冲
		   STS   WatBSPH,CNTM
		   LDS   CNTM,ZPBYTE5
		   STS   WatBSPL,CNTM
		   RJMP  SRDTEND
;===================================
;命令X10:设收费/免费模式(0XAA为收费，其它为免费)
COMMPX10:  CLR   BUFM
           STS   ZCommand,BUFM
           LDI   XH,HIGH(EESFMode)
		   LDI   XL,LOW(EESFMode)
           LDS   BUFMS,ZPBYTE0
		   CALL  WEE1BSUB
           RJMP  SRDTEND
;===================================
;命令X01:开机
COMMPX01:  CLR   BUFM
           STS   ZCommand,BUFM
           LDI   XH,HIGH(EEONOFF)
		   LDI   XL,LOW(EEONOFF)
           CLR   BUFMS             ;设非0XAA为开机
		   CALL  WEE1BSUB
           RJMP  SRDTEND
;===================================
;命令X02:关机
COMMPX02:  CLR   BUFM
           STS   ZCommand,BUFM
           LDI   XH,HIGH(EEONOFF)
		   LDI   XL,LOW(EEONOFF)
           LDI   BUFMS,0XAA        ;设0XAA为开机
		   CALL  WEE1BSUB
           RJMP  SRDTEND
;====================================
;命令X05:设备初始化
COMMPX05:  CLR   BUFM
           STS   ZCommand,BUFM
           JMP   RESET
;====================================
;命令X06:复位滤芯至设定值  ???
COMMPX06:  CLR   BUFM
           STS   ZCommand,BUFM
           LDI   XH,HIGH(EELXSV0)
		   LDI   XL,LOW(EELXSV0)
		   LDI   ZH,HIGH(ZPBYTE0)
		   LDI   ZL,LOW(ZPBYTE0)
		   LDI   CNTMS,3
		   CALL  WEENBSUB
           RJMP  SRDTEND
;====================================
;命令X0A:开启制热使能
COMMPX0A:  CLR   BUFM
           STS   ZCommand,BUFM
           LDI   XH,HIGH(EEZREN)
		   LDI   XL,LOW(EEZREN)
           LDI   BUFMS,0XAA        ;设0XAA为使能
		   CALL  WEE1BSUB
		   RJMP  SRDTEND
;====================================
;命令X09:关闭制热使能
COMMPX09:  CLR   BUFM
           STS   ZCommand,BUFM
           LDI   XH,HIGH(EEZREN)
		   LDI   XL,LOW(EEZREN)
           LDI   BUFMS,0X55         ;设0X55为非使能
		   CALL  WEE1BSUB
		   RJMP  SRDTEND
;====================================


;@9



           RJMP  SRDTEND
;=====================================
;从机出错
SRTERRP:   NOP
           LDI   BUFM,0B10000000   ;释放总线
		   STS   TWCR,BUFM 
           CALL  INDEL10MS
		   NOP 
		   CALL  INIT_TWI          ;再初始化
           NOP
           RJMP  SRDTEND
;==================================

;从机接数正确
STDOKPRO:

           NOP
SRDTEND:   NOP                     ;从机收发数处理结束
;========================================
ObjectP2END:NOP
           JMP   MainPro
;****************************************
;对象3处理程序：每100MS对象
;****************************************
ObjectP3:  ANDI  BUFM,0B11110111
           STS   Message1,BUFM
;========================================
;100MS主TASK1:键盘处理主程序
;20MS主TASK1:键盘处理   
;----------------------------------------
;专用SRAM:KeyIV/KPCnt/KRSCnt/KDelay/KFTCnt//
;---------------------------------------
;查上升沿放KeyRE中
           CLR   KeyRE	
           LDS   BUFM,PortIVB
		   COM   BUFM
		   ANDI  BUFM,0X0F
           LDS   CNTM,KeyIV
           EOR   CNTM,BUFM
		   BREQ  DNECKEYRE
           STS   KeyIV,BUFM        ;有变化则存新键值
           AND   CNTM,BUFM         ;去除下降沿的变化（只处理上升的键）
           MOV   KeyRE,CNTM
;----------------------------------------
DNECKEYRE: SBRC  FlagM,BKeyS
		   RJMP  ONKEYSPRO         ;已在键盘状态
;未在键盘状态
           TST   KeyRE
		   BRNE  PC+2
		   RJMP  KeyPAllEnd        ;不在键盘态，又无键按下
;------------------------------
           SBRS  KeyRE,BKset
           RJMP  KeyPAllEnd        ;非设置键也不处理
          LDS   BUFM,PortTIVF
          SBRS   BUFM,5
		  RJMP  KeyPAllEnd         ;拨码D1=0则不允许操作键盘。
		   CLR   BUFM
		   STS   PSETDH,BUFM
           STS   PSETDL,BUFM
;-----------------------------------
;进入键盘状态
ONKEYSPRO: TST   KeyRE
		   BRNE  PC+2
		   RJMP  KNoMPRO           ;转无键按下
;------------------------
;有键按下
           CLR   CNTM
		   STS   KDelay,CNTM       ;只要有键这种延时清零
		   SBI   PORTA, BBUZZ      ;有键则响蜂鸣
;-------------
           SBRC  KeyRE,BKSet
		   RJMP  KSETMPRO          ;转设置键主程序
		   SBRC  KeyRE,BKRS
		   RJMP  KShifMPRO         ;转右移键主程序
		   SBRC  KeyRE,BKUp
		   RJMP  KUpMPRO           ;转上移键主程序
		   SBRC  KeyRE,BKDown
		   RJMP  KDownMPRO         ;转下键主程序
		   RJMP  KNoMPRO
;=================================
;(1)、设置键主程序
KSETMPRO:  LDI   BUFM,0
		   STS   KRSCnt,BUFM       ;右移位指向左边开始
;---------
           LDS   CNTM,KPCnt	
		   CPI   CNTM,20           ;最大参数条数     &&&
		   BRCS  PC+2
           LDI   CNTM,20
		   LDI   ZL,LOW(PSETSTAB)
	       LDI   ZH,HIGH(PSETSTAB)
	       ADD   ZL,CNTM
	       BRCC  PC+2
	       INC   ZH
	       NOP 
           IJMP
;键盘参数设置编码分配表
PSETSTAB: RJMP    PSETSP0
          RJMP    PSETSP1
		  RJMP    PSETSP2
          RJMP    PSETSP3
		  RJMP    PSETSP4
          RJMP    PSETSP5
		  RJMP    PSETSP6
          RJMP    PSETSP7
		  RJMP    PSETSP8
          RJMP    PSETSP9
		  RJMP    PSETSP10
		  RJMP    PSETSP11
		  RJMP    PSETSP12
          RJMP    PSETSP13
		  RJMP    PSETSP14
          RJMP    PSETSP15
		  RJMP    PSETSP16
          RJMP    PSETSP17
		  RJMP    PSETSP18
          RJMP    PSETSP19
		  RJMP    PSETSP20
;---------------------------
;键盘参数设置编码00:验证密码
PSETSP0:   SET
           BLD   FlagM,BKeyS       ;置为键盘状态
 ;-----------------
           LDI   XH,HIGH(EESP1)
           LDI   XL,LOW(EESP1)
           RCALL REE2BSUB
           NOP
		   CALL  HEXDECMS
		   LDS   BUFM,PSETDH
           CPSE  BUFM,XH
           RJMP  MIMAERR          ;密码错
           LDS   BUFM,PSETDL
           CPSE  BUFM,XL
           RJMP  MIMAERR          ;密码错
		   RJMP  MIMAOK
;------------- 
;密码错误
MIMAERR:   LDS   BUFM,PSETDH      ;万能码：5027
           CPI   BUFM,0X50
		   BRNE  MIMAZERR         ;密码真错
           LDS   BUFM,PSETDL
           CPI   BUFM,0X27
           BREQ  MIMAWN
;------------
;密码真错
MIMAZERR:  CLR   BUFM
           STS   PSETDH,BUFM
		   STS   PSETDL,BUFM
	       RJMP  LoadSetC
;------------
;万能密码通过
MIMAWN:	   LDI   BUFM,0X12        ;万能通过则自动设为默认的1234
		   STS   PSETDH,BUFM
		   LDI   BUFM,0X34
		   STS   PSETDL,BUFM
           LDI   XH,HIGH(EESP1)
           LDI   XL,LOW(EESP1)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参数子程序
MIMAOK:    LDI   XH,HIGH(EESP1)
           LDI   XL,LOW(EESP1)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码01:设置密码
PSETSP1:   LDI   XH,HIGH(EESP1)
           LDI   XL,LOW(EESP1)
		   CALL  WEE2BPSUB
		   LDS   BUFM,CLKYY       ;	预取当前年/月
		   STS   PSETDH,BUFM
           LDS   BUFM,CLKMO
           ANDI  BUFM,0B00011111
		   STS   PSETDL,BUFM
		   LDS   CNTM,KPCnt        ;键盘设置参数号加1
		   INC   CNTM
		   STS   KPCnt,CNTM
           RJMP  LoadSetC
;-----------------------------
;键盘参数设置编码02:设置年/月
PSETSP2:   LDS   BUFM,PSETDH       ;暂存年/月
           STS   CLKYYB,BUFM
           LDS   BUFM,PSETDL
           STS   CLKMOB,BUFM
;-------------
           LDS   BUFM,CLKWW       ;	取出当前星期/日
		   ANDI  BUFM,0B00000111
		   STS   PSETDH,BUFM
           LDS   BUFM,CLKDD
		   ANDI  BUFM,0B00111111
		   STS   PSETDL,BUFM
		   LDS   CNTM,KPCnt        ;键盘设置参数号加1
		   INC   CNTM
		   STS   KPCnt,CNTM
           RJMP  LoadSetC
;-----------------------------
;键盘参数设置编码03:设置星期/日
PSETSP3:   LDS   BUFM,PSETDH       ;暂存星期/日
           STS   CLKWWB,BUFM
           LDS   BUFM,PSETDL
           STS   CLKDDB,BUFM
;-------------
           LDS   BUFM,CLKHH       ;	取出当前时/分
		   ANDI  BUFM,0B00111111
		   STS   PSETDH,BUFM
           LDS   BUFM,CLKMM
		   ANDI  BUFM,0B01111111
		   STS   PSETDL,BUFM
		   LDS   CNTM,KPCnt        ;键盘设置参数号加1
		   INC   CNTM
		   STS   KPCnt,CNTM
           RJMP  LoadSetC
;-----------------------------
;键盘参数设置编码04:设置时/分
PSETSP4:   LDS   BUFM,PSETDH       ;暂存时/分
           STS   CLKHHB,BUFM
           LDS   BUFM,PSETDL
           STS   CLKMMB,BUFM
		   LDI   BUFM,0X30         ;默认30秒
           STS   CLKSSB,BUFM
;-------------
           CALL  UPCLKDTMS         ;更正时钟日期/时间
           NOP
		   CALL  INDEL10MS
;-------------
;取出手机号段1
           LDI   XH,HIGH(EESP2)
           LDI   XL,LOW(EESP2)
		   RCALL REE2BZP
;-------
		   LDS   CNTM,KPCnt        ;键设置参数号+1
		   INC   CNTM
		   STS   KPCnt,CNTM
		   CLR   CNTM
		   STS   KP2Cnt,CNTM       ;2级键参数号清0
		   RJMP  LoadSetC
;-----------------------------
;键盘参数设置编码05:设手机号码（P2~P4）
PSETSP5:   LDS   CNTM,KP2Cnt       ;2级键参数号+1
           INC   CNTM
		   STS   KP2Cnt,CNTM
		   DEC   CNTM
		   BRNE  SP5P2CN0
;----------------
;2级码0：存1段后取出2段号
           LDI   XH,HIGH(EESP2)
           LDI   XL,LOW(EESP2)
		   CALL  WEE2BZP
           LDI   XH,HIGH(EESP3)
           LDI   XL,LOW(EESP3)
           CALL  REE2BZP
           RJMP  LoadSetC
;-----------------
SP5P2CN0:  CPI   CNTM,1   
           BRNE  SP5P2CN1            
;2级码1：存2段后取出3段号
           LDI   XH,HIGH(EESP3)
           LDI   XL,LOW(EESP3)
		   CALL  WEE2BZP
           LDI   XH,HIGH(EESP4)
           LDI   XL,LOW(EESP4)
           CALL  REE2BZP
           RJMP  LoadSetC
;----------------
;存3段，装下一参数
SP5P2CN1:  LDI   XH,HIGH(EESP4)
           LDI   XL,LOW(EESP4)
		   CALL  WEE2BZP
;-------
           LDI   XH,HIGH(EESP5)
           LDI   XL,LOW(EESP5)
		   RCALL REE2BZP
;-------
		   LDS   CNTM,KPCnt        ;键设置参数号+1
		   INC   CNTM
		   STS   KPCnt,CNTM
		   CLR   CNTM
		   STS   KP2Cnt,CNTM       ;2级键参数号清0
		   RJMP  LoadSetC
;-----------------------------
;键盘参数设置编码06:设400号码（P5~P7）
PSETSP6:   LDS   CNTM,KP2Cnt       ;2级键参数号+1
           INC   CNTM
		   STS   KP2Cnt,CNTM
		   DEC   CNTM
		   BRNE  SP6P2CN0
;----------------                 
;2级码0：存1段后取出2段号
           LDI   XH,HIGH(EESP5)
           LDI   XL,LOW(EESP5)
		   CALL  WEE2BZP
           LDI   XH,HIGH(EESP6)
           LDI   XL,LOW(EESP6)
           CALL  REE2BZP
           RJMP  LoadSetC
;-----------------
SP6P2CN0:  CPI   CNTM,1   
           BRNE  SP6P2CN1            
;2级码1：存2段后取出3段号
           LDI   XH,HIGH(EESP6)
           LDI   XL,LOW(EESP6)
		   CALL  WEE2BZP
           LDI   XH,HIGH(EESP7)
           LDI   XL,LOW(EESP7)
           CALL  REE2BZP
           RJMP  LoadSetC
;----------------
;存3段，装下一参数
SP6P2CN1:  LDI   XH,HIGH(EESP7)
           LDI   XL,LOW(EESP7)
		   CALL  WEE2BZP
;-------
           LDI   XH,HIGH(EESP8)
           LDI   XL,LOW(EESP8)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码07:设置开水温控上限和下限 （P8）  
PSETSP7:   LDI   XH,HIGH(EESP8)
           LDI   XL,LOW(EESP8)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参数子程序
		   LDI   XH,HIGH(EESP9)
           LDI   XL,LOW(EESP9)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码08:设置温水温控上限和下限 （P9）
PSETSP8:   LDI   XH,HIGH(EESP9)
           LDI   XL,LOW(EESP9)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参数子程序
		   LDI   XH,HIGH(EESP10)
           LDI   XL,LOW(EESP10)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码09:设置冰水温控上限和下限 （P10）
PSETSP9:   LDI   XH,HIGH(EESP10)
           LDI   XL,LOW(EESP10)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参子程序
		   LDI   XH,HIGH(EESP11)
           LDI   XL,LOW(EESP11)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码10:设置开水达温后的断通电时间  (p11)
;  （前字节为断时间，后字节为通时间，单位为秒）。
PSETSP10:  
           LDI    BUFM,0X65        ;@XX
		   STS    PSETDH,BUFM
           LDI    BUFM,0X45
		   STS    PSETDL,BUFM
		   CALL   MRWOSUB7         ;向ZN板发设置值
		   NOP
		   CALL   INDEL10MS
;---------------------------
           LDI   XH,HIGH(EESP11)
           LDI   XL,LOW(EESP11)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参数子程序
		   LDI   XH,HIGH(EESP12)
           LDI   XL,LOW(EESP12)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码11:设置每天自动排水开始时间（两位时、两位分） (p12)
PSETSP11:  LDI   XH,HIGH(EESP12)
           LDI   XL,LOW(EESP12)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参数子程序
		   LDI   XH,HIGH(EESP13)
           LDI   XL,LOW(EESP13)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码12:设置每次自动排水时间长度（两位分、两位秒） (p13)
PSETSP12:  LDI   XH,HIGH(EESP13)
           LDI   XL,LOW(EESP13)
		   CALL  WEE2BPSUB         ;写EEPRAM2字节参数子程序
		   LDI   XH,HIGH(EESP14)
           LDI   XL,LOW(EESP14)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码13:设置25度时标准加热功率（单位为瓦） (p14)
PSETSP13:  LDI   XH,HIGH(EESP14)
           LDI   XL,LOW(EESP14)
		   CALL  WEE2BPSUB        
		   LDI   XH,HIGH(EESP15)
           LDI   XL,LOW(EESP15)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码14:设置加热功率调整系数（单位为瓦/度）   (p15)
PSETSP14:  LDI   XH,HIGH(EESP15)
           LDI   XL,LOW(EESP15)
		   CALL  WEE2BPSUB         
		   LDI   XH,HIGH(EESP16)
           LDI   XL,LOW(EESP16)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码15: 设置流量计每升水的脉冲个数  (p16)     @5
PSETSP15:  LDI   XH,HIGH(EESP16)
           LDI   XL,LOW(EESP16)
		   CALL  WEE2BPSUB
		   LDI   XH,HIGH(EESP17)
           LDI   XL,LOW(EESP17)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码16:设置更换虑芯报警的总用水量（单位为升）         (p17)
PSETSP16:  LDI   XH,HIGH(EESP17)
           LDI   XL,LOW(EESP17)
		   CALL  WEE2BPSUB        
		   LDI   XH,HIGH(EESP18)
           LDI   XL,LOW(EESP18)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码17:设置可选择的语音段
PSETSP17:  LDI   XH,HIGH(EESP18)
           LDI   XL,LOW(EESP18)
		   CALL  WEE2BPSUB        
		   LDI   XH,HIGH(EESP19)
           LDI   XL,LOW(EESP19)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码18:
PSETSP18:  LDI   XH,HIGH(EESP19)
           LDI   XL,LOW(EESP19)
		   CALL  WEE2BPSUB  
		 RJMP  ExitKeyO             ;退出键操作   &&        
		   LDI   XH,HIGH(EESP20)
           LDI   XL,LOW(EESP20)
		   RJMP  QSETPPRO
;-----------------------------
;键盘参数设置编码19:
PSETSP19:  LDI   XH,HIGH(EESP20)
           LDI   XL,LOW(EESP20)
		   CALL  WEE2BPSUB         
		   LDI   XH,HIGH(EESP21)
           LDI   XL,LOW(EESP21)
		   RJMP  QSETPPRO
;-------------------------------
;键盘参数设置编码20:退出键盘操作
PSETSP20:  LDI   XH,HIGH(EESP21)
           LDI   XL,LOW(EESP21)
		   CALL  WEE2BPSUB
;-------------------------------
;退出键操作        
ExitKeyO:  SBI   PORTA,BBUZZ
		   CALL  INDEL500MS
           CBI   PORTA,BBUZZ
		   CLR   CNTM
		   STS   KPCnt,CNTM
           CLT
		   BLD   FlagM,BKeyS
		   CBI   PORTA,BBUZZ
           RJMP  KeyPAllEnd
;===========================================
;取出设置参数并转为十进制数放PSETDH/PSETDL中
QSETPPRO:  NOP
           RCALL REE2BSUB
           NOP
		   CALL  HEXDECMS
           STS   PSETDH,XH
		   STS   PSETDL,XL
		   LDS   CNTM,KPCnt        ;键设置参数计数统一加1
		   INC   CNTM
		   STS   KPCnt,CNTM
	       RJMP  LoadSetC
;=================================
;(2)、右移键主程序
KshifMPRO: LDS   CNTM,KRSCnt       ;加1循环
           INC   CNTM
		   CPI   CNTM,4
		   BRCS  PC+2
		   CLR   CNTM
		   STS   KRSCnt,CNTM
		   RJMP  LoadSetC
;================================= 
;(3)、上移键主程序
KUPMPRO:   LDS   CNTM,KRSCnt
           LDS   BUFM,PSETDH
		   CPI   CNTM,0
		   BRNE  KUPN0
;------------
;在千位加1
		   MOV   BUFMS,BUFM
           SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   INC   BUFMS
		   CPI   BUFMS,10
		   BRCS  PC+2
		   LDI   BUFMS,0
		   SWAP  BUFMS
		   ANDI  BUFM,0X0F
		   OR    BUFM,BUFMS
		   STS   PSETDH,BUFM
		   RJMP  LoadSetC
;-------------
KUPN0:     CPI   CNTM,1
		   BRNE  KUPN1
;------------
;在百位加1
		   MOV   BUFMS,BUFM
		   ANDI  BUFMS,0X0F
		   INC   BUFMS
		   CPI   BUFMS,10
		   BRCS  PC+2
		   LDI   BUFMS,0
		   ANDI  BUFM,0XF0
		   OR    BUFM,BUFMS
           STS   PSETDH,BUFM
		   RJMP  LoadSetC    
 ;-------------
KUPN1:     CPI   CNTM,2
		   BRNE  KUPN2
;------------
;在十位加1
           LDS   BUFM,PSETDL
		   MOV   BUFMS,BUFM
           SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   INC   BUFMS
		   CPI   BUFMS,10
		   BRCS  PC+2
		   LDI   BUFMS,0
		   SWAP  BUFMS
		   ANDI  BUFM,0X0F
		   OR    BUFM,BUFMS
		   STS   PSETDL,BUFM
		   RJMP  LoadSetC
;---------------------
KUPN2:     CPI   CNTM,3
		   BREQ  PC+2
           RJMP  LoadSetC
;------------
;在个位加1
           LDS   BUFM,PSETDL
		   MOV   BUFMS,BUFM
		   ANDI  BUFMS,0X0F
		   INC   BUFMS
		   CPI   BUFMS,10
		   BRCS  PC+2
		   LDI   BUFMS,0
		   ANDI  BUFM,0XF0
		   OR    BUFM,BUFMS
           STS   PSETDL,BUFM
		   RJMP  LoadSetC
;================================
;(4)、下移键主程序
KDownMPRO: LDS   CNTM,KRSCnt
           LDS   BUFM,PSETDH
		   CPI   CNTM,0
		   BRNE  KDOWNN0
;在千位减1
		   MOV   BUFMS,BUFM
		   SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   LDI   CNTMS,1
		   SUB   BUFMS,CNTMS
		   BRCC  PC+2
		   LDI   BUFMS,9
		   SWAP  BUFMS
		   ANDI  BUFM,0X0F
		   OR    BUFM,BUFMS
		   STS   PSETDH,BUFM
		   RJMP  LoadSetC
;--------------	  
KDOWNN0:   CPI   CNTM,1
		   BRNE  KDOWNN1
;在百位减1   
		   MOV   BUFMS,BUFM
		   ANDI  BUFMS,0X0F
           LDI   CNTMS,1
		   SUB   BUFMS,CNTMS
		   BRCC  PC+2
		   LDI   BUFMS,9
		   ANDI  BUFM,0XF0
		   OR    BUFM,BUFMS
           STS   PSETDH,BUFM
		   RJMP  LoadSetC   
;--------------
KDOWNN1:   CPI   CNTM,2
		   BRNE  KDOWNN2
;在十位减1
		   LDS   BUFM,PSETDL
		   MOV   BUFMS,BUFM
		   SWAP  BUFMS
		   ANDI  BUFMS,0X0F
           LDI   CNTMS,1
		   SUB   BUFMS,CNTMS
		   BRCC  PC+2
		   LDI   BUFMS,9
		   SWAP  BUFMS
		   ANDI  BUFM,0X0F
		   OR    BUFM,BUFMS
		   STS   PSETDL,BUFM
           RJMP  LoadSetC
;--------------
KDOWNN2:   CPI   CNTM,3
		   BREQ  PC+2
		   RJMP  LoadSetC
;在个位减1
           LDS   BUFM,PSETDL
		   MOV   BUFMS,BUFM
		   ANDI  BUFMS,0X0F
		   LDI   CNTMS,1
		   SUB   BUFMS,CNTMS
		   BRCC  PC+2
		   LDI   BUFMS,9
		   ANDI  BUFM,0XF0
		   OR    BUFM,BUFMS 
		   STS   PSETDL,BUFM
		   RJMP  LoadSetC
;=================================
;(5)、无键等待时主程序
KNoMPRO:   CBI   PORTA, BBUZZ
           LDS   BUFMS,KDelay
		   CPI   BUFMS,240         ;最大无操作时间4分钟  &&&
           BRCS  PC+2              ;操作等待中
           RJMP  KEYTOVPRO         ;无操作超时
;-----------------------------
;装设置状态显示码
LoadSetC:  NOP
           CALL   DDAllOff
           LDS    R1M,PSETDH
		   LDS    R0M,PSETDL
		   CALL   DecSegMS        ;双字节转换成段码
		   STS    LCDDB16,R3M
		   STS    LCDDB17,R2M
		   STS    LCDDB18,R1M
		   STS    LCDDB19,R0M
;---------------------
;闪烁处理
           LDS    CNTM,KFTCNT
		   INC    CNTM
		   STS    KFTCNT,CNTM
		   CPI    CNTM,3
		   BRCS   DataFEnd
           CPI    CNTM,6
		   BRCS   DFXBPRO
		   CLR    CNTM
		   STS    KFTCNT,CNTM
           RJMP   DataFEnd
;----------------
;对应位闪灭
DFXBPRO:   CLR    CNTMS
		   LDS    CNTM,KRSCnt
		   CPI    CNTM,0
		   BRNE   PC+2
           STS    LCDDB16,CNTMS
		   CPI    CNTM,1
		   BRNE   PC+2
           STS    LCDDB17,CNTMS
		   CPI    CNTM,2
		   BRNE   PC+2
           STS    LCDDB18,CNTMS
           CPI    CNTM,3
		   BRNE   PC+2
           STS    LCDDB19,CNTMS   
DataFEnd:  NOP
;-----------------------------
;装参数号代码
           LDS    CNTM,KPCnt
           CLR    CNTMS
		   LDI    BUFM,10
		   SUB    CNTM,BUFM
		   BRCS   PC+3
		   INC    CNTMS
		   RJMP   PC-3
		   ADD    CNTM,BUFM
		   SWAP   CNTMS
           OR     CNTM,CNTMS
		   MOV    R0M,CNTM
		   CLR    R1M
		   CALL   DecSegMS        ;双字节转换成段码
		   STS    LCDDB1,R1M
		   STS    LCDDB2,R0M
           RJMP   KeyPAllEnd
;-------------------------------------
;无操作超时则自动退出键盘模式样
KEYTOVPRO: SBI   PORTA,BBUZZ
		   CALL  INDEL500MS
           NOP
		   CALL  INDEL500MS
           CBI   PORTA,BBUZZ
		   CLR   CNTM
		   STS   KPCnt,CNTM
           CLT
		   BLD   FlagM,BKeyS
           RJMP  KeyPAllEnd
		   NOP
;=======================================
KeyPAllEnd:NOP                     ;键盘处理合部结束
;=======================================


;----------------------------------------         
;100MS主TASK2:液晶显示
		   CALL  D22BMS
		   NOP
;-----------------------------------------
;100MS主TASK3:处理语音
           CALL  VOBFSUB
           NOP


;========================================
ObjectP3END:NOP                    ;对象3(100MS)全部任务结束
           JMP   MainPro
;****************************************
;(本)对象3专用子程序区                        
;****************************************   
  
  
;---------------------------------- 
;MSUB 、更正时钟日期/时间子程序
;----------------------------------
;入口条件: 设置时间日期放CLKSSB开始的7字节中
;出口条件:
;占用资源:Z BUFMS
UPCLKDTMS: NOP      
;----------------------       
;先装数据          
           LDI   ZH,HIGH(MRTDFA)
		   LDI   ZL,LOW(MRTDFA)
           LDI   BUFMS,02           ;从秒地址开始		   
           ST    Z+,BUFMS
           LDS   BUFMS,CLKSSB
           ST    Z+,BUFMS           
           LDS   BUFMS,CLKMMB
           ST    Z+,BUFMS 
		   LDS   BUFMS,CLKHHB
           ST    Z+,BUFMS           
           LDS   BUFMS,CLKDDB
           ST    Z+,BUFMS 
		   LDS   BUFMS,CLKWWB
           ST    Z+,BUFMS           
           LDS   BUFMS,CLKMOB
           ST    Z+,BUFMS
		   LDS   BUFMS,CLKYYB
           ST    Z+,BUFMS
;---------------------
           LDI   BUFMS,CLKSAW       ;对时钟写
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,8            ;共发8字节
		   STS   MTXAB,BUFMS
           CLT                      ;只是发
		   BLD   FLAGI,BTZR
		   LDI   BUFMS,TWCR_STA     ;发启动位 
	       STS   TWCR,BUFMS
		   CALL  INDEL10MS
		   NOP
		   CALL  RCLKMS              ;@1
		   NOP     
           RET
;---------------------------------- 
;MSUB 、读EERAM 1字节子程序
;----------------------------------
;入口条件:地址放X指针中
;出口条件:数据放XL中
;占用资源:
REE1BSUB:  NOP
           SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
           OUT   EEARL,XL
		   OUT   EEARH,XH
           SBI   EECR,EERE
           IN    XL,EEDR
           RET
;------------------------------------
;MSUB 、读EERAM 2字节子程
;------------------------------------
;入口条件:EE地址放X指针中
;出口条件:数据放XH/XL中
;占用资源:BUFMS
REE2BSUB:  NOP
           SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
           OUT   EEARL,XL
		   OUT   EEARH,XH
           SBI   EECR,EERE
           IN    BUFMS,EEDR
           ADIW  XL,1
		   OUT   EEARL,XL
		   OUT   EEARH,XH
           SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
           SBI   EECR,EERE
           IN    XL,EEDR
           MOV   XH,BUFMS
           RET
;----------------------------------
;MSUB 、写EERAM 1字节子程 
;----------------------------------
;入口条件:地址放X中，数据放BUFMS中
;出口条件:
;占用资源:
WEE1BSUB:  SBIC  EECR,EEWE
           RJMP  PC-1             ;等空
		   OUT   EEARL,XL 
		   OUT   EEARH,XH
           OUT   EEDR,BUFMS       ;写1字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           SBIC  EECR,EEWE
		   RET
;------------------------------------
;MSUB 、写EERAM 2字节子程序
;------------------------------------
;入口条件:EE地址放X指针中，数据放ZH/ZL中
;出口条件:
;占用资源:
WEE2BSUB:  SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
		   OUT   EEARL,XL 
		   OUT   EEARH,XH
           OUT   EEDR,ZH           ;写高字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
		   ADIW  XL,1
           OUT   EEARL,XL 
		   OUT   EEARH,XH
           OUT   EEDR,ZL           ;写低字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           NOP
		   RET
;------------------------------------
;------------------------------------
;MSUB 、读EERAM 2字节直接参数
;------------------------------------
;入口条件:EE地址放X指针中
;出口条件:数据放PSETDH/PSETDL中
;占用资源:BUFMS
REE2BZP:   SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
           OUT   EEARL,XL
		   OUT   EEARH,XH
           SBI   EECR,EERE
           IN    BUFMS,EEDR
		   STS   PSETDH,BUFMS
           ADIW  XL,1
		   OUT   EEARL,XL
		   OUT   EEARH,XH
           SBIC  EECR,EEWE
           RJMP  PC-1              ;等空
           SBI   EECR,EERE
           IN    BUFMS,EEDR
           STS   PSETDL,BUFMS
           RET
;------------------------------------
;MSUB 、写EERAM 2字节直接参数
;------------------------------------
;入口条件:EE地址放X指针中，数据放PSETDH/PSETDL中
;出口条件:
;占用资源:BUFMS
WEE2BZP:   SBIC  EECR,EEWE
           RJMP  PC-1             ;等空
		   OUT   EEARL,XL 
		   OUT   EEARH,XH
		   LDS   BUFMS,PSETDH
           OUT   EEDR,BUFMS       ;写高字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           SBIC  EECR,EEWE
           RJMP  PC-1             ;等空
		   ADIW  XL,1
           OUT   EEARL,XL 
		   OUT   EEARH,XH
           LDS   BUFMS,PSETDL
           OUT   EEDR,BUFMS       ;写低字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           NOP
		   RET
;------------------------------------
;MSUB 、写EEPROM 2字节参数专用子程序(带十进制转十六进制)
;------------------------------------
;入口条件:地址放X中，十进制数在PSETDH/PSETDL中
;出口条件:
;占用资源:BUFMS CNTMS ZH ZL
WEE2BPSUB: NOP
;先把十进制数转为十六制进数
           LDS   ZL,PSETDL
		   MOV   BUFMS,ZL
		   ANDI  ZL,0X0F
		   SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   LDI   CNTMS,10
		   MUL   BUFMS,CNTMS
		   ADD   ZL,R0M
;----------
           CLR   ZH
		   LDS   BUFMS,PSETDH
		   ANDI  BUFMS,0X0F
		   LDI   CNTMS,100
		   MUL   BUFMS,CNTMS
		   ADD   ZL,R0M
		   ADC   ZH,R1M
;-----------
           LDS   CNTMS,PSETDH
           SWAP  CNTMS
		   ANDI  CNTMS,0X0F
JSZGWLOOP: TST   CNTMS
		   BREQ  JSZGWEND  
		   DEC   CNTMS
		   LDI   BUFMS,0XE8       ;最高位以1当1000（3E8）
           ADD   ZL,BUFMS
		   LDI   BUFMS,0X03
           ADC   ZH,BUFMS
           RJMP  JSZGWLOOP    
JSZGWEND:  NOP
;---------------------
           SBIC  EECR,EEWE
           RJMP  PC-1             ;等空
		   OUT   EEARL,XL 
		   OUT   EEARH,XH
           OUT   EEDR,ZH          ;写高字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           SBIC  EECR,EEWE
           RJMP  PC-1             ;等空
		   ADIW  XL,1
           OUT   EEARL,XL 
		   OUT   EEARH,XH
           OUT   EEDR,ZL          ;写低字节
           SBI   EECR,EEMWE 
           SBI   EECR,EEWE
           NOP
		   RET
;--------------------------------------
;SUB:写入EEPROM N字节子程序              
;--------------------------------------
;入口条件:EE首地址放X中，取数指针在Z中 字节数放CNTMS中，
;出口条件:
;占用资源:BUFMS
WEENBSUB: WDR  
;---------
WEENBCON: SBIC   EECR,EEWE
          RJMP   PC-1 
          OUT    EEARH,XH
		  OUT    EEARL,XL
		  LD     BUFMS,Z+
          OUT    EEDR,BUFMS
          SBI    EECR,EEMWE 
          SBI    EECR,EEWE
		  LDI    BUFMS,1           ;EE地址+1
		  ADD    XL,BUFMS
		  BRCC   PC+2
		  INC    XH
          DEC    CNTMS
		  BRNE   WEENBCON
		  NOP
		  RET
;--------------------------------------
;SUB:读EEPROM N字节子程序
;--------------------------------------
;入口条件:EE首地址放X中,存数指针在Z中,字节数放CNTMS中
;出口条件:数据放Z指针中
;占用资源:BUFIS
REENBSUB: WDR
          SBIC   EECR,EEWE
          RJMP   PC-1              ;等空
;----------------
REENBCON: OUT    EEARH,XH
          OUT    EEARL,XL
          NOP
          SBI    EECR,EERE
          IN     BUFIS,EEDR
		  ST     Z+,BUFIS
          LDI    BUFMS,1           ;EE地址+1
		  ADD    XL,BUFMS
		  BRCC   PC+2
		  INC    XH
          DEC    CNTMS
		  BRNE   REENBCON
		  RET
;----------------------------------------
   
;****************************************
;对象4处理程序：每300MS对象
;****************************************
ObjectP4:  ANDI  BUFM,0B11101111
           STS   Message1,BUFM
;========================================
           SBRS  FlagM,BKeyS
		   RJMP  TASK30MSSTART
		   CLR   BUFM
           OUT   PORTE,BUFM
		   RJMP  WTCMEND           ;在键盘设置状态则不跳过控温控处理
;========================================
;300MS主TASK1:水控温控管理
;a、异常工作状态检测（超90度、漏水、缺源水、缺热水检测）
TASK30MSSTART:
    	   SBIC  PINE,BLEAKI       ;漏水否?
           RJMP  YSPROLS           ;漏水了!
;--------
           LDS   BUFM,PortIVC
		   SBRC  BUFM,B90DI
		   RJMP  YSPROEO90         ;外部超90度了!  
		   SBRC  BUFM,BRSKI
		   RJMP  YSPROQRS          ;缺热水了!
		   SBRC  BUFM,BYSKI
		   RJMP  YSPROQYS          ;缺源水了!
		   SBRC  BUFM,B85DI
		   RJMP  YSPROEO85         ;超85度了!
;无异常
;-----------
       CALL DDT16Off
	   nop
	   call DDS10OFF
	   NOP
	   CALL DDS4OFF
	   NOP
	   CALL DDS2OFF
	   NOP
	   CALL DDS3OFF
		   LDS   BUFM,MLEDFFEN
		   ANDI  BUFM,0B11110111   ;关报警灯
		   STS   MLEDFFEN,BUFM
;-----------
	       RJMP  YSPROOK           ;水温均无异常
;------------------------------------
;异常程序_漏水
YSPROLS:   IN    BUFM,PORTE
           ANDI  BUFM,0B11000000   ;有漏水故障则水和电全关
           OUT   PORTE,BUFM
;-----------
           LDI   BUFM,06           ;漏水
           STS   RUNCOD,BUFM
;-----------
		   LDS   BUFM,MLEDFFEN
		   ORI   BUFM,0B00001000   ;开报警灯
		   STS   MLEDFFEN,BUFM
;-----------
           CALL  VOAMS18           ;置18段：“刷卡水控故障”
;-----------
           NOP
		   CALL  DDS10On           ;显示漏保
           NOP
           RJMP  WTCMEND
;-------------------------------------
;异常程序_外部超90度了
YSPROEO90: IN    BUFM,PORTE
           ANDI  BUFM,0B11000100   ;关：加热、排水阀、开水阀、温水阀
           ORI   BUFM,0B00000100   ;开：进水阀
           OUT   PORTE,BUFM
;-----------
		   LDS   BUFM,MLEDFFEN     ;开报警灯
		   ORI   BUFM,0B00001000
		   STS   MLEDFFEN,BUFM
;-----------
           CALL   VOAMS22          ;置22段：“高温超标”
           RJMP   WTCMEND
;-------------------------------------
;异常程序_缺热水
YSPROQRS: IN     BUFMS,PORTE
          ANDI   BUFMS,0B11000100  ;关：加热、排水阀、开水阀、温水阀
          ORI    BUFMS,0B00000100  ;开：进水阀
          OUT    PORTE,BUFMS
;-----------
		  LDS    BUFM,MLEDFFEN     ;开报警灯
		  ORI    BUFM,0B00001000
		  STS    MLEDFFEN,BUFM
;-----------
          CALL   VOAMS9            ;置9段：“请注意，现在缺水”
          RJMP   WTCMEND
;-------------------------------------
;异常程序_缺源水
YSPROQYS: IN     BUFM,PORTE
          ANDI   BUFM,0B11111111   ;缺水不单独控水温
          OUT    PORTE,BUFM
		  LDI    BUFM,02           ;2:为缺源水
          STS    RUNCOD,BUFM
		  CBI    PORTE,BJSFO       ;缺源水则关制水阀
;-----------
		  LDS    BUFM,MLEDFFEN     ;开报警灯
		  ORI    BUFM,0B00001000
		  STS    MLEDFFEN,BUFM
;-----------
          CALL   VOAMS9            ;置9段：“请注意，现在缺水”
          NOP
	 CLR    BUFM
	 STS    WSwitQ,BUFM
          RJMP   WTCMEND           ;缺源水就是前级缺水                      
;-------------------------------------
;异常程序_外部超85度了
YSPROEO85:IN     BUFM,PORTE
          ANDI   BUFM,0B11000100   ;关：加热、排水阀、开水阀、温水阀
          ORI    BUFM,0B00000100   ;开：进水阀
          OUT    PORTE,BUFM
;-----------
		  LDS    BUFM,MLEDFFEN     ;开报警灯
		  ORI    BUFM,0B00001000
		  STS    MLEDFFEN,BUFM
;-----------
          CALL   VOAMS22  ;置22段：“高温超标”
          RJMP   WTCMEND
;-------------
SSTASK2aEND:NOP                    ;半秒任务2a结束
YSPROOK:  NOP                      ;水温均无异常入口
;----------------------------------------
;b、水位检测       
          LDS    BUFM,PortIVC
		  SBRS   BUFM,BXSKI        ;测下水位
          RJMP	 GYTWPRO	  
          SBI    PORTE,BJSFO       ;低于下水位则开进水阀(制水)
          LDI    BUFM,1            ;1:制水        
          STS    RUNCOD,BUFM

          LDS    CNTM,TXCOMMTC
          DEC    CNTM
		  STS    TXCOMMTC,CNTM
		  BRNE   NOTXCOMM5
          CALL   MRWOSUB5          ;发制水开命令
		  NOP
          CALL   INDEL10MS
NOTXCOMM5:NOP
		  CALL   DDS5Off
		  NOP
          CALL   DDS6Off
          RJMP   WWHLCEND
;------------
;高于下水位
GYTWPRO:  CALL   DDS5Off
          NOP 
          CALL   DDS6On
          SBRS   BUFM,BSSKI        ;测上水位(水满断!!)
          RJMP   WWHLCEND		   ;处于中间水位则不动作
          CBI    PORTE,BJSFO       ;高于上水位则关进水阀
          LDI    BUFM,3            ;3:待机(非制水)
		  STS    RUNCOD,BUFM
          CALL   DDS5ON
		  NOP
		  CALL   DDS6On
;--------
          LDS    CNTM,TXCOMMTC
          DEC    CNTM
		  STS    TXCOMMTC,CNTM
		  BRNE   NOTXCOMMg
          CALL   MRWOSUB6          ;发制水关命令
		  NOP
          CALL   INDEL10MS
NOTXCOMMg:NOP
;----------------- 
WWHLCEND: NOP                      ;水位高低检测END
;-----------------
;C、温水测控处理  @9
;----------------------------------
          CALL    WRequestMS       ;整合取水请放WRequest（D2:冰水，D1:温水，D0：开水）
;-------------------------
          LDI    XH,HIGH(EEZREN)
          LDI    XL,LOW(EEZREN)
		  CALL   REE1BSUB
		  CPI    XL,0X55
		  BRNE   ONZRENSTA
;-----------------------
;制热未使能
          CALL   DDS9Off          ;关“加热”显示
          NOP
		  CALL   DDS8On           ;开“饮用”显示
          NOP
		  LDS    BUFM,WRequest
		  STS    WSwitQ,BUFM
          CBI    PORTE,BAZJRO     ;关A组加热
		  NOP
          CBI    PORTE,BBZJRO     ;关B组加热
		  RJMP   LEDQWMC          ;不处理温度
;-------------------------
;在制热使能状态
;-------------------------
ONZRENSTA:NOP
;温水取水处理
          CLR     BUFM
		  STS     WSwitQ,BUFM
		  LDS     BUFM,WRequest
		  SBRC    BUFM,1
		  RJMP    YQWSKAX
;----------
;无取温水键按下
          CBI     PORTE,BWSFO      ;确保关温水
		  LDS     BUFM,WSwitQ
		  ANDI    BUFM,0B11111101
          LDI     CNTM,1           ;第一次就启动语音
          STS     VOTCNTWS,CNTM
          RJMP    WWQSPEND
;-------------------
;有取温水键按下
YQWSKAX:  NOP      
	      LDS     BUFM,VOTCNTWS
          DEC     BUFM
		  STS     VOTCNTWS,BUFM
		  BRNE    TOWSPRO
;20S重置报语音
          LDI     BUFM,200          ;200*300MS=60秒到则重播  &&&
          STS     VOTCNTWS,BUFM
		  CALL    VOAMS5           ;段5:"本温开水为开水降温，安全卫生，请放心饮用"
;---------------
TOWSPRO:  LDS     BUFM,WSwitQ
		  SET
		  BLD     BUFM,1           ;D1 温水
          STS     WSwitQ,BUFM
WWQSPEND: NOP                      ;温水取水结束
;-------------------------
;温水加热管理
          LDS     BUFM,WWORKS 
		  SBRC    BUFM,0
		  RJMP    HasBWK           ;取开水时则温控交由开水统一处理
;------------
;无开水键按下才自处理温水
          LDS     BUFM,ADVSL2    ;@9
	      LDS     CNTM,WSCAVL
	      CP      BUFM,CNTM
		  BRCS    GYWSXX2          ;高于温水下限
;-----------------
;温水低于下限温度
          LDS    BUFM,PortIVC
		  SBRC   BUFM,BXSKI        ;测下水位
          RJMP	 ONWOFFAZ          ;无水则关A组加热	  
;水箱有水 
          SBI     PORTE,BAZJRO     ;低于温水下限且有水则开A组加热
		  CALL    DDS9On           ;开“加热”显示
		  NOP
          RJMP    WWCCPEND
		  NOP
;--------
;高于温水下限
GYWSXX2:  CALL    DDS8On           ;开“饮用”显示 @8
          LDS     CNTM,WSCAVH
   	      CP      BUFM,CNTM
		  BRCS    ONWOFFAZ
;温度处理中区间
          LDS     BUFM,WWorkS
		  SBRS    BUFM,1
		  RJMP    WWCCPEND         ;温水中间值且无取水则不处理
		  SBI     PORTE,BAZJRO     ;中间温度取水时一定开A组加热
		  CALL    DDS9On           ;开“加热”显示
		  RJMP    WWCCPEND
;--------------------------------------
;超过温水上限值
ONWOFFAZ: CBI     PORTE,BAZJRO     ;高于温水上限则关A组加热
          CALL    DDS9Off          ;关“加热”显示
;------------------ 
WWCCPEND: NOP                      ;温水测控处理END
          LDS     BUFM,ADVSL2
		  CPI     BUFM,DT90        ;温水最高温度限制 90度!
		  BRCC    PC+2
		  CBI     PORTE,BAZJRO     ;高于90度则强关A组加热
;-------------------------------------
;d、开水温控管理
HasBWK:   NOP
          LDS     BUFM,WRequest
		  BST     BUFM,0
		  LDS     BUFM,WSwitQ
          BLD     BUFM,0
		  STS     WSwitQ,BUFM
;-----------------------------          
          LDS    BUFM,PortIVC
		  SBRS   BUFM,BXSKI        ;测下水位
          RJMP   SXESKSPRO         ;水箱有水
;无源水不加热		  	
		  CBI     PORTE,BAZJRO     ;无条件关闭A组加热
		  NOP
		  CBI     PORTE,BBZJRO     ;无条件关闭B组加热
		  CALL    DDS9Off          ;关“加热”显示
		  RJMP    BWCCPEND
;----------------------------------
;有源水
SXESKSPRO:LDS     BUFM,ADVSL1
		  LDS     CNTM,KSCAVL      ;开水下限值
		  CP      BUFM,CNTM
		  BRCC    PC+2
		  RJMP    GYKSXXPRO        ;高于开水温度下限
;低于开水温度下限
          CBI     PORTE,BKSFO      ;关开水阀
;------------------------------
GYKSXXPRO:LDS     BUFM,WRequest
          SBRC    BUFM,0
          RJMP    YZQSZZT          ;已在取开水状态
;------------------------------
;在未取开水态
		  LDI     CNTM,1           ;语音时间初值
          STS     VOTCNTKS,CNTM
		  LDI     CNTM,10          ;预置3秒通断时间
          STS     CNT3SKS,CNTM
		  CBI     PORTE,BBZJRO     ;未取开水就不加热
          RJMP    BWCCPEND 
;----------------------------
;已在取开水状态 
YZQSZZT:  LDS     BUFM,ADVSL1
		  LDS     CNTM,KSCAVH      ;开水上限值
		  CP      BUFM,CNTM
		  BRCC    PC+2
		  RJMP    GYKSSXPRO        ;高于开水温度上限
;--------------------
;低于开水上限,两组同时加热
          SBI     PORTE,BAZJRO
		  NOP
		  SBI     PORTE,BBZJRO
		  CALL    DDS9On           ;开“加热”显示
          RJMP    BWCCPEND
;--------------------
;高于开水温上限,两组间断加热
GYKSSXPRO:LDS     BUFM,CNT3SKS
		  DEC     BUFM
		  STS     CNT3SKS,BUFM
		  BRNE    BWCCPEND
          LDI     BUFM,10          ;3秒   &&&
		  STS     CNT3SKS,BUFM 
;3秒到则加热取反
		  SBIC    PORTE,BBZJRO  
          RJMP    PC+6
		  SBI     PORTE,BBZJRO
		  SBI     PORTE,BAZJRO
		  CALL    DDS9On           ;开“加热”显示
          RJMP    BWCCPEND
          CBI     PORTE,BBZJRO
		  CBI     PORTE,BAZJRO
		  CALL    DDS9Off          ;关“加热”显示
;-----------------------
;超高温保护
          LDS     BUFM,ADVSL1
		  CPI     BUFM,DT95        ;&&&  ！！！
		  BRCC    PC+4
		  CBI     PORTE,BAZJRO     ;开水高于95度则强关加热
		  NOP
		  CBI     PORTE,BBZJRO
;-----------------------
BWCCPEND: NOP                      ;开水测控处理END
;----------------------------------------
;冰水管理
          LDS     BUFM,WRequest
		  BST     BUFM,2
		  LDS     BUFM,WSwitQ
          BLD     BUFM,2
		  STS     WSwitQ,BUFM     ;@7
;----------------------------------------
;e、LED灯(取水)监控
LEDQWMC:  LDS     BUFM,MLEDFFEN
          CLT
          LDS     CNTM,WRequest
		  ANDI    CNTM,0b00000111
		  BREQ    PC+2
		  SET
          BLD     BUFM,BLEDQS      ;存LED取水灯使能位
          STS     MLEDFFEN,BUFM
          NOP                      ;LED灯（取水）监控结束
;--------------------------------------
WTCMEND:  NOP                      ;至此水控温控管理全部结束
;-------------------------------------
;*************************************
;300MS主TASK2:



;========================================
ObjectP4END:NOP                    ;对象4全部任务结束
           JMP    MainPro
;****************************************
;对象5处理程序：每500MS对象
;****************************************
ObjectP5: ANDI   BUFM,0B11011111
          STS    Message1,BUFM
;========================================
;半秒主程序TASK1：运行LED灯取反
          LDS    BUFM,PORTG
		  SBRC   BUFM,0      
          RJMP   PC+3
		  ORI    BUFM,0b00000001
		  RJMP   PC+2
          ANDI   BUFM,0b11111110
		  STS    PORTG,BUFM
SSTASK1END: NOP
;---------------------------------------
;半秒主程序TASK2：时钟闪烁管理
          SBRS   FLAGI,BCLKS
		  RJMP   NOCLKSTA
          LDS    BUFM,PORTG
		  SBRC   BUFM,0     
          RJMP   PC+4
		  CALL   DDP1Off
		  RJMP   NOCLKSTA
		  NOP
          CALL   DDP1On
NOCLKSTA: NOP
;========================================
ObjectP5END:NOP                    ;对象5全部任务结束
          JMP    MainPro
;****************************************
;对象6处理程序：1S对象
;****************************************
ObjectP6:  ANDI  BUFM,0B10111111
           STS   Message1,BUFM
;========================================
           SBRS  FlagM,BKeyS
		   RJMP  Obj6START
;------------------------
;在键盘态
		   LDS   CNTM,KDelay
		   INC   CNTM
		   STS   KDelay,CNTM
		   RJMP  ObjectP6END       ;在键盘设置状态则跳过
;----------------------------------------
Obj6START:NOP                      ;对象6开始
;----------------------------------------
;1秒主TASK1:单线程任务
;----------------------------------------
;a、时钟管理        
;----------------------------------------
;软件计时
		   LDS   CNTM,CLKSCNT
		   DEC   CNTM
		   STS   CLKSCNT,CNTM
		   BRNE  SOFINCPRO
;更新时间到
           LDI   CNTM,61
		   STS   CLKSCNT,CNTM
           CALL  RCLKMS            ;每分钟更新日期/时间
           NOP
		   CALL  INDEL10MS
           RJMP  CLKAJEND
;----------------
SOFINCPRO: LDS   CNTM,CLKSS
		   INC   CNTM
		   STS   CLKSS,CNTM
		   LDI   BUFM,6
		   ADD   CNTM,BUFM
		   BRHC  CLKAJEND
           STS   CLKSS,CNTM
           LDI   BUFM,0XA0
		   ADD   CNTM,BUFM
           BRCC  CLKAJEND
           STS   CLKSS,CNTM
CLKAJEND:  NOP
;----------------------------------------
;b、装载流速数码
           CLR    CNTM
		   CLR    XH
		   LDS    XL,SpeedOP1
		   LDS    BUFM,SpeedOP2
           ADD    XL,BUFM
		   ADC    XH,CNTM
		   LDS    BUFM,SpeedOP3
           ADD    XL,BUFM
		   ADC    XH,CNTM
           CALL   DDCSOn            ;显示数据流速开 
		   NOP
;========================================
;C、每秒时对水流速流量处理
;----------------------------------------
;对每路用水升数计量
;----------------------------
;判1路(开水)流量是否满升
           LDS   XL,FlowSP1L
           LDS   XH,FlowSP1H
		   CLR   BUFM
		   LDS   CNTM,SPEEDOP1
           ADD   XL,CNTM
		   ADC   XH,BUFM
           STS   FlowSP1L,XL
		   STS   FlowSP1H,XH
           LDI   CNTM,0XFD         ;600的补码为 FDA8
		   LDI   BUFM,0XA8
		   ADD   XL,BUFM
		   ADC   XH,CNTM
		   BRCS  PC+2
           RJMP  FHINCEND1         ;1路未到1升
           STS   FlowSP1L,XL
		   STS   FlowSP1H,XH
;---------------------
;到则对1路流量加1升
           LDS   CNTM,Flow1H0
           INC   CNTM
           STS   Flow1H0,CNTM
		   LDI   XH,HIGH(EEFS1H0)
           LDI   XL,LOW(EEFS1H0)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND1
;--------
           LDS   CNTM,Flow1H1
           INC   CNTM
		   STS   Flow1H1,CNTM
           LDI   XH,HIGH(EEFS1H1)
           LDI   XL,LOW(EEFS1H1)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND1
;--------
           LDS   CNTM,Flow1H2
           INC   CNTM
		   STS   Flow1H2,CNTM
           LDI   XH,HIGH(EEFS1H2)
           LDI   XL,LOW(EEFS1H2)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND1
;--------
           LDS   CNTM,Flow1H3
           INC   CNTM
		   STS   Flow1H3,CNTM
		   LDI   XH,HIGH(EEFS1H3)
           LDI   XL,LOW(EEFS1H3)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB 
;--------
FHINCEND1: NOP
;----------------------------
;判2路(温水)流量是否满升
           LDS   XL,FlowSP2L
           LDS   XH,FlowSP2H
		   CLR   BUFM
		   LDS   CNTM,SPEEDOP2
           ADD   XL,CNTM
		   ADC   XH,BUFM
           STS   FlowSP2L,XL
		   STS   FlowSP2H,XH
           LDI   CNTM,0XFD         ;600的补码为 FDA8
		   LDI   BUFM,0XA8
		   ADD   XL,BUFM
		   ADC   XH,CNTM
		   BRCS  PC+2
           RJMP  FHINCEND2         ;2路未到1升
           STS   FlowSP2L,XL
		   STS   FlowSP2H,XH
;---------------------
;到则对2路流量加1升
           LDS   CNTM,Flow2H0
           INC   CNTM
           STS   Flow2H0,CNTM
		   LDI   XH,HIGH(EEFS2H0)
           LDI   XL,LOW(EEFS2H0)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND2
;--------
           LDS   CNTM,Flow2H1
           INC   CNTM
		   STS   Flow2H1,CNTM
           LDI   XH,HIGH(EEFS2H1)
           LDI   XL,LOW(EEFS2H1)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND2
;--------
           LDS   CNTM,Flow2H2
           INC   CNTM
		   STS   Flow2H2,CNTM
           LDI   XH,HIGH(EEFS2H2)
           LDI   XL,LOW(EEFS2H2)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND2
;--------
           LDS   CNTM,Flow2H3
           INC   CNTM
		   STS   Flow2H3,CNTM
		   LDI   XH,HIGH(EEFS2H3)
           LDI   XL,LOW(EEFS2H3)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB 
;--------
FHINCEND2: NOP
;----------------------------
;判3路(冰水)流量是否满升
           LDS   XL,FlowSP3L
           LDS   XH,FlowSP3H
		   CLR   BUFM
		   LDS   CNTM,SPEEDOP3
           ADD   XL,CNTM
		   ADC   XH,BUFM
           STS   FlowSP3L,XL
		   STS   FlowSP3H,XH
           LDI   CNTM,0XFD         ;600的补码为 FDA8
		   LDI   BUFM,0XA8
		   ADD   XL,BUFM
		   ADC   XH,CNTM
		   BRCS  PC+2
           RJMP  FHINCEND3         ;3路未到1升
           STS   FlowSP3L,XL
		   STS   FlowSP3H,XH
;---------------------
;到则对3路流量加1升
		   LDS   CNTM,Flow3H0
           INC   CNTM
           STS   Flow3H0,CNTM
		   LDI   XH,HIGH(EEFS3H0)
           LDI   XL,LOW(EEFS3H0)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND3
;--------
           LDS   CNTM,Flow3H1
           INC   CNTM
		   STS   Flow3H1,CNTM
           LDI   XH,HIGH(EEFS3H1)
           LDI   XL,LOW(EEFS3H1)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND3
;--------
           LDS   CNTM,Flow3H2
           INC   CNTM
		   STS   Flow3H2,CNTM
           LDI   XH,HIGH(EEFS3H2)
           LDI   XL,LOW(EEFS3H2)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  FHINCEND3
;--------
           LDS   CNTM,Flow3H3
           INC   CNTM
		   STS   Flow3H3,CNTM
		   LDI   XH,HIGH(EEFS3H3)
           LDI   XL,LOW(EEFS3H3)
		   MOV   BUFMS,CNTM
           CALL  WEE1BSUB 
;--------
FHINCEND3: NOP
;============================
;计算水流总量并调整为十进制数
;----------------------------
;累加三路脉冲值
           LDS   XL,FlowAPL
           LDS   XH,FlowAPH
		   CLR   BUFM
		   LDS   CNTM,SPEEDOP1
           ADD   XL,CNTM
		   ADC   XH,BUFM
		   LDS   CNTM,SPEEDOP2
           ADD   XL,CNTM
		   ADC   XH,BUFM
		   LDS   CNTM,SPEEDOP3
           ADD   XL,CNTM
		   ADC   XH,BUFM
		   STS   FlowAPL,XL
		   STS   FlowAPH,XH
;-----------------------------
		   LDI   CNTM,0XFD         ;600的补码为 FDA8
		   LDI   BUFM,0XA8
		   SET
		   ADD   XL,BUFM
		   ADC   XH,CNTM
		   BRCS  PC+4
           SUB   XL,BUFM           ;还原
		   SBC   XH,CNTM
           CLT
		   BLD   FLAGM,B1LTO       ;1升到标志
           STS   FlowAPL,XL
		   STS   FlowAPH,XH
;----------------------
           CLR   CNTM              ;计算小数
           SBIW  XL,6
           BRCS  PC+3
		   INC   CNTM
		   RJMP  PC-3
;---------------------        
           CLR   XH
		   MOV   XL,CNTM
           CALL  HexDecMS
           STS   FlowD0,XL 
;----------
           SBRS  FLAGM,B1LTO
		   RJMP  J1TZEND           ;未到1升则跳过
;-----------------------------
 ;满一升水则加一
           LDS   BUFMS,FlowD1
		   INC   BUFMS
		   CALL DECINCTZMS        ;十进数加1调整 
           STS   FlowD1,BUFMS
		   MOV   CNTM,BUFMS
           LDI   XH,HIGH(EEFASD1)
           LDI   XL,LOW(EEFASD1)
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  J1TZEND
;-------------
           LDS   BUFMS,FlowD2
		   INC   BUFMS
		   CALL DECINCTZMS        ;十进数加1调整
           STS   FlowD2,BUFMS
		   MOV   CNTM,BUFMS
           LDI   XH,HIGH(EEFASD2)
           LDI   XL,LOW(EEFASD2)
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  J1TZEND
;-------------
           LDS   BUFMS,FlowD3
		   INC   BUFMS
		   RCALL DECINCTZMS        ;十进数加1调整
           STS   FlowD3,BUFMS
		   MOV   CNTM,BUFMS
           LDI   XH,HIGH(EEFASD3)
           LDI   XL,LOW(EEFASD3)
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  J1TZEND
;-------------
           LDS   BUFMS,FlowD4
		   INC   BUFMS
		   RCALL DECINCTZMS        ;十进数加1调整
           STS   FlowD4,BUFMS
		   MOV   CNTM,BUFMS
           LDI   XH,HIGH(EEFASD4)
           LDI   XL,LOW(EEFASD4)
           CALL  WEE1BSUB
		   TST   CNTM
		   BRNE  J1TZEND
;-------------
           LDS   BUFMS,FlowD5
           INC   BUFMS
		   RCALL DECINCTZMS        ;十进数加1调整
           STS   FlowD5,BUFMS
		   MOV   CNTM,BUFMS
           LDI   XH,HIGH(EEFASD5)
           LDI   XL,LOW(EEFASD5)
           CALL  WEE1BSUB		   
J1TZEND:   NOP                     ;加1调整结束
;========================================
;00 测试专用
  ;        CALL  MRWOSUB0          
  ;		   NOP
  ;        CALL  INDEL10MS
;========================================
;d、售水管理                 @9
;========================================
;入口参数：WSwItQ WatENB SFMode
;出口参数：QWType
           LDS   BUFM,PortTIVF
           SBRC  BUFM,4
		   RJMP  PC+5
		   LDI   CNTM,0X55         ;拨码D0=0为单机，一定是非收费模式
           STS   SFMode,CNTM
		   RJMP  FORKSOUTGL
           NOP
;在联机状态 
           LDI   XH,HIGH(EESFmode)
		   LDI   XL,LOW(EESFmode)
		   CALL  REE1BSUB  
           STS   SFMode,XL
;=========================================
;对开水出水管理
FORKSOUTGL:LDS   BUFM,WWorkS
		   SBRC  BUFM,0
		   RJMP  ONOUTSKS         ;开水已在出水状态了
;开水还没有在出水状态
           LDS   BUFM,WSwItQ
		   SBRS  BUFM,0
		   RJMP  KSOUTGLEND
;开水有出水请求
           LDS   BUFM,SFMode
		   CPI   BUFM,0XAA
		   BREQ  SFMODEKS          
;非收费模式下，首次进入开水取水态
           LDI   BUFM,0XEA         ;装最大100升的脉冲数
		   STS   WatKSPH,BUFM
		   LDI   BUFM,0X60
		   STS   WatKSPL,BUFM
;-------------
;首次出开水
FOUTKS:
;水杯检测         
		   LDS   BUFM,PortIVD
		   SBRS  BUFM,CUPIN
           RJMP  QKSOK            ;有杯
;--------
           CALL  VOAMS7           ;段7: "请放好水杯"
		   CLT
           BLD   FlagM,BITKS
           RJMP  KSOUTGLEND
QKSOK:     NOP
;------------
;检测开水温度是否到高于下线
          LDS     BUFM,ADVSL1
		  LDS     CNTM,KSCAVL      ;开水下限值
		  CP      BUFM,CNTM
		  BRCC    PC+2
		  RJMP    KSGYXXP         ;高于开水温下限
;--------------------
;低于开水下限,两组同时加热
          SBI     PORTE,BAZJRO
		  NOP
		  SBI     PORTE,BBZJRO
		  CALL    DDS9On           ;开“加热”显示
;--------------------
          LDS     BUFM,VOTCNTWS
          DEC     BUFM
		  STS     VOTCNTWS,BUFM
		  BRNE    PC+6
;20S重置报语音
          LDI     BUFM,20          ;20秒到则重播  &&&
          STS     VOTCNTWS,BUFM
		  CALL    VOAMS4           ;"欢迎使用高温开水"
		  NOP
		  NOP
          RJMP    KSOUTGLEND 
;--------------------
;高于开水温度下限
KSGYXXP:   CALL  VOAMS4            ;"欢迎使用高温开水"
           LDS   BUFM,WWorkS
           ORI   BUFM,0B00000001   ;置开水出水工作状态  
		   STS   WWorkS,BUFM
           CLR   BUFM
		   STS   FlowCP1H,BUFM     ;1路当次脉冲计数清0
		   STS   FlowCP1L,BUFM
		   SBI   PORTE,BKSFO       ;打开开水阀
		   RJMP  KSOUTGLEND
;--------------------------
;在收费模式（开水）
SFMODEKS:  LDS   BUFM,WatENB
		   SBRC  BUFM,0        
		   RJMP  FOUTKS
		   RJMP  KSOUTGLEND       ;本次开水未授权
;------------------------------------
;在出水状态了（开水）		   
ONOUTSKS:  LDS   BUFM,WSwItQ         
           SBRS  BUFM,0
           RJMP  OFFOUTKS         ;停止取开水
;开水键还未放开则继续扣费
           LDS   XL,WatKSPL
           LDS   XH,WatKSPH
           LDS   ZL,FlowCP1L
           LDS   ZH,FlowCP1H
		   SUB   XL,ZL
		   SBC   XH,ZH
		   BRCC  KSOUTGLEND
;--------------------
;开水费用尽或停止取水
OFFOUTKS:  LDS   BUFM,WWorkS	   
           ANDI  BUFM,0B11111110   ;清开水工作状态  
		   STS   WWorkS,BUFM		   
           LDS   BUFM,WatENB
		   CLT
		   BLD   BUFM,0            ;清本次开水授权位
           STS   WatENB,BUFM
		   LDI   CNTM,1            ;取水类型：1为开水，2为温水，3为冰水
		   STS   QWType,CNTM   
           CBI   PORTE,BKSFO       ;关开水
           RJMP  OFFWMTPRO         ;关水后统一处理部分
KSOUTGLEND:NOP                     ;开水出水管理结束
;======================================
;对温水出水管理
           LDS   BUFM,WWorkS
		   SBRC  BUFM,1
		   RJMP  ONOUTSWS         ;温水已在出水状态了
;温水还没有在出水状态
           LDS   BUFM,WSwItQ
		   SBRS  BUFM,1
		   RJMP  WSOUTGLEND
;温水有出水请求
           LDS   BUFM,SFMode
		   CPI   BUFM,0XAA
		   BREQ  SFMODEWS          
;非收费模式下，首次进入温水取水态
           LDI   BUFM,0XEA         ;装最大100升的脉冲数
		   STS   WatWSPH,BUFM
		   LDI   BUFM,0X60
		   STS   WatWSPL,BUFM
;-------------
;首次出温水
FOUTWS:	  
;-----------
;水杯检测         
		   LDS   BUFM,PortIVD
		   SBRS  BUFM,CUPIN
           RJMP  QKSOK2            ;有杯
;--------
           CALL  VOAMS7            ;段7: "请放好水杯"
		   CLT
           BLD   FlagM,BITKS
           RJMP  WSOUTGLEND
;---------
QKSOK2:    LDS   BUFM,WWorkS
           ORI   BUFM,0B00000010   ;置温水出水工作状态  
		   STS   WWorkS,BUFM
           CLR   BUFM
		   STS   FlowCP2H,BUFM     ;2路当次脉冲计数清0
		   STS   FlowCP2L,BUFM
		   SBI   PORTE,BWSFO       ;打开温水阀
		   RJMP  WSOUTGLEND
;--------------------------
;在收费模式(温水)
SFMODEWS:  LDS   BUFM,WatENB
		   SBRC  BUFM,1        
		   RJMP  FOUTWS
		   RJMP  WSOUTGLEND       ;本次温水未授权
;------------------------------------
;在出水状态了（温水）		   
ONOUTSWS:  LDS   BUFM,WSwItQ         
           SBRS  BUFM,1
           RJMP  OFFOUTWS         ;停止取温水
;温水键还未放开则继续扣费
           LDS   XL,WatWSPL
           LDS   XH,WatWSPH
           LDS   ZL,FlowCP2L
           LDS   ZH,FlowCP2H
		   SUB   XL,ZL
		   SBC   XH,ZH
		   BRCC  WSOUTGLEND
;--------------------
;温水费用尽或停止取水
OFFOUTWS:  LDS   BUFM,WWorkS	   
           ANDI  BUFM,0B11111101   ;清温水工作状态  
		   STS   WWorkS,BUFM		   
           LDS   BUFM,WatENB
		   CLT
		   BLD   BUFM,1            ;清本次温水授权位
           STS   WatENB,BUFM
		   LDI   CNTM,2            ;取水类型：1为开水，2为温水，3为冰水
		   STS   QWType,CNTM   
           CBI   PORTE,BWSFO       ;关温水
           RJMP  OFFWMTPRO         ;关水后统一处理部分
WSOUTGLEND:NOP                     ;温水出水管理结束
;======================================
;对冰水出水管理
           LDS   BUFM,WWorkS
		   SBRC  BUFM,2
		   RJMP  ONOUTSBS         ;冰水已在出水状态了
;冰水还没有在出水状态
           LDS   BUFM,WSwItQ
		   SBRS  BUFM,2
		   RJMP  BSOUTGLEND
;冰水有出水请求
           LDS   BUFM,SFMode
		   CPI   BUFM,0XAA
		   BREQ  SFMODEBS          
;非收费模式下，首次进入冰水取水态
           LDI   BUFM,0XEA         ;装最大100升的脉冲数
		   STS   WatBSPH,BUFM
		   LDI   BUFM,0X60
		   STS   WatBSPL,BUFM
;-------------
;首次出冰水
FOUTBS:	
;水杯检测         
		   LDS   BUFM,PortIVD
		   SBRS  BUFM,CUPIN
           RJMP  QKSOK3            ;有杯
;--------
           CALL  VOAMS7            ;段7: "请放好水杯"
		   CLT
           BLD   FlagM,BITKS
           RJMP  BSOUTGLEND
;---------
QKSOK3:    LDS   BUFM,WWorkS
           ORI   BUFM,0B00000100   ;置冰水出水工作状态  
		   STS   WWorkS,BUFM
           CLR   BUFM
		   STS   FlowCP3H,BUFM     ;3路当次脉冲计数清0
		   STS   FlowCP3L,BUFM
           LDS   BUFM,PortTIVF
           SBRC  BUFM,4
           CALL  MRWOSUB3          ;通知ZN板开冰水(单机则不发)
		   NOP
		   CALL  INDEL10MS
		   RJMP  BSOUTGLEND
;--------------------------
;在收费模式(冰水)
SFMODEBS:  LDS   BUFM,WatENB
		   SBRC  BUFM,2        
		   RJMP  FOUTBS
		   RJMP  BSOUTGLEND       ;本次冰水未授权
;------------------------------------
;在出水状态了（冰水）		   
ONOUTSBS:  LDS   BUFM,WSwItQ         
           SBRS  BUFM,2
           RJMP  OFFOUTBS         ;停止取冰水
;冰水键还未放开则继续扣费
           LDS   XL,WatBSPL
           LDS   XH,WatBSPH
           LDS   ZL,FlowCP3L
           LDS   ZH,FlowCP3H
		   SUB   XL,ZL
		   SBC   XH,ZH
		   BRCC  BSOUTGLEND
;--------------------
;冰水费用尽或停止取水
OFFOUTBS:  LDS   BUFM,WWorkS	   
           ANDI  BUFM,0B11111011   ;清冰水工作状态  
		   STS   WWorkS,BUFM		   
           LDS   BUFM,WatENB
		   CLT
		   BLD   BUFM,2            ;清本次冰水授权位
           STS   WatENB,BUFM
		   LDI   CNTM,3            ;取水类型：1为开水，2为温水，3为冰水
		   STS   QWType,CNTM 
		   LDS   BUFM,PortTIVF
           SBRC  BUFM,4               
	       CALL  MRWOSUB4          ;通知ZN板关冰水（单机跳过）
		   NOP
		   CALL  INDEL10MS
           RJMP  OFFWMTPRO         ;关水后统一处理部分
BSOUTGLEND:NOP                     ;温水出水管理结束
;======================================
           NOP                     ;三水运行处理完毕
		   RJMP  SWGLEND
;======================================
;关水后统一处理部分
OFFWMTPRO: NOP
           LDS   BUFM,PortTIVF
           SBRC  BUFM,4            ;PORTF,4=0为单机模式
		   RCALL MRWOSUB2          ;单机则不发消费表
           SBI   PORTA,BBUZZ       ;蜂鸣1秒
		   CALL  INDEL500MS
		   NOP
		   CBI   PORTA,BBUZZ
;----------------------------------------
SWGLEND:   NOP                     ;售水管理结束
;========================================
;========================================
;取水状态总显示处理
           SBRC  FLAGM,Sale
		   RJMP  ONSALEPRO         ;已在显示状态
;未在显示状态
           LDS   BUFM,WWorkS
		   ANDI  BUFM,0B00000111
		   TST   BUFM
		   BRNE  PC+2
		   RJMP  SalePEND          ;无取水显示任务
;首次进入显示状态  
           SET
           BLD   FLAGM,Sale        ;置为显示状态
           LDS   BUFM,FlowD1       ;存十进制基数
           STS   FlowCBD1,BUFM
		   LDS   BUFM,FlowD0
           STS   FlowCBD0,BUFM
;-------------
           CLR   BUFM
		   STS   FlowCPSH,BUFM     ;当次脉冲计数清0
		   STS   FlowCPSL,BUFM
           STS   FlowCD1,BUFM      ;当次显示清零
	       STS   FlowCD0,BUFM   
		   RJMP  SalePEND
;-------------
;已在显示状态
ONSALEPRO: LDS   BUFM,WWorkS
		   ANDI  BUFM,0B00000111
		   TST   BUFM
		   BRNE  PC+4
;全部取水结束  
		   CLT
           BLD   FLAGM,Sale        ;清显示标志
           RJMP  SalePEND          
;--------------
;继续显示
           NOP
;-----------------------
;由FlowD1/FlowD0和基准值，计算当前流量的十进制数FlowCD1/FlowCD0
;-----------------------
           CALL  DECSUBMS          ;用总数减当次的基数=当前流量
;-----------------------
SalePEND:  NOP                     ;显示处理结束
;=======================================		
;e、温度AD值转换为数字度
;----------------------------------------
           LDS   BUFMS,ADVSL0
		   CALL  QweryAdcTMS
		   STS   THEXD0,BUFMS
		   LDS   BUFMS,ADVSL1
		   CALL  QweryAdcTMS
		   STS   THEXD1,BUFMS
		   LDS   BUFMS,ADVSL2
		   CALL  QweryAdcTMS
		   STS   THEXD2,BUFMS
		   LDS   BUFMS,ADVSL3
		   CALL  QweryAdcTMS
		   STS   THEXD3,BUFMS
;------------------------------------------

;----------------------------------------
;1秒主TASK2:选择第一行数码内容并加载
;----------------------------------------
;流量、时间、电话三选一            ;@7
		   SBRC   FLAGM,Sale
		   RJMP   SaleDISP         ;去售水状态显示
	       LDS   CNTM,H1NLDTim
           INC   CNTM
		   STS   H1NLDTim,CNTM
		   CPI   CNTM,10           ;流量显示10秒
		   BRCC  PC+2
		   RJMP  H1LCDZ_LL
;------
		   CPI   CNTM,20           ;电话显示10秒
		   BRCC  PC+2
		   RJMP  H1LCDZ_DH
;------
           CPI   CNTM,30           ;时间显示10秒
		   BRCC  PC+2
		   RJMP  H1LCDZ_SJ
;------     
           CLR   CNTM
           STS   H1NLDTim,CNTM      
;---------------------------------
;a、装载流量
;---------------------------------
H1LCDZ_LL: NOP
;装载总流量显示数字码
          LDS    R0M,FlowD0
		  LDS    R1M,FlowD1
		  CALL   DecSegMS        ;D1D0双字节转换成段码
          SET
		  LDS    BUFMS,LCDDB15
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB15,R0M
;-------------
		  LDS    BUFMS,LCDDB14
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB14,R1M
;-------------
          LDS    BUFMS,LCDDB13
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB13,R2M
;-------------
		  LDS    BUFMS,LCDDB12
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB12,R3M
;-----------------------------
          LDS    R0M,FlowD2
		  LDS    R1M,FlowD3
		  CALL   DecSegMS        ;D3D2双字节转换成段码
          SET
		  LDS    BUFMS,LCDDB11
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB11,R0M
;-------------
		  LDS    BUFMS,LCDDB10
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB10,R1M
;-------------
          LDS    BUFMS,LCDDB9
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB9,R2M
;-------------
		  LDS    BUFMS,LCDDB8
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB8,R3M
;-----------------------------  
          LDS    R0M,FlowD4
		  LDS    R1M,FlowD5
		  CALL   DecSegMS        ;D5D4双字节转换成段码
          SET
		  LDS    BUFMS,LCDDB7
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB7,R0M
;-------------
		  LDS    BUFMS,LCDDB6
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB6,R1M
;-------------
          LDS    BUFMS,LCDDB5
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB5,R2M
;-------------
		  LDS    BUFMS,LCDDB4
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB4,R3M
;-----------------
          CALL   DDT11Off
		  NOP
		  CALL   DDT12Off
		  NOP
		  CLT
          BLD    FLAGI,BCLKS
		  CALL   DDP1Off
		  NOP
		  CALL   DDP3On
		  NOP
		  CALL   DDT13On
;-------------  		  
		  CLR   BUFM
          STS   ZZDHDTim,BUFM
          RJMP  S1Task2End
;=================================
;装载售水流量显示数字码
SaleDISP: LDS    R0M,FlowCD0
		  LDS    R1M,FlowCD1
		  CALL   DecSegMS        ;D1D0双字节转换成段码
          SET
		  LDS    BUFMS,LCDDB15
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB15,R0M
;-------------
		  LDS    BUFMS,LCDDB14
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB14,R1M
;-------------
          LDS    BUFMS,LCDDB13
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB13,R2M
;-------------
		  LDS    BUFMS,LCDDB12
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB12,R3M
;-----------------------------
          LDS    BUFMS,LCDDB11
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB11,BUFMS
;-------------
          LDS    BUFMS,LCDDB10
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB10,BUFMS
;-------------
          LDS    BUFMS,LCDDB9
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB9,BUFMS
;-------------
          LDS    BUFMS,LCDDB8
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB8,BUFMS
;-------------
		  LDS    BUFMS,LCDDB7
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB7,BUFMS
;-------------
	      LDS    BUFMS,LCDDB6
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB6,BUFMS
;-------------
          LDS    BUFMS,LCDDB5
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB5,BUFMS
;-------------
		  LDS    BUFMS,LCDDB4
		  ANDI   BUFMS,0B10000000
		  STS    LCDDB4,BUFMS
;-----------------
          CALL   DDT11Off
		  NOP
		  CALL   DDT12Off
		  NOP
		  CLT
          BLD    FLAGI,BCLKS
		  CALL   DDP1Off
		  NOP
		  CALL   DDP3On
		  NOP
		  CALL   DDT13On
		  NOP
          CALL   DDT1On
;-------------  		  
		  CLR   BUFM
          STS   ZZDHDTim,BUFM
          RJMP  S1Task2End
;=================================
;b、装载时间
;---------------------------------
H1LCDZ_SJ: NOP
;装载时间显示数字码
          LDS    R0M,CLKMM       ;@4
		  LDS    R1M,CLKHH
		  CALL   DecSegMS        ;时分转段码
          SET
		  LDS    BUFMS,LCDDB15
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB15,R0M
;-------------
		  LDS    BUFMS,LCDDB14
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB14,R1M
;-------------
          LDS    BUFMS,LCDDB13
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB13,R2M
;-------------
		  LDS    BUFMS,LCDDB12
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB12,R3M
;-----------------------------
          LDS    R0M,CLKDD
		  LDS    R1M,CLKMO
		  CALL   DecSegMS        ;月日转段码
          SET
		  LDS    BUFMS,LCDDB11
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB11,R0M
;-------------
		  LDS    BUFMS,LCDDB10
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB10,R1M
;-------------
          LDS    BUFMS,LCDDB9
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB9,R2M
;-------------
		  LDS    BUFMS,LCDDB8
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB8,R3M
;-----------------------------
          LDS    R0M,CLKYY
          LDI    BUFMS,0X20
		  MOV    R1M,BUFMS
		  CALL   DecSegMS        ;年转段码
          SET
		  LDS    BUFMS,LCDDB7
		  SBRC   BUFMS,7
          BLD    R0M,7
		  STS    LCDDB7,R0M
;-------------
		  LDS    BUFMS,LCDDB6
		  SBRC   BUFMS,7
          BLD    R1M,7
		  STS    LCDDB6,R1M
;-------------
          LDS    BUFMS,LCDDB5
		  SBRC   BUFMS,7
          BLD    R2M,7
		  STS    LCDDB5,R2M
;-------------
		  LDS    BUFMS,LCDDB4
		  SBRC   BUFMS,7
          BLD    R3M,7
		  STS    LCDDB4,R3M         
;-----------------
          CALL   DDT12Off
		  NOP
		  CALL   DDT13Off
		  NOP
		  CALL   DDP3Off
		  NOP
		  CALL   DDT1On
;		  CALL   DDP1On
          SET
          BLD    FLAGI,BCLKS
		  CALL   DDT11On
		  NOP
          RJMP  S1Task2End
;---------------------------------
;c、装载电话:
;---------------------------------
H1LCDZ_DH: NOP
;---------------------------------
           LDS   CNTM,ZZDHDTim
		   TST   CNTM
		   BREQ  ZZFWDH
		   CPI   CNTM,5           ;5秒
		   BRNE  PC+2
           RJMP  ZZSJDH
		   INC   CNTM
		   CPI   CNTM,10
		   BRCS  PC+2
		   CLR   CNTM
           STS   ZZDHDTim,CNTM
		   RJMP  ZZDHTEND
;-------------------------        @5
;装载服务电话:400-6066-707
ZZFWDH:    INC   CNTM  
           STS   ZZDHDTim,CNTM
;--------------        
		   LDI   XH,HIGH(EESP5)
           LDI   XL,LOW(EESP5)
           CALL  REE2BSUB
		   MOV   R1M,XH
           MOV   R0M,XL
		   CALL  DecSegMS         ;转成高4位段码
;---------------
           SET
		   LDS   BUFM,LCDDB4
		   SBRC  BUFM,7
           BLD   R3M,7
		   STS   LCDDB4,R3M        ;SEG 1位
;-------           
		   LDS   BUFM,LCDDB5
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB5,R2M        ;SEG 2位
;------- 
		   LDS   BUFM,LCDDB6
		   SBRC  BUFM,7
           BLD   R1M,7
		   STS   LCDDB6,R1M        ;SEG 3位
;-------      
           LDI   BUFMS,0X08        ;-
		   LDS   BUFM,LCDDB7
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB7,BUFMS
;------- 
		   LDS   BUFM,LCDDB8
		   SBRC  BUFM,7
           BLD   R0M,7
		   STS   LCDDB8,R0M        ;SEG 4位
;--------------------
           LDI   XH,HIGH(EESP6)
           LDI   XL,LOW(EESP6)
           CALL  REE2BSUB
		   MOV   R1M,XH
           MOV   R0M,XL
		   CALL  DecSegMS          ;转成中4位段码
;-------------------
		   LDS   BUFM,LCDDB9
		   SBRC  BUFM,7
           BLD   R3M,7
		   STS   LCDDB9,R3M
;------- 
		   LDS   BUFM,LCDDB10
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB10,R2M
;-------   
		   LDS   BUFM,LCDDB11
		   SBRC  BUFM,7
           BLD   R1M,7
		   STS   LCDDB11,R1M
;-------  
           LDI   BUFMS,0X08        ;-
		   LDS   BUFM,LCDDB12
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB12,BUFMS
;-------  
		   LDS   BUFM,LCDDB13
		   SBRC  R0M,7
           BLD   BUFMS,7
		   STS   LCDDB13,R0M
;--------------------
           LDI   XH,HIGH(EESP7)
           LDI   XL,LOW(EESP7)
           CALL  REE2BSUB
		   MOV   R1M,XH
           MOV   R0M,XL
		   CALL  DecSegMS          ;转成中低位段码(最后两位不用)
;-------------------
		   LDS   BUFM,LCDDB14
		   SBRC  BUFM,7
           BLD   R3M,7
		   STS   LCDDB14,R3M
;-------
		   LDS   BUFM,LCDDB15
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB15,R2M
		   RJMP  ONOFFDHS
;-------------------------------  @%
;装载11位手机电话
ZZSJDH:    INC   CNTM  
           STS   ZZDHDTim,CNTM
;--------------        
		   LDI   XH,HIGH(EESP2)
           LDI   XL,LOW(EESP2)
           CALL  REE2BSUB
		   MOV   R1M,XH
           MOV   R0M,XL
		   CALL  DecSegMS         ;转成高4位段码
;---------------
           SET 
		   LDS   BUFM,LCDDB4
		   SBRC  BUFM,7
           BLD   R3M,7
		   STS   LCDDB4,R3M
;------- 
		   LDS   BUFM,LCDDB5
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB5,R2M
;------- 
		   LDS   BUFM,LCDDB6
		   SBRC  BUFM,7
           BLD   R1M,7
		   STS   LCDDB6,R1M
;-------  
		   LDS   BUFM,LCDDB7
		   SBRC  BUFM,7
           BLD   R0M,7
		   STS   LCDDB7,R0M
;--------------        
		   LDI   XH,HIGH(EESP3)
           LDI   XL,LOW(EESP3)
           CALL  REE2BSUB
		   MOV   R1M,XH
           MOV   R0M,XL
		   CALL  DecSegMS         ;转成中4位段码
;---------------
		   LDS   BUFM,LCDDB8
		   SBRC  BUFM,7
           BLD   R3M,7
		   STS   LCDDB8,R3M
;-------   
		   LDS   BUFM,LCDDB9
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB9,R2M
;-------
		   LDS   BUFM,LCDDB10
		   SBRC  BUFM,7
           BLD   R1M,7
		   STS   LCDDB10,R1M
;------- 
		   LDS   BUFM,LCDDB11
		   SBRC  BUFM,7
           BLD   R0M,7
		   STS   LCDDB11,R0M
;--------------        
		   LDI   XH,HIGH(EESP4)
           LDI   XL,LOW(EESP4)
           CALL  REE2BSUB
		   MOV   R1M,XH
           MOV   R0M,XL
		   CALL  DecSegMS         ;转成低4位段码
;---------------
		   LDS   BUFM,LCDDB12
		   SBRC  BUFM,7
           BLD   R3M,7
		   STS   LCDDB12,R3M
;-------   
		   LDS   BUFM,LCDDB13
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB13,R2M
;-------         
		   LDS   BUFM,LCDDB14
		   SBRC  BUFM,7
           BLD   R1M,7
		   STS   LCDDB14,R1M
;-------
           LDI   BUFMS,0X00        ;最后则不显
		   LDS   BUFM,LCDDB15
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB15,BUFMS
		   RJMP  ONOFFDHS
;-------------------------------
           NOP
;-------------------------------
;开关电话相关字符
ONOFFDHS:  NOP
           CALL  DDT11Off
		   CLT
           BLD   FLAGI,BCLKS
		   CALL  DDP1Off
		   NOP
		   CALL  DDP3Off
		   NOP
		   CALL  DDT13Off
		   NOP
           CALL  DDT12On
		   NOP
           CALL  DDT1On
;---------------------------------
ZZDHTEND:  NOP                     ;装载电话任务结束
;---------------------------------


S1Task2End:NOP
;---------------------------------        
;1秒主TASK3:轮流装载四路温度显示数据
;---------------------------------
;对应ADC值装BUFMS中
	       LDS   CNTM,TNLDTim
		   CPI   CNTM,5            ;1路显5秒        &&
		   BRCC  PC+7
		   LDI   BUFM,0X12         ;1段码
           STS   LCDDB0,BUFM
           LDS   BUFMS,THEXD0
           RJMP  DISTIMEND
;------
           CPI   CNTM,10           ;2路显5秒
		   BRCC  PC+7
		   LDI   BUFM,0X5D         ;2段码
           STS   LCDDB0,BUFM
           LDS   BUFMS,THEXD1
           RJMP  DISTIMEND
;------
           CPI   CNTM,15           ;3路显5秒
		   BRCC  PC+7
		   LDI   BUFM,0X5B         ;3段码
           STS   LCDDB0,BUFM
           LDS   BUFMS,THEXD2
           RJMP  DISTIMEND
;------     
           CPI   CNTM,20           ;4路显5秒
		   BRCC  PC+7
		   LDI   BUFM,0X3A         ;4段码
           STS   LCDDB0,BUFM
           LDS   BUFMS,THEXD3
           RJMP  DISTIMEND
;------ 
           CLR   CNTM
           LDI   BUFM,0X12         ;1
           STS   LCDDB0,BUFM
           LDS   BUFMS,THEXD0
DISTIMEND: INC   CNTM
           STS   TNLDTim,CNTM
;-------------
;对要显示的温度（BUFMS中）进行处理 
           SBRC  BUFMS,7
           RJMP  FWDVPRO           ;负温度值处理
		   CPI   BUFMS,110
		   BRCS  NOCG110PRO
;-------------------- 
;超110度则显示"三三三"
		   LDI   BUFMS,0X49        ;"三"符号
           SET
		   LDS   BUFM,LCDDB1
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB1,BUFMS
;-------
           LDI   BUFMS,0X49        ;"三"符号
           SET
		   LDS   BUFM,LCDDB2
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB2,BUFMS
;-------   
           LDI   BUFMS,0X49        ;"三"符号
           SET
		   LDS   BUFM,LCDDB3
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB3,BUFMS
		   RJMP  SETDTIMEND
;-------------------- 
NOCG110PRO:
;是常温处理	
           NOP
           CALL  Hex1Seg3MS        ;十六进制转为10进制温度
;----------------  
           LDI   BUFMS,0X77
		   CP    R2M,BUFMS
		   BRNE  PC+2
		   CLR   R2M               ;高位0则不显示
		   SET
		   LDS   BUFM,LCDDB1
		   SBRC  BUFM,7
           BLD   R2M,7
		   STS   LCDDB1,R2M        ;存百位温显示码
;----------------
           SET
		   LDS   BUFM,LCDDB2
		   SBRC  BUFM,7
           BLD   R1M,7
		   STS   LCDDB2,R1M        ;存十位温显示码
;----------------
           SET
		   LDS   BUFM,LCDDB3
		   SBRC  BUFM,7
           BLD   R0M,7
		   STS   LCDDB3,R0M        ;存个位温显示码
;----------------
           RJMP  SETDTIMEND
;------------------------
;负温度值处理
FWDVPRO:   NEG   BUFMS
           MOV   R1M,BUFMS
		   CPI   BUFMS,10
		   BRCS  GYN10DPRO
;-10度以下，均显示"---"
		   LDI   BUFMS,8           ;百位显示"-"号
           SET
		   LDS   BUFM,LCDDB1
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB1,BUFMS
;-------      
           LDI   BUFMS,8           ;十位显示"-"号
           SET
		   LDS   BUFM,LCDDB2
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB2,BUFMS
;-------   
           LDI   BUFMS,8           ;个位显示"-"号
           SET
		   LDS   BUFM,LCDDB3
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB3,BUFMS
		   RJMP  SETDTIMEND
;--------------
;高于-10度
GYN10DPRO: LDI   BUFMS,00          ;-9度～-1度则不显百位
           SET
		   LDS   BUFM,LCDDB1
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB1,BUFMS
;-----------
           LDI   BUFMS,8           ;十位显示"-"号
           SET
		   LDS   BUFM,LCDDB2
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB2,BUFMS
;-----------
           MOV   BUFMS,R1M
		   ANDI  BUFMS,0X0F
           CALL  DDTrans
           SET
		   LDS   BUFM,LCDDB3
		   SBRC  BUFM,7
           BLD   BUFMS,7
		   STS   LCDDB3,BUFMS      ;存个位温显示码
;--------------------------
SETDTIMEND:NOP
           CALL  DDT14ON
;----------------------------------------
;1秒主TASK4:参数表映射
;----------------------------------------
           LDS   BUFM,PortIVD
		   LSR   BUFM
           LSR   BUFM
		   ANDI  BUFM,0B00000111
		   STS   CardOut,BUFM       ;卡机状态输出到表
;----------
           LDS   BUFM,PortIVB
		   SWAP  BUFM
           ANDI  BUFM,0B00001111
           LDS   CNTM,PortIVC
           SWAP  CNTM
		   ANDI  CNTM,0B00110000
           OR    BUFM,CNTM
		   STS   WK6Out,BUFM        ;6个取水键状态输出到表
;----------
           LDS   BUFM,PortIVC
		   LSR   BUFM
		   LSR   BUFM
		   STS   EK6Out,BUFM        ;6个外部开关状态输出到表
;----------
           LDS   BUFM,PORTE
		   ANDI  BUFM,0B00111111
           STS   PoOut,BUFM         ;6个功率状态输出到表
;---------------------------------------- @7
           NOP
		   CALL   DDT16OFF         ;关滤心更换
           NOP
;========================================
;1秒主TASK5:开关机处理
;----------------------------------------
OFFMLOOP:  LDS   BUFM,PortTIVF
           SBRS  BUFM,4
		   RJMP  ONOFFMEND         ;单机则不受此控
;------------------------------------
		   LDI   XH,HIGH(EEONOFF)
		   LDI   XL,LOW(EEONOFF)
		   CALL  REE1BSUB
		   CPI   XL,0XAA
		   BRNE  ONOFFMEND
;----------------
;AA为关机
           CLR   BUFM
		   OUT   PORTE,BUFM
		   LDI   BUFM,0X0F
		   STS   PORTG,BUFM
           CALL  DDAllOff
		   NOP
		   CALL  D22BMS
;--------------
		   NOP
           LDS   BUFM,Message1
           SBRC  BUFM,2
           JMP   ObjectP2
           NOP
		   CALL  INDEL500MS
		   NOP
           CALL  INDEL500MS
		   WDR
		   RJMP  OFFMLOOP
;------------------------------
ONOFFMEND: NOP                     ;开关机结束
;------------------------------
;1秒主TASK6:自动排水
;===============================
           LDS   BUFM,PFFLAG        ;@9
		   CPI   BUFM,0XAA
		   BRNE  ZDPFSTEND	       ;无排放任务
		   SBI   PORTE,BPSFO       ;开排水阀
           LDS   XL,PFTIMEL
           LDS   XH,PFTIMEH
		   LDI   BUFM,1
		   CLR   CNTM
		   SUB   XL,BUFM
		   SBC   XH,CNTM
		   BRCS  PFTIMEND
		   STS   PFTIMEL,XL
		   STS   PFTIMEH,XH
		   RJMP  ZDPFSTEND
;--------------
;排放时间结束
PFTIMEND:   CLR  BUFM
            STS  PFFLAG,BUFM
			STS  PFTIMEH,BUFM
		    LDI  BUFM,1
			STS  PFTIMEH,BUFM   
			CBI  PORTE,BPSFO       ;关排水阀
ZDPFSTEND:  NOP
;===================================




;=========================================
ObjectP6END:NOP                    ;对象6(1S)全部任务结束
          JMP    MainPro
;****************************************
;本对象子程序区 
;****************************************
;SUB:双字节十进制减法子程序
;--------------------------------------
;入口条件:被减数放FlowD1/FlowD0中，减数放FlowCBD1/FlowCBD0中
;出口条件:差放FlowCD1/FlowCD0中
;占用资源:XH HL BUFMS CNTMS
DECSUBMS: 	LDS   XH,FlowD1
            LDS   XL,FlowD0
			CALL  DecHexMS
			MOV   ZH,XH
			MOV   ZL,XL
            LDS   XH,FlowCBD1
            LDS   XL,FlowCBD0
			CALL  DecHexMS
            SUB   ZL,XL
			SBC   ZH,XH
			MOV   XL,ZL
            MOV   XH,ZH
			CALL  HexDecMS
		    STS   FlowCD1,XH
            STS   FlowCD0,XL
			RET
;--------------------------------
;主机读写操作子程序00：测试专用
;--------------------------------
MRWOSUB0:  LDI   BUFMS,00          ;主发代码
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,16          ;发字节总数
		   STS   MTXAB,BUFMS
           LDI   BUFMS,0X30        ;指定分地址
           STS   MRTDFA,BUFMS
           CLT   ;SET              ;转收控制
		   BLD   FLAGI,BTZR        
           LDI   BUFMS,8           ;主收8字节 
		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET
;----------------------------------
;主机读写操子程序01：读时钟时间/日期
;--------------------------------
RCLKMS:    LDI   BUFMS,01          ;主发代码
           STS   MTCODE,BUFMS
           LDI   BUFMS,CLKSAW      ;时钟从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,1           ;只发一字节
		   STS   MTXAB,BUFMS
           LDI   BUFMS,02          ;指定分地址
           STS   MRTDFA,BUFMS
           SET
		   BLD   FLAGI,BTZR        ;发了则转收
           LDI   BUFMS,7           ;主收7字节 
		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET
;----------------------------------
;主机读写操作子程序02：;发当前消费单命令 @7
;----------------------------------
;入口条件：	QWTtpe  FlowCP1H/L FlowCP2H/L FlowCP3H/L
MRWOSUB2: 
		   LDI   BUFMS,02          ;主发代码
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,12          ;发数字节（加了1字节地址）
		   STS   MTXAB,BUFMS
           LDI   ZH,HIGH(MRTDFA)
           LDI   ZL,LOW(MRTDFA)
           LDI   BUFMS,0X30        ;指定表启始相对地址 30H固定为命令字
		   ST    Z+,BUFMS
           LDI   BUFMS,0X02        ;命令2：发当次用水消费记录
		   ST    Z+,BUFMS
		   LDS   BUFMS,CLKYY
           ST    Z+,BUFMS
		   LDS   BUFMS,CLKMO
           ST    Z+,BUFMS
		   LDS   BUFMS,CLKDD
           ST    Z+,BUFMS
		   LDS   BUFMS,CLKHH
           ST    Z+,BUFMS
		   LDS   BUFMS,CLKMM
           ST    Z+,BUFMS
		   LDS   BUFMS,CLKSS
           ST    Z+,BUFMS
;--------------
           LDS   BUFMS,QWType
           CPI   BUFMS,1
		   BRNE  ZZWYLN1
;装开水用量 
		   LDS   BUFMS,FlowCP1H
           ST    Z+,BUFMS
		   LDS   BUFMS,FlowCP1L
           ST    Z+,BUFMS
           RJMP  ZZWYLEND
;---------------
ZZWYLN1:   CPI   BUFMS,2
		   BRNE  ZZWYLN2
;装温水用量 
		   LDS   BUFMS,FlowCP2H
           ST    Z+,BUFMS
		   LDS   BUFMS,FlowCP2L
           ST    Z+,BUFMS
           RJMP  ZZWYLEND
;---------------
ZZWYLN2:   CPI   BUFMS,3
		   BRNE  ZZWYLN3
;装冰水用量 
		   LDS   BUFMS,FlowCP3H
           ST    Z+,BUFMS
		   LDS   BUFMS,FlowCP3L
           ST    Z+,BUFMS
           RJMP  ZZWYLEND
;----------------
ZZWYLN3:   NOP                     ;超过3种非法，不再发送
           RET
;-------------------------------
ZZWYLEND:  LDS   BUFMS,QWType      ;消费类型
		   ST    Z+,BUFMS
           LDI   BUFMS,1           ;是脉冲
		   ST    Z+,BUFMS     	   
           CLT ;SET                ;仅发不收
		   BLD   FLAGI,BTZR        
;          LDI   BUFMS,XX          ;主收XX字节
;		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET
;----------------------------------
;主机读写操作子程序03：开冰水命令
;----------------------------------
MRWOSUB3:  LDI   BUFMS,03          ;主发代码
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,2           ;发数字节（加了1字节地址）
		   STS   MTXAB,BUFMS
           LDI   BUFMS,0X30        ;表地址 30H是固定的命令地址
           STS   MRTDFA,BUFMS
           LDI   BUFMS,3           ;命令3:通知开冰水
           STS   MRTDA1,BUFMS
           CLT ; SET               ;SET:发了则转收, CLT:只发数据
		   BLD   FLAGI,BTZR
;          LDI   BUFMS,XX          ;主收XX字节
;		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET
;----------------------------------
;主机读写操作子程序04：关冰水命令
;----------------------------------
MRWOSUB4:  LDI   BUFMS,04          ;主发代码
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,2           ;发数字节（加了1字节地址）
		   STS   MTXAB,BUFMS
           LDI   BUFMS,0X30        ;表地址 30H是固定的命令地址
           STS   MRTDFA,BUFMS
           LDI   BUFMS,4           ;命令4:通知关冰水
           STS   MRTDA1,BUFMS
           CLT ; SET               ;SET:发了则转收, CLT:只发数据
		   BLD   FLAGI,BTZR
;          LDI   BUFMS,XX          ;主收XX字节
;		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET
;----------------------------------
;主机读写操作子程序05：开制水
;----------------------------------
MRWOSUB5:  LDI   BUFMS,05          ;主发代码
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,2           ;发数字节（加了1字节地址）
		   STS   MTXAB,BUFMS
           LDI   BUFMS,0X30        ;表地址 30H是固定的命令地址
           STS   MRTDFA,BUFMS
           LDI   BUFMS,5           ;开制水
           STS   MRTDA1,BUFMS
           CLT ; SET               ;SET:发了则转收, CLT:只发数据
		   BLD   FLAGI,BTZR
;          LDI   BUFMS,XX          ;主收XX字节
;		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET
;----------------------------------
;主机读写操作子程序06：关制水
;----------------------------------
MRWOSUB6:  LDI   BUFMS,06          ;主发代码   &&
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,2           ;发数字节（加了1字节地址）
		   STS   MTXAB,BUFMS
           LDI   BUFMS,0X30        ;表地址 30H是固定的命令地址
           STS   MRTDFA,BUFMS
           LDI   BUFMS,6           ;关制水     &&
           STS   MRTDA1,BUFMS
           CLT ; SET               ;SET:发了则转收, CLT:只发数据
		   BLD   FLAGI,BTZR
;          LDI   BUFMS,XX          ;主收XX字节
;		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET         
;----------------------------------
;主机读写操作子程序07：转发冰水设置值
;----------------------------------
MRWOSUB7:  LDI   BUFMS,07          ;主发代码   &&
           STS   MTCODE,BUFMS
           LDI   BUFMS,ZNBSAW      ;从机地址
		   STS   SLADDW,BUFMS
		   LDI   BUFMS,3           ;发数字节（加了1字节地址）
		   STS   MTXAB,BUFMS
           LDI   BUFMS,0X30        ;表地址 30H是固定的命令地址
           STS   MRTDFA,BUFMS
           LDI   BUFMS,7           ;命令7：     &&
           STS   MRTDA1,BUFMS
		   LDS   BUFMS,PSETDH
		   STS   MRTDA2,BUFMS
           LDS   BUFMS,PSETDL
		   STS   MRTDA3,BUFMS
           CLT ; SET               ;SET:发了则转收, CLT:只发数据
		   BLD   FLAGI,BTZR
;          LDI   BUFMS,XX          ;主收XX字节
;		   STS   MRXAB,BUFMS
		   LDI   BUFMS,TWCR_STA    ;发启动位 
	       STS   TWCR,BUFMS
           RET    
;----------------------------------

	
;----------------------------------



;****************************************
;对象7处理程序：每分钟处理对象   @9
;****************************************
ObjectP7: ANDI   BUFM,0B01111111
          STS    Message1,BUFM
;========================================
;每分钟对象任务1：判是否到自动排放时间
           LDS   BUFM,PFFLAG
		   CPI   BUFM,0XAA
		   BREQ  COMZDPFEND         ;正在排放则跳过
;----------------------
           LDI   XH,HIGH(EESP13)
           LDI   XL,LOW(EESP13)
           CALL  REE2BSUB
           LDS   BUFM,CLKHH
		   CPSE  BUFM,XH
		   RJMP  COMZDPFEND
           LDS   BUFM,CLKMM
		   CPSE  BUFM,XL
		   RJMP  COMZDPFEND
;时间到了，把排放时间转为秒 
           LDI   XH,HIGH(EESP14)
           LDI   XL,LOW(EESP14)
           CALL  REE2BSUB
           LDI   BUFM,60
		   MUL   BUFM,XH
		   CLR   XH
		   ADD   XL,R0M
		   ADC   XH,R1M
      LDI   XH,0
	  LDI   XL,120            ;@9
		   STS   PFTIMEH,XH
		   STS   PFTIMEL,XL
		   LDI   BUFM,0XAA
		   STS   PFFLAG,BUFM       ;存排放标志AA
COMZDPFEND:NOP                     ;比较排放结束
;========================================
ObjectP7END:NOP                    ;对象7全部任务结束
          JMP    MainPro
;****************************************
;对象8处理程序：
;****************************************
ObjectP8: ANDI   BUFM,0B11111110
          STS    Message2,BUFM
;========================================


;========================================
ObjectP8END:NOP                    ;对象8全部任务结束
          JMP    MainPro
;****************************************
;对象9处理程序：
;****************************************
ObjectP9: ANDI   BUFM,0B11111101
          STS    Message2,BUFM
;========================================


;========================================
ObjectP9END:NOP                    ;对象9全部任务结束
          JMP    MainPro
;****************************************
;对象10处理程序：
;****************************************
ObjectP10:ANDI   BUFM,0B11111011
          STS    Message2,BUFM
;========================================


;========================================
ObjectP10END:NOP                   ;对象10全部任务结束
          JMP    MainPro
;****************************************
;对象11处理程序：
;****************************************
ObjectP11:ANDI   BUFM,0B11110111
          STS    Message2,BUFM
;========================================


;========================================
ObjectP11END:NOP                   ;对象11全部任务结束
          JMP    MainPro
;****************************************
;对象12处理程序：
;****************************************
ObjectP12:ANDI   BUFM,0B11101111
          STS    Message2,BUFM
;========================================


;========================================-
ObjectP12END:NOP                   ;对象12全部任务结束
          JMP    MainPro
;****************************************
;对象13处理程序：
;****************************************
ObjectP13:ANDI   BUFM,0B11011111
          STS    Message2,BUFM
;========================================



;========================================
ObjectP13END:NOP                   ;对象13全部任务结束
          JMP   MainPro
;****************************************
;对象14处理程序：
;****************************************
ObjectP14:ANDI   BUFM,0B10111111
          STS    Message2,BUFM
;========================================


;========================================
ObjectP14END:NOP                   ;对象14全部任务结束
          JMP    MainPro
;****************************************
;对象15处理程序：
;****************************************
ObjectP15:ANDI   BUFM,0B01111111
          STS    Message2,BUFM
;========================================


;========================================
ObjectP15END:NOP                   ;对象15全部任务结束
           JMP    MainPro
;**********************************************		
;**********************************************
;三、主程序专用子程序区
;**********************************************
TESTSY:    SBI   PORTA,BBUZZ
           CALL  INDEL10MS   
           CBI   PORTA,BBUZZ
           RET
;------------------------------------------
;MSUB2、发送显示屏命令字节子程序
;------------------------------------------
;入口条件：命令放BUFMS中
;出口和件：
;占用资源:CNTMS
SendDCMS:CBI    PORTA,BDCS       ;片选置低
;发送命令头：100
          SBI    PORTA,BDDATA
		  NOP
          NOP
		  CBI    PORTA,BDWR
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  SBI    PORTA,BDWR        ;写入1
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
          CBI    PORTA,BDDATA
		  NOP
		  NOP
		  NOP
		  NOP
		  SBI    PORTA,BDWR        ;写入0
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  SBI    PORTA,BDWR       ;写入0
;---------
;发送9位命令
          LDI    CNTMS,9
		  NOP
SEND9BCLP:NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
		  SBRC   BUFMS,7
          SBI    PORTA,BDDATA
		  SBRS   BUFMS,7
          CBI    PORTA,BDDATA
		  NOP
		  NOP
          NOP
          SBI    PORTA,BDWR      ;写入1高位
		  LSL    BUFMS
		  DEC    CNTMS
		  BRNE   SEND9BCLP
		  NOP
		  NOP
          SBI    PORTA,BDCS
		  RET
;------------------------------------------
;------------------------------------------
;MSUB3、显示22字节子程序
;------------------------------------------
;入口条件：数据已放在LCDDB0开始的22个字节内，BLCDD=1则更新显示
;出口和件：
;占用资源:BUFMS CNTMS R3M
D22BMS:   SBRS   FlagM,BLCDD
          RJMP   SEND22BEND       ;无显示任务
          CLT
		  BLD    FlagM,BLCDD
;发送写头：101
          CBI    PORTA,BDCS       ;片选置低
          SBI    PORTA,BDDATA
		  NOP
          NOP
		  CBI    PORTA,BDWR
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  SBI    PORTA,BDWR        ;写入1
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
          CBI    PORTA,BDDATA
		  NOP
		  NOP
		  NOP
		  NOP
		  SBI    PORTA,BDWR        ;写入0
          NOP
		  NOP
		  NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
          SBI    PORTA,BDDATA
		  NOP
		  NOP
		  NOP
		  NOP
		  SBI    PORTA,BDWR       ;写入1
;---------
;发送6位写起始地址00
          LDI    CNTMS,6
SEND6BWDLP:NOP
		  NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
          CBI    PORTA,BDDATA
		  NOP
		  NOP
          NOP
		  NOP
		  NOP
          SBI    PORTA,BDWR      ;写入1高位地址
		  DEC    CNTMS
		  BRNE   SEND6BWDLP
;----------
;发送固定的22字节显示数据
          LDI    XH,HIGH(LCDDB0)
		  LDI    XL,LOW(LCDDB0)
          LDI    BUFMS,22
		  MOV    R3M,BUFMS
SEND22BLP: 
          LD     BUFMS,X+
          LDI    CNTMS,8
SEND8BDLP:NOP
		  NOP
		  NOP
		  CBI    PORTA,BDWR
		  SBRC   BUFMS,0
          SBI    PORTA,BDDATA
		  SBRS   BUFMS,0
          CBI    PORTA,BDDATA
		  NOP
		  NOP
          NOP
          SBI    PORTA,BDWR      ;写入1低位数据
		  LSR    BUFMS
		  DEC    CNTMS
		  BRNE   SEND8BDLP
;------
          DEC    R3M
		  BRNE   SEND22BLP
		  NOP
		  NOP
          SBI    PORTA,BDCS
SEND22BEND:NOP
		  NOP
		  RET
;----------------------------------------------
;----------------------------------------------
;MSUB、LCD置数子程序组
;----------------------------------------------
;入口条件：
;出口和件：
;占用资源：BUFMS CNTMS
;------------------------------------
;说明:显示数据全屏开关
 DDAllOn:  LDI    BUFMS,0XFF
           RJMP   PC+2 
 DDAllOff: LDI    BUFMS,00
           LDI    CNTMS,22
		   LDI    XH,HIGH(LCDDB0)
           LDI    XL,LOW(LCDDB0)
	       ST     X+,BUFMS
	       DEC    CNTMS
           BRNE   PC-2
		   SET
		   BLD    FlagM,BLCDD
		   RET	     
;--------------------------------------
;说明:显示数据T1~T10开关: 公共饮水设备领先品牌
DDT1On:    SET
           RJMP  PC+2
DDT1Off:   CLT
           LDS   BUFMS,LCDDB1
           BLD   BUFMS,7
		   STS   LCDDB1,BUFMS
		   LDS   BUFMS,LCDDB2
           BLD   BUFMS,7
		   STS   LCDDB2,BUFMS
		   LDS   BUFMS,LCDDB5
           BLD   BUFMS,7
		   STS   LCDDB5,BUFMS
		   LDS   BUFMS,LCDDB6
           BLD   BUFMS,7
		   STS   LCDDB6,BUFMS
		   LDS   BUFMS,LCDDB8
           BLD   BUFMS,7
		   STS   LCDDB8,BUFMS
		   LDS   BUFMS,LCDDB10
           BLD   BUFMS,7
		   STS   LCDDB10,BUFMS
		   LDS   BUFMS,LCDDB12
           BLD   BUFMS,7
		   STS   LCDDB12,BUFMS
		   LDS   BUFMS,LCDDB13
           BLD   BUFMS,7
		   STS   LCDDB13,BUFMS
		   LDS   BUFMS,LCDDB14
           BLD   BUFMS,7
		   STS   LCDDB14,BUFMS
		   LDS   BUFMS,LCDDB15
           BLD   BUFMS,7
		   STS   LCDDB15,BUFMS
		   SET
		   BLD   FlagM,BLCDD
		   RET
;------------------------------
;说明：显示数据T11开关: 日历 年 月 日
DDT11On:   SET
           RJMP  PC+2
DDT11Off:  CLT
           LDS   BUFMS,LCDDB4
           BLD   BUFMS,7
		   STS   LCDDB4,BUFMS
		   LDS   BUFMS,LCDDB7
           BLD   BUFMS,7
		   STS   LCDDB7,BUFMS
		   LDS   BUFMS,LCDDB9
           BLD   BUFMS,7
		   STS   LCDDB9,BUFMS
		   LDS   BUFMS,LCDDB11
           BLD   BUFMS,7
		   STS   LCDDB11,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据T12开关: 服务电话
DDT12On:   SET
           RJMP  PC+2
DDT12Off:  CLT
           LDS   BUFMS,LCDDB3
           BLD   BUFMS,7
		   STS   LCDDB3,BUFMS
           RET
;------------------------------
;说明：显示数据T13开关: 流量/L
DDT13On:   SET
           RJMP  PC+2
DDT13Off:  CLT
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,6
		   STS   LCDDB20,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据T14开关: C度
DDT14On:   SET
           RJMP  PC+2
DDT14Off:  CLT
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,0
		   STS   LCDDB20,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据T15开关: 级净化
DDT15On:   SET
           RJMP  PC+2
DDT15Off:  CLT
           LDS   BUFMS,LCDDB0
           BLD   BUFMS,7
		   STS   LCDDB0,BUFMS
           RET
;------------------------------
;说明：显示数据T16开关: 滤芯更换
DDT16On:   SET
           RJMP  PC+2
DDT16Off:  CLT
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,7
		   STS   LCDDB21,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S2开关: (定时)关
DDS2On:    SET
           RJMP  PC+2
DDS2Off:   CLT
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,3
		   STS   LCDDB21,BUFMS
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,2
		   STS   LCDDB21,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S3开关: (定时)开
DDS3On:    SET
           RJMP  PC+2
DDS3Off:   CLT
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,3
		   STS   LCDDB21,BUFMS
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,1
		   STS   LCDDB21,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S4开关: 杀菌
DDS4On:    SET
           RJMP  PC+2
DDS4Off:   CLT
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,0
		   STS   LCDDB21,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S5S7开关: 上(水位)
DDS5On:    SET
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,3
		   STS   LCDDB20,BUFMS
           RJMP  PC+2
DDS5Off:   CLT
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,1
		   STS   LCDDB20,BUFMS
;----------------
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S6S7开关: 下(水位)
DDS6On:    SET
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,3
		   STS   LCDDB20,BUFMS
           RJMP  PC+2
DDS6Off:   CLT
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,2
		   STS   LCDDB20,BUFMS
;----------------
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S8开关: 饮用
DDS8On:    SET
           RJMP  PC+2
DDS8Off:   CLT
           LDS   BUFMS,LCDDB20
           BLD   BUFMS,7
		   STS   LCDDB20,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S9开关: 加热
DDS9On:    SET
           RJMP  PC+2
DDS9Off:   CLT
           LDS   BUFMS,LCDDB16
           BLD   BUFMS,7
		   STS   LCDDB16,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据S10开关: 加热
DDS10On:   SET
           RJMP  PC+2
DDS10Off:  CLT
           LDS   BUFMS,LCDDB17
           BLD   BUFMS,7
		   STS   LCDDB17,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据P1P2开关: (时钟号:)
DDP1On:    SET
           RJMP  PC+2
DDP1Off:   CLT
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,6
		   STS   LCDDB21,BUFMS
		   LDS   BUFMS,LCDDB21
           BLD   BUFMS,5
		   STS   LCDDB21,BUFMS
		   SET
		   BLD   FlagM,BLCDD
           RET
;------------------------------
;说明：显示数据P3开关: (上串数的".")
DDP3On:    SET
           RJMP  PC+2
DDP3Off:   CLT
           LDS   BUFMS,LCDDB21
           BLD   BUFMS,4
		   STS   LCDDB21,BUFMS
		   SET
		   BLD   FlagM,BLCDD 
           RET
;------------------------------
;说明：显示数据流速开子程序
;入口条件：流速脉冲放XH/XL中
;出口条件：对应显示数据放流速显示区
DDCSOn:    NOP
           RCALL HexDecMS        ;转成十进制
           MOV   R1M,XH
           MOV   R0M,XL
		   RCALL DecSegMS        ;转成段码
		   LDS   BUFMS,LCDDB16
		   CLR   R3M             ;流速百位不显示
		   SET
		   SBRC  BUFMS,7
           BLD   R3M,7
		   STS   LCDDB16,R3M     
;-------------
		   LDS   BUFMS,LCDDB17
           LDI   CNTMS,0X77
		   CPSE  R2M,CNTMS
		   RJMP  PC+2
		   CLR   R2M             ;如十位为0则不显示
		   SBRC  BUFMS,7
           BLD   R2M,7
		   STS   LCDDB17,R2M     ;存流速十位码
;-------------
           BLD   R1M,7           ;开小数点P4
		   STS   LCDDB18,R1M     ;存流速十位码
;-------------
		   BLD   R0M,7           ;开 "升/分"
		   STS   LCDDB19,R0M     ;存流速小数位码
;--------------
		   BLD   FlagM,BLCDD
           RET
;----------------------------------
;1字节渐变量调整为十进制数子程序
;----------------------------------
;入口条件：BUFMS
;出口条件：BUFMS C
;占用资源：CNTMS
DECINCTZMS: LDI   CNTMS,6
		   ADD   BUFMS,CNTMS
           BRHS  PC+3
		   SUB   BUFMS,CNTMS
		   RJMP  HEX1BZHEND
		   LDI   CNTMS,0X60
		   ADD   BUFMS,CNTMS
           BRCS  PC+2
		   SUB   BUFMS,CNTMS
HEX1BZHEND:NOP
           RET
;----------------------------------
;1字节十进制数转为十六进制数子程序
;----------------------------------
;入口条件：十进制数 BUFMS
;出口条件：十六进制数 BUFMS
;占用资源：CNTMS R0
Dec1BHexMS:LDI   CNTMS,10
           MOV   R0M,CNTMS   
           MOV   CNTMS,BUFMS
           SWAP  CNTMS
		   ANDI  CNTMS,0X0F
		   MUL   R0M,CNTMS
		   ANDI  BUFMS,0X0F
		   ADD   BUFMS,R0M
		   RET
;----------------------------------
;2字节十进制数转为十六进制数子程序
;----------------------------------
;入口条件：十进制数放XH XL中
;出口条件：十六进制数 放XH XL中
;占用资源：CNTMS R0
DecHexMS:  LDI   BUFMS,10  
           MOV   R0M,BUFMS   
           MOV   BUFMS,XL
           SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   MUL   R0M,BUFMS
		   ANDI  XL,0X0F
		   ADD   XL,R0M
;----------------------
		   LDI   BUFMS,100  
           MOV   R0M,BUFMS   
           MOV   BUFMS,XH
		   ANDI  BUFMS,0X0F
		   MUL   R0M,BUFMS
		   CLR   CNTMS
		   ADD   XL,R0M
		   ADC   CNTMS,R1M
         
		   MOV   BUFMS,XH
		   MOV   XH,CNTMS
           SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   INC   BUFMS
JFTJLOOP:  DEC   BUFMS
		   BRNE  PC+2
		 RET
		   LDI   CNTMS,0XE8     ;1000=03E8H
           ADD   XL,CNTMS
		   LDI   CNTMS,3
		   ADC   XH,CNTMS
		   RJMP  JFTJLOOP
;----------------------------------
;1字节十六进制转换为三位段码子程序
;----------------------------------
;入口条件：BUFMS
;出口条件：三位段码放R2M/R1M/R0M中
;占用资源：CNTMS
Hex1Seg3MS:CLR   R2M
           CLR   R1M
		   CLR   R0M
           LDI   CNTMS,100
		   SUB   BUFMS,CNTMS
           BRCS  PC+3
		   INC   R2M
		   RJMP  PC-3
		   ADD   BUFMS,CNTMS
;----------
           LDI   CNTMS,10
		   SUB   BUFMS,CNTMS
           BRCS  PC+3
		   INC   R1M
		   RJMP  PC-3
		   ADD   BUFMS,CNTMS
;----------
           MOV   R0M,BUFMS
;--------------------------
           MOV   BUFMS,R2M
		   RCALL DDTrans
		   MOV   R2M,BUFMS
           MOV   BUFMS,R1M
		   RCALL DDTrans
		   MOV   R1M,BUFMS
		   MOV   BUFMS,R0M
		   RCALL DDTrans
		   MOV   R0M,BUFMS
           RET
;---------------------------------------
;MSUB 、十六进制转为十进制数（只适于9999以内的数）
;---------------------------------------
;入口条件:十六进制数放XH/XL中
;出口条件:十进制数放XH/XL中
;占用资源:BUFMS/R1M/R0M
HexDecMS: MOV   R1M,XH
          MOV   R0M,XL
		  LDI   BUFMS,0X0F         ;9999=0X270F
		  SUB   R0M,BUFMS
          LDI   BUFMS,0X27
		  SBC   R1M,BUFMS
          BRCS  PC+3
		  LDI   XH,0X27
          LDI   XL,0X0F
;----------------------
          CLR    R1M
		  CLR    R0M		  
CONV10CON:SBIW   XL,10
		  BRCS   CONV10END
		  LDI    BUFMS,0X10
		  ADD    R0M,BUFMS
		  LDI    BUFMS,0XA0
		  CPSE   R0M,BUFMS
		  RJMP   CONV10CON
		  CLR    R0M
		  INC    R1M
          MOV    BUFMS,R1M
		  ANDI   BUFMS,0X0F
		  CPI    BUFMS,10
		  BRNE   CONV10CON
		  LDI    BUFMS,6
		  ADD    R1M,BUFMS
		  RJMP   CONV10CON
;-----------------
CONV10END:NOP
          ADIW   XL,10
          OR     XL,R0M
		  MOV    XH,R1M
          NOP
          RET
;--------------------------------
;MSUB 、2字节十进制数转4位段码子程序
;--------------------------------
;入口条件:R1M/R0M
;出口条件:R3M/R2M/R1M/R0M
;占用资源:BUFMS
DecSegMS:  MOV   BUFMS,R1M
           SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   RCALL DDTrans
           MOV   R3M,BUFMS
		   MOV   BUFMS,R1M
		   ANDI  BUFMS,0X0F
		   RCALL DDTrans
           MOV   R2M,BUFMS

           MOV   BUFMS,R0M
           SWAP  BUFMS
		   ANDI  BUFMS,0X0F
		   RCALL DDTrans
           MOV   R1M,BUFMS
		   MOV   BUFMS,R0M
		   ANDI  BUFMS,0X0F
		   RCALL DDTrans
           MOV   R0M,BUFMS
		   RET
;-----------------------------------	
;LCD段码转换子程序
;-----------------------------------
;入口条件：0~9的数放BUFMS中
;出口条件：段码回放BUFMS中
DDTrans:   CPI   BUFMS,0
           BRNE  PC+3
		   LDI   BUFMS,0X77    ;0 段码
           RET
           CPI   BUFMS,1
           BRNE  PC+3
		   LDI   BUFMS,0X12    ;1 段码
           RET
           CPI   BUFMS,2
           BRNE  PC+3
		   LDI   BUFMS,0X5D    ;2 段码
           RET
		   CPI   BUFMS,3
           BRNE  PC+3
		   LDI   BUFMS,0X5B    ;3 段码
           RET
           CPI   BUFMS,4
           BRNE  PC+3
		   LDI   BUFMS,0X3A    ;4 段码
		   RET
           CPI   BUFMS,5
           BRNE  PC+3
		   LDI   BUFMS,0X6B    ;5 段码
		   RET
           CPI   BUFMS,6
           BRNE  PC+3
		   LDI   BUFMS,0X6F    ;6 段码
		   RET
           CPI   BUFMS,7
           BRNE  PC+3
		   LDI   BUFMS,0X52    ;7 段码
		   RET
           CPI   BUFMS,8
           BRNE  PC+3
		   LDI   BUFMS,0X7F    ;8 段码
		   RET
           CPI   BUFMS,9
           BRNE  PC+3
		   LDI   BUFMS,0X7B    ;9 段码
		   RET
		   LDI   BUFMS,0X00    ;超0~9则不显示
		   RET
;-------------------------------------		   
;-------------------------------------
;说明：显示数据流速关子程序
DDCSOff:   LDS   BUFMS,LCDDB16
		   ANDI  BUFMS,0B00000001
		   STS   LCDDB16,BUFMS    ;清流速百位码
;-------------
		   LDS   BUFMS,LCDDB17
		   ANDI  BUFMS,0B00000001
		   STS   LCDDB17,BUFMS    ;清流速十位码
;-------------
           CLR   BUFMS
		   STS   LCDDB18,BUFMS    ;清流速个位码，并关小数点P4
;-------------
           CLR   BUFMS
		   STS   LCDDB19,BUFMS    ;清流速小数位码，并关“升/分”
;--------------
		   SET
		   BLD   FlagM,BLCDD 
           RET
;-------------------------------
;------------------------------------------
;查询ADC的温度值子程序
;------------------------------------------
;入口条件:ADC值放BUFMS中
;出口条件:温度值放BUFMS中（负数用补码表示）
;占用资源:
QweryAdcTMS:
          MOV    R0M,BUFMS
		  LDI    BUFMS,10           ;从-10度开始查
		  MOV    R1M,BUFMS
		  NEG    R1M
		  LDI    BUFMS,121          ;只定义了120个值  &&&
		  MOV    R2M,BUFMS
CZWVLOOP: MOV    BUFMS,R1M
          RCALL  QweryTAdcMS        ;取回一ADC值
		  CP     R0M,BUFMS
          BRCC   CZWVJMP            ;查到了
		  DEC    R2M
		  BREQ   CZWVJMP 
		  INC    R1M
		  RJMP   CZWVLOOP
CZWVJMP:  MOV    BUFMS,R1M
ADCTTABEND:NOP
		  RET    
;------------------------------------------
;查询温度的ADC值子程序
;------------------------------------------
;入口条件:温度值放BUFMS中（负数用补码表示）
;出口条件:ADC值放BUFMS中
;占用资源:CNTMS
QweryTAdcMS:
          LDI    CNTMS,10            ;负10度偏移量
		  ADD    BUFMS,CNTMS
		  CPI    BUFMS,120           ;参数限制 最高110度
		  BRCS   PC+2
          LDI    BUFMS,120
		  LDI    ZL,LOW(TADCTAB)
	      LDI    ZH,HIGH(TADCTAB)
		  LSL    BUFMS               ;*2
	      ADD    ZL,BUFMS
	      BRCC   PC+2
	      INC    ZH
          IJMP
;----------------------------------------------
;数字温度与ADC值对应表(-10度至110度)
;---------------------
TADCTAB:  LDI    BUFMS,208   ;DTN10
		  RET
          LDI    BUFMS,206   ;DTN9     
          RET
		  LDI    BUFMS,204   ;DTN8     
          RET
		  LDI    BUFMS,202   ;DTN7     
          RET
		  LDI    BUFMS,200   ;DTN6
		  RET     
          LDI    BUFMS,198   ;DTN5     
          RET
		  LDI    BUFMS,196   ;DTN4     
          RET
		  LDI    BUFMS,194   ;DTN3     
          RET
		  LDI    BUFMS,192   ;DTN2     
          RET
		  LDI    BUFMS,190   ;DTN1     
          RET
		  LDI    BUFMS,188   ;DT0      
          RET
		  LDI    BUFMS,186   ;DT1      
          RET
		  LDI    BUFMS,184   ;DT2      
          RET
		  LDI    BUFMS,181   ;DT3      
          RET
		  LDI    BUFMS,179   ;DT4      
          RET
		  LDI    BUFMS,177   ;DT5      
          RET
		  LDI    BUFMS,174   ;DT6      
          RET
		  LDI    BUFMS,172   ;DT7      
          RET
		  LDI    BUFMS,170   ;DT8      
          RET
		  LDI    BUFMS,167   ;DT9      
          RET
		  LDI    BUFMS,165   ;DT10     
          RET
		  LDI    BUFMS,162   ;DT11
		  RET
		  LDI    BUFMS,160   ;DT12     
          RET
		  LDI    BUFMS,157   ;DT13     
          RET
		  LDI    BUFMS,155   ;DT14
		  RET     
          LDI    BUFMS,153   ;DT15     
          RET
		  LDI    BUFMS,150   ;DT16     
          RET
		  LDI    BUFMS,148   ;DT17     
          RET
		  LDI    BUFMS,145   ;DT18     
          RET
		  LDI    BUFMS,143   ;DT19     
          RET
		  LDI    BUFMS,142   ;DT20     
          RET
		  LDI    BUFMS,138   ;DT21     
          RET
		  LDI    BUFMS,135   ;DT22     
          RET
		  LDI    BUFMS,133   ;DT23     
          RET
		  LDI    BUFMS,130   ;DT24     
          RET
		  LDI    BUFMS,128   ;DT25     
          RET
		  LDI    BUFMS,126   ;DT26     
          RET
		  LDI    BUFMS,123   ;DT27     
          RET
		  LDI    BUFMS,121   ;DT28     
          RET
		  LDI    BUFMS,118   ;DT29     
          RET
		  LDI    BUFMS,116   ;DT30     
          RET
		  LDI    BUFMS,113   ;DT31     
          RET
		  LDI    BUFMS,112   ;DT32     
          RET
		  LDI    BUFMS,110   ;DT33     
          RET
		  LDI    BUFMS,107   ;DT34     
          RET
		  LDI    BUFMS,105   ;DT35     
          RET
		  LDI    BUFMS,103   ;DT36     
          RET
		  LDI    BUFMS,100   ;DT37     
          RET
		  LDI    BUFMS,98    ;DT38     
          RET
		  LDI    BUFMS,96    ;DT39     
          RET
		  LDI    BUFMS,94    ;DT40
          RET
		  LDI    BUFMS,92    ;DT41
          RET
		  LDI    BUFMS,90    ;DT42
          RET
		  LDI    BUFMS,88    ;DT43
          RET
		  LDI    BUFMS,86    ;DT44
          RET
		  LDI    BUFMS,84    ;DT45
          RET
		  LDI    BUFMS,82    ;DT46
          RET
		  LDI    BUFMS,80    ;DT47
          RET
		  LDI    BUFMS,79    ;DT48
          RET
		  LDI    BUFMS,77    ;DT49
          RET
		  LDI    BUFMS,75    ;DT50
          RET
		  LDI    BUFMS,73    ;DT51 
          RET
		  LDI    BUFMS,72    ;DT52 
          RET
		  LDI    BUFMS,70    ;DT53
          RET
		  LDI    BUFMS,68    ;DT54
          RET
		  LDI    BUFMS,67    ;DT55
          RET
		  LDI    BUFMS,65    ;DT56
          RET
		  LDI    BUFMS,64    ;DT57
          RET
		  LDI    BUFMS,62    ;DT58
          RET
		  LDI    BUFMS,61    ;DT59
          RET
		  LDI    BUFMS,59    ;DT60
          RET
		  LDI    BUFMS,58    ;DT61 
          RET
		  LDI    BUFMS,56    ;DT62 
          RET
		  LDI    BUFMS,55    ;DT63 
          RET
		  LDI    BUFMS,54    ;DT64 
          RET
		  LDI    BUFMS,53    ;DT65
          RET
		  LDI    BUFMS,51    ;DT66 
          RET
		  LDI    BUFMS,50    ;DT67 
          RET
		  LDI    BUFMS,49    ;DT68
          RET
		  LDI    BUFMS,48    ;DT69
          RET
		  LDI    BUFMS,47    ;DT70 
          RET
		  LDI    BUFMS,45    ;DT71 
          RET
		  LDI    BUFMS,44    ;DT72 
          RET
		  LDI    BUFMS,43    ;DT73 
          RET
		  LDI    BUFMS,42    ;DT74
          RET
		  LDI    BUFMS,41    ;DT75
          RET
		  LDI    BUFMS,40    ;DT76
          RET
		  LDI    BUFMS,39    ;DT77
          RET
		  LDI    BUFMS,38    ;DT78
          RET
		  LDI    BUFMS,37    ;DT79 
          RET
		  LDI    BUFMS,37    ;DT80 
          RET
		  LDI    BUFMS,36    ;DT81 
          RET
		  LDI    BUFMS,35    ;DT82 
          RET
		  LDI    BUFMS,34    ;DT83 
          RET
		  LDI    BUFMS,33    ;DT84 
          RET
		  LDI    BUFMS,32    ;DT85
          RET
		  LDI    BUFMS,32    ;DT86 
          RET
		  LDI    BUFMS,31    ;DT87 
          RET
		  LDI    BUFMS,30    ;DT88 
          RET
		  LDI    BUFMS,29    ;DT89 
          RET
		  LDI    BUFMS,29    ;DT90 
          RET
		  LDI    BUFMS,28    ;DT91 
          RET
		  LDI    BUFMS,27    ;DT92   
          RET
		  LDI    BUFMS,27    ;DT93 
          RET
		  LDI    BUFMS,26    ;DT94 
          RET
		  LDI    BUFMS,26    ;DT95 
          RET
		  LDI    BUFMS,25    ;DT96 
          RET
		  LDI    BUFMS,24    ;DT97 
          RET
		  LDI    BUFMS,24    ;DT98 
          RET
		  LDI    BUFMS,23    ;DT99 
          RET
		  LDI    BUFMS,23    ;DT100 
          RET
		  LDI    BUFMS,22    ;DT101 
          RET
		  LDI    BUFMS,22    ;DT102 
          RET
		  LDI    BUFMS,21    ;DT103   
          RET
		  LDI    BUFMS,21    ;DT104  
          RET
		  LDI    BUFMS,20    ;DT105  
          RET
		  LDI    BUFMS,20    ;DT106  
          RET
		  LDI    BUFMS,19    ;DT107 
          RET
		  LDI    BUFMS,19    ;DT108 
          RET
		  LDI    BUFMS,18    ;DT109 
          RET
		  LDI    BUFMS,17    ;DT110
		  RET
		  LDI    BUFMS,17    ;DT111
		  RET
		  LDI    BUFMS,16    ;DT112
		  RET
		  LDI    BUFMS,15    ;DT113
		  RET
;-----------------------------------------


;----------------------------------------------
;MSUB4、取水请求归一子程序
;----------------------------------------------
;入口条件：PortIVB PortIVD
;出口和件：取水请求放WRequest中低三位（D2:冰水，D1:温水，D0：开水）
;占用资源：BUFMS CNTMS
;说明：
WRequestMS: CLR  CNTMS
;与手动取水键值相或
           LDS   BUFMS,PortIVB
           SWAP  BUFMS
		   ANDI  BUFMS,0B00000111
		   OR    CNTMS,BUFMS
;------------
;与卡机取水值相或
           LDS   BUFMS,PortIVD
           LSR   BUFMS
		   LSR   BUFMS
           ANDI  BUFMS,0B00000111
		   OR    CNTMS,BUFMS
           STS   WRequest,CNTMS
		   RET
;----------------------------------------------
;------------------------------------------
;MSUB5、语音报警置位子程序组
;------------------------------------------
;入口条件：2～22对照表
;出口条件：VOLine1/2/3对应位置位
VOAMS2:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b00000001
          RJMP   SVOL1PRO
VOAMS3:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b00000010
          RJMP   SVOL1PRO
VOAMS4:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b00000100
          RJMP   SVOL1PRO
VOAMS5:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b00001000
          RJMP   SVOL1PRO
VOAMS6:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b00010000
          RJMP   SVOL1PRO
VOAMS7:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b00100000
          RJMP   SVOL1PRO
VOAMS8:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b01000000
          RJMP   SVOL1PRO
VOAMS9:   LDS    BUFMS,VOLine1
		  ORI    BUFMS,0b10000000
SVOL1PRO: STS    VOLine1,BUFMS
          RET
;------------
VOAMS10:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b00000001
          RJMP   SVOL2PRO
VOAMS11:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b00000010
          RJMP   SVOL2PRO
VOAMS12:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b00000100
          RJMP   SVOL2PRO
VOAMS13:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b00001000
          RJMP   SVOL2PRO
VOAMS14:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b00010000
          RJMP   SVOL2PRO
VOAMS15:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b00100000
          RJMP   SVOL2PRO
VOAMS16:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b01000000
          RJMP   SVOL2PRO
VOAMS17:  LDS    BUFMS,VOLine2
		  ORI    BUFMS,0b10000000
SVOL2PRO: STS    VOLine2,BUFMS
          RET
;------------
VOAMS18:  LDS    BUFMS,VOLine3
		  ORI    BUFMS,0b00000001
          RJMP   SVOL3PRO
VOAMS19:  LDS    BUFMS,VOLine3
		  ORI    BUFMS,0b00000010
          RJMP   SVOL3PRO
VOAMS20:  LDS    BUFMS,VOLine3
		  ORI    BUFMS,0b00000100
          RJMP   SVOL3PRO
VOAMS21:  LDS    BUFMS,VOLine3
		  ORI    BUFMS,0b00001000
          RJMP   SVOL3PRO
VOAMS22:  LDS    BUFMS,VOLine3
		  ORI    BUFMS,0b00010000
SVOL3PRO: STS    VOLine3,BUFMS
          RET
;----------------------------------------------
;MSUB6、语音播放子程序（100MS查询一次）
;----------------------------------------------
;入口条件：VOLine1 VOLine2 VOLine3
;出口和件：播后自动清请求标志
;占用资源：BUFMS CNTMS
VOBFSUB:  NOP
          LDS    BUFMS,PortIVF
          SBRS   BUFMS,BVOEFI
          RJMP   VOBPEND           ;遇忙则返
;------------
          CLR    CNTMS
          LDS    BUFMS,VOLine3
		ANDI   BUFMS,0B00011111   ;高三位暂未用
		  CPSE   BUFMS,CNTMS
		  RJMP   VOBFXCB3
		  LDS    BUFMS,VOLine2
		  CPSE   BUFMS,CNTMS
		  RJMP   VOBFXCB2
		  LDS    BUFMS,VOLine1
		  CPSE   BUFMS,CNTMS
		  RJMP   VOBFXCB1
		  NOP
		  RJMP   VOBPEND           ;没有语音播放任务则返
;--------------
VOBFXCB1: LDI    CNTMS,1
          CLT
;--------
          INC    CNTMS
		  SBRS   BUFMS,0
          RJMP   PC+5
          BLD    BUFMS,0
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,1
          RJMP   PC+5
          BLD    BUFMS,1
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,2
          RJMP   PC+5
          BLD    BUFMS,2
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,3
          RJMP   PC+5
          BLD    BUFMS,3
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,4
          RJMP   PC+5
          BLD    BUFMS,4
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,5
          RJMP   PC+5
          BLD    BUFMS,5
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,6
          RJMP   PC+5
          BLD    BUFMS,6
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,7
          RJMP   PC+5
          BLD    BUFMS,7
		  STS    VOLine1,BUFMS
		  RJMP   VOTXRH
;--------
		  RJMP   VOBPEND
;-----------------------
VOBFXCB2: LDI    CNTMS,9
          CLT
;--------
          INC    CNTMS
		  SBRS   BUFMS,0
          RJMP   PC+5
          BLD    BUFMS,0
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,1
          RJMP   PC+5
          BLD    BUFMS,1
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,2
          RJMP   PC+5
          BLD    BUFMS,2
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,3
          RJMP   PC+5
          BLD    BUFMS,3
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,4
          RJMP   PC+5
          BLD    BUFMS,4
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,5
          RJMP   PC+5
          BLD    BUFMS,5
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,6
          RJMP   PC+5
          BLD    BUFMS,6
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,7
          RJMP   PC+5
          BLD    BUFMS,7
		  STS    VOLine2,BUFMS
		  RJMP   VOTXRH
;--------
		  RJMP   VOBPEND
;-----------------------
VOBFXCB3: LDI    CNTMS,17
          CLT
;--------
          INC    CNTMS
		  SBRS   BUFMS,0
          RJMP   PC+5
          BLD    BUFMS,0
		  STS    VOLine3,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,1
          RJMP   PC+5
          BLD    BUFMS,1
		  STS    VOLine3,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,2
          RJMP   PC+5
          BLD    BUFMS,2
		  STS    VOLine3,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
          SBRS   BUFMS,3
          RJMP   PC+5
          BLD    BUFMS,3
		  STS    VOLine3,BUFMS
		  RJMP   VOTXRH
;--------
          INC    CNTMS
		  SBRS   BUFMS,4
          RJMP   PC+5
          BLD    BUFMS,4
		  STS    VOLine3,BUFMS
		  RJMP   VOTXRH
;--------
		  RJMP   VOBPEND
;-----------------------
;发送复位头
VOTXRH:   SBI    PORTA,BVORET
          RCALL  DEL50US
		  CBI    PORTA,BVORET
          RCALL  DEL50US
;-----------
;发计数脉冲
          NOP
		  RCALL  VOSPHSUB
          DEC    CNTMS
		  BRNE   PC-3
VOBPEND:  NOP
		  RET
;---------------
;语音单脉冲头子程序
;占用资源：BUFMS
 VOSPHSUB: SBI   PORTA,BVOPUL
          RCALL  DEL50US
		  CBI    PORTA,BVOPUL
          RCALL  DEL50US
          NOP
		  RET
;---------------
;延时50US子程序
DEL50US:  LDI    BUFMS,40      ;1.25*40=50US
ELD50ULOOP:
          NOP
		  NOP
		  NOP
		  NOP
          NOP
          NOP
		  NOP
		  DEC    BUFMS
		  BRNE   ELD50ULOOP	  
          RET
;**********************************************

  
;**********************************************
;四、中断服务程序区
;**********************************************
;********************************
;（一）、ADC转换结束中断程
;;*******************************
;入口条件：相应均值ADVSHn/ADVSLn中
INT_ADC:  IN     SREG_B,SREG      ;备份状态寄存器
          PUSH   WHI
		  PUSH   WLI
          IN     WLI,ADCL          ;读AD转换值放W中
		  IN     WHI,ADCH
;------------------------------
;累加保存相应的原始值
          LDS    BUFIS,ADV16SL    ;累加16次原始值
          ADD    BUFIS,WLI
		  STS    ADV16SL,BUFIS
		  LDS    BUFIS,ADV16SH     
		  ADC    BUFIS,WHI
		  STS    ADV16SH,BUFIS
          LDS    BUFIS,ADV16CNT
		  DEC    BUFIS
		  STS    ADV16CNT,BUFIS
		  BREQ   PC+2 
          RJMP   ADCEND             ;16次未到
;--------------
;16次到处理
		  LDS    WHI,ADV16SH
		  LDS    WLI,ADV16SL
		  LDI    BUFIS,16
		  STS    ADV16CNT,BUFIS 
		  CLR    BUFIS
		  STS    ADV16SH,BUFIS
		  STS    ADV16SL,BUFIS     
;除16得平均值
          LSR    WHI
		  ROR    WLI
          LSR    WHI
		  ROR    WLI
		  LSR    WHI   
		  ROR    WLI
          LSR    WHI
		  ROR    WLI
;------------
;目前暂时只8位
        LSR    WHI           ;%%%
		ROR    WLI
        LSR    WHI
		ROR    WLI
;-------------
;均值加在相应通道值存储器上
          IN     BUFIS,ADMUX
          ANDI   BUFIS,0b00000011
          CPI    BUFIS,0
		  BRNE   PC+2
		  RJMP   SADCP0
	      CPI    BUFIS,1
		  BRNE   PC+2
		  RJMP   SADCP1
		  CPI    BUFIS,2
		  BRNE   PC+2
		  RJMP   SADCP2	  
	 	  CPI    BUFIS,3
		  BRNE   PC+2
		  RJMP   SADCP3	
		  RJMP   ADVSEND
;---------
;存0路ADC值:环境水温   
SADCP0:   STS    ADVSH0,WHI
          STS    ADVSL0,WLI
          RJMP   ADVSEND
;---------
;存1路ADC值：温水水温   
SADCP1:   STS    ADVSH1,WHI
          STS    ADVSL1,WLI
          RJMP   ADVSEND
;---------
;存2路ADC值：开水水温   
SADCP2:   STS    ADVSH2,WHI
          STS    ADVSL2,WLI
          RJMP   ADVSEND
;---------
;存3路ADC值: 超高温  
SADCP3:   STS    ADVSH3,WHI
          STS    ADVSL3,WLI	  
ADVSEND:  NOP
;------------------------
;通道进1切换
          IN     BUFIS,ADMUX
          INC    BUFIS
		  ANDI   BUFIS,0b01000011
          ORI    BUFIS,0b01000000 ;AVCC参考源，右对齐
          OUT    ADMUX,BUFIS
;--------------------------
;AD转换结束
ADCEND:   NOP
          POP    WLI
		  POP    WHI       
          OUT    SREG,SREG_B      ;还原状态寄存器
          NOP
		  RETI
;*********************************
;(二)、T0中断程序（20MS中断，8MHZ晶振，1024分频）
;*********************************
TIM0_OVF: NOP
          WDR
          IN     SREG_B,SREG       ;备份状态寄存器
		  PUSH   WHI
		  PUSH   WLI
          LDI    BUFI,100         ;置初值100          &&&
          OUT    TCNT0,BUFI
;----------------------------
;测试水流用
;      IN     CNTI,TCNT1L        ;   %%
;      INC    CNTI
;	  OUT    TCNT1L,CNTI
;---------
;      IN     CNTI,TCNT2
;	  INC    CNTI
;      OUT    TCNT2,CNTI
;---------
;      LDS    BUFI,TCNT3L
;	  INC    BUFI
;	  STS    TCNT3L,BUFI
;----------------------------------
;T0中断任务1:置程序定时对象之标志位
;----------------------------------
T20MSStart:LDS   BUFI,Message1
          SET
		  BLD    BUFI,B20MST
;--------
		  LDS    CNTI,POT100MS
		  INC    CNTI
		  STS    POT100MS,CNTI
		  CPI    CNTI,5           ;100MS
		  BRNE   TEST300MST
          CLR    CNTI
		  STS    POT100MS,CNTI
          BLD    BUFI,B100MST     ;置100MS对象标志
;--------
TEST300MST:
          LDS    CNTI,POT300MS
		  INC    CNTI
		  STS    POT300MS,CNTI
		  CPI    CNTI,15           ;300MS
		  BRNE   TEST500MST
          CLR    CNTI
		  STS    POT300MS,CNTI
          BLD    BUFI,B300MST      ;置300MS对象标志
;--------
TEST500MST:
          LDS    CNTI,POT500MS
		  INC    CNTI
		  STS    POT500MS,CNTI
		  CPI    CNTI,25           ;500MS
		  BRNE   TEST1ST
          CLR    CNTI
		  STS    POT500MS,CNTI
          BLD    BUFI,B500MST      ;置500MS对象标志
;--------
TEST1ST:
          LDS    CNTI,POT1S
		  INC    CNTI
          STS    POT1S,CNTI
		  CPI    CNTI,50           ;1S
		  BREQ   PC+4
          STS    Message1,BUFI     ;未到1秒
          RJMP   TASK1END
		  NOP
          CLR    CNTI
		  STS    POT1S,CNTI
          BLD    BUFI,B1ST         ;置1S对象标志
          STS    Message1,BUFI
;------------------------------
;1秒中断程序
;------------------------------
;1S中断任务1：采集流速放SpeedOP1/SpeedOP2/SpeedOP3中
          IN     BUFI,TCNT1L
		  IN     CNTI,TCNT1H
          TST    CNTI
		  BREQ   PC+2
		  LDI    BUFI,0XFF
		  STS    SpeedOP1,BUFI
		  CLR    BUFI
		  OUT    TCNT1H,BUFI
		  OUT    TCNT1L,BUFI
;---------
          IN     BUFI,TCNT2
		  STS    SpeedOP2,BUFI
		  CLR    BUFI
		  OUT    TCNT2,BUFI
;---------
          LDS    BUFI,TCNT3L
		  LDS    CNTI,TCNT3H
          TST    CNTI
		  BREQ   PC+2
		  LDI    BUFI,0XFF
		  STS    SpeedOP3,BUFI
		  CLR    BUFI
		  STS    TCNT3H,BUFI
		  STS    TCNT3L,BUFI
;---------------------------
;1S中断任务2：当前流量脉冲计数
           LDS   CNTI,FlowCP1L
           LDS   BUFI,SPEEDOP1
		   ADD   CNTI,BUFI
		   STS   FlowCP1L,CNTI
		   BRCC  PC+6
		   LDS   CNTI,FlowCP1H
		   INC   CNTI
		   STS	 FlowCP1H,CNTI
		   NOP
;------------------------ 
           LDS   CNTI,FlowCP2L
           LDS   BUFI,SPEEDOP2
		   ADD   CNTI,BUFI
		   STS   FlowCP2L,CNTI
		   BRCC  PC+6
		   LDS   CNTI,FlowCP2H
		   INC   CNTI
		   STS	 FlowCP2H,CNTI
		   NOP
;------------------------ 
           LDS   CNTI,FlowCP3L
           LDS   BUFI,SPEEDOP3
		   ADD   CNTI,BUFI
		   STS   FlowCP3L,CNTI
		   BRCC  PC+6
		   LDS   CNTI,FlowCP3H
		   INC   CNTI
		   STS	 FlowCP3H,CNTI
		   NOP
;============================
;1S中断任务3：置1分钟标志
           LDS   CNTI,CLKSS
		   CPI   CNTI,0X30
		   BRNE  S1I3PEND
		   LDS   BUFI,Message1
           ORI   BUFI,0B10000000    ;置1分对象标志      
           STS   Message1,BUFI
S1I3PEND:  NOP
;----------------------------
TASK1END: NOP
;---------------------------------
;T0中断任务2:去抖动后的输入口值存PortIVA(B/C/D/E/F/G)中
;--------------------------------
;读B口处理
          IN     BUFIS,PINB        
;         ANDI   BUFIS,0B11111111  ;屏蔽B口非输入位
		  LDS    WLI,PortTIVB
          CP     WLI,BUFIS
          BREQ   PC+4
          STS    PortTIVB,BUFIS     ;是干扰则更新B口临时时值
          RJMP   RPortEndB
;------
          LDS    WHI,PortIVB
		  CPSE   WHI,WLI
		  STS    PortIVB,WLI        ;有变化则更新B口输入值
RPortEndB:NOP                       ;读B口结束
;-------------------------
;读C口处理
          IN     BUFIS,PINC        
;         ANDI   BUFIS,0B11111111   ;屏蔽C口非输入位
		  LDS    WLI,PortTIVC
          CP     WLI,BUFIS
          BREQ   PC+4
          STS    PortTIVC,BUFIS     ;是干扰则更新C口临时时值
          RJMP   RPortEndC
;------
          LDS    WHI,PortIVC
		  CPSE   WHI,WLI
		  STS    PortIVC,WLI        ;有变化则更新C口输入值
RPortEndC:NOP                       ;读C口结束  
;-------------------------
;读D口处理
          IN     BUFIS,PIND        
          ANDI   BUFIS,0B00111100   ;屏蔽D口非输入位
		  LDS    WLI,PortTIVD
          CP     WLI,BUFIS
          BREQ   PC+4
          STS    PortTIVD,BUFIS     ;是干扰则更新D口临时时值
          RJMP   RPortEndD
;------
          LDS    WHI,PortIVD
		  CPSE   WHI,WLI
		  STS    PortIVD,WLI        ;有变化则更新D口输入值
RPortEndD:NOP
;-------------------------
;读E口处理
          IN     BUFIS,PINE        
          ANDI   BUFIS,0B00000001   ;屏蔽E口非输入位
		  LDS    WLI,PortTIVE
          CP     WLI,BUFIS
          BREQ   PC+4
          STS    PortTIVE,BUFIS     ;是干扰则更新E口临时时值
          RJMP   RPortEndE
;------
          LDS    WHI,PortIVE
		  CPSE   WHI,WLI
		  STS    PortIVE,WLI        ;有变化则更新E口输入值
RPortEndE:NOP
;-------------------------
;读F口处理
          IN     BUFIS,PINF        
          ANDI   BUFIS,0B11110000  ;屏蔽F口非输入位
		  LDS    WLI,PortTIVF
          CP     WLI,BUFIS
          BREQ   PC+4
          STS    PortTIVF,BUFIS    ;是干扰则更新F口临时时值
          RJMP   RPortEndF
;------
          LDS    WHI,PortIVF
		  CPSE   WHI,WLI
		  STS    PortIVF,WLI        ;有变化则更新F口输入值
RPortEndF:NOP
;---------------------------------
;T0中断任务3:据MLEDFFEN值驱动LED快闪
;---------------------------------
          LDS    BUFIS,MLEDFFCNT
          DEC    BUFIS
		  BRNE   LEDFFNTON
          LDI    BUFIS,4
		  STS    MLEDFFCNT,BUFIS
          LDS    BUFIS,MLEDFFEN
          COM    BUFIS
		  LDS    CNTIS,PORTG
          SET
		  SBRS   CNTIS,BLEDYX
          CLT
		  BLD    BUFIS,BLEDYX        
		  STS    PORTG,BUFIS
		  RJMP   LEDFFEND
;-------
LEDFFNTON:STS    MLEDFFCNT,BUFIS
		  CPI    BUFIS,2
		  BRNE   LEDFFEND
          LDI    BUFIS,0B00001111
          LDS    CNTIS,PORTG
          CLT
		  SBRS   CNTIS,BLEDYX
		  BLD    BUFIS,BLEDYX
          STS    PORTG,BUFIS
LEDFFEND: NOP
;---------------------------------
T0INTEND: NOP                      ;T0中断程序结束
          POP    WLI
		  POP    WHI       
          OUT    SREG,SREG_B
          NOP
		  RETI
;*********************************
;(三)、TWI中断程序 
;*********************************
TWI_IPRO:  IN    SREG_B,SREG       ;备份状态寄存器
		   PUSH  WHI
		   PUSH  WLI
		   PUSH  YH   
		   PUSH  YL
;----------------------
;首先检测状态处理
;----------------------
;对主机态的检测
           LDS   CNTI,TWSR
		   CPI   CNTI,0X08
		   BRNE  PC+2
		   RJMP  StartOk          ;启动位发成功
		   CPI   CNTI,0X18
		   BRNE  PC+2
		   RJMP  SLAWOK           ;SAL+W发成功
		   CPI   CNTI,0X28
		   BRNE  PC+2
		   RJMP  MDataTOk         ;主机数据发成功
           CPI   CNTI,0X10
		   BRNE  PC+2
		   RJMP  RStartOk         ;重启动位发成功
           CPI   CNTI,0X40
		   BRNE  PC+2
		   RJMP  SLAROk           ;SAL+R发成功
           CPI   CNTI,0X50
		   BRNE  PC+2
		   RJMP  MDataROk         ;主机数据收成功
           CPI   CNTI,0X58
		   BRNE  PC+2
		   RJMP  MFNACKOK         ;主机返回非应答成功
;-----------------------
;对从机接收状态的检测
           CPI   CNTI,0X60  
		   BRNE  PC+2
		   RJMP  RSLAWOK          ;收自身的SAL+W成功
           CPI   CNTI,0X68
           BRNE  PC+2
           RJMP  RSLAWOK_F        ;收自身的SAL+W成功,但先前作为主机被仲裁失败
           CPI   CNTI,0X80
		   BRNE  PC+2
		   RJMP  SDataROk         ;从机收数据成功		   
           CPI   CNTI,0XA0
		   BRNE  PC+2
		   RJMP  SRXDEND          ;收到停止位
;-----------------------
;对从机发送状态的检测
           CPI   CNTI,0XA8  
		   BRNE  PC+2
		   RJMP  RSLAROK          ;收自身的SAL+R成功
           CPI   CNTI,0XB0
           BRNE  PC+2
           RJMP  RSLAROK_F        ;收自身的SAL+R成功,但先前作为主机被仲裁失败
           CPI   CNTI,0XB8
		   BRNE  PC+2
		   RJMP  SDataTOk         ;从机发数据成功		   
           CPI   CNTI,0XC0
		   BRNE  PC+2
		   RJMP  STXDEND          ;收到非应答位
;----------------------------
;进一步查错
           CPI   CNTI,0X20        ;SLA+W无应答
           BRNE  PC+2
           RJMP  SNACKPRO
           CPI   CNTI,0X30        ;发数后无应答
           BRNE  PC+2
           RJMP  SNACKPRO
		   CPI   CNTI,0X48        ;SLA+R无应答
           BRNE  PC+2
           RJMP  SNACKPRO
;----------------
           CPI   CNTI,0X38        ;主发失败无应答
           BRNE  PC+2
           RJMP  MTXERRPRO
           CPI   CNTI,0X00        ;非法操作，有冲突
           BRNE  PC+2
           RJMP  ERRTWOPRO
		   NOP
		   RJMP  OtherErr         ;其它错误
;----------------------------
;对出错的处理
;----------------------------
;从机无应答:内部错误代码51
SNACKPRO: 
           LDS   BUFI,Message1
           ORI   BUFI,0B00000010   ;置TWI主机操作后任务标志
           STS   Message1,BUFI
		   LDI   BUFI,0X50         ;50H:从机无应答   
		   STS   MOENDFI,BUFI
           LDI   BUFI,TWCR_STO	  
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;------------
;主机发错误(38H)
MTXERRPRO: STS   MOENDFI,BUFI      ;对于38H错误，主程序不介入，
           LDI   BUFI,TWCR_CIF     ;总线空后会自动发启动位的
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;------------
;错误操作，有冲突（00H）
ERRTWOPRO: LDI   BUFI,TWCR_STO
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;------------
;以上都不是，是作异常处理
OtherErr:  LDI   BUFI,TWCR_STO
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;==================================
;发启始位成功
StartOk:  
		   LDS   BUFI,SLADDW       ;发从机SLA+W
		   STS   TWDR,BUFI
		   LDI   BUFI,TWCR_CIFN    ;清中断标志
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-------------------------
;发SLA+W成功
SLAWOK:    LDI   YH,HIGH(MRTDFA)   ;发首字节据（也就是写入从机数据的首地址）	  
		   LDI   YL,LOW(MRTDFA)
           LD    BUFI,Y    
		   STS   TWDR,BUFI
		   LDI   BUFI,TWCR_CIFN
		   STS   TWCR,BUFI
		   CLR   BUFI
		   STS   MRTPOIN,BUFI       ;主机发计数清0
		   RJMP  TWIEND
;-------------------------
;主机数据发成功
MDataTOk:  LDS   CNTI,MTXAB
           DEC   CNTI
		   STS   MTXAB,CNTI
		   BREQ  MTXDEND           ;数据发完
;----------------
;发数据继续      
           LDI   YH,HIGH(MRTDFA)
           LDI   YL,LOW(MRTDFA)
           LDS   CNTI,MRTPOIN       ;主发计数加1
           INC   CNTI
		   STS   MRTPOIN,CNTI
           ADD   YL,CNTI
		   BRCC  PC+2
		   INC   YH
           LD    BUFI,Y    
		   STS   TWDR,BUFI
           LDI   BUFI,TWCR_CIFN
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-----------------
;发数据完毕
MTXDEND:   SBRC  FLAGI,BTZR    
           RJMP  TZRPRO            ;有转为读任务
;------------
		   LDS   BUFI,Message1
           ORI   BUFI,0B00000010   ;置TWI主机操作后任务标志
           STS   Message1,BUFI
		   LDI   BUFI,0XA0         ;A0H:主机单发成功
		   STS   MOENDFI,BUFI
           LDI   BUFI,TWCR_STO     ;发停止位
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-----------------
;由发转收
TZRPRO:    CLT
		   BLD   FLAGI,BTZR
           LDI   BUFI,TWCR_STA     ;重发起始位   
		   STS   TWCR,BUFI
		   RJMP  TWIEND 
;=====================================
;重启动位发成功
RStartOk:  LDS   BUFI,SLADDW       ;发SLA+R  
		   INC   BUFI
		   STS   TWDR,BUFI
		   LDI   BUFI,TWCR_CIFN     ;清中断标志
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-------------------------
;发SLA+R成功
SLAROK:    CLR   BUFI
		   STS   MRTPOIN,BUFI      ;主机收发计数清0
           LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-------------------------
;主机数据收成功
MDataROk:  LDI   YH,HIGH(MRTDFA)  
           LDI   YL,LOW(MRTDFA)
           LDS   CNTI,MRTPOIN      ;主机收发计数加1后收数
           INC   CNTI
		   STS   MRTPOIN,CNTI
		   DEC   CNTI
           ADD   YL,CNTI
		   BRCC  PC+2
		   INC   YH
           LDS   BUFI,TWDR
           ST    Y,BUFI
;-------------
           LDS   CNTI,MRXAB
           DEC   CNTI
		   STS   MRXAB,CNTI
		   BREQ  MRXDEND           ;主机收数据完毕
		   LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-----------------
;主机收数据完毕
MRXDEND:   
;-----------------------
           LDI   BUFI,TWCR_CIFN    ;结束时一定要行发非应位才行！！
	       STS   TWCR,BUFI
           LDS   BUFI,MTCODE       ;1号代码为时钟专用
           CPI   BUFI,1
		   BRNE  PC+2
		   RJMP  CLKREPRO          ;是读时钟
;--------------
           LDS   BUFI,Message1
           ORI   BUFI,0B00000010   ;置TWI主机操作后任务标志
           STS   Message1,BUFI
		   LDI   BUFI,0XA1         ;A1H:主机发收成功
		   STS   MOENDFI,BUFI           
	       RJMP  TWIEND
;---------------------------
;主机返回非应答成功
MFNACKOK:  LDI   BUFI,TWCR_STO     ;发停止位
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;=====================================
;对从机接收处理部分    @8
;=====================================
;;接收自身的SLA+W成功，但先前主机失败(68H)
RSLAWOK_F: LDS   BUFI,Message1
           ORI   BUFI,0B00000010   ;置TWI主机操作后任务标志
           STS   Message1,BUFI
		   LDI   BUFI,0X51         ;51H:主机收发失败
		   STS   MOENDFI,BUFI
;---------------------------------
;接收自身的SLA+W成功(60H)
RSLAWOK:   CLR   BUFI
		   STS   SRTCNT,BUFI       ;从机收发计数清0
		   LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
		   
;---------------------------------
;从机数据接收成功(80H)
SDataROk:  
           LDS   CNTI,SRTCNT       ;S从机收发计数器+1
           INC   CNTI
		   STS   SRTCNT,CNTI
		   DEC   CNTI
           TST   CNTI
		   BRNE  SDataRCON
;----------------
;是首字节则一定是表地址
           LDS   BUFI,TWDR
;检查相对地址合法性
           CPI   BUFI,0X40         ;有效为0X40~0X79H
           BRCS  XDADDFF
           CPI   BUFI,0X80
		   BRCC  XDADDFF
;-------------------------
;相对写表地址合法
           STS   SReadFA,BUFI
           LDI   YHI,HIGH(PTPFADD)
           LDI   YLI,LOW(PTPFADD)
		   ADD   YLI,BUFI
		   BRCC  PC+2
		   INC   YHI
		   STS   TabPoinH,YHI      ;保存新的表地址指针
           STS   TabPoinL,YLI
;-------------
           LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;----------------
;相对写表地址非法
XDADDFF:   LDI   BUFI,TWCR_CIFN
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;----------------
;从机数据收继续 (80H)
SDataRCON: LDS   YHI,TabPoinH
           LDS   YLI,TabPoinL
           LDS   BUFI,TWDR
		   ST    Y,BUFI             ;存数后指针加1
		   ADIW  YLI,1
		   STS   TabPoinH,YHI
           STS   TabPoinL,YLI
		   LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   LDI   BUFI,0X55         ;以最后一个地址作为校正
           STS   STOPCECK,BUFI
		   RJMP  TWIEND
;-----------------------------------
;收到停止位，从机接收数据完毕（A0H）
;信息在SReadFA SRTCNT SOENDFI中
SRXDEND:   CLR   CNTI
           LDS   BUFI,STOPCECK        ;校验 55H
           STS   STOPCECK,CNTI
		   CPI   BUFI,0X55
 		   BREQ  STOPBDOK
		   LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
	       NOP
		   RJMP  TWIEND
;----------------
;STOP位的数据合法
STOPBDOK:  LDS   BUFI,Message1
           ORI   BUFI,0B00000100   ;置TWI从机接收数据完成消息位
           STS   Message1,BUFI
           LDI   BUFI,0XA1         ;0XA1:从机收数完成
		   STS   SOENDFI,BUFI
           LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;=====================================
;对从机发送处理部分
;=====================================
;接收自身的SLA+R成功,但先前发作为主机失败(B0H)           @1
RSLAROK_F: LDS   BUFI,Message1
           ORI   BUFI,0B00000010   ;置TWI主机操作后任务标志
           STS   Message1,BUFI
		   LDI   BUFI,0X51         ;51H:主机收发失败   
		   STS   MOENDFI,BUFI
;---------------------------------
;接收自身的SLA+R成功(A8H)
RSLAROK:   CLR   BUFI
		   STS   SRTCNT,BUFI        ;从机收发计数清0
		   LDS   YHI,TabPoinH       ;装表地址指针
           LDS   YLI,TabPoinL
           LD    BUFI,Y
		   STS   TWDR,BUFI
		   ADIW  YLI,1
		   STS   TabPoinH,YHI
           STS   TabPoinL,YLI
		   LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;---------------------------------
;从机数据发送成功(B8H)则继续从发
SDataTOk:  
           LDS   CNTI,SRTCNT       ;计数
		   INC   CNTI
		   STS   SRTCNT,CNTI
;----------------
           LDS   YHI,TabPoinH       ;装表地址指针
           LDS   YLI,TabPoinL
           LD    BUFI,Y
		   STS   TWDR,BUFI
		   ADIW  YLI,1
		   STS   TabPoinH,YHI
           STS   TabPoinL,YLI
		   LDI   BUFI,TWCR_CIF
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;-----------------------------------
;收到非应答位，从机发数据完毕（0xC0）
;信息在SXDADD SRTCNT 中 
STXDEND: ;  LDI   BUFI,0XA0           ;0XA0:从机发数完成
		 ;  STS   SOENDFI,BUFI
         ;  CALL  TESTSY
           LDI   BUFI,TWCR_CIF       ;对方仅读表则主程序无需介入
		   STS   TWCR,BUFI
		   RJMP  TWIEND
;====================================
;中断内处理程序  
;------------------------------------
;读的是时间/日期
CLKREPRO:  CLR   BUFI
           STS   MTCODE,BUFI
           LDI   YH,HIGH(MRTDFA)
		   LDI   YL,LOW(MRTDFA)
		   LD    BUFI,Y+
           ANDI  BUFI,0B01111111   ;秒
		   STS   CLKSS,BUFI
           LD    BUFI,Y+
		   ANDI  BUFI,0B01111111   ;分
		   STS   CLKMM,BUFI
		   LD    BUFI,Y+
		   ANDI  BUFI,0B00111111   ;时
		   STS   CLKHH,BUFI
		   LD    BUFI,Y+
		   ANDI  BUFI,0B00111111   ;日
		   STS   CLKDD,BUFI
		   LD    BUFI,Y+
		   ANDI  BUFI,0B00111111   ;星期
		   STS   CLKWW,BUFI
           LD    BUFI,Y+
		   ANDI  BUFI,0B00011111   ;月
		   STS   CLKMO,BUFI
		   LD    BUFI,Y+
           ANDI  BUFI,0B01111111   ;年
		   STS   CLKYY,BUFI           
		   RJMP  TWIEND	    
;---------------------------------
           NOP
		   NOP

;------------------------------------------------
TWIEND:    NOP                       ;TWI中断程序结束
;------------------------------------------------
           POP   YL
		   POP   YH 
		   POP   WLI
		   POP   WHI       
           OUT   SREG,SREG_B
           NOP
		   RETI
;************************************************
;五、中断程序专用子程序区
;************************************************


;************************************************
.EXIT
;************************************************
;************************************************
;六、程序后备区
;************************************************



