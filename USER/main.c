#include "led.h"
#include "delay.h"
#include "sys.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "nb.h"
#include "onenet.h"
#include "mqttkit.h"
#include "exti.h"
#include "sim800c.h"
#include "SR501.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//S81 STM32F103RC 开发板
//主文件
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************
u16 adcx;
void dispmq2()
{
    char buff[100]={0};
    //显示温度
    sprintf(buff, "MQ2:%d", adcx);
    Gui_DrawFont_GBK16(16,32,VIOLET,BLACK,buff);        
}

void display()
{
    char buff[100]={0};
    int yOffset=0;
		
		//提示
    Gui_DrawFont_GBK16(0,0,VIOLET,WHITE,"感谢使用亿研电子");
    Gui_DrawFont_GBK16(16,16,VIOLET,WHITE,"BC35G开发板");		

    //显示mq2
    dispmq2();
    
    yOffset=52;

    //第一行
//    sprintf(buff, "SN:%s", nb.sn);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第二行  
    //yOffset+=10;
//    sprintf(buff, "IMEI:%s", nb.imei);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);


    //第三行
//    yOffset+=10;
//    sprintf(buff, "IMSI:%s", nb.imsi);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第四行
//    yOffset+=10;
    sprintf(buff, "NBand:%d",nb.nband);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);


    //第五行
    yOffset+=10;
    if(strlen(nb.gOperator)>0)
    {
        sprintf(buff, "operator:%s", nb.gOperator);
    }
    else
    {
        sprintf(buff, "operator:search ...", nb.gOperator);
    }
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第六行
    yOffset+=10;
    if(nb.RegStatus==1 || nb.RegStatus==5)
    {
        sprintf(buff, "RegStatus:Registered");
    }
    else
    {
        sprintf(buff, "RegStatus:search ...");
    }
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第七行
    yOffset+=10;
    sprintf(buff, "CSQ:%d", nb.csq);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第八行
//    yOffset+=10;
//    sprintf(buff, "Attach:%d",nb.attach);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第九行
    yOffset+=10;
    sprintf(buff, "L:%s",nb.localIp);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //第九行
//    yOffset+=10;
//    sprintf(buff, "ConnectionStatus:%d",nb.ConnectionStatus);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //
    yOffset+=10;
    sprintf(buff, "MODE:%s  PORT:%d",(nb.net.mode==NB_TCP)?"TCP":"UDP", nb.net.remote_port);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    yOffset+=10;
    sprintf(buff, "S:%s",nb.net.remote_ip);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);    
}

void STM32_Init(void)
{
    delay_init();            //延时函数初始化      
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
  
    LED_Init();             //初始化与LED连接的硬件接口
    Lcd_Init();             //初始化LCD 
		//EXTIX_Init();
		SR501_CONFIG();
	  Adc_Init();
    uart1_init(115200);     //串口初始化为115200
    //uart2_init(115200);     //串口初始化为115200
    uart3_init(9600);       //串口初始化为9600
}


int main(void)
{	 
    char buff[100]={0};
    u16 count=0;
		u8 red_sence;
		char message[30];
    STM32_Init();//系统初始化

    //LCD提示
    LCD_P6x8Str(20,10,WHITE,BLACK,"sz-yy.taobao.com");
    Gui_DrawFont_GBK16(16,20,VIOLET,WHITE,"深圳亿研电子");
    Gui_DrawFont_GBK16(32,40,BLUE,WHITE,"欢迎您");
    Gui_DrawFont_GBK16(20,60,RED,WHITE,"1.44彩色TFT");
    Gui_DrawFont_GBK16(0,80,VIOLET,BLACK,"感谢使用亿研电子");
    Gui_DrawFont_GBK16(16,100,VIOLET,BLACK,"BC35G开发板");
    NB_init(MODE, ONENET_SERVER, ONENET_PORT);//NB模块初始化
    delay_ms(1000);  
    Lcd_Clear(WHITE);//清屏
    display();//显示
		
    while(1)
    {
        //先接入服务器
        while(nb.mqtt_state != MQTT_REQUEST_IDLE)
        {
            //Gui_DrawFont_GBK16(0,60,BLUE,WHITE,"ontnet connecting!");   //LCD提示
            OneNet_DevLink();//如果没有接入onenet就要接入
            delay_ms(1000);
            NB_GetData();//检测串口，查看是否有主动数据
            delay_ms(1000);
            NB_GetData();//检测串口，查看是否有主动数据
        }

        //周期发送数据
        while(nb.mqtt_state == MQTT_REQUEST_IDLE)
        {
            if(0==(count%20000))
            {
                onenet_mqtt_send_heart();     //装载并发送心跳包     
            }
            
            if((count%500)==0)
            {
                NB_CycleCheck();
                //Lcd_Clear(WHITE);
                display();
            }

            if(0==(count%500))
            {       
             
								adcx=Get_Adc_Average(ADC_Channel_1,10);
							  if(SR501)
									red_sence = 'Y';
								else
									red_sence = 'N';
                OneNet_SendData(adcx, red_sence);
								if(SR501&&adcx>1400)
								{
									sprintf(message,"REDSENCE:%c,MQ2:%d",red_sence,adcx);
									sim800c_send_message(message);
								}
            }

            if((count%50)==0) LED1=!LED1;//闪灯，表示程序在运行

            count++;
            NB_GetData();//检测串口，查看是否有主动数据
            delay_ms(10);  
        }
      
    }
}



	/*key=KEY_Scan();

	if(key==KEY1_PRES)
	{
			onenet_mqtt_publish_topic("mqtt_topic_onenet", "hello");
			Buzzer_flash();
	}
	else if(key==KEY2_PRES)
	{
			onenet_mqtt_publish_topic("mqtt_topic_onenet", "123");
			Buzzer_flash();
	}
	else if(key==KEY3_PRES)
	{
			onenet_mqtt_publish_topic("mqtt_topic_onenet", "abc");
			Buzzer_flash();
	}
	*/

