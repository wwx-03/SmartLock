#ifndef __WIFI_H
#define __WIFI_H

#include "usart.h"

#define ESP_USER_WIFI_SSID "desktop"
#define ESP_USER_WIFI_PASSWORD "wwx12345"

typedef struct esp_io_t
{
	host_io_t rst;
} esp_io_t;

typedef struct esp_handle_t
{
	UART_HandleTypeDef *huartx;

	uint8_t *rxBufferAddr;
	uint16_t rxBufferSize;
	uint8_t *txBufferAddr;
	uint16_t txBufferSize;

	char ssid[32];
	char pwd[32];

	esp_io_t io;

	uint8_t status;
} esp_handle_t;

int ESP_TransmitCommand(esp_handle_t *handle, uint32_t timeout, char *cmd, ...);
void ESP_Init(esp_handle_t *handle, const char *ssid, const char *password);
int ESP_Run(esp_handle_t *p);

#endif
