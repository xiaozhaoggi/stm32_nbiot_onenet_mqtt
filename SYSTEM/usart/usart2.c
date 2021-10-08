#include "sys.h"
#include "usart2.h"	  
#include "led.h"	  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//串口2初始化,485发送/接收引脚初始化	   
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************



//功能：串口2初始化
//参数：无
//返回：无
void uart2_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2时钟

    //USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

    //USART2_RX	  GPIOA.3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

    USART_Init(USART2, &USART_InitStructure); //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2 
    //uart2_485_init();
}

//功能：串口2中断服务程序，接收到数据会进入这个函数
//参数：无
//返回：无
u16 USART2_RX_STA=0; 
//u8 USART2_RX_BUF[200];
void USART2_IRQHandler(void)
{
    u8 Res;
    //	USART_ClearFlag(USART2,USART_FLAG_TC);
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
    {
        Res=USART_ReceiveData(USART2);	//读取接收到的数据
				//USART2_RX_BUF[USART2_RX_STA++]=Res;		//记录接收到的值
        //USART2_send_buff((u8*)&Res, 1);
    } 
} 

//功能：串口2发送一个字符
//参数：temp发送的字符
//返回：无
void USART2_send_char(u8 temp)
{      
    USART_SendData(USART2,(u8)temp);      
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);  
      
}  


//功能：串口2发送字符串
//参数：buf发送的字符串,len字符串的长度
//返回：无
void USART2_send_buff(u8* buf,u32 len)
{
    u32 i;
    for(i=0;i<len;i++)  
        USART2_send_char(buf[i]);  
} 


//功能：控制485的接收发射脚初始化
//参数：无
//返回：无
void uart2_485_init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    //CS
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );//PB1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // PB1 推挽 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    MAX485_RX_TX=MAX485_RX; 
}

//mode:1:发送　　0：接收
void Set485Mode(u8 mode)
{
    if(mode==1)
    {
        MAX485_RX_TX=MAX485_TX; //发送
    }
    else
    {
        MAX485_RX_TX=MAX485_RX; //接收
    }
}

