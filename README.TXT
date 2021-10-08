实验器材:
	STM32+BC35G开发板一块
	
实验目的:
	1、通过MQTT协议和ONENET服务器通信
	
硬件资源:
	1,按键,wake_up(key1):PA0,key2:PC0, key3:CP13，
	2,灯,LED1:PA8，LED2：PD2，LED3：PB12
	3,25Q16，CS：PC4，S0：PA6，SI：PA7，CLK：PA5
	4,LCD(128*128)，SDI：PB15，SCL：PB13，RESET：PB14，DC：PC6，BLK：PC7
	5,蜂鸣器，PC1
	6,串口1，TX：PA9，RX：PA10
	7,串口2，TX：PA2，RX：PA3
	8,串口3，TX：PB10，RX：PA11
	9,BC35G NB模块，接在串口3上，
	10,DS18B20:PB5
	11,RS232/485接口，通过跳线接在串口2(PA2和PA3)上，485的接收/发送使能脚PB1
	12,空余的引脚：PA4,PA11,PA12，
	　　　　　　　 PB6,PB7,PB8,PB9
	　　　　　　　 PC2,PC3，PC5，PC8，PC9，PC10,PC11,PC12
	
	
实验现象:
	1、周期采集板上的18B20传感器，LCD显示温度
	2、通过MQTT协议周期上传温度到ONENET
	3、通过下发指令控制开发板灯和蜂鸣器状态
	4、可以在ONENET上创建应用程序，控制开发板灯和蜂鸣器状态
	
注意事项:
	注意排针上的跳线
	1,我们使用的LCD型号是:ST7735S
	2,我们使用的蜂鸣器是有源蜂鸣器
	3、保证USB口输出调试信息：RX1和TX USB短接，TX1和RX USB短接
	4、保证BC35模块通信正常：RX3和NB TX短接，TX3和NB RX短接


	




































