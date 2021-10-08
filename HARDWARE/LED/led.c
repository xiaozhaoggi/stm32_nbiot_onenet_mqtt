#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//LED�Ƴ�ʼ����LED1:PA8��LED2��PD2��LED3��PB12
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


//��ʼ��PA8��PB12��PD2Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PA8,PD2�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED1-->PA8 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA8
    GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA8�����

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED2-->PD2 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD2
    GPIO_SetBits(GPIOD,GPIO_Pin_2);						 //PD2�����


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //LED3-->PB12 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);           //�����趨������ʼ��GPIOB12
    GPIO_SetBits(GPIOB,GPIO_Pin_12);            //PB12�����
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

 
