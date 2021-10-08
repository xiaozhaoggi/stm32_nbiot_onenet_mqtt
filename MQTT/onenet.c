/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "nb.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "buzzer.h"

//C库
#include <string.h>
#include <stdio.h>


//NB接收到的onenet数据处理
u8 NB_RxData(u8* data, u16 len)
{
    if(NULL==data) return 0;

    if(nb.mqtt_state==MQTT_REQUEST_INIT)
    {
        //等待接入响应
        if(MQTT_UnPacketRecv(data) == MQTT_PKT_CONNACK)
        {
            switch(MQTT_UnPacketConnectAck(data))
            {
                case 0:NB_Printf("Tips: 连接成功\r\n");nb.mqtt_state=MQTT_REQUEST_IDLE; break;
                
                case 1:NB_Printf("WARN: 连接失败：协议错误\r\n");break;
                case 2:NB_Printf("WARN: 连接失败：非法的clientid\r\n");break;
                case 3:NB_Printf("WARN: 连接失败：服务器失败\r\n");break;
                case 4:NB_Printf("WARN: 连接失败：用户名或密码错误\r\n");break;
                case 5:NB_Printf("WARN: 连接失败：非法链接(比如token非法)\r\n");break;
                
                default:NB_Printf("ERR: 连接失败：未知错误\r\n");break;
            }
        }
    }
    else
    {
        OneNet_RevPro(data);
    }

    return 0;
}



//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	0-成功	1-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;

   
    //先创建TCP连接
    if(nb.isConnect==0)
    {
        NB_CreateConnect(nb.net.mode, nb.net.remote_ip, nb.net.remote_port);
    }

	NB_Printf( "OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);

    //接入onenet
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		NB_SendData(mqttPacket._data, mqttPacket._len);			//上传平台

        nb.mqtt_state = MQTT_REQUEST_INIT;//改变状态
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		NB_Printf("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

//温度的内容
unsigned char OneNet_FillBuf(char *buf,u16 mq, unsigned char red_sence)
{
	
	char text[50];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");

    //温度
	memset(text, 0, sizeof(text));
	sprintf(text, "\"MQ2\":%d", mq);
	strcat(buf, text);


    //led1
	memset(text, 0, sizeof(text));
	strcat(buf, ",");
	sprintf(text, "\"redSence\":%c", red_sence);
	strcat(buf, text);


	strcat(buf, "}");
	memset(text, 0, sizeof(text));
	return strlen(buf);

}


//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(u16 mq, unsigned char red_sence)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short body_len = 0, i = 0;
    	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf, mq, red_sence);									//获取当前需要发送的数据流的总长度
    
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			NB_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			NB_Printf("Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			NB_Printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;

	unsigned char qos = 0;
	static unsigned short pkt_id = 0;

    
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10]={0};
	int num = 0;

        
	type = MQTT_UnPacketRecv(cmd);

    NB_Printf("OneNet_RevPro type=%d.\r\n",type);

    
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发

			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				NB_Printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					NB_Printf("Tips:	Send CmdResp\r\n");
					
					NB_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				NB_Printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;

        case MQTT_PKT_PINGRESP:
        {
            NB_Printf("Tips:  HeartBeat OK\r\n");
            break;
        }
           
        case MQTT_PKT_PUBLISH:  //接收的Publish消息
        {
            if(MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id) == 0)
            {
                NB_Printf("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",cmdid_topic, topic_len, req_payload, req_len);
                
                //执行命令回调------------------------------------------------------------
                //CALLBACK_Execute(req_payload);
                
                switch(qos)
                {
                    case 1:                                                         //收到publish的qos为1，设备需要回复Ack
                    {
                        if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
                        {
                            NB_Printf("Tips:  Send PublishAck\r\n");
                            NB_SendData(mqttPacket._data, mqttPacket._len);
                            MQTT_DeleteBuffer(&mqttPacket);
                        }
                        break;
                    }
                    case 2: //收到publish的qos为2，设备先回复Rec
                    {       //平台回复Rel，设备再回复Comp
                        if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
                        {
                            NB_Printf("Tips:  Send PublishRec\r\n");
                            NB_SendData(mqttPacket._data, mqttPacket._len);
                            MQTT_DeleteBuffer(&mqttPacket);
                        }
                        break;
                    }
                    default:
                        break;
                }
                
            }
            break;
        }
        case MQTT_PKT_PUBREC:   //发送Publish消息，平台回复的Rec，设备需回复Rel消息
        {
            if(MQTT_UnPacketPublishRec(cmd) == 0)
            {
                NB_Printf("Tips:  Rev PublishRec\r\n");
                if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
                {
                    NB_Printf("Tips:  Send PublishRel\r\n");
                    NB_SendData(mqttPacket._data, mqttPacket._len);
                    MQTT_DeleteBuffer(&mqttPacket);
                }
            }
            break;
        }   
        case MQTT_PKT_PUBREL://收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
        {
            if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
            {
                NB_Printf("Tips:  Rev PublishRel\r\n");
                if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
                {
                    NB_Printf("Tips:  Send PublishComp\r\n");
                    NB_SendData(mqttPacket._data, mqttPacket._len);
                    MQTT_DeleteBuffer(&mqttPacket);
                }
            }
            break;
        }
        case MQTT_PKT_PUBCOMP:  //发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
        {
            if(MQTT_UnPacketPublishComp(cmd) == 0)
            {
                NB_Printf("Tips:  Rev PublishComp\r\n");
            }
            break;
        }
        case MQTT_PKT_SUBACK:   //发送Subscribe消息的Ack
        {
            if(MQTT_UnPacketSubscribe(cmd) == 0)
            {
                NB_Printf("Tips:  MQTT Subscribe OK\r\n");
            }
            else
            {
                NB_Printf("Tips:  MQTT Subscribe Err\r\n");
            }
            break;
        }
        case MQTT_PKT_UNSUBACK: //发送UnSubscribe消息的Ack
        {
            if(MQTT_UnPacketUnSubscribe(cmd) == 0)
            {
                NB_Printf("Tips:  MQTT UnSubscribe OK\r\n");
            }
            else
            {
                NB_Printf("Tips:  MQTT UnSubscribe Err\r\n");
            }
            break;
        } 		
		default:
			result = -1;
		break;
	}
		
	if(result == -1)
		return;

    
	dataPtr = strchr(req_payload, ':');					//搜索':'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;

        num = 0;
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式

		if(strstr((char *)req_payload, "led1"))		//搜索"led1"
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				LED1_ON;
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				LED1_OFF;
			}
		}
														//下同
		else if(strstr((char *)req_payload, "led2"))
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				LED2_ON;
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				LED2_OFF;
			}
		}
		else if(strstr((char *)req_payload, "led3"))
		{		
			if(num == 1)								//控制数据如果为1，代表开
			{
				LED3_ON;
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				LED3_OFF;
			}
		}
		else if(strstr((char *)req_payload, "buzzer"))
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				BUZZER_ON;
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				BUZZER_OFF;
			}
		}
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}

/**************************************************************
函数名称 : onenet_mqtt_send_heart
函数功能 : 发送心跳
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void onenet_mqtt_send_heart(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	NB_Printf("send heart\r\n");
	MQTT_PacketPing(&mqttPacket);
    NB_SendData(mqttPacket._data, mqttPacket._len);
	MQTT_DeleteBuffer(&mqttPacket);									//删包
}


/**************************************************************
函数名称 : onenet_mqtt_publish_topic
函数功能 : onenet mqtt 发布主题消息
输入参数 : topic：发布的主题
		   msg：消息内容
返回值  	 : PUBLISH_MSG_OK: 发布消息成功
		   PUBLISH_MSG_ERROR：发布消息失败
备注		 : 无
**************************************************************/
_Bool onenet_mqtt_publish_topic(const char *topic, const char *msg)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	NB_Printf("publish topic: %s, msg: %s\r\n", topic, msg);

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL2, 0, 1, &mqttPacket) == 0)
	{
		NB_SendData(mqttPacket._data, mqttPacket._len);//上传平台
		MQTT_DeleteBuffer(&mqttPacket);	//删包
		return 0;
	}
	else
	{
		return 1;
	}

}

/*  发布主题为"hello_topic"，消息为"hello_world" */
//if(0 == onenet_mqtt_publish_topic("hello_topic", "hello_world"))
//{;}

 
/**************************************************************
函数名称 : onenet_mqtt_subscribe_topic
函数功能 : onenet mqtt 订阅主题
输入参数 : topic：订阅的topic
		   topic_cnt：topic个数
返回值  	 : SUBSCRIBE_TOPIC_OK:订阅成功
		   SUBSCRIBE_TOPIC_ERROR：订阅失败
备注		 : 无
**************************************************************/
_Bool onenet_mqtt_subscribe_topic(const signed char *topic[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
 
	for(; i < topic_cnt; i++)
	{
		NB_Printf("subscribe topic: %s\r\n", topic[i]);
	}
    
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topic, topic_cnt, &mqttPacket) == 0)
	{
		NB_SendData(mqttPacket._data, mqttPacket._len);//上传平台
		MQTT_DeleteBuffer(&mqttPacket);											//删包
		return 0;
	}
	else
	{
		return 1;
	}
}

//主题订阅方法
//const signed char *g_mqtt_topics[] = {"mqtt_topic", "topic_test"};
//if(0 == onenet_mqtt_subscribe_topic(g_mqtt_topics, 2))



