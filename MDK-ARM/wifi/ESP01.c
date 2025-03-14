#include "ESP01.h"
#include "stm32f1xx_hal.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define ESP_SET_RST(_H_, _B_) ((esp_handle_t *)_H_)->io.rst.GPIOx->BSRR = ((esp_handle_t *)_H_)->io.rst.pin << 16u * ((_B_) == 0)

int ESP_TransmitCommand(esp_handle_t *handle, uint32_t timeout, char *cmd, ...)
{
  int retval = 0;
  uint32_t endMillis = 0;
  va_list args;
  memset(handle->txBufferAddr, '\0', handle->txBufferSize);
  va_start(args, cmd);
  vsprintf((char *)handle->txBufferAddr, cmd, args);
  va_end(args);

  endMillis = HAL_GetTick() + timeout;
  do {
    retval = HAL_UART_Transmit_DMA(handle->huartx, handle->txBufferAddr, strlen((char *)handle->txBufferAddr));
  } while(HAL_GetTick() <= endMillis && retval != HAL_OK);
  return retval;
}

void ESP_Init(esp_handle_t *p, const char *ssid, const char *password)
{
  strcpy(p->ssid, ssid);
  strcpy(p->pwd, password);
}

int ESP_Run(esp_handle_t *p)
{
  static int status = 0;
  static uint32_t previousMillis = 0, time = 0;
  uint32_t currentMillis = 0;

  currentMillis = HAL_GetTick();
  if(currentMillis - previousMillis <= time)
  {
    return -1;
  }
  previousMillis = currentMillis;

  switch(status)
  {
    case 0:
      ESP_SET_RST(p, 0);
      HAL_Delay(5);
      ESP_SET_RST(p, 1);
      time = 2000;
      status ++;
      break;
    case 1:
      if(strstr((char *)p->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p, 1000, "AT\r\n");
      time = 1000;
      break;
    case 2:
      if(strstr((char *)p->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p, 1000, "AT+CWMODE=1\r\n");
      time = 1000;
      break;
    case 3:
      if(strstr((char *)p->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p, 1000, "AT+CWDHCP=1,1\r\n");
      time = 1000;
      break;
    case 4:
      if(strstr((char *)p->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p, 1000, "AT+CWJAP=\"%s\",\"%s\"\r\n", p->ssid, p->pwd);
      time = 5000;
      break;
    case 5: 
      time = 0;
      p->status = 1;
      break;
    default: break;
  }

  return status;
}

