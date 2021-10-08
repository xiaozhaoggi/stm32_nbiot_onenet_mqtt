#include "buzzer.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//���������ơ�ʹ��PC1���ƣ��͵�ƽ����
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


//���ܣ���������ʼ��
//������on��1���졡0����
//���أ���
void Buzzer_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;
    		

    //buzzer
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC1�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PC1 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC1
    GPIO_SetBits(GPIOC,GPIO_Pin_1);						 //PC1�����
}

//���ܣ����Ʒ�����
//������on��1���졡0����
//���أ���
void Buzzer_on(int on){
    if(on>0) BUZZER=0;
    else BUZZER=1;
}

//���ܣ����Ʒ�������һ��
//��������
//���أ���
void Buzzer_flash(void){

    Buzzer_on(1);
    delay_ms(100);
    Buzzer_on(0);
}


 
