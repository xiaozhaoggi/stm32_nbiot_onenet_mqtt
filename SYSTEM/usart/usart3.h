#ifndef __USART3_H
#define __USART3_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sys.h" 
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//����3ͷ�ļ�
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


//����봮���жϽ��գ��벻Ҫע�����º궨��
extern void uart3_init(u32 bound);
extern void USART3_send_buff(u8 buf[],u32 len);
extern void USART3_send_char(u8 temp);
#endif


