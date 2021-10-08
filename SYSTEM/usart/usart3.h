#ifndef __USART3_H
#define __USART3_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sys.h" 
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//串口3头文件
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


//如果想串口中断接收，请不要注释以下宏定义
extern void uart3_init(u32 bound);
extern void USART3_send_buff(u8 buf[],u32 len);
extern void USART3_send_char(u8 temp);
#endif


