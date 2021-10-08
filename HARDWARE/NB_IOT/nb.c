#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "nb.h"
#include "led.h"
#include "Lcd_Driver.h"
#include "GUI.h"


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//ESP8266　AT命令交互实现，可以修改WiFi为AP模式，STA模式，可以让WiFi工作于TCP/UDP模式
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************




char gAtParseBuff[MAX_AT_PARSE_LEN]={0};
NB_DATA nb;

void NB_CloseSocket(void);


//NB调试信息的输出
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

//延时
void NB_ms(u16 ms)
{
  delay_ms(ms);
}

//换一行，用于插入新接收的字符串
void changeLine(void)
{
  u8 index=0;
  
  nb.parseAt.insertCurCount=0;
  nb.parseAt.insertIdx++;
  if(nb.parseAt.insertIdx>=MAX_AT_PARSE_LINE)
  {
    nb.parseAt.insertIdx=0;
  }
  
  //清空存在位置
  index=nb.parseAt.insertIdx;
  memset(nb.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
  nb.parseAt.count[index]=0;
}

//串口2收到一个字符，插入到插入到数组里，
//根据收到的数据是否是\r或者\n切换到下一行
void AddUartChar(unsigned char c)
{
  u8 index=nb.parseAt.insertIdx;//插入的行
  u16 count=nb.parseAt.count[index];//插入的位置
  static u8 lastCH=0;
  //if(c=='\r' || c=='\n')
  if(0)
  {
    if(count>0)//接收到的字符大于0才能换
    {
      changeLine();//进入下一行，等待插入
    }
  }
  else
  {
    if(count>=MAX_AT_PARSE_LEN)
    {
      changeLine();//进入下一行，等待插入
    }
    else
    {
      nb.parseAt.line[index][count]=c;
      nb.parseAt.insertCurCount++;
      nb.parseAt.count[index]+=1;

      //如果正在发送短消息，收到>号就换行
      if(count==0&&c=='>')
      {
        changeLine();//进入下一行，等待插入
      }
      else if(lastCH=='\r'&& c=='\n')
      {
        changeLine();//进入下一行，等待插入
      }
    }
  }

  lastCH=c;//记录最后一个字符

}

//取出一行串口3数据
//<buff> 接收数据的缓冲
//返回：接收到的数据长度，0表示没有数据接收
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
            changeLine();//读3行，行数和字数都不变，就主动换行
            NB_Printf("*******changeLine*******\r\n");
        }
          
        return 0;
    }
    else
    {
        u16 len=0;
        u8 index=nb.parseAt.parseIdx;//读取的行

        //读取行的内容

        len=nb.parseAt.count[index];
        memcpy(buff, nb.parseAt.line[index], len);


        NB_Printf("[%d],[%d],%s",nb.parseAt.parseIdx,len,nb.parseAt.line[index]);

        memset(nb.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
        nb.parseAt.count[index]=0;

        //变成下一索引
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

//取出一行串口3数据
//u8*　socket
//len　长度
//
void NB_InsertSocketAndLen(u8 socket, u16 len)
{
    u8 index=nb.rx.insertIdx;//插入的行

    nb.rx.socket[index]=socket;
    nb.rx.len[index]=len;

    nb.rx.insertIdx++;
    if(nb.rx.insertIdx>=MAX_RX_DATA_LEN)
    {
        nb.rx.insertIdx=0;
    }
}



//取出一行串口3数据
//u8*　socket
//len　长度
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
        u8 index=nb.rx.parseIdx;//读取的行


        NB_Printf("GetRX, [%d],%d,%d.\r\n",index, nb.rx.socket[index],nb.rx.len[index]);        

        *socket=nb.rx.socket[index];
        *len=nb.rx.len[index];

        nb.rx.socket[index]=0;
        nb.rx.len[index]=0;

        //变成下一索引
        nb.rx.parseIdx++;
        if(nb.rx.parseIdx>=MAX_RX_DATA_LEN)
        {
            nb.rx.parseIdx=0;
        }
    }

    return 1;
}


//NB模块开机上电
//PB0开机脚
//on大于0开机，等于0关机
void NB_powerOn(u8 on)
{
    GPIO_InitTypeDef  GPIO_InitStructure;


    //如果未开机, 拉高IO口控制模块开机
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能PB时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         //PB0 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);          //根据设定参数初始化GPIOB0

    //先断电再上电
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);             //PB0输出低,拉低EN脚
    if(on>0)
    {
        NB_ms(500);
        GPIO_SetBits(GPIOB,GPIO_Pin_0);             //PB0输出高,拉高EN脚
    }
    NB_Printf("\r\npower\r\n");
}

//写一行数据到串口3
//<cmd> 发送的命令
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


//写一行数据到串口3
//<cmd> 发送的命令
void writeAtCmd(u8* cmd)
{
  u16 len=0;
  if(cmd==NULL) return;

  len=strlen((const char*)cmd);
  
  USART3_send_buff(cmd,len);

  NB_Printf("AT>>>:%s\r\n", cmd);
}

//cmd:要发送的命令
//wait:1：等待结果　0:不等待，写完串口直接返回
//返回:超时返回2，命令正确执行返回1,命令错误执行返回3，其它0,
u8 NB_SendAT(char* cmd, u8 wait)
{
    u32 len;
    u8 ok_error=0;
    u8 timeout=0;

    if(cmd==NULL) return 0;

    //写串口
    writeAtCmd((u8*)cmd);

    //不等待，写完串口直接返回
    if(0==wait)
    {
        return 0;
    }

    //等待结果
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
                ok_error=1;//正常
            }
            else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
            {
                ok_error=3;//错误
            }
            else
            {
                //解析数据
                NB_parseDefaultMsg(gAtParseBuff,len);        
            }
        }

        timeout++;

        if(ok_error>0) break;

        if(timeout>200)
        {
            ok_error=2;//超时
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

//等待主动消息“+CSCON:X”
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

//接收数据
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


//socket主动关闭
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


//默认解析返回的数据
//cmd:要解析的命令
//len:命令长度
//返回:命令已处理返回1，未处理返回0,
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
    u8 timeout=10;//最多查10次
    
    while(timeout-->0)
    {
        NB_SendAT("AT+COPS?\r\n",1);

        //如果查询到就退出
        if(strlen(nb.gOperator)>0) break;

        NB_ms(3000);//延时3000ms再查
    }
}

//设置网络
void NB_SetNetWork(void)
{
    u8 cmd[30]={0};
    u8 nband=N_BAND_MOBILE;//默认是移动卡
    
    if(strlen(nb.imsi)>0)
    {
        if((char*)strstr((const char*)nb.imsi,(const char*)"46011")!=NULL)
        {
            //根据IMSI号，查询到是电信卡
            nband=N_BAND_TELECOM;
        }
    }
    
    NB_SendAT("AT+NBAND?\r\n",1);

    if(nband!=nb.nband)
    {
        sprintf(cmd, "AT+NBAND=%d\r\n", nband);

        //AT+NBAND=8 //移动为8，电信为5
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
    u8 tryCount=20;//最多查20次
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
                    break;//正常
                }
                else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                    ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
                {
                    break;//错误
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


        //如果查询到就退出
        if(strlen(nb.sn)>0) break;

        NB_ms(1000);//延时1000ms再查
    }while(tryCount-->0);
}

void NB_GetImei(void)
{
    u8 timeout=20;//最多查20次
    
    while(timeout-->0)
    {
        NB_SendAT("AT+CGSN=1\r\n",1);

        //如果查询到就退出
        if(strlen(nb.imei)>0) break;

        NB_ms(1000);//延时1000ms再查
    }
}

void NB_GetImsi(void)
{
    u8 timeout=20;//最多查20次
    
    while(timeout-->0)
    {
        NB_SendAT("AT+CIMI\r\n",1);

        //如果查询到就退出
        if(strlen(nb.imsi)>0) break;

        NB_ms(1000);//延时1000ms再查
    }
}

//网络注册状态
void NB_GetNetworkRegistrationStatus(void)
{
  NB_SendAT("AT+CEREG?\r\n",1);
} 

//附着是否成功
//返回1表示成功 
u8 NB_CheckAttach(void)
{
    NB_SendAT("AT+CGATT?\r\n",1);

    if(nb.attach==1)
    {
        NB_SendAT("AT+CGPADDR\r\n",1);//取IP
        return 1;
    }

    memset(nb.localIp, 0, sizeof(nb.localIp));
    return 0;
} 

//返回1表示附着attack成功
u8 NB_Attach(void)
{
  int tryTime=10;

  if(NB_CheckAttach()==1) return 1;
  
  NB_SendAT("AT+CGDCONT=1,\"IP\",\"HUAWEI.COM\"\r\n",1);//设置APN
  NB_SendAT("AT+CGATT=1\r\n",1);//PS Attach

  while(tryTime>0)
  {
    if(NB_CheckAttach()==0) return 1;
    NB_ms(500);
    tryTime--;
  }

  return 0;
}

//创建socket
void NB_CreateSocket(u8* cmd)
{
    u16 len=0;
    u8 timeout=0;//

    if(cmd==NULL) return;

    NB_SendAT(cmd,0);//发送命令

    while(1)
    {
        NB_ms(50);
        len=NB_GetLine(gAtParseBuff);
        if(len>0)
        {
            if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
            {
                break;//正常
            }
            else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
            {
                break;//错误
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



//打开数据连接
//uint8 mode 连接类型 
//uint8* s_ip　服务器ip
//uint16 s_port 服务器端口
void NB_CreateConnect(u8 mode, u8* s_ip, u16 s_port)
{
    u8 cmd[50]={0};


    //创建socket
    if(mode==NB_TCP)
    {
        if(nb.net.socket==-1)
        {
            NB_CreateSocket("AT+NSOCR=STREAM,6,10000,1\r\n");
        }

        //socket创建成功后，开始连接服务器
        if(nb.net.socket>=0)
        {
            //连接远程服务器
            
            //AT+NSOCO=<socket>,<remote_addr>,<remote_port>
            sprintf(cmd, "AT+NSOCO=%d,%s,%d\r\n",nb.net.socket,s_ip,s_port);
            if(1==NB_SendAT(cmd,1))
            {
                //命令正确执行
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

        //通过创建socket来判断是否有连接
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

//16进制数字转字母
char NumberToLetter(u8 number)
{
	char buff[]="0123456789ABCDEF";

	if(number>15) return 0;

	return buff[number];

}


//ASCLL码转成16进制字符串
//一个ASCLL码会转成2个字节
//u8* ascll, 要转换的ASCLL码字符串
//u16 ascll_len, 要转换的ASCLL码长度
//u8* hex, 保存16进制字符串的缓冲
//u16 hex_len，保存16进制字符串缓冲的长度
//返回hex长度

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

//16进制数字转字母
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


//16进制字符串转成ASCLL码字符串
//2个字节转成一个ASCLL码
//u8* ascll, 要转换的ASCLL码字符串
//u16 ascll_len, 要转换的ASCLL码长度
//u8* hex, 保存16进制字符串的缓冲
//u16 hex_len，保存16进制字符串缓冲的长度
//返回ascll长度
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



//发送NB数据
//u8* data  //要发送的内容
//u16 data  //要发送长度
//返回　1:正确执行　0：错误执行
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
        
        //ASCLL码转成16进制字符串
        nb.net.len=AscllToHex(data, len, nb.net.data, MAX_NB_TX_RX_LEN);


        if(nb.net.mode==NB_TCP)
        {
            //命令格式:AT+NSOSD=<socket>,<length>,<data>[,<flag>[,sequence]]
            
            sprintf(cmd, "AT+NSOSD=%d,%d,",nb.net.socket, len);
        }
        else if(nb.net.mode==NB_UDP)
        {
            //命令格式:AT+NSOST=<socket>,<remote_addr>,<remote_port>,<length>,<data>[,<sequence>]
            sprintf(cmd, "AT+NSOST=%d,%s,%d,%d,",nb.net.socket,nb.net.remote_ip,nb.net.remote_port, len);
        }else return 0;

        //发送串口命令
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
                    break;//正常
                }
                else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
                    ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
                {
                    break;//错误
                }
                else if(len==2 && (char*)strstr((const char*)gAtParseBuff,(const char*)"\r\n")!=NULL)
                {
                    continue;
                }
                else
                {
                    //收到的数据格式
                    //<socket>,<ip_addr>,<port>,<length>,<data>,<remaining_length>
                    //其中<socket>和<length>要和发送的一样
                    //<data>部分就是实际的接收内容
                    
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

                                //socket和长度一样，就是请求的数据包
                                if(s1==s && len1==l)
                                {
                                    //找到<data>开头的地方
                                    ptr = (char*)strstr((const char*)ptr,(const char*)",");
                                    if(ptr!=NULL)
                                    {
                                        ptr++;
                                        //找到<data>结尾的地方
                                        ptr1 = (char*)strstr((const char*)ptr,(const char*)",");
                                        if(ptr1!=NULL)
                                        {
                                           //计算<data>长度
                                            len=ptr1-ptr;

                                            if(len>0)
                                            {
                                                nb.net.len=0;
                                                memset(nb.net.data, 0, MAX_NB_TX_RX_LEN);


                                                //解析数据
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

                    //默认处理
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
    nb.isConnect=0;//默认数据未连接
    nb.net.socket=-1;
    nb.mqtt_state=MQTT_REQUEST_INIT;

    nb.net.mode=mode;
    sprintf(nb.net.remote_ip, s_ip);//服务器IP，字符串
    nb.net.remote_port=s_port;//服务器PORT


    //模块上电,
    NB_powerOn(1);
    
    NB_SendAT("ATE0\r\n",1);
    NB_SendAT("AT+CMEE=1\r\n",1);//打开错误通知
    NB_SendAT("AT+CEREG=1\r\n",1);
    NB_SendAT("AT+COPS=0\r\n",1);//自动注册
    NB_SendAT("AT+CSCON=1\r\n",1);//打开连接状态主动通知
    NB_SendAT("AT+CGATT=1\r\n",1);//PS Attach
    NB_GetImsi();//IMSI OK
    NB_SetNetWork();//设置网络
    NB_CheckCops();//NB OK
    NB_CheckCSQ();//NB OK
    NB_GetSn(); //SN
    NB_GetImei();//IMEI OK
    NB_GetNetworkRegistrationStatus();
}


