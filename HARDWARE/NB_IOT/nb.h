#ifndef __800A_H
#define __800A_H	 
#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "delay.h"


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//ESP8266　AT命令交互实现，可以修改WiFi为AP模式，STA模式，可以让WiFi工作于TCP/UDP模式
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************



#ifndef NULL
#define NULL ((char *)0)
#endif

#define MAX_AT_PARSE_LINE     20    //最大解析行
#define MAX_AT_PARSE_LEN      500  ///接收命令最大字数
#define MAX_NB_TX_RX_LEN     (MAX_AT_PARSE_LEN)    //发送接收最大字节
#define MAX_RX_DATA_LEN    5     //保存接收的socket最大数量


typedef enum _mqtt_request_state_{
    MQTT_REQUEST_INIT=0,//未注册
    MQTT_REQUEST_IDLE,//注册成功，空闲中
} MQTT_REQUEST_STATE;


//1:TCP client   2:UDP
enum
{
  NB_TCP=1,  
  NB_UDP=2,
};


typedef enum {

    //移动为8，电信为5
    N_BAND_TELECOM=5,
    N_BAND_MOBILE=8,
} N_BAND_TYPE;


typedef enum {
    GPRS_INIT=0,//初始化
    GPRS_IDLE,//空闲
    GPRS_OUTGOING,//呼出
    GPRS_INCOMING,//呼入
    GPRS_SPEAKING,//通话
    GPRS_RX_SMS,//收到信息
    
}GPRS_STATUS ;


//用户配置
typedef struct _user_config_
{
    u8 mode;//0:TCP,1:UDP
    u8 ip[4];//服务器IP
    u16 port;//服务器PORT
    u16 lPort;//本地PORT
    char LocakIp[4];//本地IP
}UserConfig;


//AT命令解析
typedef struct _at_parse_cmd_
{
    u8 line[MAX_AT_PARSE_LINE][MAX_AT_PARSE_LEN];//AT命令共三行
    __IO u16 count[MAX_AT_PARSE_LINE];//每一行字数
    __IO u8 insertIdx;//插入行索引
    __IO u8 parseIdx;//解析行索引
    __IO u16 insertCurCount;//插入的位置
    u8 laseCH;//插入的最后一个字符
    
}AtParseCmd;

//网络相关的数据
typedef struct _net_data_
{
    s8 socket;//0~7的一个整数    
    u8 mode;//NB_UDP或者NB_TCP
    u8 remote_ip[20];//服务器IP，字符串
    u16 remote_port;//服务器PORT

    u16 len;//发送或者接收的数据长度
    u8 data[MAX_NB_TX_RX_LEN];//发送编码数据或者接收编码前数据
}NetData;

//保存接收socket和len
typedef struct _rx_data_
{
    u8 socket[MAX_RX_DATA_LEN];//保存接收的socket
    u16 len[MAX_RX_DATA_LEN];//保存长度
    __IO u8 insertIdx;//插入行索引
    __IO u8 parseIdx;//解析行索引    
}RxData;


typedef struct _nb_data_
{
    UserConfig mConfig;//用户配置

    u8 isConnect;//数据是否已连接 1：表示已连接
    AtParseCmd  parseAt;//收到的命令缓冲

    u8 gOperator[30];//运营商
    u8 csq;//信号强度
    u8 sn[30];//产品序列号Product Serial Number
    u8 imei[30];//IMEI号
    u8 imsi[30];//IMSI号
    u8 RegStatus;//注册状态，注册上是1或者5
    u8 ConnectionStatus;//数据连接状态，1表示数据连接
    u8 nband;//模块的频段，移动为8，电信为5
    u8 attach;
    u8 localIp[20];//本地IP

    NetData net;//网络发送接收数据相关
    RxData rx;
    
    //MQTT状态，只能idle过程中能发起请求
    MQTT_REQUEST_STATE mqtt_state;
    short t;//板上的温度
}NB_DATA;


extern char gAtParseBuff[MAX_AT_PARSE_LEN];
extern NB_DATA nb;

extern void NB_Printf(const char * pFormat, ...);
extern u16 NB_GetLine(char* buff);
extern void NB_ms(u16 ms);
extern void AddUartChar(unsigned char c);
extern void NB_powerOn(u8 on);
extern u8 NB_parseDefaultMsg(u8* cmd, u32 len);
extern u8 NB_Attach(void);
extern void NB_CheckCSQ(void);
extern void NB_CheckCops(void);		 
extern void NB_CreateConnect(u8 mode, u8* s_ip, u16 s_port);//打开GPRS连接
extern void NB_CycleCheck(void);
extern unsigned char NB_SendData(u8* data, u16 len);
extern void RebootNb(void);
extern u8 NB_onenetTest(void);
extern u8 NB_onenetNotify(void);
extern void NB_GetData(void);
extern void NB_close(void);
extern void NB_init(u8 mode, u8* s_ip, u16 s_port);
extern void NB_GetNetworkRegistrationStatus(void);

#endif
