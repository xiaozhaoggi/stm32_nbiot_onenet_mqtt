
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//S81 STM32F103RC ������
//LCD��ʾ����ʾ�����ӿڶ���
//LCD�ͺţ�ST7735S
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num) ;
void LCD_P6x8Str(unsigned char x, unsigned char y, u16 fc, u16 bc,unsigned char ch[]);
void LCD_DrawFont_GBK16_line(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void LCD_DrawFont_GBK16_updata(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void LCD_P6x8Str_line(unsigned char x, unsigned char y, u16 fc, u16 bc,unsigned char ch[]);
void LCD_P6x8Str_updata(unsigned char x, unsigned char y, u16 fc, u16 bc,unsigned char ch[]);


