#include "sys.h"
#include "usart2.h"	  
#include "led.h"	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//����2��ʼ��,485����/�������ų�ʼ��	   
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************



//���ܣ�����2��ʼ��
//��������
//���أ���
void uart2_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2ʱ��

    //USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2

    //USART2_RX	  GPIOA.3��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

    USART_Init(USART2, &USART_InitStructure); //��ʼ������2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
    //uart2_485_init();
}

//���ܣ�����2�жϷ�����򣬽��յ����ݻ�����������
//��������
//���أ���
u16 USART2_RX_STA=0; 
//u8 USART2_RX_BUF[200];
void USART2_IRQHandler(void)
{
    u8 Res;
    //	USART_ClearFlag(USART2,USART_FLAG_TC);
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
    {
        Res=USART_ReceiveData(USART2);	//��ȡ���յ�������
				//USART2_RX_BUF[USART2_RX_STA++]=Res;		//��¼���յ���ֵ
        //USART2_send_buff((u8*)&Res, 1);
    } 
} 

//���ܣ�����2����һ���ַ�
//������temp���͵��ַ�
//���أ���
void USART2_send_char(u8 temp)
{      
    USART_SendData(USART2,(u8)temp);      
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);  
      
}  


//���ܣ�����2�����ַ���
//������buf���͵��ַ���,len�ַ����ĳ���
//���أ���
void USART2_send_buff(u8* buf,u32 len)
{
    u32 i;
    for(i=0;i<len;i++)  
        USART2_send_char(buf[i]);  
} 


//���ܣ�����485�Ľ��շ���ų�ʼ��
//��������
//���أ���
void uart2_485_init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    //CS
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );//PB1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // PB1 ���� 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    MAX485_RX_TX=MAX485_RX; 
}

//mode:1:���͡���0������
void Set485Mode(u8 mode)
{
    if(mode==1)
    {
        MAX485_RX_TX=MAX485_TX; //����
    }
    else
    {
        MAX485_RX_TX=MAX485_RX; //����
    }
}

