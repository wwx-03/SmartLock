#include "Key.h"

#define KEY_SET_R1(_HANDLE_, _BIT_) ((key_handle_t *)(_HANDLE_))->IO.R1.GPIOx->BSRR = ((key_handle_t *)(_HANDLE_))->IO.R1.pin << 16u * ((_BIT_) == 0)
#define KEY_SET_R2(_HANDLE_, _BIT_) ((key_handle_t *)(_HANDLE_))->IO.R2.GPIOx->BSRR = ((key_handle_t *)(_HANDLE_))->IO.R2.pin << 16u * ((_BIT_) == 0)
#define KEY_SET_R3(_HANDLE_, _BIT_) ((key_handle_t *)(_HANDLE_))->IO.R3.GPIOx->BSRR = ((key_handle_t *)(_HANDLE_))->IO.R3.pin << 16u * ((_BIT_) == 0)
#define KEY_SET_R4(_HANDLE_, _BIT_) ((key_handle_t *)(_HANDLE_))->IO.R4.GPIOx->BSRR = ((key_handle_t *)(_HANDLE_))->IO.R4.pin << 16u * ((_BIT_) == 0)

#define KEY_READ_C1(_HANDLE_) (((key_handle_t *)(_HANDLE_))->IO.C1.GPIOx->IDR & ((key_handle_t *)(_HANDLE_))->IO.C1.pin)
#define KEY_READ_C2(_HANDLE_) (((key_handle_t *)(_HANDLE_))->IO.C2.GPIOx->IDR & ((key_handle_t *)(_HANDLE_))->IO.C2.pin)
#define KEY_READ_C3(_HANDLE_) (((key_handle_t *)(_HANDLE_))->IO.C3.GPIOx->IDR & ((key_handle_t *)(_HANDLE_))->IO.C3.pin)
#define KEY_READ_C4(_HANDLE_) (((key_handle_t *)(_HANDLE_))->IO.C4.GPIOx->IDR & ((key_handle_t *)(_HANDLE_))->IO.C4.pin)

uint16_t Key_GetNum(key_handle_t *handle)
{
	static uint32_t previousMillis = 0;
	uint32_t currentMillis = HAL_GetTick();
	
	if(currentMillis - previousMillis >= 20)
	{
		previousMillis = currentMillis;
		handle->previousStatus = handle->currentStatus;
	}
	
	KEY_SET_R1(handle, 1); KEY_SET_R2(handle, 0); KEY_SET_R3(handle, 0); KEY_SET_R4(handle, 0);
	if(KEY_READ_C1(handle) != 0) handle->currentStatus = 1; else handle->currentStatus = 0;
	if(KEY_READ_C2(handle) != 0) handle->currentStatus = 2; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C3(handle) != 0) handle->currentStatus = 3; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C4(handle) != 0) handle->currentStatus = 4; else handle->currentStatus = handle->currentStatus;

	KEY_SET_R1(handle, 0); KEY_SET_R2(handle, 1); KEY_SET_R3(handle, 0); KEY_SET_R4(handle, 0);
	if(KEY_READ_C1(handle) != 0) handle->currentStatus = 5; else handle->currentStatus = handle->currentStatus;;
	if(KEY_READ_C2(handle) != 0) handle->currentStatus = 6; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C3(handle) != 0) handle->currentStatus = 7; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C4(handle) != 0) handle->currentStatus = 8; else handle->currentStatus = handle->currentStatus;

	KEY_SET_R1(handle, 0); KEY_SET_R2(handle, 0); KEY_SET_R3(handle, 1); KEY_SET_R4(handle, 0);
	if(KEY_READ_C1(handle) != 0) handle->currentStatus = 9; else handle->currentStatus = handle->currentStatus;;
	if(KEY_READ_C2(handle) != 0) handle->currentStatus = 10; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C3(handle) != 0) handle->currentStatus = 11; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C4(handle) != 0) handle->currentStatus = 12; else handle->currentStatus = handle->currentStatus;

	KEY_SET_R1(handle, 0); KEY_SET_R2(handle, 0); KEY_SET_R3(handle, 0); KEY_SET_R4(handle, 1);
	if(KEY_READ_C1(handle) != 0) handle->currentStatus = 13; else handle->currentStatus = handle->currentStatus;;
	if(KEY_READ_C2(handle) != 0) handle->currentStatus = 14; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C3(handle) != 0) handle->currentStatus = 15; else handle->currentStatus = handle->currentStatus;
	if(KEY_READ_C4(handle) != 0) handle->currentStatus = 16; else handle->currentStatus = handle->currentStatus;

	
	if(handle->previousStatus != 0 && handle->currentStatus == 0)
	{
		handle->num = handle->previousStatus;
		handle->previousStatus = 0;
	}
	else
	{
		handle->num = 0;
	}

	return handle->num;

	
}