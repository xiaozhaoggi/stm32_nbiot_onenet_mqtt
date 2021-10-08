#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//����1ͷ�ļ�
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************

	  	
void uart1_init(u32 bound);
void uart1_send_buff(u8* buf,u32 len);
void uart1_send_char(u8 temp);
void uart1_send_message(char *s);

#endif


