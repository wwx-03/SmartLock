#include "motor.h"
#include <string.h>

static const uint8_t forward[4] = {0x03, 0x06, 0x0c, 0x09};
static const uint8_t reverse[4] = {0x03, 0x09, 0x0c, 0x06};
static const uint8_t *pArry;

#define Motor_SET_MD0(_H_, _B_) (_H_)->IO.MD0.GPIOx->BSRR = (_H_)->IO.MD0.pin << 16u * ((_B_) == 0)
#define Motor_SET_MD1(_H_, _B_) (_H_)->IO.MD1.GPIOx->BSRR = (_H_)->IO.MD1.pin << 16u * ((_B_) == 0)
#define Motor_SET_MD2(_H_, _B_) (_H_)->IO.MD2.GPIOx->BSRR = (_H_)->IO.MD2.pin << 16u * ((_B_) == 0)
#define Motor_SET_MD3(_H_, _B_) (_H_)->IO.MD3.GPIOx->BSRR = (_H_)->IO.MD3.pin << 16u * ((_B_) == 0)

void Motor_Set(motor_handle_t *p, uint8_t data, uint8_t direction)
{
  pArry = direction == MOTOR_DIRECTION_FORWARD ? forward : reverse;
  uint8_t temp = pArry[data];
  switch(temp)
  {
    case 0x00: Motor_SET_MD0(p, 0); Motor_SET_MD1(p, 0); Motor_SET_MD2(p, 0); Motor_SET_MD3(p, 0); break;
    case 0x03: Motor_SET_MD0(p, 1); Motor_SET_MD1(p, 1); Motor_SET_MD2(p, 0); Motor_SET_MD3(p, 0); break;
    case 0x06: Motor_SET_MD0(p, 0); Motor_SET_MD1(p, 1); Motor_SET_MD2(p, 1); Motor_SET_MD3(p, 0); break;
    case 0x09: Motor_SET_MD0(p, 1); Motor_SET_MD1(p, 0); Motor_SET_MD2(p, 0); Motor_SET_MD3(p, 1); break;
    case 0x0c: Motor_SET_MD0(p, 0); Motor_SET_MD1(p, 0); Motor_SET_MD2(p, 1); Motor_SET_MD3(p, 1); break;
    default: Motor_SET_MD0(p, 1); Motor_SET_MD1(p, 1); Motor_SET_MD2(p, 0); Motor_SET_MD3(p, 0); break;
  }
}
