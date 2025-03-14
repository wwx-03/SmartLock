#ifndef __LCDCMD_H
#define __LCDCMD_H

typedef enum lcd_cmd_t
{
	NOP = 0x00,					/* 什么都不做 */
	SOFTWARE_RESET = 0x01,		/* 软件复位 */
	READ_DISPLAY_ID = 0x02,		/* 读取显示ID */
	READ_DISPLAY_STATUS = 0x03, /*  */
	READ_DISPLAY_POWER_MODE = 0x04,

	SLEEP_IN = 0x10,
	SLEEP_OUT = 0x11,
	PARTIAL_MODE_ON = 0x12,
	PARTIAL_MODE_OFF = 0x13,
	DISPLAY_INVERSION_OFF = 0x20,
	DISPLAY_INVERSION_ON = 0x21,
	GAMMA_CURVE_SELECT = 0x26,
	DISPLAY_OFF = 0x28,
	DISPLAY_ON = 0x29,
	COLUMN_ADDRESS_SET = 0x2a,
	ROW_ADDRESS_SET = 0x2b,
	RAM_WRITE = 0x2c,
	RGB_SET = 0x2d,
	RAM_REAR = 0x2e,
	PARTIAL_ADDRESS_SET = 0x30,
	SCROLL_AREA_SET = 0x33,
	TEARING_EFFECT_LINE_OFF = 0x34,
	TEARING_EFFECT_MODE_ON = 0x35,
	MEMORY_DATA_ACCESS_CONTROL = 0x36, /* 定义帧内存读写扫描的方向 */
	SCROLL_START_ADDRESS_OF_RAM = 0x37,
	IDLE_MODE_OFF = 0x38,
	IDLE_MODE_ON = 0x39,
	INTERFACE_PIXEL_FORMAT = 0x3a,
	FRAME_RATE_CONTROL_1 = 0xb1, /* 在正常模式下的帧速率设置 */
	FRAME_RATE_CONTROL_2 = 0xb2, /* 在空闲模式下的帧速率设置 */
	FRAME_RATE_CONTROL_3 = 0xb3, /* 在部分显示模式下的帧速率设置 */
	POWER_CONTROL_1 = 0xc0, /* 在正常模式下的电源控制 */
	POWER_CONTROL_2 = 0xc1, /* 在正常模式下的电源控制 */
	POWER_CONTROL_3 = 0xc2, /* 在正常模式下的电源控制 */
	POWER_CONTROL_4 = 0xc3, /* 在正常模式下的电源控制 */
	POWER_CONTROL_5 = 0xc4, /* 在正常模式下的电源控制 */
} lcd_cmd_t;

#endif
