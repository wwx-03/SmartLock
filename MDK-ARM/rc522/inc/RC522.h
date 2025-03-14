#ifndef __RC522_H
#define __RC522_H

#include "main.h"
#include "RC522_RegDef.h"

#define USE_RC522PROTOCOL_SPI

typedef struct rc522_io_t rc522_io_t;
typedef struct rc522_register_t rc522_register_t;
typedef struct rc522_handle_t rc522_handle_t;

struct rc522_io_t
{
  host_io_t SDA;
  host_io_t SCK;
  host_io_t MOSI;
  host_io_t MISO;
  host_io_t IRQ;
  host_io_t RST;
};
struct rc522_register_t
{
  uint8_t address;
  uint8_t value;
};
struct rc522_handle_t
{
  rc522_io_t IO;
  rc522_register_t reg;
};

uint8_t RC522_WriteRegister(rc522_handle_t *handle, uint8_t reg_addr, uint8_t command);
uint8_t RC522_ReadRegister(rc522_handle_t *handle, uint8_t reg_addr);
uint8_t RC522_SetRegisterBitMask(rc522_handle_t *handle, uint8_t reg_addr, uint8_t bitmask);
uint8_t RC522_ClearRegisterBitMask(rc522_handle_t *handle, uint8_t reg_addr, uint8_t bitmask);

int RC522_Init(rc522_handle_t *handle);

/* FIFO函数定义开始 */
int RC522_WriteFIFOBuffer(rc522_handle_t *handle, uint8_t *data, uint8_t data_length);
int RC522_ReadFIFOBuffer(rc522_handle_t *handle, uint8_t *store_arry, uint8_t length);
void RC522_ClearFIFOBuffer(rc522_handle_t *handle);
/* FIFO函数定义结束 */

/* 定时器单元函数定义开始 */
uint8_t RC522_SetTMode(rc522_handle_t *handle, uint8_t TAuto, uint8_t TGated, uint8_t TAutoRestart);
int RC522_SetTPrescaler(rc522_handle_t *handle, uint16_t psc);
int RC522_SetTReload(rc522_handle_t *handle, uint16_t reload);
uint16_t RC522_ReadTCounter(rc522_handle_t *handle);
void RC522_SwitchTimer(rc522_handle_t *handle, uint8_t status);
/* 定时器单元函数定义结束 */

int RC522_PcdCommand(rc522_handle_t *handle, 
                     uint8_t rc522_command, 
                     uint8_t *data_to_fifo, 
                     size_t data_len, 
                     uint8_t *fifo_to_data, 
                     size_t *fifo_bit_len);

int RC522_PcdRequest(rc522_handle_t *handle, uint8_t req_code, uint8_t *tag_type);
int RC522_PcdAnticollsion(rc522_handle_t *handle, uint8_t *uid);

/////////////////////////////////////////////////////////////////////
//MF522命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位
#define PCD_CALCCRC           0x03               //CRC计算

/////////////////////////////////////////////////////////////////////
//Mifare_One卡片命令字
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define MI_OK                          0
#define MI_NOTAGERR                    (-1)
#define MI_ERR                         (-2)

#define RC522_WRITE					   0
#define RC522_READ					   1
#define PCD_OPEN					   1
#define PCD_CLOSE					   0

#endif

/**
 * 
 * RC522寄存器描述：
 * CommandReg Register	命令寄存器	地址 0x01	复位值	0x20
 * 
 * Bit		7			6			5			4			3			2			1			0
 * 标签								RcvOff		PowerDown				Command[3:0]
 * RcvOff：置高关闭模拟部分
 * PowerDown：置高进入软下电模式。置低则为RC522启动唤醒过程。在这个过程中，该位仍为1，为0表示已经准备好。
 * Command：根据命令代码激活一个命令。读取此寄存器可显示实际执行了哪个命令（见第18.3节“MFRC522命令概述”）。
 * 命令						命令代码			概述
 * Idle						0000			没有动作；取消当前的命令执行。
 * Mem						0001			在内部缓冲区存储25个字节。
 * Generate RandomID		0010			生成一个10字节的随机ID号。
 * CalcCRC					0011			激活CRC协处理器或进行自检。
 * Transmit					0100			从FIFO缓冲区传输数据
 * NoCmdChange				0111			没有命令更改。该命令可以在不触及命令的情况下修改命令寄存器中的不同位。例如省电。
 * Receive					1000			激活接收器电源。
 * Transceive				1100			将数据从FIFO缓冲区传输到天线，并在传输后自动激活接收器。
 * MFAuthent				1110			作为读取器执行MIFARE标准认证
 * SoftReset				1111			复位RC522。
 * 
 * 
 * TxControlReg 功能：控制天线驱动引脚Tx1和Tx2的行为
 * 地址：0x14		复位值：0x80
 * 
 * Bit		7			6			5			4			3			2			1			0
 * 		InvTx2RFOn	InvTx1RFOn	InvTx2RFOff	InvTx1RFOff	  Tx2CW					Tx2RFEn		Tx1RFEN
 * 
 * InvTx2RFOn：在Tx2引脚反转输出信号，如果Tx2使能的话。
 * InvTx1RFOn：在Tx1引脚反转输出信号，如果Tx1使能的话。
 * InvTx2RFOff：关闭在Tx2引脚的反转功能。
 * InvTx1RFOff：关闭在Tx1引脚的反转功能。
 * Tx2CW：使能该位则在Tx2输出调制过的13.56MHz载波，失能则输出未调制的载波。
 * Tx2RFEN：使能该位则在TX2引脚输出调制过的13.56MHz的载波。
 * Tx1RFEN：使能该位则在Tx1引脚输出调制过的13.56MHz的载波。
 */
