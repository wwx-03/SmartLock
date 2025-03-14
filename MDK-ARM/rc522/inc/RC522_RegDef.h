#ifndef __RC522_REGDEF_H
#define __RC522_REGDEF_H

typedef enum commienreg_bitmask_t
{
    TIMERIEN = 0x01, /*允许定时器中断请求*/
    ERRIEN = 0x02, /*允许错误中断请求*/
    LOALERTIEN = 0x04, /*允许FIFO低于水平线警报请求*/
    HIALERTIEN = 0x08, /*允许FIFO高于水平线警报请求*/
    IDLEIEN = 0x10, /*允许空闲中断请求*/
    RXIEN = 0x20, /*允许接收器中断请求*/
    TXIEN = 0x40, /*允许发送器中断请求*/
    IRQINV = 0x80, /*IRQ引脚信号倒置*/
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
#define     CommandReg            0x01			/* 启动或停止命令的执行 */
#define     CommIEnReg            0x02    		/* 使能或失能中断请求位 */
#define     DivlEnReg             0x03    		/* 使能或失能中断请求位 */
#define     ComIrqReg             0x04    		/* 包含中断请求位 */
#define     DivIrqReg             0x05			/* 包含中断请求位 */
#define     ErrorReg              0x06    		/* 展示上次命令执行的错误码 */
#define     Status1Reg            0x07    		/* 包含CRC，中断，FIFO缓冲区状态位 */
#define     Status2Reg            0x08   		/* 包含接收器、发射器和数据模式检测器的状态位 */
#define     FIFODataReg           0x09			/* 输入和输出64字节FIFO缓冲区 */
#define     FIFOLevelReg          0x0A			/* 指示FIFO中存储的字节数 */
#define     WaterLevelReg         0x0B			/* 定义FIFO缓冲区不足和溢出警告 */
#define     ControlReg            0x0C			/* 杂项控制位 */
#define     BitFramingReg         0x0D			/* 位导向框架的调整 */
#define     CollReg               0x0E			/* 定义在射频接口上检测到的第一个位冲突 */
#define     RFU0F                 0x0F			/* 保留 */
// PAGE 1     
#define     RFU10                 0x10			/* 保留 */
#define     ModeReg               0x11			/* 定义发送和接收的一般模式设置。 */
#define     TxModeReg             0x12			/* 定义传输过程中的数据速率 */
#define     RxModeReg             0x13			/* 定义接收期间的数据速率 */
#define     TxControlReg          0x14			/* 控制天线驱动器引脚Tx1和Tx2的逻辑行为 */
#define     TxASKReg              0x15			/* 控制发送调制的设置 */
#define     TxSelReg              0x16			/* 为模拟部分选择内部源 */
#define     RxSelReg              0x17			/* 选择内部接收器设置 */
#define     RxThresholdReg        0x18			/* 选择位解码器的阈值 */
#define     DemodReg              0x19			/* 定义解调器设置 */
#define     RFU1A                 0x1A			/* 保留 */
#define     RFU1B                 0x1B			/* 保留 */
#define     MfTxReg	           	  0x1C			/* 控制一些MIFARE通信传输参数 */
#define     MfRxReg               0x1D			/* 控制一些MIFARE通信传输参数 */
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F			/* 选择UART接口传输速率 */
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21			/* 显示CRC计算的实际MSB值 */
#define     CRCResultRegL         0x22			/* 显示CRC计算的实际LSB值 */
#define     RFU23                 0x23			
#define     ModWidthReg           0x24			/* 控制调制宽度设置 */
#define     RFU25                 0x25			
#define     RFCfgReg              0x78			/* 配置接收增益 */
#define     GsNReg                0x27			/* 选择驱动导通时，天线驱动引脚TX1和TX2的n驱动导通。 */
#define     CWGsPReg      	      0x28			/* 定义无调制时p -驱动器的电导 */
#define     ModGsPReg             0x29			/* 定义调制期间的驱动器p输出电导 */
#define     TModeReg              0x2A			/* 定义定时器的设置 */
#define     TPrescalerReg         0x2B			/* 定义定时器的设置 */
#define     TReloadRegH           0x2C			/* 描述16位长定时器的重新加载值高8位 */
#define     TReloadRegL           0x2D			/* 描述16位长定时器的重新加载值低8位 */
#define     TCounterValueRegH     0x2E			/* 描述定时器的计数值的高8位 */
#define     TCounterValueRegL     0x2F			/* 描述定时器的计数值的低8位 */
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
