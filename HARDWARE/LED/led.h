#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//LED��ͷ�ļ���LED1:PA8��LED2��PD2��LED3��PB12
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


#define LED1 PAout(8)// PA8
#define LED2 PDout(2)// PD2	
#define LED3 PBout(12)// PB12	

#define LED1_ON (LED1=0)//��LED1
#define LED1_OFF (LED1=1)//�ر�LED1
#define LED2_ON (LED2=0)//��LED2
#define LED2_OFF (LED2=1)//�ر�LED2
#define LED3_ON (LED3=0)//��LED3
#define LED3_OFF (LED3=1)//�ر�LED3


void LED_Init(void);//��ʼ��
void led1On(int on);
void led2On(int on);
void led3On(int on);


		 				    
#endif
