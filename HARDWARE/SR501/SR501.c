#include "SR501.h"


void SR501_CONFIG(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化


}

