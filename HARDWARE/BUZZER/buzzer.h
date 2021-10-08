#ifndef __BUZZER_H
#define __BUZZER_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//���������ơ�ʹ��PC1���ƣ��͵�ƽ����
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


#define BUZZER PCout(1)// PC1	

#define BUZZER_ON (BUZZER=0)//��
#define BUZZER_OFF (BUZZER=1)//�ر�


void Buzzer_Init(void);//��ʼ��
void Buzzer_on(int on);
void Buzzer_flash(void);

		 				    
#endif
