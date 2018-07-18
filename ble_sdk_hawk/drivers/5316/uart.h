#ifndef  _UART_H
#define  _UART_H

#include "register.h"
#include "gpio.h"

/* UART Pin enum define. */
typedef enum{
	UART_PIN_PA3A4,
	UART_PIN_PB4B5,
	UART_PIN_PC4C5,
}eUART_PinTypeDef;

/* 5316 UART hardware flow pin define. */
typedef enum
{
	UART_FLOW_CTRL_CTS_PA1,
	UART_FLOW_CTRL_RTS_PA2,

	UART_FLOW_CTRL_CTS_PB2,
	UART_FLOW_CTRL_RTS_PB3,

	UART_FLOW_CTRL_CTS_PB7,
	UART_FLOW_CTRL_RTS_PB6,

	UART_FLOW_CTRL_CTS_PC2,
	UART_FLOW_CTRL_RTS_PC3,
}eUART_FlowCtrlPinTypeDef;


/**
 *  @brief  Define parity type
 */
typedef enum {
    PARITY_NONE = 0,
    PARITY_EVEN,
    PARITY_ODD,
}eUART_ParityTypeDef;

/**
 *  @brief  Define the length of stop bit
 */
typedef enum {
    STOP_BIT_ONE = 0,
    STOP_BIT_ONE_DOT_FIVE = BIT(12),
    STOP_BIT_TWO = BIT(13),
}eUART_StopBitTypeDef;

/* UART interrupt flag define. only use for DMA mode of UART. */
typedef enum{
	UART_Flag_RxDone = 0x01,
	UART_Flag_TxDone = 0x02,
}eUART_IrqFlagTypeDef;

enum {
	UART_DMA_RX_IRQ_DIS = 0,
	UART_DMA_RX_IRQ_EN  = 1,
	UART_DMA_TX_IRQ_DIS = 0,
	UART_DMA_TX_IRQ_EN  = 1,
};

enum {
	UART_NODMA_RX_IRQ_DIS = 0,
	UART_NODMA_RX_IRQ_EN  = 1,
	UART_NODMA_TX_IRQ_DIS = 0,
	UART_NODMA_TX_IRQ_EN  = 1,
};

#define CLK32M_UART9600         do{\
									uart_Init(302,10,PARITY_NONE,STOP_BIT_ONE);\
									uart_DmaModeInit(UART_DMA_TX_IRQ_EN, UART_DMA_RX_IRQ_EN);\
								}while(0)
#define CLK32M_UART115200       do{\
									uart_Init(30,8,PARITY_NONE,STOP_BIT_ONE);\
									uart_DmaModeInit(UART_DMA_TX_IRQ_EN, UART_DMA_RX_IRQ_EN);\
								}while(0)
#define CLK16M_UART115200       do{\
									uart_Init(9,13,PARITY_NONE,STOP_BIT_ONE);\
									uart_DmaModeInit(UART_DMA_TX_IRQ_EN, UART_DMA_RX_IRQ_EN);\
								}while(0)
#define CLK16M_UART9600         do{\
									uart_Init(118,13,PARITY_NONE,STOP_BIT_ONE);\
									uart_DmaModeInit(UART_DMA_TX_IRQ_EN, UART_DMA_RX_IRQ_EN);\
								}while(0)

					 //TxRx
#define UART_GPIO_INIT_PA3A4()   uart_pin_init(UART_PIN_PA3A4)
#define UART_GPIO_INIT_PB4B5()   uart_pin_init(UART_PIN_PB4B5)
#define UART_GPIO_INIT_PC4C5()   uart_pin_init(UART_PIN_PC4C5)



extern void uart_pin_init(eUART_PinTypeDef uartPin);
extern void uart_Reset(void);
extern unsigned char uart_ErrorCLR(void);
extern unsigned char uart_Init(unsigned short uartCLKdiv, unsigned char bwpc,
		                       eUART_ParityTypeDef Parity, eUART_StopBitTypeDef StopBit);

/* Only use for Normal Mode. */
extern void uart_notDmaModeInit(unsigned char rx_level,unsigned char tx_level,
		                        unsigned char rx_irq_en,unsigned char tx_irq_en);
extern unsigned char rx_id;
extern unsigned char uart_notDmaModeRevData(void);
extern unsigned char tx_id;
extern unsigned char uart_notDmaModeSendByte(unsigned char uartData);
//interrupt flag will be cleared automatically
extern unsigned char uart_ndma_get_rx_irq_flag(void);

/* Only use for DMA mode */
extern void uart_DmaModeInit(unsigned char dmaTxIrqEn, unsigned char dmaRxIrqEn);
extern void uart_RecBuffInit(unsigned char *recAddr, unsigned short recBuffLen);
extern unsigned char uart_Send(unsigned char* addr);
extern unsigned char uart_get_irq_flag(eUART_IrqFlagTypeDef UART_Flag);
extern void uart_clear_irq_flag(eUART_IrqFlagTypeDef UART_Flag);
extern unsigned char uart_tx_is_busy(void);

/* Use for Hardware Flow of UART */
extern void UART_FlowCtrlPinInit(eUART_FlowCtrlPinTypeDef flowCtrlPin);
extern void UART_RTS_Init(unsigned char rtsTriggerLevel, unsigned char isInvertRtsValue,
		                  unsigned char rtsManualModeEn, unsigned char rtsEnable);
extern void UART_RTS_SetPinValue(unsigned char pinValue);
extern void UART_CTS_Init(unsigned char pinValue, unsigned char ctsEnable);



/* Reserved for compatibility */
enum UARTIRQSOURCE{
	UARTNONEIRQ = 0,
	UARTRXIRQ = BIT(0),
	UARTTXIRQ = BIT(1),
};

enum{
	UARTRXIRQ_MASK  = BIT(0),
	UARTTXIRQ_MASK  = BIT(1),
	UARTIRQ_MASK    = UARTRXIRQ_MASK | UARTTXIRQ_MASK,
};

//Use for Normal mode
#define GET_UART_NOT_DMA_IRQ()  ((reg_uart_status0 & FLD_UART_IRQ_FLAG) ? 1:0)//not dma mode,1: occur uart irq; 0:not uart irq

//use for DMA mode
extern unsigned char uart_Send_kma(unsigned char* addr);
extern enum UARTIRQSOURCE uart_IRQSourceGet(void);
extern enum UARTIRQSOURCE uart_IRQSourceGet_kma(void);
extern void uart_BuffInit(unsigned char *recAddr, unsigned short recBuffLen, unsigned char *txAddr);
//void uart_clr_tx_busy_flag(void);

#endif
