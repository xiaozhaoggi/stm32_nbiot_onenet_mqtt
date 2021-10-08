#ifndef _ONENET_H_
#define _ONENET_H_

#include "onenet_config.h"


u8 NB_RxData(u8* data, u16 len);


_Bool OneNet_DevLink(void);

void OneNet_SendData(u16 mq, unsigned char red_sence);

void OneNet_RevPro(unsigned char *cmd);


/**************************************************************
函数名称 : onenet_mqtt_send_heart
函数功能 : 发送心跳
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void onenet_mqtt_send_heart(void);


/**************************************************************
函数名称 : onenet_mqtt_publish_topic
函数功能 : onenet mqtt 发布主题消息
输入参数 : topic：发布的主题
		   msg：消息内容
返回值  	 : PUBLISH_MSG_OK: 发布消息成功
		   PUBLISH_MSG_ERROR：发布消息失败
备注		 : 无
**************************************************************/
_Bool onenet_mqtt_publish_topic(const char *topic, const char *msg);


/**************************************************************
函数名称 : onenet_mqtt_subscribe_topic
函数功能 : onenet mqtt 订阅主题
输入参数 : topic：订阅的topic
		   topic_cnt：topic个数
返回值  	 : SUBSCRIBE_TOPIC_OK:订阅成功
		   SUBSCRIBE_TOPIC_ERROR：订阅失败
备注		 : 无
**************************************************************/
_Bool onenet_mqtt_subscribe_topic(const signed char *topic[], unsigned char topic_cnt);

#endif
