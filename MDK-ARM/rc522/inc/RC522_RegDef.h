#ifndef __RC522_REGDEF_H
#define __RC522_REGDEF_H

typedef enum commienreg_bitmask_t
{
    TIMERIEN = 0x01, /*����ʱ���ж�����*/
    ERRIEN = 0x02, /*��������ж�����*/
    LOALERTIEN = 0x04, /*����FIFO����ˮƽ�߾�������*/
    HIALERTIEN = 0x08, /*����FIFO����ˮƽ�߾�������*/
    IDLEIEN = 0x10, /*��������ж�����*/
    RXIEN = 0x20, /*����������ж�����*/
    TXIEN = 0x40, /*���������ж�����*/
    IRQINV = 0x80, /*IRQ�����źŵ���*/
}commienreg_bitmask_t;

typedef enum divienreg_bitmask_t
{
    RFU0 = 0x01,
    RFU1 = 0x02,
    CRCIEN = 0x04,
    RFU3 = 0x80,
    MFINACTIEN = 0x10,
    RFU5 = 0x20,
    RFU6 = 0x40,
    IRQ_PUSH_PULL = 0x80,
}divienreg_bitmask_t;

// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01			/* ������ֹͣ�����ִ�� */
#define     CommIEnReg            0x02    		/* ʹ�ܻ�ʧ���ж�����λ */
#define     DivlEnReg             0x03    		/* ʹ�ܻ�ʧ���ж�����λ */
#define     ComIrqReg             0x04    		/* �����ж�����λ */
#define     DivIrqReg             0x05			/* �����ж�����λ */
#define     ErrorReg              0x06    		/* չʾ�ϴ�����ִ�еĴ����� */
#define     Status1Reg            0x07    		/* ����CRC���жϣ�FIFO������״̬λ */
#define     Status2Reg            0x08   		/* ������������������������ģʽ�������״̬λ */
#define     FIFODataReg           0x09			/* ��������64�ֽ�FIFO������ */
#define     FIFOLevelReg          0x0A			/* ָʾFIFO�д洢���ֽ��� */
#define     WaterLevelReg         0x0B			/* ����FIFO������������������ */
#define     ControlReg            0x0C			/* �������λ */
#define     BitFramingReg         0x0D			/* λ�����ܵĵ��� */
#define     CollReg               0x0E			/* ��������Ƶ�ӿ��ϼ�⵽�ĵ�һ��λ��ͻ */
#define     RFU0F                 0x0F			/* ���� */
// PAGE 1     
#define     RFU10                 0x10			/* ���� */
#define     ModeReg               0x11			/* ���巢�ͺͽ��յ�һ��ģʽ���á� */
#define     TxModeReg             0x12			/* ���崫������е��������� */
#define     RxModeReg             0x13			/* ��������ڼ���������� */
#define     TxControlReg          0x14			/* ������������������Tx1��Tx2���߼���Ϊ */
#define     TxASKReg              0x15			/* ���Ʒ��͵��Ƶ����� */
#define     TxSelReg              0x16			/* Ϊģ�ⲿ��ѡ���ڲ�Դ */
#define     RxSelReg              0x17			/* ѡ���ڲ����������� */
#define     RxThresholdReg        0x18			/* ѡ��λ����������ֵ */
#define     DemodReg              0x19			/* ������������ */
#define     RFU1A                 0x1A			/* ���� */
#define     RFU1B                 0x1B			/* ���� */
#define     MfTxReg	           	  0x1C			/* ����һЩMIFAREͨ�Ŵ������ */
#define     MfRxReg               0x1D			/* ����һЩMIFAREͨ�Ŵ������ */
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F			/* ѡ��UART�ӿڴ������� */
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21			/* ��ʾCRC�����ʵ��MSBֵ */
#define     CRCResultRegL         0x22			/* ��ʾCRC�����ʵ��LSBֵ */
#define     RFU23                 0x23			
#define     ModWidthReg           0x24			/* ���Ƶ��ƿ������ */
#define     RFU25                 0x25			
#define     RFCfgReg              0x78			/* ���ý������� */
#define     GsNReg                0x27			/* ѡ��������ͨʱ��������������TX1��TX2��n������ͨ�� */
#define     CWGsPReg      	      0x28			/* �����޵���ʱp -�������ĵ絼 */
#define     ModGsPReg             0x29			/* ��������ڼ��������p����絼 */
#define     TModeReg              0x2A			/* ���嶨ʱ�������� */
#define     TPrescalerReg         0x2B			/* ���嶨ʱ�������� */
#define     TReloadRegH           0x2C			/* ����16λ����ʱ�������¼���ֵ��8λ */
#define     TReloadRegL           0x2D			/* ����16λ����ʱ�������¼���ֵ��8λ */
#define     TCounterValueRegH     0x2E			/* ������ʱ���ļ���ֵ�ĸ�8λ */
#define     TCounterValueRegL     0x2F			/* ������ʱ���ļ���ֵ�ĵ�8λ */
// PAGE 3      
#define     RFU30                 0x30			/*  */
#define     TestSel1Reg           0x31			/*  */
#define     TestSel2Reg           0x32			/*  */
#define     TestPinEnReg          0x33			/*  */
#define     TestPinValueReg       0x34			/*  */
#define     TestBusReg            0x35			/*  */
#define     AutoTestReg           0x36			/*  */
#define     VersionReg            0x37			/*  */
#define     AnalogTestReg         0x38			/*  */
#define     TestDAC1Reg           0x39			/*  */  
#define     TestDAC2Reg           0x3A			/*  */   
#define     TestADCReg            0x3B			/*  */   
#define     RFU3C                 0x3C			/*  */   
#define     RFU3D                 0x3D			/*  */   
#define     RFU3E                 0x3E			/*  */   
#define     RFU3F		 		  0x3F			/*  */

#endif
