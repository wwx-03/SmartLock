#include <stdio.h>
#include "display.h"

int Display_Clear(void *dis_if)
{
  return ((display_if_t *)dis_if)->clear(dis_if);
}

int Display_Fill(void *dis_if, unsigned short xStart, unsigned short xEnd, unsigned short yStart, unsigned short yEnd, unsigned short color)
{
  return ((display_if_t *)dis_if)->fill(dis_if, xStart, xEnd, yStart, yEnd, color);
}

int Display_DrawPoint(void *dis_if, unsigned short x, unsigned short y)
{
  return ((display_if_t *)dis_if)->drawPoint(dis_if, x, y);
}

int Display_DrawLine(void *dis_if, unsigned short xStart, unsigned short xEnd, unsigned short yStart, unsigned short yEnd)
{
  return ((display_if_t *)dis_if)->drawLine(dis_if, xStart, xEnd, yStart, yEnd);
}

int Display_DrawRectangle(void *dis_if, unsigned short xStart, unsigned short xEnd, unsigned short yStart, unsigned short yEnd)
{
  return ((display_if_t *)dis_if)->drawRectangle(dis_if, xStart, xEnd, yStart, yEnd);
}

int Display_DrawCircle(void *dis_if, unsigned short x, unsigned short y, unsigned char r)
{
  return ((display_if_t *)dis_if)->drawCircle(dis_if, x, y, r);
}

int Display_ShowChar(void *dis_if, unsigned short x, unsigned short y, char c)
{
  return ((display_if_t *)dis_if)->showChar(dis_if, x, y, c);
}

int Display_ShowString(void *dis_if, unsigned short x, unsigned short y, char *str)
{
  return ((display_if_t *)dis_if)->showString(dis_if, x, y, str);
}

int Display_ShowChinese(void *dis_if, unsigned short x, unsigned short y, char *str)
{
  return ((display_if_t *)dis_if)->showChinese(dis_if, x, y, str);
}

int Display_Show(void *dis_if, unsigned short x, unsigned short y, char *str, ...)
{
  char buffer[128] = {0};
	va_list args;
	va_start(args, str);
	vsprintf(buffer, str, args);
	va_end(args);
  
  return ((display_if_t *)dis_if)->show(dis_if, x, y, buffer);
}

int Display_Print(void *dis_if, char *format, ...)
{
  char buffer[128] = {0};
  va_list arg;
  va_start(arg, format);
  vsprintf(buffer, format, arg);
  va_end(arg);
  return ((display_if_t *)dis_if)->print(dis_if, buffer);
}

int Display_ShowPicture(void *dis_if, unsigned short x, unsigned short y, unsigned short length, unsigned short width, const unsigned char *pic)
{
  return ((display_if_t *)dis_if)->showPicture(dis_if, x, y, length, width, pic);
}
