#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//DS18B20�¶ȼ�⣬����������PB5��
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************



//IO��������
#define DS18B20_IO_IN()  {GPIOB->CRL&=0XFF0FFFFF;GPIOB->CRL|=8<<20;}
#define DS18B20_IO_OUT() {GPIOB->CRL&=0XFF0FFFFF;GPIOB->CRL|=3<<20;}
////IO��������											   
#define	DS18B20_DQ_OUT PBout(5) //���ݶ˿�	PB5
#define	DS18B20_DQ_IN  PBin(5)  //���ݶ˿�	PB5 
   	
u8 DS18B20_Init(void);//��ʼ��DS18B20
short DS18B20_Get_Temp(void);//��ȡ�¶�
void DS18B20_Start(void);//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);//����һ���ֽ�
u8 DS18B20_Read_Bit(void);//����һ��λ
u8 DS18B20_Check(void);//����Ƿ����DS18B20
void DS18B20_Rst(void);//��λDS18B20    
#endif















