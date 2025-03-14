#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct motor_io_t
{
  host_io_t MD0;
  host_io_t MD1;
  host_io_t MD2;
  host_io_t MD3;
} motor_io_t;

typedef struct motor_handle_t
{
  motor_io_t IO;

} motor_handle_t;

#define MOTOR_DIRECTION_FORWARD 1
#define MOTOR_DIRECTION_REVERSE 0

void Motor_Set(motor_handle_t *p, uint8_t data, uint8_t direction);

#ifdef __cplusplus
}
#endif

#endif
