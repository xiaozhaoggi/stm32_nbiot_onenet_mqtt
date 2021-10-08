#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//����2ͷ�ļ�
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


#define	MAX485_TX 		1//485���Ϳ���
#define	MAX485_RX 		0//���տ���
#define	MAX485_RX_TX 		PBout(1)  //MAX485���ͽ�������
	  	
void uart2_init(u32 bound);
void uart2_485_init(void);
void USART2_send_char(u8 temp);
void USART2_send_buff(u8* buf,u32 len);
void Set485Mode(u8 mode);////mode:1:���͡���0������


#endif


