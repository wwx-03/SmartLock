#include "lcd.h"

#if defined USE_LCD_DRIVER_ST7789

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lcdcmd.h"
#include "Font.h"

#define LCD_SET_SDA(_HANDLE_, _BIT_) ((lcd_handle_t *)_HANDLE_)->IO.SDA.GPIOx->BSRR \
												 = ((lcd_handle_t *)_HANDLE_)->IO.SDA.pin \
												  << 16u * ((uint16_t)(_BIT_) == 0)

#define LCD_SET_SCL(_HANDLE_, _BIT_) ((lcd_handle_t *)_HANDLE_)->IO.SCL.GPIOx->BSRR \
												 = ((lcd_handle_t *)_HANDLE_)->IO.SCL.pin \
												  << 16u * ((uint16_t)(_BIT_) == 0)
#define LCD_SET_RES(_HANDLE_, _BIT_) ((lcd_handle_t *)_HANDLE_)->IO.RES.GPIOx->BSRR \
												 = ((lcd_handle_t *)_HANDLE_)->IO.RES.pin \
												  << 16u * ((uint16_t)(_BIT_) == 0)
#define LCD_SET_DC(_HANDLE_, _BIT_) ((lcd_handle_t *)_HANDLE_)->IO.DC.GPIOx->BSRR \
												 = ((lcd_handle_t *)_HANDLE_)->IO.DC.pin \
												  << 16u * ((uint16_t)(_BIT_) == 0)
#define LCD_SET_CS(_HANDLE_, _BIT_) ((lcd_handle_t *)_HANDLE_)->IO.CS.GPIOx->BSRR \
												 = ((lcd_handle_t *)_HANDLE_)->IO.CS.pin \
												  << 16u * ((uint16_t)(_BIT_) == 0)
#define LCD_SET_BLK(_HANDLE_, _BIT_) ((lcd_handle_t *)_HANDLE_)->IO.BLK.GPIOx->BSRR \
												 = ((lcd_handle_t *)_HANDLE_)->IO.BLK.pin \
												  << 16u * ((uint16_t)(_BIT_) == 0)

int spi_start(lcd_handle_t *handle)
{
	LCD_SET_CS(handle, 0);
	return 0;
}

int spi_stop(lcd_handle_t *handle)
{
	LCD_SET_CS(handle, 1);
	return 0;
}

int spi_write(lcd_handle_t *handle, uint8_t Data)
{
	for(uint8_t i = 0; i < 8; i ++)
	{
		LCD_SET_SCL(handle, 0);
		LCD_SET_SDA(handle, Data & 0x80);
		LCD_SET_SCL(handle, 1);
		Data <<= 1;
	}
	LCD_SET_SDA(handle, 0);
	return 0;
}


int LCD_WriteData8(lcd_handle_t *handle, uint8_t Data)
{
	spi_start(handle);
	spi_write(handle, Data);
	spi_stop(handle);

	return 0;
}

int LCD_WriteData16(lcd_handle_t *handle, uint16_t Data)
{
	spi_start(handle);
	spi_write(handle, Data >> 8);
	spi_write(handle, Data);
	spi_stop(handle);

	return 0;
}

inline void LCD_WriteDataStart(lcd_handle_t *handle)
{
	spi_start(handle);
}

inline void LCD_WriteData8Continue(lcd_handle_t *handle, uint8_t data)
{
	spi_write(handle, data);
}

inline void LCD_WriteData16Continue(lcd_handle_t *handle, uint16_t data)
{
	spi_write(handle, data >> 8);
	spi_write(handle, data);
}

inline void LCD_WriteDataStop(lcd_handle_t *handle)
{
	spi_stop(handle);
}

int LCD_WriteCommand(lcd_handle_t *handle, uint16_t cmd)
{
	LCD_SET_DC(handle, 0);
	spi_start(handle);
	spi_write(handle, cmd);
	spi_stop(handle);
	LCD_SET_DC(handle, 1);

	return 0;
}

/**
 * @brief:LCDÉèÖÃµØÖ·
 * @param XS:XÖáÆðÊ¼µØÖ·, µ±MV = 0Ê±, XSµÄÈ¡Öµ·¶Î§[0,127]. µ±MV = 1Ê±, XSÈ¡Öµ·¶Î§[0,159].
 * @param XE:XÖáÖÕÖ¹µØÖ·, È¡Öµ·¶Î§¸úXSÍ¬Àí.
 * @param YS:YÖáÆðÊ¼µØÖ·, µ±MV = 0Ê±, YSµÄÈ¡Öµ·¶Î§[0,159]. µ±MV = 1Ê±, YSÈ¡Öµ·¶Î§[0,127].
 * @param YE:YÖáÖÕÖ¹µØÖ·, È¡Öµ·¶Î§¸úYSÍ¬Àí.
 * @attion:µ±µØÖ·ÉèÖÃ³¬³öÈ¡Öµ·¶Î§, ³¬³ö·¶Î§µÄÊý¾Ý½«»á±»ºöÊÓ.
 */
int LCD_AddressSet(lcd_handle_t *handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye)
{
	/*ï¿½Ð¶Ï¿ï¿½ï¿½È¸ß¶ï¿½ï¿½Ç·ï¿½Ô½ï¿½ï¿½*/
	if(handle->width < xs || handle->width < xe) 
	{
		return -1;
	}
	else if(handle->height < ys || handle->height < ye)
	{
		return -2;
	}
	else
	{
		/* non-useage */
	}

	/*ï¿½ï¿½ï¿½ï¿½Ð´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?*/
	handle->cursor_x = xe + 1;
	handle->cursor_y = ys;

	LCD_WriteCommand(handle, COLUMN_ADDRESS_SET);
  LCD_WriteData16(handle, xs);
  LCD_WriteData16(handle, xe);
  LCD_WriteCommand(handle, ROW_ADDRESS_SET);
  LCD_WriteData16(handle, ys + 20);
  LCD_WriteData16(handle, ye + 20);

	return 0;
}


int LCD_DrawPoint(void *handle, uint16_t x, uint16_t y)
{
	LCD_AddressSet(handle, x, x, y, y);
	LCD_WriteData16(handle, ((lcd_handle_t *)handle)->pen);
	return 0;
}

int LCD_Clear(void *handle)
{
	LCD_AddressSet(handle, 0, ((lcd_handle_t *)handle)->width - 1, 0, ((lcd_handle_t *)handle)->height - 1);
	LCD_WriteCommand(handle, RAM_WRITE);
	spi_start(handle);
	for(uint16_t i = 0; i < ((lcd_handle_t *)handle)->height; i ++)
	{
		for(uint16_t j = 0; j < ((lcd_handle_t *)handle)->width; j ++)
		{
			spi_write(handle, ((lcd_handle_t *)handle)->backColor >> 8);
			spi_write(handle, ((lcd_handle_t *)handle)->backColor);
		}
	}
	spi_stop(handle);
	((lcd_handle_t *)handle)->cursor_x = 0;
	((lcd_handle_t *)handle)->cursor_y = 0;
	return 0;
}

int LCD_FillColor(void *handle, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye, uint16_t color)
{
	uint16_t i = 0, j = 0, temp_x = ((lcd_handle_t *)handle)->cursor_x, temp_y = ((lcd_handle_t *)handle)->cursor_y;
	LCD_AddressSet(handle, xs, xe, ys, ye);
	LCD_WriteCommand(handle, RAM_WRITE);
	spi_start(handle);
	for(i = ys; i <= ye; i ++)
	{
		for(j = xs; j <= xe; j ++)
		{
			spi_write(handle, color >> 8);
			spi_write(handle, color);
		}
	}
	spi_stop(handle);

	((lcd_handle_t *)handle)->cursor_x = temp_x;
	((lcd_handle_t *)handle)->cursor_y = temp_y;
	

	return 0;
}

int LCD_DrawLine(void *handle, uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 
	delta_y=y2-y1;
	uRow=x1;//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
	uCol=y1;
	if(delta_x>0)incx=1; //ï¿½ï¿½ï¿½Ãµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 
	else if (delta_x==0)incx=0;//ï¿½ï¿½Ö±ï¿½ï¿½ 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//Ë®Æ½ï¿½ï¿½ 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //Ñ¡È¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(handle, uRow,uCol);//ï¿½ï¿½ï¿½ï¿½
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
	return 0;
}

int LCD_DrawRectangle(void *handle, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
	LCD_DrawLine(handle, x1,y1,x2,y1);
	LCD_DrawLine(handle, x1,y1,x1,y2);
	LCD_DrawLine(handle, x1,y2,x2,y2);
	LCD_DrawLine(handle, x2,y1,x2,y2);
	return 0;
}

int LCD_DrawCircle(void *handle, uint16_t x0,uint16_t y0,uint16_t r)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(handle, x0-b,y0-a);             //3           
		LCD_DrawPoint(handle, x0+b,y0-a);             //0           
		LCD_DrawPoint(handle, x0-a,y0+b);             //1                
		LCD_DrawPoint(handle, x0-a,y0-b);             //2             
		LCD_DrawPoint(handle, x0+b,y0+a);             //4               
		LCD_DrawPoint(handle, x0+a,y0-b);             //5
		LCD_DrawPoint(handle, x0+a,y0+b);             //6 
		LCD_DrawPoint(handle, x0-b,y0+a);             //7
		a++;
		if((a*a+b*b)>(r*r))//ï¿½Ð¶ï¿½Òªï¿½ï¿½ï¿½Äµï¿½ï¿½Ç·ï¿½ï¿½Ô?
		{
			b--;
		}
	}
	return 0;
}

int LCD_ShowChar(void *handle, uint16_t xs, uint16_t ys, char Char)
{
	uint16_t xe, ye;
	switch(((lcd_handle_t *)handle)->fontSize)
	{
		case 12 * 8:
			xe = xs + 8;
			ye = ys + 12;
			LCD_AddressSet(handle, xs, xe - 1, ys, ye - 1);
			LCD_WriteCommand(handle, RAM_WRITE);
			for(uint8_t i = ys; i < ye; i ++)
			{
				for(uint8_t j = xs; j < xe; j ++)
				{
					if(ASCII_1208[Char - ' '][i - ys] & (0x01 << (j - xs)))
					{
						LCD_WriteData16(handle, ((lcd_handle_t *)handle)->pen);
					}
					else
					{
						LCD_WriteData16(handle, ((lcd_handle_t *)handle)->backColor);
					}
				}
			}
			break;
		case 16 * 8:
			xe = xs + 8;
			ye = ys + 16;
			LCD_AddressSet(handle, xs, xe - 1, ys, ye - 1);
			LCD_WriteCommand(handle, RAM_WRITE);
			for(uint8_t i = ys; i < ye; i ++)
			{
				for(uint8_t j = xs; j < xe; j ++)
				{
					if(ASCII_1608[Char - ' '][i - ys] & (0x01 << (j - xs)))
					{
						LCD_WriteData16(handle, ((lcd_handle_t *)handle)->pen);
					}
					else
					{
						LCD_WriteData16(handle, ((lcd_handle_t *)handle)->backColor);
					}
				}
			}
			break;
	}

	return 0;
}

int LCD_ShowString(void *handle, uint16_t xs, uint16_t ys, char *str)
{
	uint8_t Step_X = 0; /* ï¿½ï¿½ï¿½ï¿½ */
	switch(((lcd_handle_t *)handle)->fontSize)
	{
		case 12 * 8: Step_X = 8; break;
		case 16 * 8: Step_X = 8; break;
		case 16 * 16: /*non-usage*/; break;
	}
	for(uint8_t i = 0; str[i] != '\0'; i ++)
	{
		LCD_ShowChar(handle, xs + Step_X * i, ys, str[i]);
	}

	return 0;
}

int LCD_ShowChineseChar(lcd_handle_t *handle, uint16_t xs, uint16_t ys, const char *cn)
{
	uint16_t index = 0, xe = 0, ye = 0;
	for(index = 0; index < sizeof(GB_1616) / sizeof(GB_1616[0]); index ++)
	{
		if(strncmp(cn, GB_1616[index].index, 2) == 0)
		{
			break;
		}
	}
	/* ï¿½Ò³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */

	switch(((lcd_handle_t *)handle)->fontSize)
	{
		case 12 * 8: /*non-usage*/; break;
		case 16 * 8: xe = xs + 16; ye = ys + 16; break;
		case 16 * 16: xe = xs + 16; ye = ys + 16; break;
	}
	LCD_AddressSet(handle, xs, xe - 1, ys, ye - 1);
	LCD_WriteCommand(handle, RAM_WRITE);
	spi_start(handle);
	for(uint8_t i = 0; i < ye - ys; i ++)
	{
		for(uint8_t j = 0; j < xe - xs; j ++)
		{
			if(GB_1616[index].content[i * 2 + j / 8] & (0x01 << (j % 8)))
			{
				spi_write(handle, ((lcd_handle_t *)handle)->pen >> 8);
				spi_write(handle, ((lcd_handle_t *)handle)->pen);
			}
			else
			{
				spi_write(handle, ((lcd_handle_t *)handle)->backColor >> 8);
				spi_write(handle, ((lcd_handle_t *)handle)->backColor);
			}
		}
	}
	spi_stop(handle);

	return index;
}

int LCD_ShowChinese(void *handle, uint16_t xs, uint16_t ys, char *str)
{
	uint8_t Step_X = 0; /* ï¿½ï¿½ï¿½ï¿½ */
	switch(((lcd_handle_t *)handle)->fontSize)
	{
		case 12 * 8: /* non-usage */; break;
		case 16 * 8:
		case 16 * 16: Step_X = 16; break;
	}
	for(uint8_t i = 0; *(str + i) != '\0'; i += 2)
	{
		LCD_ShowChineseChar(handle, xs, ys, str + i);
		xs += Step_X;
	}

	return 0;
}


int LCD_Print(void *handle, char *format)
{
	uint16_t index = 0;

	for(index = 0; format[index] != '\0'; index ++)
	{
		if(format[index] < 128)
		{
			if(format[index] == '\r')
			{
				((lcd_handle_t *)handle)->cursor_x = 0;
				continue;
			}
			else if(format[index] == '\n')
			{
				if(((lcd_handle_t *)handle)->cursor_y + 16 > ((lcd_handle_t *)handle)->height)
				{
					((lcd_handle_t *)handle)->cursor_y = 0;
				}
				else
				{
					((lcd_handle_t *)handle)->cursor_y += 16;
				}
				continue;
			}
			else if(format[index] == '\b')
			{
				if(((lcd_handle_t *)handle)->cursor_x >= 8)
				{
					((lcd_handle_t *)handle)->cursor_x -= 8;
				}
				else
				{
					((lcd_handle_t *)handle)->cursor_x = ((lcd_handle_t *)handle)->width - 8;
					if(((lcd_handle_t *)handle)->cursor_y >= 16)
					{
						((lcd_handle_t *)handle)->cursor_y -= 16;
					}
					else
					{
						((lcd_handle_t *)handle)->cursor_x = 0;
						((lcd_handle_t *)handle)->cursor_y = 0;
					}
				}
			}
			else
			{
				LCD_ShowChar(handle, ((lcd_handle_t *)handle)->cursor_x, ((lcd_handle_t *)handle)->cursor_y, format[index]);
			}
			if(((lcd_handle_t *)handle)->cursor_x + 8 > ((lcd_handle_t *)handle)->width)
			{
				
				if(((lcd_handle_t *)handle)->cursor_y + 16 > ((lcd_handle_t *)handle)->height) /*ï¿½ß¶ï¿½ï¿½ï¿½ï¿?*/
				{
					/*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä»ï¿½ï¿½ï¿½ï¿½ï¿½Â´ï¿?0ï¿½ï¿½0ï¿½Ð¿ï¿½Ê¼ï¿½ï¿½Ê¾*/
					((lcd_handle_t *)handle)->cursor_x = 0;
					((lcd_handle_t *)handle)->cursor_y = 0;
				}
				else /*ï¿½ß¶ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿?*/
				{
					((lcd_handle_t *)handle)->cursor_x = 0;
					((lcd_handle_t *)handle)->cursor_y += 16;
				}
			}
		}
		else
		{
			LCD_ShowChineseChar(handle, ((lcd_handle_t *)handle)->cursor_x, ((lcd_handle_t *)handle)->cursor_y, format + index);
			index ++;
			if(((lcd_handle_t *)handle)->cursor_x + 16 > ((lcd_handle_t *)handle)->width)
			{
				
				if(((lcd_handle_t *)handle)->cursor_y + 16 > ((lcd_handle_t *)handle)->height) /*ï¿½ß¶ï¿½ï¿½ï¿½ï¿?*/
				{
					/*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä»ï¿½ï¿½ï¿½ï¿½ï¿½Â´ï¿?0ï¿½ï¿½0ï¿½Ð¿ï¿½Ê¼ï¿½ï¿½Ê¾*/
					((lcd_handle_t *)handle)->cursor_x = 0;
					((lcd_handle_t *)handle)->cursor_y = 0;
				}
				else /*ï¿½ß¶ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿?*/
				{
					((lcd_handle_t *)handle)->cursor_x = 0;
					((lcd_handle_t *)handle)->cursor_y += 16;
				}
			}
		}
	}

	return index;
}

int LCD_Show(void *handle, uint16_t xs, uint16_t ys, char *str)
{
	((lcd_handle_t *)handle)->cursor_x = xs;
	((lcd_handle_t *)handle)->cursor_y = ys;
	LCD_Print(handle, str);
	return 0;
}

int LCD_Init(lcd_handle_t *handle, lcd_init_t *initStructure)
{
	/*ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½*/
	handle->IO = initStructure->IO;
	handle->width = initStructure->width;
	handle->height = initStructure->height;
	handle->fontSize = initStructure->fontSize;
	handle->fontWidth = initStructure->fontWidth;
	handle->fontHeight = initStructure->fontHeight;
	handle->pen = initStructure->pen;
	handle->backColor = initStructure->backColor;
	handle->cursor_x = 0;
	handle->cursor_y = 0;
	/*ï¿½ï¿½ï¿½ï¿½Ó¿ï¿?*/
	handle->clear = LCD_Clear;
	handle->fill = LCD_FillColor;
	handle->drawPoint = LCD_DrawPoint;
	handle->drawLine = LCD_DrawLine;
	handle->drawRectangle = LCD_DrawRectangle;
	handle->drawCircle = LCD_DrawCircle;
	handle->showChar = LCD_ShowChar;
	handle->showString = LCD_ShowString;
	handle->showChinese = LCD_ShowChinese;
	handle->show = LCD_Show;
	handle->print = LCD_Print;
	handle->showPicture = NULL;

	LCD_SET_RES(handle, 0);
	HAL_Delay(1);
	LCD_SET_RES(handle, 1);
	HAL_Delay(120);

	// LCD_SET_BLK(handle, 1);//ï¿½ò¿ª±ï¿½ï¿½ï¿½
	// HAL_Delay(100);

  LCD_WriteCommand(handle, 0x11); // Sleep out
	HAL_Delay(120);					// Delay 120ms
	
	//************* Start Initial Sequence **********//
  LCD_WriteCommand(handle, 0x36);// set MX MY RGB mode
  LCD_WriteData8(handle, 0xC0);

  LCD_WriteCommand(handle, 0x3A);
  LCD_WriteData8(handle, 0x05);

  LCD_WriteCommand(handle, 0xB2);
  LCD_WriteData8(handle, 0x0C);
  LCD_WriteData8(handle, 0x0C);
  LCD_WriteData8(handle, 0x00);
  LCD_WriteData8(handle, 0x33);
  LCD_WriteData8(handle, 0x33);

  LCD_WriteCommand(handle, 0xB7);
  LCD_WriteData8(handle, 0x35);

  LCD_WriteCommand(handle, 0xBB);
  LCD_WriteData8(handle, 0x32); // Vcom=1.35V

  LCD_WriteCommand(handle, 0xC2);
  LCD_WriteData8(handle, 0x01);

  LCD_WriteCommand(handle, 0xC3);
  LCD_WriteData8(handle, 0x15); // GVDD=4.8V  ÑÕÉ«Éî¶È

  LCD_WriteCommand(handle, 0xC4);
  LCD_WriteData8(handle, 0x20); // VDV, 0x20:0v

  LCD_WriteCommand(handle, 0xC6);
  LCD_WriteData8(handle, 0x0F); // 0x0F:60Hz

  LCD_WriteCommand(handle, 0xD0);
  LCD_WriteData8(handle, 0xA4);
  LCD_WriteData8(handle, 0xA1);

  LCD_WriteCommand(handle, 0xE0);
  LCD_WriteData8(handle, 0xD0);
  LCD_WriteData8(handle, 0x08);
  LCD_WriteData8(handle, 0x0E);
  LCD_WriteData8(handle, 0x09);
  LCD_WriteData8(handle, 0x09);
  LCD_WriteData8(handle, 0x05);
  LCD_WriteData8(handle, 0x31);
  LCD_WriteData8(handle, 0x33);
  LCD_WriteData8(handle, 0x48);
  LCD_WriteData8(handle, 0x17);
  LCD_WriteData8(handle, 0x14);
  LCD_WriteData8(handle, 0x15);
  LCD_WriteData8(handle, 0x31);
  LCD_WriteData8(handle, 0x34);

  LCD_WriteCommand(handle, 0xE1);
  LCD_WriteData8(handle, 0xD0);
  LCD_WriteData8(handle, 0x08);
  LCD_WriteData8(handle, 0x0E);
  LCD_WriteData8(handle, 0x09);
  LCD_WriteData8(handle, 0x09);
  LCD_WriteData8(handle, 0x15);
  LCD_WriteData8(handle, 0x31);
  LCD_WriteData8(handle, 0x33);
  LCD_WriteData8(handle, 0x48);
  LCD_WriteData8(handle, 0x17);
  LCD_WriteData8(handle, 0x14);
  LCD_WriteData8(handle, 0x15);
  LCD_WriteData8(handle, 0x31);
  LCD_WriteData8(handle, 0x34);
  LCD_WriteCommand(handle, 0x21);
//------------------------------------End ST7735S Gamma Sequence-----------------------------//

  LCD_WriteCommand(handle, 0x3A); // 65k mode
	LCD_WriteData8(handle, 0x05);
	LCD_Clear(handle);
  LCD_WriteCommand(handle, 0x29); // ´ò¿ªÆÁÄ»

	return 0;
}

#endif
