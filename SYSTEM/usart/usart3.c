#include "sys.h"
#include "usart3.h"
#include "nb.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//串口3初始化		   
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************

//功能：串口3初始化
//参数：无
//返回：无
void uart3_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟

    //USART3_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10

    //USART3_RX	  GPIOB.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.11  

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART3, ENABLE);                    //使能串口3 
}

//功能：串口3中断服务程序，接收到数据会进入这个函数
//参数：无
//返回：无
void USART3_IRQHandler(void)
{
    u8 Res;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
    {
        Res =USART_ReceiveData(USART3);	//读取接收到的数据
        AddUartChar((unsigned char) Res);//保存收到的字符
    } 
} 

//功能：串口3发送一个字符
//参数：temp发送的字符
//返回：无
void USART3_send_char(u8 temp)     
{
    USART_SendData(USART3,(u8)temp);
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);      
}  

//功能：串口3发送字符串
//参数：buf发送的字符串,len字符串的长度
//返回：无
void USART3_send_buff(u8* buf,u32 len) 
{
    u32 i;

    for(i=0;i<len;i++)
        USART3_send_char(buf[i]);
}



