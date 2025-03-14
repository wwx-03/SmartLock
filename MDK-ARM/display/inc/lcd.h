#ifndef __LCD_H
#define __LCD_H

#define USE_LCD_DRIVER_ST7735
//#define USE_LCD_DRIVER_ST7789

#include "main.h"
#include "display.h"

typedef struct lcd_io_t lcd_io_t;
typedef struct lcd_init_t lcd_init_t;
typedef struct lcd_handle_t lcd_handle_t;

struct lcd_io_t
{
	host_io_t SCL;
	host_io_t SDA;
	host_io_t RES;
	host_io_t DC;
	host_io_t CS;
	host_io_t BLK;
};

struct lcd_init_t
{
	lcd_io_t IO; /*引脚*/
	uint16_t width; /*宽度*/
	uint16_t height; /*高度*/
	uint16_t fontSize; /*字体大小*/
	uint16_t fontWidth;
	uint16_t fontHeight;
	uint16_t pen; /*画笔颜色*/
	uint16_t backColor; /*背景颜色*/
};

struct lcd_handle_t
{
	union
	{
		display_if_t display_if;
		struct
		{
			int (*clear)(void *);
			int (*fill)(void *, unsigned short /*xStart*/, unsigned short /*xEnd*/, unsigned short /*yStart*/, unsigned short /*yEnd*/, unsigned short /*color*/);
			int (*drawPoint)(void *, unsigned short, unsigned short);
			int (*drawLine)(void *, unsigned short /*xStart*/, unsigned short /*xEnd*/, unsigned short /*yStart*/, unsigned short /*yEnd*/);
			int (*drawRectangle)(void *, unsigned short /*xStart*/, unsigned short /*xEnd*/, unsigned short /*yStart*/, unsigned short /*yEnd*/);
			int (*drawCircle)(void *, unsigned short /*x*/, unsigned short /*y*/, unsigned short /*r*/);

			int (*showChar)(void *, unsigned short /*x*/, unsigned short /*y*/, char);
			int (*showString)(void *, unsigned short /*x*/, unsigned short /*y*/, char *);
			int (*showChinese)(void *, unsigned short /*x*/, unsigned short /*y*/, char *);
			int (*show)(void *, unsigned short, unsigned short, char *);
			int (*print)(void *, char */*format*/);

			int (*showPicture)(void *, unsigned short /*x*/, unsigned short /*y*/, unsigned short /*length*/, unsigned short /*width*/, const unsigned char */*pic[]*/);
		};
	};


	lcd_io_t IO; /*引脚*/
	uint16_t width; /*宽度*/
	uint16_t height; /*高度*/
	uint16_t cursor_x; /*x光标*/
	uint16_t cursor_y; /*y光标*/
	uint16_t fontSize; /*字体大小*/
	uint16_t fontWidth;
	uint16_t fontHeight;
	uint16_t pen; /*画笔颜色*/
	uint16_t backColor; /*背景颜色*/
};

int LCD_Init(lcd_handle_t *handle, lcd_init_t *initStructure);

void LCD_WriteDataStart(lcd_handle_t *handle);
void LCD_WriteData8Continue(lcd_handle_t *handle, uint8_t data);
void LCD_WriteData16Continue(lcd_handle_t *handle, uint16_t data);
void LCD_WriteDataStop(lcd_handle_t *handle);

int LCD_WriteCommand(lcd_handle_t *handle, uint16_t cmd);
int LCD_WriteData8(lcd_handle_t *handle, uint8_t Data);
int LCD_WriteData16(lcd_handle_t *handle, uint16_t Data);
int LCD_AddressSet(lcd_handle_t *handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色

#endif
