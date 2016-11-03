/**************************************************************************************************
  Filename:       	uart.c
  Author:			junjun.xu@telink-semi.com
  Created Date:	2016/06/05

  Description:    This file contains the uart driver functions for the Telink 8267. Before the uart can work, user should first choose the UART TX, RX output
  			  pins (disable the correspond pins' gpio functions, gpio initate)


**************************************************************************************************/

#include "uart.h"
#include "../../proj/tl_common.h"
//#include "../mcu/watchdog_i.h"

#define		STARTTX			write_reg8(0x800524,0x02)//trigger dma
#define		TXDONE			(((*(volatile unsigned char  *)0x80009e) & 0x01) ? 1:0)
#define		RXERRORCLR		(*(volatile unsigned char  *)0x80009d |= 0x40)
#define		RXERROR			(((*(volatile unsigned char  *)0x80009d) & 0x80) ? 1:0)

#if(MCU_CORE_TYPE == MCU_CORE_8266)
#define UART_CONTINUE_DELAY_EN          1
#else
#define UART_CONTINUE_DELAY_EN          0
#endif

volatile unsigned char uart_tx_busy_flag = 0;                   // must "volatile"
static unsigned char *tx_buff = NULL;
#if(UART_CONTINUE_DELAY_EN)
static volatile unsigned int uart_continue_delay_time = 0;      // must "volatile"
static unsigned int uart_tx_done_delay_us = 900;
#endif

void uart_set_tx_busy_flag(){
    uart_tx_busy_flag = 1;
    #if(UART_CONTINUE_DELAY_EN)
    uart_continue_delay_time = 0;
    #endif
}

void uart_clr_tx_busy_flag(){
    #if(UART_CONTINUE_DELAY_EN)
    uart_continue_delay_time = clock_time() | 1; // make sure not zero
    #else
    uart_tx_busy_flag = 0;
    #endif
}

void uart_set_tx_done_delay (u32 t)
{
#if(UART_CONTINUE_DELAY_EN)
	uart_tx_done_delay_us = t;
#endif
}

unsigned char uart_tx_is_busy(){
#if(UART_CONTINUE_DELAY_EN)
   if (uart_continue_delay_time && clock_time_exceed(uart_continue_delay_time, uart_tx_done_delay_us))
   {
    	uart_continue_delay_time = 0;
    	uart_tx_busy_flag = 0;
   }
    return uart_tx_busy_flag;
#else
    return (!TXDONE);
#endif
}

/**********************************************************
*
*	@brief	reset uart module
*
*	@param	none
*
*	@return	none
*/
void uart_Reset(void){

	write_reg8(0x800061,0x80);
	write_reg8(0x800061,0x00);

}
/**********************************************************
*
*	@brief	clear error state of uart rx, maybe used when application detected UART not work
*
*	@parm	none
*
*	@return	'1' RX error flag rised and cleard success; '0' RX error flag not rised
*
*/
unsigned char uart_ErrorCLR(void){
	if(RXERROR){
		RXERRORCLR;
		return 1;
	}
	return 0;
}


/*******************************************************
*
*	@brief	uart initiate, set uart clock divider, bitwidth and the uart work mode
*
*	@param	uartCLKdiv - uart clock divider
*			bwpc - bitwidth, should be set to larger than 2
*			en_rx_irq - '1' enable rx irq; '0' disable.
*			en_tx_irq - enable tx irq; '0' disable.
*			hdwC - enum variable of hardware control functions
*
*	@return	'1' set success; '0' set error probably bwpc smaller than 3.
*
*		BaudRate = sclk/((uartCLKdiv+1)*(bwpc+1))
*		SYCLK = 16Mhz
		115200		9			13
		9600		103			15
*
*		SYCLK = 32Mhz
*		115200		19			13
		9600		237			13
*/

unsigned char uart_Init(unsigned short uartCLKdiv, unsigned char bwpc,unsigned char en_rx_irq,unsigned char en_tx_irq,enum HARDWARECONTROL hdwC){

	if(bwpc<3)
		return 0;
	write_reg16(0x800094,(uartCLKdiv|0x8000));//set uart clk divider and enable clock divider
	write_reg8(0x800096,(0x30|bwpc));//set bit width and enable rx/tx DMA
	write_reg8(0x80009a,(bwpc+1)*12);//one byte includes 12 bits at most
	write_reg8(0x80009b,1);//For save

	//write_reg8(0x800097,0x00);//No clts and rts
	write_reg8(0x800097,(unsigned char)hdwC);//set hardware control function

	//receive DMA and buffer details

	write_reg8(0x800503,0x01);//set DMA 0 mode to 0x01 for receive
	write_reg8(0x800507,0x00);//DMA1 mode to send
	if(en_rx_irq){
		*(volatile unsigned char  *)0x800521 |= 0x01;//open dma1 interrupt mask
		*(volatile unsigned char  *)0x800640 |= 0x10;//open dma interrupt mask
		//write_reg8(0x800643,0x01);//enable intterupt
	}
	if(en_tx_irq){
		*(volatile unsigned char  *)0x800521 |= 0x02;//open dma1 interrupt mask
		*(volatile unsigned char  *)0x800640 |= 0x10;//open dma interrupt mask
		//write_reg8(0x800643,0x01);//enable intterupt
	}
	return 1;

}

/********************************************************************************
*	@brief	uart send data function, this  function tell the DMA to get data from the RAM and start
*			the DMA send function
*
*	@param	sendBuff - send data buffer
*
*	@return	'1' send success; '0' DMA busy
*/
unsigned char uart_Send(unsigned char* addr){
	if(TXDONE){
		write_reg16(0x800504,addr);//packet data, start address is sendBuff+1
		STARTTX;
		return 1;
	}
		return 0;

}

/********************************************************************************
*	@brief	uart send data function, this  function tell the DMA to get data from the RAM and start
*			the DMA send function
*
*	@param	sendBuff - send data buffer
*
*	@return	'1' send success; '0' DMA busy
*/
unsigned char uart_Send_kma(unsigned char* addr){
    unsigned long len = *((unsigned long *)addr);

    if(len > 252){
        return 0;
    }

    if (uart_tx_is_busy ())
    {
    	return 0;
    }

    uart_set_tx_busy_flag();

    write_reg16(0x800504, (u16)(u32)addr);//packet data, start address is sendBuff+1

	STARTTX;

	return 1;
}

/****************************************************************************************
*
*	@brief	data receive buffer initiate function. DMA would move received uart data to the address space, uart packet length
*			needs to be no larger than (recBuffLen - 4).
*
*	@param	*recAddr:	receive buffer's address info.
*			recBuffLen:	receive buffer's length, the maximum uart packet length should be smaller than (recBuffLen - 4)
*
*	@return	none
*/

void uart_RecBuffInit(unsigned char *recAddr, unsigned short recBuffLen){
	unsigned char bufLen;
	bufLen = recBuffLen/16;
	write_reg16(0x800500,(unsigned short)(recAddr));//set receive buffer address
	write_reg8(0x800502,bufLen);//set receive buffer size
}

/****************************************************************************************
*
*	@brief	data receive buffer initiate function. DMA would move received uart data to the address space, uart packet length
*			needs to be no larger than (recBuffLen - 4).
*
*	@param	*recAddr:	receive buffer's address info.
*			recBuffLen:	receive buffer's length, the maximum uart packet length should be smaller than (recBuffLen - 4)
*
*	@return	none
*/

void uart_BuffInit(unsigned char *recAddr, unsigned short recBuffLen, unsigned char *txAddr){
	unsigned char bufLen;
	bufLen = recBuffLen/16;
	write_reg16(0x800500,(unsigned short)((unsigned int)(recAddr)));//set receive buffer address
	write_reg8(0x800502,bufLen);//set receive buffer size

    tx_buff = txAddr;
}

/******************************************************************************
*
*	@brief		get the uart IRQ source and clear the IRQ status, need to be called in the irq process function
*
*	@return		uart_irq_src- enum variable of uart IRQ source, 'UARTRXIRQ' or 'UARTTXIRQ'
*
*/
enum UARTIRQSOURCE uart_IRQSourceGet(void){
	unsigned char irqS;
	irqS = read_reg8(0x800526);
	write_reg8(0x800526,irqS);//CLR irq source

	return (irqS & UARTIRQ_MASK);
}
/************************Application Example******************
unsigned char recBuff[128];//Declare a receive buffer
void uart_useExample(void ){
	CLK32M_UART9600;
	uart_RecBuffInit(&recBuff,128);
	//Initial IO,UART rx & tx declare
	write_reg8(0x800596,0xC3);
	write_reg8(0x8005B2,0x3C);
}
*/

enum UARTIRQSOURCE uart_IRQSourceGet_kma(void){
	unsigned char irqS;
	irqS = read_reg8(0x800526);
	write_reg8(0x800526,irqS);//CLR irq source
#if(!UART_CONTINUE_DELAY_EN)
	if(irqS & 0x01)	return UARTRXIRQ;
	if(irqS & 0x02)	return UARTTXIRQ;

	return UARTRXIRQ;
#else
	return (irqS & UARTIRQ_MASK);
#endif
}


//////////////////////////////////////////////////////////////////////
void uart_io_init(unsigned char uart_io_sel){
#if(MCU_CORE_TYPE == MCU_CORE_8266)
	uart_io_sel = uart_io_sel;
	gpio_set_func(GPIO_UTX, AS_UART);
	gpio_set_func(GPIO_URX, AS_UART);
#else
    if(UART_GPIO_8267_PA6_PA7 == uart_io_sel){
        UART_GPIO_CFG_PA6_PA7();
    }else if(UART_GPIO_8267_PC2_PC3 == uart_io_sel){
        UART_GPIO_CFG_PC2_PC3();
    }else if(UART_GPIO_8267_PB2_PB3 == uart_io_sel){
        UART_GPIO_CFG_PB2_PB3();
    }
#endif
}
