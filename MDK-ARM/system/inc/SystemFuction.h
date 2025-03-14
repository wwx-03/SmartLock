/**
 * file name "SystemFunction.h"
 * file encoding "GB2312"
 */
#ifndef __SYSTEMFUNCTION_H
#define __SYSTEMFUNCTION_H

#define USE_ERROR_LOCK 1	//使用错误过多锁定

#if(USE_ERROR_LOCK == 1)

#define MAX_ERROR_TIME 5	//最多错误次数定义

#endif

#include "AS608.h"
#include "lcd.h"
#include "display.h"
#include "password.h"
#include "Key.h"
#include "RC522.h"
#include "OV2640.h"
#include "onenet.h"
#include "motor.h"

#define MAX_SYSTEM_FUNCTIONAL_MEMBERS 32
#define SYSTEM_UNLOCK_MILLISECONDS 8000
#define SYSTEM_ERRORLOCK_MILLISECONDS 180000

typedef struct system_handle_t system_handle_t;

typedef int (*func_t)(system_handle_t *);

struct system_handle_t
{
	func_t funcArry[MAX_SYSTEM_FUNCTIONAL_MEMBERS]; //函数指针
  char funcName[MAX_SYSTEM_FUNCTIONAL_MEMBERS][32]; //函数名

	//系统设备
	lcd_handle_t *lcd;
	as608_handle_t *as608;
	rc522_handle_t *rc522;
	esp_handle_t *esp;
	ov2640_handle_t *ov2640;
	key_handle_t *key;
	password_structure_t *password;
	onenet_handle_t *onenet;
#if(USE_ERROR_LOCK == 1)
	TIM_HandleTypeDef *htim;
#endif
	motor_handle_t *motor;
	
	uint8_t index; //内容索引
	uint8_t pageContent; //每页有多少个功能
	uint8_t content; //总共有多少个功能
	uint8_t deviceStatus; //设备状态	0表示正常, 1表示异常
	uint8_t lockStatus; // 门锁状态
	uint32_t lockStartMillis;	//解锁起始时间
	uint32_t lockEndMillis; //解锁结束时间

#if(USE_ERROR_LOCK == 1) 
	int errorTimes; //密码错误次数
	uint32_t errorLockStartMillis; //锁死时间
	uint32_t errorUnlockMillis; //解锁时间
	uint8_t errorLock;	//错误锁死
#endif
};


void System_ShowCursor(system_handle_t *system);//显示光标
void System_ShowFunction(system_handle_t *system);//显示功能
void System_DoFunction(system_handle_t *system);//执行功能

int System_UnlockByPassword(system_handle_t *system);//通过密码解锁
int System_ChangePassword(system_handle_t *system);//更改密码

int System_UnlockByFingerprint(system_handle_t *system);//通过指纹解锁
int System_AddFingerprint(system_handle_t *system);//添加指纹
int System_DeleteFingerprint(system_handle_t *system);//删除指纹
int System_SearchFingerprintNum(system_handle_t *system);//查看指纹数量

int System_WatchMonitor(system_handle_t *system);//查看监控
int System_UnlockByIDCard(system_handle_t *system);
int System_AddIDCard(system_handle_t *system);
int System_Init(system_handle_t *sys, uint8_t pageContent);//系统初始化
int System_DeInit(system_handle_t *system);//系统反初始化

int System_ShowRestErrorMillis(system_handle_t *system);
int System_UpdateLockStatus(system_handle_t *system);
int System_UpdataMotor(system_handle_t *system);

#define SYSTEM_CANCELED -1
#define SYSTEM_PASSWORD_SET_SUCCESS 1

#define SYSTEM_INIT_OK 0
#define SYSTEM_INIT_ERROR_LCD -1
#define SYSTEM_INIT_ERROR_AS608 -2
#define SYSTEM_INIT_ERRPR_OV2640 -3
#define SYSTEM_INIT_ERROR_RC522 -4

#define SYSTEM_DEVICE_ERROR_AS608 1
#define SYSTEM_DEVICE_ERROR_OV2640 2
#define SYSTEM_DEVICE_ERROR_RC522 4
#define SYSTEM_DEVICE_ERROR_ESP01 8

#endif
