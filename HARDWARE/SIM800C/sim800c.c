#include "usart.h"	 
#include "delay.h"
char hc[2] = {0x0a,0x0d};
char *s1 = "AT+CSCS=\"GSM\"";
char *s2 = "AT+CMGF=1";
char *s3 = "AT+CMGS=\"13229321127\"";
void sim800c_send_message(char *s)
{
	//uart1_send_buff(s1, 100);//设置GSM字符集
	uart1_send_message(s1);
	uart1_send_buff(hc, sizeof(hc));
	delay_ms(1000);
	
	//uart1_send_buff(s2, 100);//设置文本模式
	uart1_send_message(s2);
	uart1_send_buff(hc, sizeof(hc));
	delay_ms(1000);
	
	//uart1_send_buff(s3, 100);//设置发送号码
	uart1_send_message(s3);
	uart1_send_buff(hc, sizeof(hc));
	delay_ms(1000);
	
	//uart1_send_buff(s, 100);//发送内容
	uart1_send_message(s);
	delay_ms(1000);
	
	uart1_send_char(0x1a);
}

