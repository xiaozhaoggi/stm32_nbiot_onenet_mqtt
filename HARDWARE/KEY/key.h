#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//������ʼ���������ֱ����IO���ϣ�wake_up(key1):PA0,key2:PC0, key3:CP13��
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************



#define KEY1   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����1(WK_UP) 
#define KEY2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//��ȡ����2
#define KEY3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//��ȡ����3

 

#define KEY1_PRES  1	//KEY1����(��WK_UP/KEY_UP)
#define KEY2_PRES 	2	//KEY2����
#define KEY3_PRES	3	//KEY3����


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(void);  	//����ɨ�躯��					    
#endif
