/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "nb.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "buzzer.h"

//C��
#include <string.h>
#include <stdio.h>


//NB���յ���onenet���ݴ���
u8 NB_RxData(u8* data, u16 len)
{
    if(NULL==data) return 0;

    if(nb.mqtt_state==MQTT_REQUEST_INIT)
    {
        //�ȴ�������Ӧ
        if(MQTT_UnPacketRecv(data) == MQTT_PKT_CONNACK)
        {
            switch(MQTT_UnPacketConnectAck(data))
            {
                case 0:NB_Printf("Tips: ���ӳɹ�\r\n");nb.mqtt_state=MQTT_REQUEST_IDLE; break;
                
                case 1:NB_Printf("WARN: ����ʧ�ܣ�Э�����\r\n");break;
                case 2:NB_Printf("WARN: ����ʧ�ܣ��Ƿ���clientid\r\n");break;
                case 3:NB_Printf("WARN: ����ʧ�ܣ�������ʧ��\r\n");break;
                case 4:NB_Printf("WARN: ����ʧ�ܣ��û������������\r\n");break;
                case 5:NB_Printf("WARN: ����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
                
                default:NB_Printf("ERR: ����ʧ�ܣ�δ֪����\r\n");break;
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
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;

   
    //�ȴ���TCP����
    if(nb.isConnect==0)
    {
        NB_CreateConnect(nb.net.mode, nb.net.remote_ip, nb.net.remote_port);
    }

	NB_Printf( "OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);

    //����onenet
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		NB_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨

        nb.mqtt_state = MQTT_REQUEST_INIT;//�ı�״̬
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		NB_Printf("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

//�¶ȵ�����
unsigned char OneNet_FillBuf(char *buf,u16 mq, unsigned char red_sence)
{
	
	char text[50];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");

    //�¶�
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
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(u16 mq, unsigned char red_sence)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
    	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf, mq, red_sence);									//��ȡ��ǰ��Ҫ���͵����������ܳ���
    
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			NB_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			NB_Printf("Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			NB_Printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
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
		case MQTT_PKT_CMD:															//�����·�

			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				NB_Printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					NB_Printf("Tips:	Send CmdResp\r\n");
					
					NB_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				NB_Printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;

        case MQTT_PKT_PINGRESP:
        {
            NB_Printf("Tips:  HeartBeat OK\r\n");
            break;
        }
           
        case MQTT_PKT_PUBLISH:  //���յ�Publish��Ϣ
        {
            if(MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id) == 0)
            {
                NB_Printf("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",cmdid_topic, topic_len, req_payload, req_len);
                
                //ִ������ص�------------------------------------------------------------
                //CALLBACK_Execute(req_payload);
                
                switch(qos)
                {
                    case 1:                                                         //�յ�publish��qosΪ1���豸��Ҫ�ظ�Ack
                    {
                        if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
                        {
                            NB_Printf("Tips:  Send PublishAck\r\n");
                            NB_SendData(mqttPacket._data, mqttPacket._len);
                            MQTT_DeleteBuffer(&mqttPacket);
                        }
                        break;
                    }
                    case 2: //�յ�publish��qosΪ2���豸�Ȼظ�Rec
                    {       //ƽ̨�ظ�Rel���豸�ٻظ�Comp
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
        case MQTT_PKT_PUBREC:   //����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
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
        case MQTT_PKT_PUBREL://�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
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
        case MQTT_PKT_PUBCOMP:  //����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp
        {
            if(MQTT_UnPacketPublishComp(cmd) == 0)
            {
                NB_Printf("Tips:  Rev PublishComp\r\n");
            }
            break;
        }
        case MQTT_PKT_SUBACK:   //����Subscribe��Ϣ��Ack
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
        case MQTT_PKT_UNSUBACK: //����UnSubscribe��Ϣ��Ack
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

    
	dataPtr = strchr(req_payload, ':');					//����':'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;

        num = 0;
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ

		if(strstr((char *)req_payload, "led1"))		//����"led1"
		{
			if(num == 1)								//�����������Ϊ1������
			{
				LED1_ON;
			}
			else if(num == 0)							//�����������Ϊ0�������
			{
				LED1_OFF;
			}
		}
														//��ͬ
		else if(strstr((char *)req_payload, "led2"))
		{
			if(num == 1)								//�����������Ϊ1������
			{
				LED2_ON;
			}
			else if(num == 0)							//�����������Ϊ0�������
			{
				LED2_OFF;
			}
		}
		else if(strstr((char *)req_payload, "led3"))
		{		
			if(num == 1)								//�����������Ϊ1������
			{
				LED3_ON;
			}
			else if(num == 0)							//�����������Ϊ0�������
			{
				LED3_OFF;
			}
		}
		else if(strstr((char *)req_payload, "buzzer"))
		{
			if(num == 1)								//�����������Ϊ1������
			{
				BUZZER_ON;
			}
			else if(num == 0)							//�����������Ϊ0�������
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
�������� : onenet_mqtt_send_heart
�������� : ��������
������� : ��
����ֵ  	 : ��
��ע		 : ��
**************************************************************/
void onenet_mqtt_send_heart(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	NB_Printf("send heart\r\n");
	MQTT_PacketPing(&mqttPacket);
    NB_SendData(mqttPacket._data, mqttPacket._len);
	MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
}


/**************************************************************
�������� : onenet_mqtt_publish_topic
�������� : onenet mqtt ����������Ϣ
������� : topic������������
		   msg����Ϣ����
����ֵ  	 : PUBLISH_MSG_OK: ������Ϣ�ɹ�
		   PUBLISH_MSG_ERROR��������Ϣʧ��
��ע		 : ��
**************************************************************/
_Bool onenet_mqtt_publish_topic(const char *topic, const char *msg)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	NB_Printf("publish topic: %s, msg: %s\r\n", topic, msg);

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL2, 0, 1, &mqttPacket) == 0)
	{
		NB_SendData(mqttPacket._data, mqttPacket._len);//�ϴ�ƽ̨
		MQTT_DeleteBuffer(&mqttPacket);	//ɾ��
		return 0;
	}
	else
	{
		return 1;
	}

}

/*  ��������Ϊ"hello_topic"����ϢΪ"hello_world" */
//if(0 == onenet_mqtt_publish_topic("hello_topic", "hello_world"))
//{;}

 
/**************************************************************
�������� : onenet_mqtt_subscribe_topic
�������� : onenet mqtt ��������
������� : topic�����ĵ�topic
		   topic_cnt��topic����
����ֵ  	 : SUBSCRIBE_TOPIC_OK:���ĳɹ�
		   SUBSCRIBE_TOPIC_ERROR������ʧ��
��ע		 : ��
**************************************************************/
_Bool onenet_mqtt_subscribe_topic(const signed char *topic[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
 
	for(; i < topic_cnt; i++)
	{
		NB_Printf("subscribe topic: %s\r\n", topic[i]);
	}
    
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topic, topic_cnt, &mqttPacket) == 0)
	{
		NB_SendData(mqttPacket._data, mqttPacket._len);//�ϴ�ƽ̨
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
		return 0;
	}
	else
	{
		return 1;
	}
}

//���ⶩ�ķ���
//const signed char *g_mqtt_topics[] = {"mqtt_topic", "topic_test"};
//if(0 == onenet_mqtt_subscribe_topic(g_mqtt_topics, 2))



