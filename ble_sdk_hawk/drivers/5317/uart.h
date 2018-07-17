#ifndef  _UART_H
#define  _UART_H

#include "driver_config.h"
#include "register.h"
#include "gpio.h"


/* UART Pin enum define. */
typedef enum{
	UART_PIN_PA1A2,
	UART_PIN_PB4B5,
	UART_PIN_PC4C5,
}UART_PinTypeDef;

typedef enum{
	UART_TX_PA1 = GPIO_PA1,
	UART_TX_PB4 = GPIO_PB4,
	UART_TX_PC4 = GPIO_PC4,
}UART_TxPinDef;

// rx: A0 B0 B7 C3 C5 D6
typedef enum{
	UART_RX_PA2 = GPIO_PA2,
	UART_RX_PB5 = GPIO_PB5,
	UART_RX_PC5 = GPIO_PC5,
}UART_RxPinDef;

/* 5317 UART hardware flow pin define. */
typedef enum{
	UART_FLOW_CTRL_CTS_PB2,//目前不能使用
	UART_FLOW_CTRL_RTS_PB3,

	UART_FLOW_CTRL_CTS_PB7,//目前不能使用
	UART_FLOW_CTRL_RTS_PB6,

	UART_FLOW_CTRL_CTS_PC2,
	UART_FLOW_CTRL_RTS_PC3,
}UART_FlowCtrlPinTypeDef;
#endif


/**
 *  @brief  Define parity type
 */
typedef enum {
    PARITY_NONE = 0,
    PARITY_EVEN,
    PARITY_ODD,
}UART_ParityTypeDef;

/**
 *  @brief  Define the length of stop bit
 */
typedef enum {
    STOP_BIT_ONE = 0,
    STOP_BIT_ONE_DOT_FIVE = BIT(12),
    STOP_BIT_TWO = BIT(13),
}UART_StopBitTypeDef;

/* UART interrupt flag define. only use for DMA mode of UART. */
typedef enum{
	UART_Flag_RxDone = 0x01,
	UART_Flag_TxDone = 0x02,
}UART_IrqFlagTypeDef;


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

#if (MCU_CORE_TYPE == MCU_CORE_5316)
                     	 //TxRx
	#define UART_GPIO_INIT_PA3A4()   uart_pin_init(UART_PIN_PA3A4)
	#define UART_GPIO_INIT_PB4B5()   uart_pin_init(UART_PIN_PB4B5)
	#define UART_GPIO_INIT_PC4C5()   uart_pin_init(UART_PIN_PC4C5)
#elif(MCU_CORE_TYPE == MCU_CORE_5317)
						  //TxRx
	#define UART_GPIO_INIT_PA1A2()   uart_pin_init(UART_PIN_PA1A2)
	#define UART_GPIO_INIT_PB4B5()   uart_pin_init(UART_PIN_PB4B5)
	#define UART_GPIO_INIT_PC4C5()   uart_pin_init(UART_PIN_PC4C5)
#endif

extern void uart_pin_set(UART_TxPinDef tx_pin, UART_RxPinDef rx_pin);
extern void uart_init(unsigned int BaudRate, UART_ParityTypeDef Parity,
		                       UART_StopBitTypeDef StopBit);
extern void uart_Reset(void);
extern unsigned char uart_error_clr(void);

/* Only use for Normal Mode. -------------------------------------------------*/
extern void uart_notDmaModeInit(unsigned char rx_level,unsigned char tx_level,
		                        unsigned char rx_irq_en,unsigned char tx_irq_en);
extern unsigned char rx_id;
extern unsigned char uart_notDmaModeRevData(void);
extern unsigned char tx_id;
extern unsigned char uart_notDmaModeSendByte(unsigned char uartData);
//interrupt flag will be cleared automatically
extern unsigned char uart_ndma_get_rx_irq_flag(void);

/* Only use for DMA mode -----------------------------------------------------*/
//extern void uart_DmaModeInit(unsigned char dmaTxIrqEn, unsigned char dmaRxIrqEn);
extern void uart_dma_enable(unsigned char rx_dma_en, unsigned char tx_dma_en);
extern void uart_rx_buff_init(unsigned char *recAddr, unsigned short recBuffLen);
extern unsigned char uart_Send(unsigned char* addr);
extern void uart_irq_enable(unsigned char rx_irq_en,unsigned char tx_irq_en);
extern unsigned char uart_get_irq_flag(UART_IrqFlagTypeDef UART_Flag);
extern void uart_clear_irq_flag(UART_IrqFlagTypeDef UART_Flag);
extern unsigned char uart_tx_is_busy(void);

/* Use for Hardware Flow of UART ---------------------------------------------*/
extern void UART_FlowCtrlPinInit(UART_FlowCtrlPinTypeDef flowCtrlPin);
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

