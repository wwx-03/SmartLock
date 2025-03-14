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

/* FIFO�������忪ʼ */
int RC522_WriteFIFOBuffer(rc522_handle_t *handle, uint8_t *data, uint8_t data_length);
int RC522_ReadFIFOBuffer(rc522_handle_t *handle, uint8_t *store_arry, uint8_t length);
void RC522_ClearFIFOBuffer(rc522_handle_t *handle);
/* FIFO����������� */

/* ��ʱ����Ԫ�������忪ʼ */
uint8_t RC522_SetTMode(rc522_handle_t *handle, uint8_t TAuto, uint8_t TGated, uint8_t TAutoRestart);
int RC522_SetTPrescaler(rc522_handle_t *handle, uint16_t psc);
int RC522_SetTReload(rc522_handle_t *handle, uint16_t reload);
uint16_t RC522_ReadTCounter(rc522_handle_t *handle);
void RC522_SwitchTimer(rc522_handle_t *handle, uint8_t status);
/* ��ʱ����Ԫ����������� */

int RC522_PcdCommand(rc522_handle_t *handle, 
                     uint8_t rc522_command, 
                     uint8_t *data_to_fifo, 
                     size_t data_len, 
                     uint8_t *fifo_to_data, 
                     size_t *fifo_bit_len);

int RC522_PcdRequest(rc522_handle_t *handle, uint8_t req_code, uint8_t *tag_type);
int RC522_PcdAnticollsion(rc522_handle_t *handle, uint8_t *uid);

/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
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
 * RC522�Ĵ���������
 * CommandReg Register	����Ĵ���	��ַ 0x01	��λֵ	0x20
 * 
 * Bit		7			6			5			4			3			2			1			0
 * ��ǩ								RcvOff		PowerDown				Command[3:0]
 * RcvOff���ø߹ر�ģ�ⲿ��
 * PowerDown���ø߽������µ�ģʽ���õ���ΪRC522�������ѹ��̡�����������У���λ��Ϊ1��Ϊ0��ʾ�Ѿ�׼���á�
 * Command������������뼤��һ�������ȡ�˼Ĵ�������ʾʵ��ִ�����ĸ��������18.3�ڡ�MFRC522�������������
 * ����						�������			����
 * Idle						0000			û�ж�����ȡ����ǰ������ִ�С�
 * Mem						0001			���ڲ��������洢25���ֽڡ�
 * Generate RandomID		0010			����һ��10�ֽڵ����ID�š�
 * CalcCRC					0011			����CRCЭ������������Լ졣
 * Transmit					0100			��FIFO��������������
 * NoCmdChange				0111			û��������ġ�����������ڲ����������������޸�����Ĵ����еĲ�ͬλ������ʡ�硣
 * Receive					1000			�����������Դ��
 * Transceive				1100			�����ݴ�FIFO���������䵽���ߣ����ڴ�����Զ������������
 * MFAuthent				1110			��Ϊ��ȡ��ִ��MIFARE��׼��֤
 * SoftReset				1111			��λRC522��
 * 
 * 
 * TxControlReg ���ܣ�����������������Tx1��Tx2����Ϊ
 * ��ַ��0x14		��λֵ��0x80
 * 
 * Bit		7			6			5			4			3			2			1			0
 * 		InvTx2RFOn	InvTx1RFOn	InvTx2RFOff	InvTx1RFOff	  Tx2CW					Tx2RFEn		Tx1RFEN
 * 
 * InvTx2RFOn����Tx2���ŷ�ת����źţ����Tx2ʹ�ܵĻ���
 * InvTx1RFOn����Tx1���ŷ�ת����źţ����Tx1ʹ�ܵĻ���
 * InvTx2RFOff���ر���Tx2���ŵķ�ת���ܡ�
 * InvTx1RFOff���ر���Tx1���ŵķ�ת���ܡ�
 * Tx2CW��ʹ�ܸ�λ����Tx2������ƹ���13.56MHz�ز���ʧ�������δ���Ƶ��ز���
 * Tx2RFEN��ʹ�ܸ�λ����TX2����������ƹ���13.56MHz���ز���
 * Tx1RFEN��ʹ�ܸ�λ����Tx1����������ƹ���13.56MHz���ز���
 */
