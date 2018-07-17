#include "uart.h"
#include "gpio.h"
#include <common/types.h>


//Use for DMA mode
#define STARTTX           (reg_dma_tx_rdy0 |= BIT(1))                     //trigger dma1 channel to transfer.dma1 is the uart tx channel
#define TXDONE            ((reg_uart_status1 & FLD_UART_TX_DONE) ? 1:0)   //1:uart module has send all data.0:still has data to send
#define RXERRORCLR        (reg_uart_status0  |= FLD_UART_RX_ERR_CLR)      //if uart module occur error,this bit can clear error flag bit.
#define RXERROR           ((reg_uart_status0 & FLD_UART_RX_ERR_FLAG)? 1:0)//uart module error status flag bit.


/**
 * @Brief: Check UART busy flag.
 * @Param: None.
 * @ReVal: None.
 */
unsigned char uart_tx_is_busy(){
    return (!TXDONE);
}

/**
 * @Brief: UART Pin initialization.
 * @Param: uartPin ->
 * @ReVal: None.
 */
void uart_pin_init(eUART_PinTypeDef uartPin)
{
	if(uartPin == UART_PIN_PA3A4)
	{
		//Disable GPIO function of PA3 PA4.
		gpio_set_func(GPIO_PA3,AS_UART);//Tx
		gpio_set_func(GPIO_PA4,AS_UART);//Rx

		gpio_set_output_en(GPIO_PA3,1);
		gpio_set_input_en(GPIO_PA4,1);

		//gpio_setup_up_down_resistor(GPIO_PA3,PM_PIN_PULLUP_10K);
		gpio_setup_up_down_resistor(GPIO_PA4,PM_PIN_PULLUP_10K);

		//Set PA3 PA4 as UART.
		GPIOA_AF->RegBits.P3_AF = GPIOA3_UART_TX;
		GPIOA_AF->RegBits.P4_AF = GPIOA4_UART_RX;

		/* Disable UART function of other Pins. */
		//PB4 PB5
		if(GPIOB_AF->RegBits.P4_AF == GPIOB4_UART_TX)//PB4
		{
			gpio_set_func(GPIO_PB4,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P5_AF == GPIOB5_UART_RX)//PB5
		{
			gpio_set_func(GPIO_PB5,AS_GPIO);
		}
		//PC4 PC5
		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_UART_TX)//PC4
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_UART_RX)//PC5
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
	}
	else if(uartPin == UART_PIN_PB4B5)
	{
		//Disable GPIO function of PB4 PB5.
		gpio_set_func(GPIO_PB4,AS_UART);//Tx
		gpio_set_func(GPIO_PB5,AS_UART);//Rx

		gpio_set_output_en(GPIO_PB4,1);
		gpio_set_input_en(GPIO_PB5,1);

		gpio_setup_up_down_resistor(GPIO_PB5,PM_PIN_PULLUP_10K);

		//Set PB4 PB5 as UART.
		GPIOB_AF->RegBits.P4_AF = GPIOB4_UART_TX;
		GPIOB_AF->RegBits.P5_AF = GPIOB5_UART_RX;

		/* Disable UART function of other Pins. */
		//PA3 PA4
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_UART_TX)//PA3
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_UART_RX)//PA4
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}
		//PC4 PC5
		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_UART_TX)//PC4
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_UART_RX)//PC5
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
	}
	else if(uartPin == UART_PIN_PC4C5)
	{
		//Disable GPIO function of PC4 PC5.
		gpio_set_func(GPIO_PC4,AS_UART);//Tx
		gpio_set_func(GPIO_PC5,AS_UART);//Rx

		gpio_set_output_en(GPIO_PC4,1);
		gpio_set_input_en(GPIO_PC5,1);

		gpio_setup_up_down_resistor(GPIO_PC5,PM_PIN_PULLUP_10K);

		//Set PC4 PC5 as UART.
		GPIOC_AF->RegBits.P4_AF = GPIOC4_UART_TX;
		GPIOC_AF->RegBits.P5_AF = GPIOC5_UART_RX;

		/* Disable UART function of other Pins. */
		//PA3 PA4
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_UART_TX)//PA3
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_UART_RX)//PA4
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}
		//PB4 PB5
		if(GPIOB_AF->RegBits.P4_AF == GPIOB4_UART_TX)//PB4
		{
			gpio_set_func(GPIO_PB4,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P5_AF == GPIOB5_UART_RX)//PB5
		{
			gpio_set_func(GPIO_PB5,AS_GPIO);
		}
	}
}

/**
 *	@brief	reset uart module
 *	@param	none
 *	@return	none
 */
void uart_Reset(void){
	BM_SET(reg_rst_clk0, FLD_RST_UART);
	BM_CLR(reg_rst_clk0, FLD_RST_UART);
}

/**
 *	@brief	clear error state of uart rx, maybe used when application detected UART not work
 *	@parm	none
 *	@return	'1' RX error flag rised and cleard success; '0' RX error flag not rised
 */
unsigned char uart_ErrorCLR(void){
	if(RXERROR){
		RXERRORCLR;
		return 1;
	}
	return 0;
}


/**
 *	@brief	uart initiate, set uart clock divider, bitwidth and the uart work mode
 *	@param	uartCLKdiv - uart clock divider
 *			bwpc - bitwidth, should be set to larger than 2
 *	@return	'1' set success; '0' set error probably bwpc smaller than 3.
 *		BaudRate = sclk/((uartCLKdiv+1)*(bwpc+1))
 *		SYCLK = 16Mhz
 *		115200		9			13
 *		9600		103			15
 *
 *		SYCLK = 32Mhz
 *		115200		19			13
 *		9600		237			13
 */
#define  FLD_UART_BW_MUL1  0        // timeout is bit_width*1
#define  FLD_UART_BW_MUL2  1        // timeout is bit_width*2
#define  FLD_UART_BW_MUL3  2        // timeout is bit_width*3
#define  FLD_UART_BW_MUL4  3        // timeout is bit_width*4
unsigned char uart_Init(unsigned short uartCLKdiv,  unsigned char bwpc,
		                eUART_ParityTypeDef Parity, eUART_StopBitTypeDef StopBit)
{
	if(bwpc<3)
		return 0;
	/*******************1.config bautrate and timeout********************************/
	//set uart clk divider and enable clock divider
	reg_uart_clk_div = MASK_VAL(FLD_UART_CLK_DIV, uartCLKdiv, FLD_UART_CLK_DIV_EN, 1);

	//set bit width
	reg_uart_ctrl0   = MASK_VAL( FLD_UART_BWPC, bwpc);
	//set timeout period
	reg_uart_rx_timeout = MASK_VAL(FLD_UART_TIMEOUT_BW, (bwpc+1)*12, FLD_UART_TIMEOUT_MUL, FLD_UART_BW_MUL2);

	/*******************2.config parity function*************************************/
	if(Parity){                                          // if need parity, config parity function
		BM_SET(reg_uart_ctrl0, FLD_UART_PARITY_EN);      //enable parity

		if(PARITY_EVEN == Parity){
			BM_CLR(reg_uart_ctrl0, FLD_UART_PARITY_SEL); //enable even parity
		}
		else if(PARITY_ODD == Parity){
			BM_SET(reg_uart_ctrl0, FLD_UART_PARITY_SEL); //enable odd parity
		}
		else{
			return 0;
		}
	}
	else{ //if not parity,close the parity function
		BM_CLR(reg_uart_ctrl0, FLD_UART_PARITY_EN);  //close parity function
	}

	/******************3.config stop bit**********************************************/
	BM_CLR(reg_uart_ctrl0, FLD_UART_STOP_BIT);
	reg_uart_ctrl0 |= MASK_VAL(FLD_UART_STOP_BIT, StopBit);

	return 1;
}

/**
 * @brief     enable uart DMA mode,config uart dam interrupt.
 * @param[in] dmaTxIrqEn -- whether or not enable UART TX interrupt.
 * @param[in] dmaRxIrqEn -- whether or not enable UART RX interrupt.
 * @return    none
 */
void uart_DmaModeInit(unsigned char dmaTxIrqEn, unsigned char dmaRxIrqEn)
{
	//1.enable UART DMA mode
	BM_SET(reg_uart_ctrl0, FLD_UART_RX_DMA_EN | FLD_UART_TX_DMA_EN);

    //2.config DMAx mode
	BM_SET(reg_dma0_ctrl, FLD_DMA_WR_MEM); //set DMA0 mode to 0x01 for receive.write to memory
	BM_CLR(reg_dma1_ctrl, FLD_DMA_WR_MEM); //set DMA1 mode to 0x00 for send. read from memory

	//3.config dma irq
	if(dmaRxIrqEn){
		BM_SET(reg_dma_chn_irq_msk, FLD_DMA_UART_RX); //enable uart rx dma interrupt
		BM_SET(reg_irq_mask, FLD_IRQ_DMA_EN);
	}else{
		BM_CLR(reg_dma_chn_irq_msk, FLD_DMA_UART_RX); //disable uart rx dma interrupt
	}

	if(dmaTxIrqEn){
		BM_SET(reg_dma_chn_irq_msk, FLD_DMA_UART_TX);  //enable uart tx dma interrupt
		BM_SET(reg_irq_mask, FLD_IRQ_DMA_EN);
	}else{
		BM_CLR(reg_dma_chn_irq_msk, FLD_DMA_UART_TX);  //disable uart tx dma interrupt
	}
}

/**
 * @brief     config the number level setting the irq bit of status register 0x9d
 *            ie 0x9d[3].
 *            If the cnt register value(0x9c[0,3]) larger or equal than the value of 0x99[0,3]
 *            or the cnt register value(0x9c[4,7]) less or equal than the value of 0x99[4,7],
 *            it will set the irq bit of status register 0x9d, ie 0x9d[3]
 * @param[in] rx_level - receive level value. ie 0x99[0,3]
 * @param[in] tx_level - transmit level value.ie 0x99[4,7]
 * @param[in] rx_irq_en - 1:enable rx irq. 0:disable rx irq
 * @param[in] tx_irq_en - 1:enable tx irq. 0:disable tx irq
 * @return    none
 * @notice    suggust closing tx irq.
 */
void uart_notDmaModeInit(unsigned char rx_level,unsigned char tx_level,unsigned char rx_irq_en,unsigned char tx_irq_en)
{
	//1.set the trig level.
	BM_CLR(reg_uart_ctrl2, FLD_UART_CTRL3_RX_IRQ_TRIG_LEVEL);
	reg_uart_ctrl2 |= MASK_VAL(FLD_UART_CTRL3_RX_IRQ_TRIG_LEVEL, rx_level);

	BM_CLR(reg_uart_ctrl2, FLD_UART_CTRL3_TX_IRQ_TRIG_LEVEL);
	reg_uart_ctrl2 |= MASK_VAL(FLD_UART_CTRL3_TX_IRQ_TRIG_LEVEL, tx_level);

	//2.config the irq.
	if(rx_irq_en){
		BM_SET(reg_uart_ctrl0, FLD_UART_RX_IRQ_EN); //enable uart rx irq
		BM_SET(reg_irq_mask, FLD_IRQ_UART_EN);      //enable uart irq.
	}else{
		BM_CLR(reg_uart_ctrl0, FLD_UART_RX_IRQ_EN); //disable uart rx irq
	}
	if(tx_irq_en){
		BM_SET(reg_uart_ctrl0, FLD_UART_TX_IRQ_EN);  //enable uart tx irq
		BM_SET(reg_irq_mask, FLD_IRQ_UART_EN);       //enable uart irq
	}else{
		BM_CLR(reg_uart_ctrl0, FLD_UART_TX_IRQ_EN);  //disable uart tx irq
	}
}

/**
 * @ brief   in not dma mode, receive the data.
 *           the method to read data should be like this: read received data in the order from 0x90 to 0x93.
 *           then repeat the order.
 * @ param[in] none
 * @ return    the data received from the uart.
 */
unsigned char rx_id = 0;
unsigned char uart_notDmaModeRevData(void)
{
	//static unsigned char rx_id = 0;
	unsigned char res = 0;

	res = read_reg8(0x90 + rx_id);
	rx_id++;
	rx_id &= 0x03;
	return res;
}

/**
 * @brief     uart send data function with not DMA method.
 *            variable uart_TxIndex,it must loop the four registers 0x90 0x91 0x92 0x93 for the design of SOC.
 *            so we need variable to remember the index.
 * @param[in] uartData - the data to be send.
 * @return    1: send success ; 0: uart busy
 */
unsigned char tx_id = 0;
unsigned char uart_notDmaModeSendByte(unsigned char uartData)
{
	//static unsigned char uart_TxIndex = 0;

	while((read_reg8(0x9c)>>4) > 7 );  //t_buf_cnt < 8, data send OK
	write_reg8(0x90 + tx_id, uartData);
	tx_id++;
	tx_id &= 0x03;    //cycle the four register 0x90 0x91 0x92 0x93.

	return 0;
}

/**
 *	@brief	uart send data function, this  function tell the DMA to get data from the RAM and start
 *			the DMA send function
 *	@param	sendBuff - send data buffer
 *	@return	'1' send success; '0' DMA busy
 */
unsigned char uart_Send(unsigned char* addr){
	if(uart_tx_is_busy()){
		return 0;
	}
	reg_dma1_addr = (unsigned int)addr & 0xffff;   //packet data, start address is sendBuff+1
	STARTTX;
	return 1;
}

/**
 *	@brief	data receive buffer initiate function. DMA would move received uart data to the address space, uart packet length
 *			needs to be no larger than (recBuffLen - 4).
 *	@param	*recAddr:	receive buffer's address info.
 *			recBuffLen:	receive buffer's length, the maximum uart packet length should be smaller than (recBuffLen - 4)
 *	@return	none
 */

void uart_RecBuffInit(unsigned char *recAddr, unsigned short recBuffLen){
	unsigned char bufLen;
	bufLen = recBuffLen>>4;
	reg_dma0_addr = (unsigned int)(recAddr) & 0xffff;//set receive buffer address

	BM_CLR(reg_dma0_ctrl, FLD_DMA_BUF_SIZE);
	reg_dma0_ctrl |= MASK_VAL(FLD_DMA_BUF_SIZE, bufLen);  //set receive buffer size
}


/**
 * @Brief: Set TX buffer MAX length of UART in DMA mode.(MAX Length < 512B)
 * @Param: txBuffLen ->
 * @Return: None.
 */
void uart_txBuffInit(unsigned short txBuffLen){
	unsigned char bufLen;
	bufLen = txBuffLen >> 4;

	BM_CLR(reg_dma1_ctrl, FLD_DMA_BUF_SIZE);
	reg_dma1_ctrl |= MASK_VAL(FLD_DMA_BUF_SIZE, bufLen); //set receive buffer size
}

/**
 *	@brief	data receive buffer initiate function. DMA would move received uart data to the address space, uart packet length
 *			needs to be no larger than (recBuffLen - 4).
 *	@param	*recAddr:	receive buffer's address info.
 *			recBuffLen:	receive buffer's length, the maximum uart packet length should be smaller than (recBuffLen - 4)
 *	@return	none
 */
static unsigned char   *tx_buff = NULL;
void uart_BuffInit(unsigned char *recAddr, unsigned short recBuffLen, unsigned char *txAddr){
	unsigned char bufLen;
	bufLen = recBuffLen>>4;
	reg_dma0_addr = (unsigned short)((unsigned int)(recAddr)); //set receive buffer address

	BM_CLR(reg_dma0_ctrl, FLD_DMA_BUF_SIZE);
	reg_dma0_ctrl |= MASK_VAL(FLD_DMA_BUF_SIZE, bufLen); //set receive buffer size

    tx_buff = txAddr;
}

/**
 * @Brief: Only use for Normal mode of UART.(GaoQiu add)
 * @Param:
 * @Return:
 */
unsigned char uart_ndma_get_rx_irq_flag(void)
{
	return (reg_uart_status0 & FLD_UART_IRQ_FLAG) ? 1:0;
}

/**
 * @Brief: Only use for DMA mode of UART.(GaoQiu add)
 * @Param:
 * @Return:
 */
unsigned char uart_get_irq_flag(eUART_IrqFlagTypeDef UART_Flag)
{
	return (reg_dma_irq_src & UART_Flag) ?1:0;
}

/**
 * @Brief: Only use for DMA mode of UART.(GaoQiu add)
 * @Param:
 * @Return:
 */
void uart_clear_irq_flag(eUART_IrqFlagTypeDef UART_Flag)
{
	reg_dma_irq_src = UART_Flag;
}


/**
 *	@brief:  get the uart IRQ source and clear the IRQ status, need to be called in the irq process function
 *  @Param:  None.
 *	@return: uart_irq_src- enum variable of uart IRQ source, 'UARTRXIRQ' or 'UARTTXIRQ'
 */
enum UARTIRQSOURCE uart_IRQSourceGet(void){
	unsigned char irqS;

	irqS = reg_dma_irq_src;
	reg_dma_irq_src = irqS; //clear irq source

	if(irqS & 0x01){
		return UARTRXIRQ;
	}
	else if(irqS & 0x02){
		return UARTTXIRQ;
	}
	else{
		return UARTNONEIRQ;
	}
}

/**
 ******************************************************************************
 *------------------------Application Example----------------------------------
 * unsigned char recBuff[128];//Declare a receive buffer
 *
 * void uart_useExample(void)
 * {
 *      UART_GPIO_INIT_PA3A4();
 * 		CLK32M_UART115200;
 * 		uart_RecBuffInit(&recBuff,128);
 * }
 *******************************************************************************
 */


enum UARTIRQSOURCE uart_IRQSourceGet_kma(void){
	unsigned char irqS;

	irqS = reg_dma_irq_src;
	reg_dma_irq_src = irqS; //clear irq source

	if(irqS & 0x01)
		return UARTRXIRQ;
	if(irqS & 0x02)
		return UARTTXIRQ;

	return UARTRXIRQ;
}

/**
 *	@brief	uart send data function, this  function tell the DMA to get data from the RAM and start
 *			the DMA send function
 *	@param	sendBuff - send data buffer
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

    //uart_set_tx_busy_flag();

    reg_dma1_addr = (unsigned short)(unsigned int)addr;  //packet data, start address is sendBuff+1

	STARTTX;

	return 1;
}

/*-----------------------------GaoQiu add-------------------------------------*/
/**
 * @Brief:  UART CTS/RTS Pin initialization.
 * @Param:  flowCtrlPin -> CTS/RTS Pin.
 * @Retval: None.
 */
void UART_FlowCtrlPinInit(eUART_FlowCtrlPinTypeDef flowCtrlPin)
{
	/* CTS Pin Configuration. ------------------------------------------------*/
	if(flowCtrlPin == UART_FLOW_CTRL_CTS_PA1)
	{
		//Disable GPIO function of PA1
		gpio_set_func(GPIO_PA1,AS_UART);//CTS

		//Set PA1 as CTS function
		GPIOA_AF->RegBits.P1_AF = GPIOA1_UART_CTS;

		/* Disable CTS function of other GPIO Pin. */
		//PB2
		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_UART_CTS_OR_SPI_DI)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		//PC2
		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_UART_CTS)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);
		}
		//PB7
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_UART_CTS)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_CTS_PB2)
	{
		//Disable GPIO function of PB2
		gpio_set_func(GPIO_PB2,AS_UART);//CTS

		//Set PB2 as CTS function
		GPIOB_AF->RegBits.P2_AF = GPIOB2_UART_CTS_OR_SPI_DI;

		/* Disable CTS function of other GPIO Pin. */
		//PA1
		if(GPIOA_AF->RegBits.P1_AF == GPIOA1_UART_CTS)
		{
			gpio_set_func(GPIO_PA1,AS_GPIO);
		}
		//PC2
		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_UART_CTS)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);
		}
		//PB7
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_UART_CTS)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_CTS_PB7)
	{
		//Disable GPIO function of PB7
		gpio_set_func(GPIO_PB7,AS_UART);

		//Set PB7 as CTS function
		GPIOB_AF->RegBits.P7_AF = GPIOB7_UART_CTS;

		/* Disable CTS function of other GPIO Pin. */
		//PA1
		if(GPIOA_AF->RegBits.P1_AF == GPIOA1_UART_CTS)
		{
			gpio_set_func(GPIO_PA1,AS_GPIO);
		}
		//PB2
		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_UART_CTS_OR_SPI_DI)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		//PC2
		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_UART_CTS)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_CTS_PC2)
	{
		//Disable GPIO function of PC2
		gpio_set_func(GPIO_PC2,AS_UART);//CTS

		//Set PC2 as CTS function
		GPIOC_AF->RegBits.P2_AF = GPIOC2_UART_CTS;

		/* Disable CTS function of other GPIO Pin. */
		//PA1
		if(GPIOA_AF->RegBits.P1_AF == GPIOA1_UART_CTS)
		{
			gpio_set_func(GPIO_PA1,AS_GPIO);
		}
		//PB2
		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_UART_CTS_OR_SPI_DI)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		//PB7
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_UART_CTS)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}
	}
	/* RTS Pin Configuration. ------------------------------------------------*/
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PA2)
	{
		//Disable GPIO function of PA2
		gpio_set_func(GPIO_PA2,AS_UART);//RTS

		//Set PA2 as RTS function
		GPIOA_AF->RegBits.P2_AF = GPIOA2_UART_RTS;

		/* Disable RTS function of other GPIO Pin. */
		//PB3
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_UART_RTS_OR_SPI_CK)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}
		//PB6
		if(GPIOB_AF->RegBits.P6_AF ==GPIOB6_UART_RTS)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		//PC3
		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_UART_RTS)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PB3)
	{
		//Disable GPIO function of PB3
		gpio_set_func(GPIO_PB3,AS_UART);//RTS

		//Set PB3 as RTS function
		GPIOB_AF->RegBits.P3_AF = GPIOB3_UART_RTS_OR_SPI_CK;
		reg_gpio_pb_multi_func_select |= FLD_PB_MULTI_FUNC_SEL;//must

		/* Disable RTS function of other GPIO Pin. */
		//PA2
		if(GPIOA_AF->RegBits.P2_AF == GPIOA2_UART_RTS)
		{
			gpio_set_func(GPIO_PA2,AS_GPIO);
		}
		//PB6
		if(GPIOB_AF->RegBits.P6_AF ==GPIOB6_UART_RTS)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		//PC3
		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_UART_RTS)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PB6)
	{
		//Disable GPIO function of PB6
		gpio_set_func(GPIO_PB6,AS_UART);

		//Set PB6 as RTS function
		GPIOB_AF->RegBits.P6_AF = GPIOB6_UART_RTS;

		/* Disable RTS function of other GPIO Pin. */
		//PA2
		if(GPIOA_AF->RegBits.P2_AF == GPIOA2_UART_RTS)
		{
			gpio_set_func(GPIO_PA2,AS_GPIO);
		}
		//PB3
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_UART_RTS_OR_SPI_CK)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}
		//PC3
		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_UART_RTS)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PC3)
	{
		//Disable GPIO function of PC3
		gpio_set_func(GPIO_PC3,AS_UART);//RTS

		//Set PC3 as RTS function
		GPIOC_AF->RegBits.P3_AF = GPIOC3_UART_RTS;

		/* Disable RTS function of other GPIO Pin. */
		//PA2
		if(GPIOA_AF->RegBits.P2_AF == GPIOA2_UART_RTS)
		{
			gpio_set_func(GPIO_PA2,AS_GPIO);
		}
		//PB3
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_UART_RTS_OR_SPI_CK)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}
		//PB6
		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_UART_RTS)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
	}
}

/**
 * @Brief:  UART RTS initialization.
 * @Param:
 * @Retval: None.
 */
void UART_RTS_Init(unsigned char rtsTriggerLevel, unsigned char isInvertRtsValue,
		           unsigned char rtsManualModeEn, unsigned char rtsEnable)
{
	//Set RTS trigger level.
	UART_CTRL2->Bits.RTS_TriggerLevel = rtsTriggerLevel;

	//whether RTS Pin value is inverted.
	UART_CTRL2->Bits.RTS_ValueInvert = isInvertRtsValue;

	//Set RTS as Manual mode.
	UART_CTRL2->Bits.RTS_ManualModeEnable = rtsManualModeEn;

	//Enable RTS.
	UART_CTRL2->Bits.RTS_Enable = rtsEnable;
}

/**
 * @Brief:  Set UART RTS Pin as High/Low level when RTS works in manual mode.
 * @Param:  pinValue -> RTS Pin value.
 * @Retval: None.
 */
void UART_RTS_SetPinValue(unsigned char pinValue)
{
	UART_CTRL2->Bits.RTS_Value = pinValue;
}

/**
 * @Brief:  UART CTS initialization.
 * @Param:
 * @Retval: None.
 */
void UART_CTS_Init(unsigned char pinValue, unsigned char ctsEnable)
{
	if(pinValue)
	{
		reg_uart_ctrl0 |= FLD_UART_CTS_I_SELECT;
	}
	else
	{
		reg_uart_ctrl0 &= ~FLD_UART_CTS_I_SELECT;
	}

	if(ctsEnable)
	{
		reg_uart_ctrl0 |= FLD_UART_CTS_EN;
	}
	else
	{
		reg_uart_ctrl0 &= ~FLD_UART_CTS_EN;
	}
}
/*-------------------------- End of File -------------------------------------*/

