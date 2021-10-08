#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "nb.h"
#include "led.h"
#include "Lcd_Driver.h"
#include "GUI.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//ESP8266��AT�����ʵ�֣������޸�WiFiΪAPģʽ��STAģʽ��������WiFi������TCP/UDPģʽ
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************




char gAtParseBuff[MAX_AT_PARSE_LEN]={0};
NB_DATA nb;

void NB_CloseSocket(void);


//NB������Ϣ�����
void NB_Printf(const char * pFormat, ...)
{
#if 1
	int len=0;
	char info[512] = {0};
	va_list args;
		
	va_start(args, pFormat);
	vsprintf(&info[0], (const char*)pFormat, args);
	va_end(args);

	len=strlen((const char *)info);
	
  //uart1_send_buff((u8*)info, len);
#endif
}

//��ʱ
void NB_ms(u16 ms)
{
  delay_ms(ms);
}

//��һ�У����ڲ����½��յ��ַ���
void changeLine(void)
{
  u8 index=0;
  
  nb.parseAt.insertCurCount=0;
  nb.parseAt.insertIdx++;
  if(nb.parseAt.insertIdx>=MAX_AT_PARSE_LINE)
  {
    nb.parseAt.insertIdx=0;
  }
  
  //��մ���λ��
  index=nb.parseAt.insertIdx;
  memset(nb.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
  nb.parseAt.count[index]=0;
}

//����2�յ�һ���ַ������뵽���뵽�����
//�����յ��������Ƿ���\r����\n�л�����һ��
void AddUartChar(unsigned char c)
{
  u8 index=nb.parseAt.insertIdx;//�������
  u16 count=nb.parseAt.count[index];//�����λ��
  static u8 lastCH=0;
  //if(c=='\r' || c=='\n')
  if(0)
  {
    if(count>0)//���յ����ַ�����0���ܻ�
    {
      changeLine();//������һ�У��ȴ�����
    }
  }
  else
  {
    if(count>=MAX_AT_PARSE_LEN)
    {
      changeLine();//������һ�У��ȴ�����
    }
    else
    {
      nb.parseAt.line[index][count]=c;
      nb.parseAt.insertCurCount++;
      nb.parseAt.count[index]+=1;

      //������ڷ��Ͷ���Ϣ���յ�>�žͻ���
      if(count==0&&c=='>')
      {
        changeLine();//������һ�У��ȴ�����
      }
      else if(lastCH=='\r'&& c=='\n')
      {
        changeLine();//������һ�У��ȴ�����
      }
    }
  }

  lastCH=c;//��¼���һ���ַ�

}

//ȡ��һ�д���3����
//<buff> �������ݵĻ���
//���أ����յ������ݳ��ȣ�0��ʾû�����ݽ���
u16 NB_GetLine(char* buff)
{
    static u8 line=0;
    static u16 insertCount=0;
    static u8 checkCount=0;
    memset(buff, 0, MAX_AT_PARSE_LEN);
    if(nb.parseAt.parseIdx==nb.parseAt.insertIdx)
    {
        if(line==nb.parseAt.parseIdx
          && nb.parseAt.count[line]>0
          && insertCount==nb.parseAt.count[line])
        {
            checkCount++;
        }
        else
        {
            line=nb.parseAt.parseIdx;
            insertCount=nb.parseAt.count[line];
            checkCount=0;
        }

        if(checkCount>3)
        {
            changeLine();//��3�У����������������䣬����������
            NB_Printf("*******changeLine*******\r\n");
        }
          
        return 0;
    }
    else
    {
        u16 len=0;
        u8 index=nb.parseAt.parseIdx;//��ȡ����

        //��ȡ�е�����

        len=nb.parseAt.count[index];
        memcpy(buff, nb.parseAt.line[index], len);


        NB_Printf("[%d],[%d],%s",nb.parseAt.parseIdx,len,nb.parseAt.line[index]);

        memset(nb.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
        nb.parseAt.count[index]=0;

        //�����һ����
        nb.parseAt.parseIdx++;
        if(nb.parseAt.parseIdx>=MAX_AT_PARSE_LINE)
        {
            nb.parseAt.parseIdx=0;
        }

        line=0;
        insertCount=0;
        checkCount=0;
        return len;
    }

    return 0;
}

//ȡ��һ�д���3����
//u8*��socket
//len������
//
void NB_InsertSocketAndLen(u8 socket, u16 len)
{
    u8 index=nb.rx.insertIdx;//�������

    nb.rx.socket[index]=socket;
    nb.rx.len[index]=len;

    nb.rx.insertIdx++;
    if(nb.rx.insertIdx>=MAX_RX_DATA_LEN)
    {
        nb.rx.insertIdx=0;
    }
}



//ȡ��һ�д���3����
//u8*��socket
//len������
//
u8 NB_GetRxSocketAndLen(u8* socket, u16* len)
{
    if(socket==NULL) return 0;
    if(len==NULL) return 0;
    
    if(nb.rx.parseIdx==nb.rx.insertIdx)
    {
        return 0;
    }
    else
    {
        u8 index=nb.rx.parseIdx;//��ȡ����


        NB_Printf("GetRX, [%d],%d,%d.\r\n",index, nb.rx.socket[index],nb.rx.len[index]);        

        *socket=nb.rx.socket[index];
        *len=nb.rx.len[index];

        nb.rx.socket[index]=0;
        nb.rx.len[index]=0;

        //�����һ����
        nb.rx.parseIdx++;
        if(nb.rx.parseIdx>=MAX_RX_DATA_LEN)
        {
            nb.rx.parseIdx=0;
        }
    }

    return 1;
}


//NBģ�鿪���ϵ�
//PB0������
//on����0����������0�ػ�
void NB_powerOn(u8 on)
{
    GPIO_InitTypeDef  GPIO_InitStructure;


    //���δ����, ����IO�ڿ���ģ�鿪��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��PBʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         //PB0 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);          //�����趨������ʼ��GPIOB0

    //�ȶϵ����ϵ�
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);             //PB0�����,����EN��
    if(on>0)
    {
        NB_ms(500);
        GPIO_SetBits(GPIOB,GPIO_Pin_0);             //PB0�����,����EN��
    }
    NB_Printf("\r\npower\r\n");
}

//дһ�����ݵ�����3
//<cmd> ���͵�����
void writeAtPrintf(const char * pFormat, ...)
{
#if 1
  int len=0;
  char info[512] = {0};
  va_list args;
    
  va_start(args, pFormat);
  vsprintf(&info[0], (const char*)pFormat, args);
  va_end(args);

  len=strlen((const char *)info);
  USART3_send_buff((u8*)info,len);
#endif
}


//дһ�����ݵ�����3
//<cmd> ���͵�����
void writeAtCmd(u8* cmd)
{
  u16 len=0;
  if(cmd==NULL) return;

  len=strlen((const char*)cmd);
  
  USART3_send_buff(cmd,len);

  NB_Printf("AT>>>:%s\r\n", cmd);
}

//cmd:Ҫ���͵�����
//wait:1���ȴ������0:���ȴ���д�괮��ֱ�ӷ���
//����:��ʱ����2��������ȷִ�з���1,�������ִ�з���3������0,
u8 NB_SendAT(char* cmd, u8 wait)
{
    u32 len;
    u8 ok_error=0;
    u8 timeout=0;

    if(cmd==NULL) return 0;

    //д����
    writeAtCmd((u8*)cmd);

    //���ȴ���д�괮��ֱ�ӷ���
    if(0==wait)
    {
        return 0;
    }

    //�ȴ����
    ok_error=0;
    timeout=0;
    while(1)
    {
        NB_ms(50);
        len=NB_GetLine(gAtParseBuff);
        if(len>0)
        {
            if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
            {
                ok_error=1;//����
            }
            else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
            {
                ok_error=3;//����
            }
            else
            {
                //��������
                NB_parseDefaultMsg(gAtParseBuff,len);        
            }
        }

        timeout++;

        if(ok_error>0) break;

        if(timeout>200)
        {
            ok_error=2;//��ʱ
            break;
        }
    }

    NB_Printf((u8*)"END AT:%s",cmd);

    return ok_error;
}

u8 NB_parseOperator(u8* cmd, u32 len)
{  
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+COPS:");

    if(ptr!=NULL)
    {
        ptr+=6;
        ptr = (char*)strstr((const char*)ptr,(const char*)",");
        if(ptr==NULL) return 0;
        ptr++;

        ptr = (char*)strstr((const char*)ptr,(const char*)",");
        if(ptr==NULL) return 0;
        ptr++;

        if(ptr!=NULL)
        {
            int index=0;
            while(ptr!=NULL && *ptr!='\"') ptr++;
            ptr++;

            memset(nb.gOperator, 0, 30);
            while(index<20 && ptr!=NULL && *ptr!='\"')
            {

                nb.gOperator[index]=*ptr;
                ptr++;
                index++;
            }
        }

        return 1;
    }

    return 0;
}

u8 NB_parseCsq(u8* cmd, u32 len)
{  
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+CSQ:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=5;
        while(ptr!=NULL && *ptr==' ') ptr++;

        nb.csq=atoi(ptr);
        return 1;
    }
  

  return 0;
}

u8 NB_parseSn(u8* cmd, u32 len)
{  
    char *ptr = cmd;
    if(cmd==NULL) return 0;

    if(ptr!=NULL)
    {
        u8 index=0;

        while(index<20 && ptr!=NULL&& *ptr!='\r'&&*ptr!='\n')
        {
            nb.sn[index]=*ptr;
            ptr++;
            index++;
        }

        return 1;
    }

    return 0;
}


u8 NB_parseImei(u8* cmd, u32 len)
{  
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+CGSN:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=6;
        while(ptr!=NULL && *ptr==' ') ptr++;

        while(index<20 && ptr!=NULL && (*ptr>='0'&&*ptr<='9'))
        {
            nb.imei[index]=*ptr;
            ptr++;
            index++;
        }

        return 1;
    }

    return 0;
}

u8 NB_parseImsi(u8* cmd, u32 len)
{  
    char *ptr = cmd;
    if(cmd==NULL) return 0;

    if(*ptr>='0'&&*ptr<='9')
    {
        u8 index=0;

        while(index<20 && ptr!=NULL && (*ptr>='0'&&*ptr<='9'))
        {
            nb.imsi[index]=*ptr;
            ptr++;
            index++;
        }

        return 1;

    }

    return 0;
}

u8 NB_parseRegistrationStatus(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+CEREG:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=7;
        while(ptr!=NULL && *ptr==' ') ptr++;


        ptr = (char*)strstr((const char*)ptr,(const char*)",");
        if(ptr==NULL) return 0;
        ptr++;

        nb.RegStatus=atoi(ptr);
        return 1;        
    }

    return 0;
}

//�ȴ�������Ϣ��+CSCON:X��
u8 NB_parseSignallingConnectionStatus(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+CSCON:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=7;
        while(ptr!=NULL && *ptr==' ') ptr++;

        nb.ConnectionStatus=atoi(ptr);
        return 1;
    }

    return 0;
}


u8 NB_parseNBand(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+NBAND:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=7;
        while(ptr!=NULL && *ptr==' ') ptr++;

        nb.nband=atoi(ptr);
        return 1;
    }

    return 0;
}


u8 NB_parseCheckAttach(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+CGATT:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=7;
        while(ptr!=NULL && *ptr==' ') ptr++;

        nb.attach=atoi(ptr);

        return 1;
    }

    return 0;
}

u8 NB_parseGetLocalIp(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+CGPADDR:");

    if(ptr!=NULL)
    {
        u8 index=0;
        ptr+=9;
        while(ptr!=NULL && *ptr==' ') ptr++;

        ptr = (char*)strstr((const char*)ptr,(const char*)",");
        if(ptr==NULL) return 0;
        ptr++;

        memset(nb.localIp, 0, sizeof(nb.localIp));
        while(index<20 && ptr!=NULL && ((*ptr>='0'&&*ptr<='9')||*ptr=='.'))
        {
            nb.localIp[index]=*ptr;
            ptr++;
            index++;
        }

        return 1;
    }

    return 0;
}

//��������
u8 NB_parseReceiveCommand(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+NSONMI:");

    if(ptr!=NULL)
    {
        u8 socket=0;
        u16 dataLen=0;
        u8 buff[20]={0};
        
        ptr+=8;
        socket=atoi(ptr);

        ptr = (char*)strstr((const char*)ptr,(const char*)",");
        if(ptr==NULL) return 0;
        ptr++;
        dataLen=atoi(ptr);

        printf("socket:%d, dataLen=%d.\r\n", socket, dataLen);

        NB_InsertSocketAndLen(socket, dataLen);
        
        return 1;
    }

    return 0;
}


//socket�����ر�
u8 NB_parseSocketClose(u8* cmd, u32 len)
{
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    ptr = (char*)strstr((const char*)cmd,(const char*)"+NSOCLI:");

    if(ptr!=NULL)
    {
        u8 type=0;
        u16 dataLen=0;
        u8 buff[20]={0};
        
        ptr+=8;
        while(ptr!=NULL && *ptr==' ') ptr++;
        type=atoi(ptr);

        printf("type=%d.\r\n", type);
        //if(2==type) NB_CloseSocket();

        NB_CloseSocket();

        
        return 1;
    }

    return 0;
}


//Ĭ�Ͻ������ص�����
//cmd:Ҫ����������
//len:�����
//����:�����Ѵ�����1��δ������0,
u8 NB_parseDefaultMsg(u8* cmd, u32 len)
{  
    char *ptr = NULL;
    if(cmd==NULL) return 0;

    if(NB_parseOperator( cmd, len)) return 1;
    if(NB_parseCsq( cmd, len)) return 1;
    if(NB_parseImei( cmd, len)) return 1;
    if(NB_parseImsi( cmd, len)) return 1;
    if(NB_parseRegistrationStatus( cmd, len)) return 1;
    if(NB_parseSignallingConnectionStatus( cmd, len)) return 1;
    if(NB_parseNBand( cmd, len)) return 1;
    if(NB_parseGetLocalIp( cmd, len)) return 1;
    if(NB_parseCheckAttach( cmd, len)) return 1;
    if(NB_parseReceiveCommand( cmd, len)) return 1;
    if(NB_parseSocketClose( cmd, len)) return 1;

    return 0;
}

void NB_CheckCops(void)
{
    u8 timeout=10;//����10��
    
    while(timeout-->0)
    {
        NB_SendAT("AT+COPS?\r\n",1);

        //�����ѯ�����˳�
        if(strlen(nb.gOperator)>0) break;

        NB_ms(3000);//��ʱ3000ms�ٲ�
    }
}

//��������
void NB_SetNetWork(void)
{
    u8 cmd[30]={0};
    u8 nband=N_BAND_MOBILE;//Ĭ�����ƶ���
    
    if(strlen(nb.imsi)>0)
    {
        if((char*)strstr((const char*)nb.imsi,(const char*)"46011")!=NULL)
        {
            //����IMSI�ţ���ѯ���ǵ��ſ�
            nband=N_BAND_TELECOM;
        }
    }
    
    NB_SendAT("AT+NBAND?\r\n",1);

    if(nband!=nb.nband)
    {
        sprintf(cmd, "AT+NBAND=%d\r\n", nband);

        //AT+NBAND=8 //�ƶ�Ϊ8������Ϊ5
        if(1==NB_SendAT(cmd,1))
        {
            nb.nband=nband;
        }
    }
}


void NB_CheckCSQ(void)
{
  NB_SendAT("AT+CSQ\r\n",1);
}

void NB_GetSn(void)
{
    u16 len=0;
    u8 tryCount=20;//����20��
    u8 timeout=0;//

    do
    {
        NB_SendAT("AT+CGSN\r\n",0);

        while(1)
        {
            NB_ms(50);
            len=NB_GetLine(gAtParseBuff);
            if(len>0)
            {
                if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
                {
                    break;//����
                }
                else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                    ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
                {
                    break;//����
                }
                else if(len==2 && (char*)strstr((const char*)gAtParseBuff,(const char*)"\r\n")!=NULL)
                {
                    continue;
                }
                else if(len==17)
                {
                    NB_parseSn(gAtParseBuff,len);
                }                
            }
        
            timeout++;
                
            if(timeout>100)
            {
                break;
            }
        }


        //�����ѯ�����˳�
        if(strlen(nb.sn)>0) break;

        NB_ms(1000);//��ʱ1000ms�ٲ�
    }while(tryCount-->0);
}

void NB_GetImei(void)
{
    u8 timeout=20;//����20��
    
    while(timeout-->0)
    {
        NB_SendAT("AT+CGSN=1\r\n",1);

        //�����ѯ�����˳�
        if(strlen(nb.imei)>0) break;

        NB_ms(1000);//��ʱ1000ms�ٲ�
    }
}

void NB_GetImsi(void)
{
    u8 timeout=20;//����20��
    
    while(timeout-->0)
    {
        NB_SendAT("AT+CIMI\r\n",1);

        //�����ѯ�����˳�
        if(strlen(nb.imsi)>0) break;

        NB_ms(1000);//��ʱ1000ms�ٲ�
    }
}

//����ע��״̬
void NB_GetNetworkRegistrationStatus(void)
{
  NB_SendAT("AT+CEREG?\r\n",1);
} 

//�����Ƿ�ɹ�
//����1��ʾ�ɹ� 
u8 NB_CheckAttach(void)
{
    NB_SendAT("AT+CGATT?\r\n",1);

    if(nb.attach==1)
    {
        NB_SendAT("AT+CGPADDR\r\n",1);//ȡIP
        return 1;
    }

    memset(nb.localIp, 0, sizeof(nb.localIp));
    return 0;
} 

//����1��ʾ����attack�ɹ�
u8 NB_Attach(void)
{
  int tryTime=10;

  if(NB_CheckAttach()==1) return 1;
  
  NB_SendAT("AT+CGDCONT=1,\"IP\",\"HUAWEI.COM\"\r\n",1);//����APN
  NB_SendAT("AT+CGATT=1\r\n",1);//PS Attach

  while(tryTime>0)
  {
    if(NB_CheckAttach()==0) return 1;
    NB_ms(500);
    tryTime--;
  }

  return 0;
}

//����socket
void NB_CreateSocket(u8* cmd)
{
    u16 len=0;
    u8 timeout=0;//

    if(cmd==NULL) return;

    NB_SendAT(cmd,0);//��������

    while(1)
    {
        NB_ms(50);
        len=NB_GetLine(gAtParseBuff);
        if(len>0)
        {
            if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
            {
                break;//����
            }
            else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
            {
                break;//����
            }
            else if(len==3 && gAtParseBuff[0]>='0' && gAtParseBuff[0]<='9')
            {
                nb.net.socket=gAtParseBuff[0]-0x30;
            }
        }

        timeout++;
            
        if(timeout>100)
        {
            break;
        }
    }

    NB_Printf("nb.net.socket=%d.\r\n", nb.net.socket);

}

void NB_CloseSocket(void)
{
    u8 cmd[50]={0};

    
    printf("NB_CloseSocket.\r\n");

    sprintf(cmd, "AT+NSOCL=%d\r\n",nb.net.socket);
    if(1==NB_SendAT(cmd,1))
    {
    }


    nb.net.socket=-1;
    nb.isConnect=0;
    nb.mqtt_state=MQTT_REQUEST_INIT;
}



//����������
//uint8 mode �������� 
//uint8* s_ip��������ip
//uint16 s_port �������˿�
void NB_CreateConnect(u8 mode, u8* s_ip, u16 s_port)
{
    u8 cmd[50]={0};


    //����socket
    if(mode==NB_TCP)
    {
        if(nb.net.socket==-1)
        {
            NB_CreateSocket("AT+NSOCR=STREAM,6,10000,1\r\n");
        }

        //socket�����ɹ��󣬿�ʼ���ӷ�����
        if(nb.net.socket>=0)
        {
            //����Զ�̷�����
            
            //AT+NSOCO=<socket>,<remote_addr>,<remote_port>
            sprintf(cmd, "AT+NSOCO=%d,%s,%d\r\n",nb.net.socket,s_ip,s_port);
            if(1==NB_SendAT(cmd,1))
            {
                //������ȷִ��
                nb.isConnect=1;
            }
            else
            {
                NB_CloseSocket();
            }
        }        
    }
    else if(mode==NB_UDP)
    {
        nb.net.socket=-1;
        nb.isConnect=0;
    
        NB_CreateSocket("AT+NSOCR=DGRAM,17,10000,1\r\n");

        //ͨ������socket���ж��Ƿ�������
        if(nb.net.socket>=0)
        {
            nb.isConnect=1;
        }
    }else return;


}

void Reboot(void)
{
  NB_SendAT("AT+NRB\r\n" , 0);
}

void NB_CycleCheck(void)
{
    NB_GetNetworkRegistrationStatus();
    NB_CheckCSQ();
    NB_Attach();
}

//16��������ת��ĸ
char NumberToLetter(u8 number)
{
	char buff[]="0123456789ABCDEF";

	if(number>15) return 0;

	return buff[number];

}


//ASCLL��ת��16�����ַ���
//һ��ASCLL���ת��2���ֽ�
//u8* ascll, Ҫת����ASCLL���ַ���
//u16 ascll_len, Ҫת����ASCLL�볤��
//u8* hex, ����16�����ַ����Ļ���
//u16 hex_len������16�����ַ�������ĳ���
//����hex����

u16 AscllToHex(u8* ascll, u16 ascll_len, u8* hex, u16 hex_len)
{
    u8 temp=0;
    int i=0;
    u16 len=0;

    if(ascll==NULL) return 0;
    if(hex==NULL) return 0;

    for(i=0; i<ascll_len && i<(hex_len/2); i++)
    {
        temp= ascll[i];
        hex[i*2]=NumberToLetter((temp>>4)&0x0f);
        hex[i*2+1]=NumberToLetter(temp&0x0f);
        len+=2;
    }

    return len;
}

//16��������ת��ĸ
u8 LetterToNumber(u8 c)
{
    int i=0;
	u8 buff[]="0123456789ABCDEF";

    for(i=0; i<16; i++)
    {
        if(buff[i]==c) return i;
    }

    return 0;
}


//16�����ַ���ת��ASCLL���ַ���
//2���ֽ�ת��һ��ASCLL��
//u8* ascll, Ҫת����ASCLL���ַ���
//u16 ascll_len, Ҫת����ASCLL�볤��
//u8* hex, ����16�����ַ����Ļ���
//u16 hex_len������16�����ַ�������ĳ���
//����ascll����
u16 HexToAscll(u8* ascll, u16 ascll_len, u8* hex, u16 hex_len)
{
    int i=0;
    u16 len=0;
    u8 h=0,l=0;
    

    if(ascll==NULL) return 0;
    if(hex==NULL) return 0;

    for(i=0; i<hex_len;)
    {
        h= hex[i];
        l= hex[i+1];
        ascll[len]=(LetterToNumber(h)<<4) + (LetterToNumber(l));
        i+=2;
        len++;

        //if(len>=ascll_len-1) break;
    }

    return len;
}



//����NB����
//u8* data  //Ҫ���͵�����
//u16 data  //Ҫ���ͳ���
//���ء�1:��ȷִ�С�0������ִ��
unsigned char NB_SendData(u8* data, u16 len)
{
    char cmd[100]={0};
    u16 L=0;

    NB_Printf((u8*)"NB_SendData:%s.\r\n",data);

    if(nb.isConnect==0)
    {
        NB_CreateConnect(nb.net.mode, nb.net.remote_ip, nb.net.remote_port);
    }

    NB_Printf((u8*)"Connect:%d,socket:%d.\r\n",nb.isConnect, nb.net.socket);

    if(nb.isConnect>0)
    {
        nb.net.len=0;
        memset(nb.net.data, 0, MAX_NB_TX_RX_LEN);
        
        //ASCLL��ת��16�����ַ���
        nb.net.len=AscllToHex(data, len, nb.net.data, MAX_NB_TX_RX_LEN);


        if(nb.net.mode==NB_TCP)
        {
            //�����ʽ:AT+NSOSD=<socket>,<length>,<data>[,<flag>[,sequence]]
            
            sprintf(cmd, "AT+NSOSD=%d,%d,",nb.net.socket, len);
        }
        else if(nb.net.mode==NB_UDP)
        {
            //�����ʽ:AT+NSOST=<socket>,<remote_addr>,<remote_port>,<length>,<data>[,<sequence>]
            sprintf(cmd, "AT+NSOST=%d,%s,%d,%d,",nb.net.socket,nb.net.remote_ip,nb.net.remote_port, len);
        }else return 0;

        //���ʹ�������
        NB_SendAT(cmd,0);
        NB_SendAT(nb.net.data,0);
        if(1==NB_SendAT("\r\n",1)) return 1;

    }

    return 0;
}

void NB_GetData(void)
{
    u16 timeout=0;
    char cmd[100]={0};
    u8 s=0;
    u16 len=0, l=0;
    

    while (1)
    {
        len=NB_GetLine(gAtParseBuff);
        
        if(len>0)
        {
            NB_parseDefaultMsg(gAtParseBuff, len);
        }
        else break;
    }
    
    if(NB_GetRxSocketAndLen(&s, &l)>0)
    {
        sprintf(cmd, "AT+NSORF=%d,%d\r\n", s, l);
        NB_SendAT(cmd,0);
    
        while(1)
        {
            NB_ms(50);
            len=NB_GetLine(gAtParseBuff);
            if(len>0)
            {
                if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
                {
                    break;//����
                }
                else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                    ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
                {
                    break;//����
                }
                else if(len==2 && (char*)strstr((const char*)gAtParseBuff,(const char*)"\r\n")!=NULL)
                {
                    continue;
                }
                else
                {
                    //�յ������ݸ�ʽ
                    //<socket>,<ip_addr>,<port>,<length>,<data>,<remaining_length>
                    //����<socket>��<length>Ҫ�ͷ��͵�һ��
                    //<data>���־���ʵ�ʵĽ�������
                    
                    u8 s1=0;
                    u16 len1=0;
                    u8* ptr = gAtParseBuff;
                    u8* ptr1 = NULL; 

                    s1=atoi(ptr);
                    ptr = (char*)strstr((const char*)ptr,(const char*)",");
                    if(ptr!=NULL)
                    {
                        ptr++;
                        ptr = (char*)strstr((const char*)ptr,(const char*)",");
                        if(ptr!=NULL)
                        {
                            ptr++;
                            
                            ptr = (char*)strstr((const char*)ptr,(const char*)",");
                            if(ptr!=NULL)
                            {
                                ptr++;
                                len1=atoi(ptr);

                                //socket�ͳ���һ����������������ݰ�
                                if(s1==s && len1==l)
                                {
                                    //�ҵ�<data>��ͷ�ĵط�
                                    ptr = (char*)strstr((const char*)ptr,(const char*)",");
                                    if(ptr!=NULL)
                                    {
                                        ptr++;
                                        //�ҵ�<data>��β�ĵط�
                                        ptr1 = (char*)strstr((const char*)ptr,(const char*)",");
                                        if(ptr1!=NULL)
                                        {
                                           //����<data>����
                                            len=ptr1-ptr;

                                            if(len>0)
                                            {
                                                nb.net.len=0;
                                                memset(nb.net.data, 0, MAX_NB_TX_RX_LEN);


                                                //��������
                                                nb.net.len=HexToAscll(nb.net.data, MAX_NB_TX_RX_LEN, ptr, len);

                                                printf("RX:%d,%s.\r\n", nb.net.len,nb.net.data);
                                                NB_RxData(nb.net.data, nb.net.len);
                                                
                                                continue;
                                            }
                                        }
                                    }

                                   
                                    
                                }
                            }
                            
                        }
                    }

                    //Ĭ�ϴ���
                    NB_parseDefaultMsg(gAtParseBuff, len);
                }
            }
        
            timeout++;
                
            if(timeout>100)
            {
                break;
            }
        }
    }
}



void NB_close(void)
{
  
}

void NB_init(u8 mode, u8* s_ip, u16 s_port)
{
    memset(&nb, 0, sizeof(NB_DATA));
    nb.isConnect=0;//Ĭ������δ����
    nb.net.socket=-1;
    nb.mqtt_state=MQTT_REQUEST_INIT;

    nb.net.mode=mode;
    sprintf(nb.net.remote_ip, s_ip);//������IP���ַ���
    nb.net.remote_port=s_port;//������PORT


    //ģ���ϵ�,
    NB_powerOn(1);
    
    NB_SendAT("ATE0\r\n",1);
    NB_SendAT("AT+CMEE=1\r\n",1);//�򿪴���֪ͨ
    NB_SendAT("AT+CEREG=1\r\n",1);
    NB_SendAT("AT+COPS=0\r\n",1);//�Զ�ע��
    NB_SendAT("AT+CSCON=1\r\n",1);//������״̬����֪ͨ
    NB_SendAT("AT+CGATT=1\r\n",1);//PS Attach
    NB_GetImsi();//IMSI OK
    NB_SetNetWork();//��������
    NB_CheckCops();//NB OK
    NB_CheckCSQ();//NB OK
    NB_GetSn(); //SN
    NB_GetImei();//IMEI OK
    NB_GetNetworkRegistrationStatus();
}


