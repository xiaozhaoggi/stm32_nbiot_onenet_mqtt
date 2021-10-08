#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//按键初始化，按键分别接在IO口上，wake_up(key1):PA0,key2:PC0, key3:CP13，
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************



//功能：按键初始化函数
//参数：无
//返回：无
void KEY_Init(void) //IO初始化
{ 
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTE时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_13;//KEY1-KEY2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC1,13

    //初始化 WK_UP-->GPIOA.0	  下拉输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
}


//按键处理函数
//返回按键值
//0，没有任何按键按下
//1，KEY1按下　WK_UP
//2，KEY2按下
//3，KEY3按下 
//注意此函数有响应优先级,KEY1>KEY2>KEY3!!
u8 KEY_Scan(void)
{	 
    if(KEY1==1||KEY2==0 || KEY3==0)
    {
        delay_ms(50);//去抖动 

        if(KEY1==1)
        {
            while(KEY1==1)
            {
                delay_ms(10);
            }
            return KEY1_PRES;//按键松开后，返回键值
        }
        else if(KEY2==0)
        {
            while(KEY2==0)
            {
                delay_ms(10);
            }
            return KEY2_PRES;//按键松开后，返回键值
        }
        else if(KEY3==0)
        {
            while(KEY3==0)
            {
                delay_ms(10);
            }
            return KEY3_PRES;//按键松开后，返回键值
        }
    }
  
    return 0;// 无按键按下
}
