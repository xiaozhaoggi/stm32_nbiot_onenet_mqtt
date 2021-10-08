#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//LED灯初始化，LED1:PA8，LED2：PD2，LED3：PB12
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


//初始化PA8，PB12和PD2为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PA8,PD2端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED1-->PA8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA8
    GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA8输出高

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED2-->PD2 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD2
    GPIO_SetBits(GPIOD,GPIO_Pin_2);						 //PD2输出高


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //LED3-->PB12 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);           //根据设定参数初始化GPIOB12
    GPIO_SetBits(GPIOB,GPIO_Pin_12);            //PB12输出高
}

void led1On(int on)
{
    if(on>0)
    {
        LED1=0;
    }
    else
    {
        LED1=1;
    }
}

void led2On(int on)
{
    if(on>0)
    {
        LED2=0;
    }
    else
    {
        LED2=1;
    }
}

void led3On(int on)
{
    if(on>0)
    {
        LED3=0;
    }
    else
    {
        LED3=1;
    }
}

 
