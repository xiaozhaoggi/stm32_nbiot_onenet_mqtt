#ifndef _ONENET_H_
#define _ONENET_H_

#include "onenet_config.h"


u8 NB_RxData(u8* data, u16 len);


_Bool OneNet_DevLink(void);

void OneNet_SendData(u16 mq, unsigned char red_sence);

void OneNet_RevPro(unsigned char *cmd);


/**************************************************************
�������� : onenet_mqtt_send_heart
�������� : ��������
������� : ��
����ֵ  	 : ��
��ע		 : ��
**************************************************************/
void onenet_mqtt_send_heart(void);


/**************************************************************
�������� : onenet_mqtt_publish_topic
�������� : onenet mqtt ����������Ϣ
������� : topic������������
		   msg����Ϣ����
����ֵ  	 : PUBLISH_MSG_OK: ������Ϣ�ɹ�
		   PUBLISH_MSG_ERROR��������Ϣʧ��
��ע		 : ��
**************************************************************/
_Bool onenet_mqtt_publish_topic(const char *topic, const char *msg);


/**************************************************************
�������� : onenet_mqtt_subscribe_topic
�������� : onenet mqtt ��������
������� : topic�����ĵ�topic
		   topic_cnt��topic����
����ֵ  	 : SUBSCRIBE_TOPIC_OK:���ĳɹ�
		   SUBSCRIBE_TOPIC_ERROR������ʧ��
��ע		 : ��
**************************************************************/
_Bool onenet_mqtt_subscribe_topic(const signed char *topic[], unsigned char topic_cnt);

#endif
