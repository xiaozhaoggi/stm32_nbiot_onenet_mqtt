#include "sys.h"
#include "usart3.h"
#include "nb.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//����3��ʼ��		   
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************

//���ܣ�����3��ʼ��
//��������
//���أ���
void uart3_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��

    //USART3_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.10

    //USART3_RX	  GPIOB.11��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.11  

    //Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); //��ʼ������3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 
}

//���ܣ�����3�жϷ�����򣬽��յ����ݻ�����������
//��������
//���أ���
void USART3_IRQHandler(void)
{
    u8 Res;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
    {
        Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
        AddUartChar((unsigned char) Res);//�����յ����ַ�
    } 
} 

//���ܣ�����3����һ���ַ�
//������temp���͵��ַ�
//���أ���
void USART3_send_char(u8 temp)     
{
    USART_SendData(USART3,(u8)temp);
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);      
}  

//���ܣ�����3�����ַ���
//������buf���͵��ַ���,len�ַ����ĳ���
//���أ���
void USART3_send_buff(u8* buf,u32 len) 
{
    u32 i;

    for(i=0;i<len;i++)
        USART3_send_char(buf[i]);
}



