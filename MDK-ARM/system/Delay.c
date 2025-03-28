/*
Systick功能实现us延时，参数SYSCLK为系统时钟
*/
#include "Delay.h"

void Delay_us(uint32_t udelay)
{
    uint32_t startval, tickn, delays, wait;

    startval = SysTick->VAL;
    tickn = HAL_GetTick();
    // sysc = 72000;  //SystemCoreClock / (1000U / uwTickFreq);
    delays = udelay * 72; // sysc / 1000 * udelay;
    if (delays > startval)
    {
        while (HAL_GetTick() == tickn)
        {
        }
        wait = 72000 + startval - delays;
        while (wait < SysTick->VAL)
        {
        }
    }
    else
    {
        wait = startval - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn)
        {
        }
    }
}
