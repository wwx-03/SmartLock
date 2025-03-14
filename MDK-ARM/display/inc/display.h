#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdarg.h>

#if defined (__CC_ARM)
#pragma anon_unions
#endif

typedef struct display_if_t display_if_t;

struct display_if_t
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
    int (*show)(void *, unsigned short, unsigned short, const char *);
    int (*print)(void *, char */*format*/);

    int (*showPicture)(void *, unsigned short /*x*/, unsigned short /*y*/, unsigned short /*length*/, unsigned short /*width*/, const unsigned char */*pic[]*/);
};

int Display_Clear(void *dis_if);
int Display_Fill(void *dis_if, unsigned short xStart, unsigned short xEnd, unsigned short yStart, unsigned short yEnd, unsigned short color);
int Display_DrawPoint(void *dis_if, unsigned short x, unsigned short y);
int Display_DrawLine(void *dis_if, unsigned short xStart, unsigned short xEnd, unsigned short yStart, unsigned short yEnd);
int Display_DrawRectangle(void *dis_if, unsigned short xStart, unsigned short xEnd, unsigned short yStart, unsigned short yEnd);
int Display_DrawCircle(void *dis_if, unsigned short x, unsigned short y, unsigned char r);
int Display_ShowChar(void *dis_if, unsigned short x, unsigned short y, char c);
int Display_ShowString(void *dis_if, unsigned short x, unsigned short y, char *str);
int Display_ShowChinese(void *dis_if, unsigned short x, unsigned short y, char *str);
int Display_Show(void *dis_if, unsigned short x, unsigned short y, char *str, ...);
int Display_Print(void *dis_if, char *format, ...);
int Display_ShowPicture(void *dis_if, unsigned short x, unsigned short y, unsigned short length, unsigned short width, const unsigned char *pic);

#endif
