#ifndef __800A_H
#define __800A_H	 
#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "delay.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//ESP8266��AT�����ʵ�֣������޸�WiFiΪAPģʽ��STAģʽ��������WiFi������TCP/UDPģʽ
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************



#ifndef NULL
#define NULL ((char *)0)
#endif

#define MAX_AT_PARSE_LINE     20    //��������
#define MAX_AT_PARSE_LEN      500  ///���������������
#define MAX_NB_TX_RX_LEN     (MAX_AT_PARSE_LEN)    //���ͽ�������ֽ�
#define MAX_RX_DATA_LEN    5     //������յ�socket�������


typedef enum _mqtt_request_state_{
    MQTT_REQUEST_INIT=0,//δע��
    MQTT_REQUEST_IDLE,//ע��ɹ���������
} MQTT_REQUEST_STATE;


//1:TCP client   2:UDP
enum
{
  NB_TCP=1,  
  NB_UDP=2,
};


typedef enum {

    //�ƶ�Ϊ8������Ϊ5
    N_BAND_TELECOM=5,
    N_BAND_MOBILE=8,
} N_BAND_TYPE;


typedef enum {
    GPRS_INIT=0,//��ʼ��
    GPRS_IDLE,//����
    GPRS_OUTGOING,//����
    GPRS_INCOMING,//����
    GPRS_SPEAKING,//ͨ��
    GPRS_RX_SMS,//�յ���Ϣ
    
}GPRS_STATUS ;


//�û�����
typedef struct _user_config_
{
    u8 mode;//0:TCP,1:UDP
    u8 ip[4];//������IP
    u16 port;//������PORT
    u16 lPort;//����PORT
    char LocakIp[4];//����IP
}UserConfig;


//AT�������
typedef struct _at_parse_cmd_
{
    u8 line[MAX_AT_PARSE_LINE][MAX_AT_PARSE_LEN];//AT�������
    __IO u16 count[MAX_AT_PARSE_LINE];//ÿһ������
    __IO u8 insertIdx;//����������
    __IO u8 parseIdx;//����������
    __IO u16 insertCurCount;//�����λ��
    u8 laseCH;//��������һ���ַ�
    
}AtParseCmd;

//������ص�����
typedef struct _net_data_
{
    s8 socket;//0~7��һ������    
    u8 mode;//NB_UDP����NB_TCP
    u8 remote_ip[20];//������IP���ַ���
    u16 remote_port;//������PORT

    u16 len;//���ͻ��߽��յ����ݳ���
    u8 data[MAX_NB_TX_RX_LEN];//���ͱ������ݻ��߽��ձ���ǰ����
}NetData;

//�������socket��len
typedef struct _rx_data_
{
    u8 socket[MAX_RX_DATA_LEN];//������յ�socket
    u16 len[MAX_RX_DATA_LEN];//���泤��
    __IO u8 insertIdx;//����������
    __IO u8 parseIdx;//����������    
}RxData;


typedef struct _nb_data_
{
    UserConfig mConfig;//�û�����

    u8 isConnect;//�����Ƿ������� 1����ʾ������
    AtParseCmd  parseAt;//�յ��������

    u8 gOperator[30];//��Ӫ��
    u8 csq;//�ź�ǿ��
    u8 sn[30];//��Ʒ���к�Product Serial Number
    u8 imei[30];//IMEI��
    u8 imsi[30];//IMSI��
    u8 RegStatus;//ע��״̬��ע������1����5
    u8 ConnectionStatus;//��������״̬��1��ʾ��������
    u8 nband;//ģ���Ƶ�Σ��ƶ�Ϊ8������Ϊ5
    u8 attach;
    u8 localIp[20];//����IP

    NetData net;//���緢�ͽ����������
    RxData rx;
    
    //MQTT״̬��ֻ��idle�������ܷ�������
    MQTT_REQUEST_STATE mqtt_state;
    short t;//���ϵ��¶�
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
extern void NB_CreateConnect(u8 mode, u8* s_ip, u16 s_port);//��GPRS����
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
