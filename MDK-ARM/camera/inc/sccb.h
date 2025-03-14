#ifndef __SCCB_H
#define __SCCB_H

#include "main.h"

typedef struct sccb_io_t
{
    host_io_t scl;
    host_io_t sda;
}sccb_io_t;

typedef struct sccb_handle_t
{
    sccb_io_t io;
    unsigned char id_addr; 
    unsigned char sub_addr;
    unsigned char wdata;
    unsigned char rdata;
}sccb_handle_t;

int SCCB_WriteData(sccb_handle_t *handle, unsigned char sub_addr, unsigned char data);
int SCCB_ReadData(sccb_handle_t *handle, unsigned char sub_addr);

#define SCCB_SDA(__HANDLE__, __BIT_STATUS__) ((sccb_handle_t *)__HANDLE__)->io.sda.GPIOx->BSRR = ((sccb_handle_t *)__HANDLE__)->io.sda.pin << (16u * ((__BIT_STATUS__) == 0))
#define SCCB_READ_SDA(__HANDLE__) ((sccb_handle_t *)__HANDLE__)->io.sda.GPIOx->IDR & ((sccb_handle_t *)__HANDLE__)->io.sda.pin

#define SCCB_SCL(__HANDLE__, __BIT_STATUS__) ((sccb_handle_t *)__HANDLE__)->io.scl.GPIOx->BSRR = ((sccb_handle_t *)__HANDLE__)->io.scl.pin << (16u * ((__BIT_STATUS__) == 0))

#endif
