#ifndef __KEY_H
#define __KEY_H

#include "main.h"

typedef struct key_io_t key_io_t;
typedef struct key_handle_t key_handle_t;

struct key_io_t
{
    host_io_t R1;
    host_io_t R2;
    host_io_t R3;
    host_io_t R4;
    host_io_t C1;
    host_io_t C2;
    host_io_t C3;
    host_io_t C4;
};

struct key_handle_t
{
    key_io_t IO;
    uint16_t currentStatus;
    uint16_t previousStatus;
    uint16_t num;
};

uint16_t Key_GetNum(key_handle_t *handle);

#endif
