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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//���ļ�
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************
u16 adcx;
void dispmq2()
{
    char buff[100]={0};
    //��ʾ�¶�
    sprintf(buff, "MQ2:%d", adcx);
    Gui_DrawFont_GBK16(16,32,VIOLET,BLACK,buff);        
}

void display()
{
    char buff[100]={0};
    int yOffset=0;
		
		//��ʾ
    Gui_DrawFont_GBK16(0,0,VIOLET,WHITE,"��лʹ�����е���");
    Gui_DrawFont_GBK16(16,16,VIOLET,WHITE,"BC35G������");		

    //��ʾmq2
    dispmq2();
    
    yOffset=52;

    //��һ��
//    sprintf(buff, "SN:%s", nb.sn);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //�ڶ���  
    //yOffset+=10;
//    sprintf(buff, "IMEI:%s", nb.imei);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);


    //������
//    yOffset+=10;
//    sprintf(buff, "IMSI:%s", nb.imsi);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //������
//    yOffset+=10;
    sprintf(buff, "NBand:%d",nb.nband);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);


    //������
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

    //������
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

    //������
    yOffset+=10;
    sprintf(buff, "CSQ:%d", nb.csq);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //�ڰ���
//    yOffset+=10;
//    sprintf(buff, "Attach:%d",nb.attach);
//    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //�ھ���
    yOffset+=10;
    sprintf(buff, "L:%s",nb.localIp);
    LCD_P6x8Str(0, yOffset, BLUE,WHITE,buff);

    //�ھ���
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
    delay_init();            //��ʱ������ʼ��      
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  
    LED_Init();             //��ʼ����LED���ӵ�Ӳ���ӿ�
    Lcd_Init();             //��ʼ��LCD 
		//EXTIX_Init();
		SR501_CONFIG();
	  Adc_Init();
    uart1_init(115200);     //���ڳ�ʼ��Ϊ115200
    //uart2_init(115200);     //���ڳ�ʼ��Ϊ115200
    uart3_init(9600);       //���ڳ�ʼ��Ϊ9600
}


int main(void)
{	 
    char buff[100]={0};
    u16 count=0;
		u8 red_sence;
		char message[30];
    STM32_Init();//ϵͳ��ʼ��

    //LCD��ʾ
    LCD_P6x8Str(20,10,WHITE,BLACK,"sz-yy.taobao.com");
    Gui_DrawFont_GBK16(16,20,VIOLET,WHITE,"�������е���");
    Gui_DrawFont_GBK16(32,40,BLUE,WHITE,"��ӭ��");
    Gui_DrawFont_GBK16(20,60,RED,WHITE,"1.44��ɫTFT");
    Gui_DrawFont_GBK16(0,80,VIOLET,BLACK,"��лʹ�����е���");
    Gui_DrawFont_GBK16(16,100,VIOLET,BLACK,"BC35G������");
    NB_init(MODE, ONENET_SERVER, ONENET_PORT);//NBģ���ʼ��
    delay_ms(1000);  
    Lcd_Clear(WHITE);//����
    display();//��ʾ
		
    while(1)
    {
        //�Ƚ��������
        while(nb.mqtt_state != MQTT_REQUEST_IDLE)
        {
            //Gui_DrawFont_GBK16(0,60,BLUE,WHITE,"ontnet connecting!");   //LCD��ʾ
            OneNet_DevLink();//���û�н���onenet��Ҫ����
            delay_ms(1000);
            NB_GetData();//��⴮�ڣ��鿴�Ƿ�����������
            delay_ms(1000);
            NB_GetData();//��⴮�ڣ��鿴�Ƿ�����������
        }

        //���ڷ�������
        while(nb.mqtt_state == MQTT_REQUEST_IDLE)
        {
            if(0==(count%20000))
            {
                onenet_mqtt_send_heart();     //װ�ز�����������     
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

            if((count%50)==0) LED1=!LED1;//���ƣ���ʾ����������

            count++;
            NB_GetData();//��⴮�ڣ��鿴�Ƿ�����������
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

