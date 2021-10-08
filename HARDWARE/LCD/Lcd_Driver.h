#ifndef __Lcd_Driver_H
#define __Lcd_Driver_H


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//LCD��ʾ����ʼ����SDI��PB15��SCL��PB13��RESET��PB14��DC��PC6��BLK��PC7
//LCD�ͺţ�ST7735S
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************



#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//��ɫ0 3165 00110 001011 00101
#define GRAY1   0x8410      	//��ɫ1      00000 000000 00000
#define GRAY2   0x4208      	//��ɫ2  1111111111011111
#define VIOLET  0xf81f  //��ɫ



#define LCD_CTRL   	  	  GPIOB		//����TFT���ݶ˿�
#define LCD_CTRL2   	  	GPIOC		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_7  //MCU_PC7--->>TFT --BL
#define LCD_RS         	GPIO_Pin_6	//PB11--->>TFT --RS/DC
//#define LCD_CS        	GPIO_Pin_12  //MCU_PB11--->>TFT --CS/CE
#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL/SCK
#define LCD_RST     	  GPIO_Pin_14	//PB14--->>TFT --RESET
#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA/DIN


//Һ�����ƿ���1�������궨��
#define	LCD_CS_SET  	LCD_CTRL->BSRR=LCD_CS    
#define	LCD_RS_SET  	LCD_CTRL2->BSRR=LCD_RS    
#define	LCD_SDA_SET  	LCD_CTRL->BSRR=LCD_SDA    
#define	LCD_SCL_SET  	LCD_CTRL->BSRR=LCD_SCL    
#define	LCD_RST_SET  	LCD_CTRL->BSRR=LCD_RST    
#define	LCD_LED_SET  	LCD_CTRL2->BSRR=LCD_LED   

//Һ�����ƿ���0�������궨��
#define	LCD_CS_CLR  	LCD_CTRL->BRR=LCD_CS    
#define	LCD_RS_CLR  	LCD_CTRL2->BRR=LCD_RS    
#define	LCD_SDA_CLR  	LCD_CTRL->BRR=LCD_SDA    
#define	LCD_SCL_CLR  	LCD_CTRL->BRR=LCD_SCL    
#define	LCD_RST_CLR  	LCD_CTRL->BRR=LCD_RST    
#define	LCD_LED_CLR  	LCD_CTRL2->BRR=LCD_LED 


void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_ClearRect(u16 x1,u16 y1, u16 x2,u16 y2, u16 Color);
void Lcd_Updata(void);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
unsigned int Lcd_ReadPoint(u16 x,u16 y);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void LCD_WriteData_16Bit(u16 Data);
void Lcd_Blk(u8 on);

#endif
