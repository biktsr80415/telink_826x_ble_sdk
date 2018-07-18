#include "uart.h"
#include "clock.h"

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

#if 0
/**
 * @Brief: UART Pin initialization.
 * @Param: uartPin ->
 * @ReVal: None.
 */
void uart_pin_init(UART_PinTypeDef uartPin)
{
	if(uartPin == UART_PIN_PA1A2)
	{
		//Disable GPIO function of PA1/PA2.
		gpio_set_func(GPIO_PA1,AS_UART);//Tx
		gpio_set_func(GPIO_PA2,AS_UART);//Rx

		//must
		gpio_set_input_en(GPIO_PA1,1);
		gpio_set_input_en(GPIO_PA2,1);

		//Set PA1/PA2 as UART.
		GPIOA_AF->RegBits.P1_AF = GPIOA1_UART_TX;
		GPIOA_AF->RegBits.P2_AF = GPIOA2_UART_RX;

		/* Disable UART function of other Pins. */
		if(GPIOB_AF->RegBits.P4_AF == GPIOB4_UART_TX)
		{
			gpio_set_func(GPIO_PB4,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P5_AF == GPIOB5_UART_RX)
		{
			gpio_set_func(GPIO_PB5,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_UART_TX)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_UART_RX)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
	}
	else if(uartPin == UART_PIN_PB4B5)
	{
		//Disable GPIO function of PB4/PB5.
		gpio_set_func(GPIO_PB4,AS_UART);//Tx
		gpio_set_func(GPIO_PB5,AS_UART);//Rx

		//must
		gpio_set_input_en(GPIO_PB4,1);
		gpio_set_input_en(GPIO_PB5,1);

		//Set PB4/PB5 as UART.
		GPIOB_AF->RegBits.P4_AF = GPIOB4_UART_TX;
		GPIOB_AF->RegBits.P5_AF = GPIOB5_UART_RX;

		/* Disable UART function of other Pins. */
		if(GPIOA_AF->RegBits.P1_AF == GPIOA1_UART_TX)
		{
			gpio_set_func(GPIO_PA1,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P2_AF == GPIOA2_UART_RX)
		{
			gpio_set_func(GPIO_PA2,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_UART_TX)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_UART_RX)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
	}
	else if(uartPin == UART_PIN_PC4C5)
	{
		//Disable GPIO function of PC4/PC5.
		gpio_set_func(GPIO_PC4,AS_UART);//Tx
		gpio_set_func(GPIO_PC5,AS_UART);//Rx

		//must
		gpio_set_input_en(GPIO_PC4,1);
		gpio_set_input_en(GPIO_PC5,1);

		GPIOC_AF->RegBits.P4_AF = GPIOC4_UART_TX;
		GPIOC_AF->RegBits.P5_AF = GPIOC5_UART_RX;

		/* Disable UART function of other Pins. */
		if(GPIOA_AF->RegBits.P1_AF == GPIOA1_UART_TX)
		{
			gpio_set_func(GPIO_PA1,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P2_AF == GPIOA2_UART_RX)
		{
			gpio_set_func(GPIO_PA2,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P4_AF == GPIOB4_UART_TX)
		{
			gpio_set_func(GPIO_PB4,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P5_AF == GPIOB5_UART_RX)
		{
			gpio_set_func(GPIO_PB5,AS_GPIO);
		}
	}
}
#else
void uart_pin_set(UART_TxPinDef tx_pin, UART_RxPinDef rx_pin)
{
	/**
	 * Note: pullup setting must before uart gpio config,
	 *       cause it will lead to ERR data to uart RX buffer
	 *
	 * PM_PIN_PULLUP_1M   PM_PIN_PULLUP_10K
	 */
	gpio_setup_up_down_resistor(tx_pin, PM_PIN_PULLUP_1M);  //must, for stability and prevent from current leakage
	gpio_setup_up_down_resistor(rx_pin, PM_PIN_PULLUP_10K);  //must  for stability and prevent from current leakage

	gpio_set_func(tx_pin, AS_AF); // set tx pin
	gpio_set_func(rx_pin, AS_AF); // set rx pin

	gpio_set_input_en(tx_pin, 1);  //experiment shows that tx_pin should open input en(confirmed by qiuwei)
	gpio_set_input_en(rx_pin, 1);  //
}
#endif

/**
 *	@brief	reset uart module
 *	@param	none
 *	@return	none
 */
void uart_reset(void){
	BM_SET(reg_rst1, FLD_RST1_RS232);
	BM_CLR(reg_rst1, FLD_RST1_RS232);
}

/**
 *	@brief	clear error state of uart rx, maybe used when application detected UART not work
 *	@parm	none
 *	@return	'1' RX error flag rised and cleard success; '0' RX error flag not rised
 */
unsigned char uart_error_clr(void){
	if(RXERROR){
		RXERRORCLR;
		return 1;
	}
	return 0;
}

/**
 * @Brief:  This function is used to look for the prime.if the prime is finded,it will
 * 		    return 1, or return 0.
 * @Param:  None.
 * @Return: None.
 */
static unsigned char IsPrime(unsigned int n)
{
	unsigned int i = 5;
	if(n <= 3){
		return 1; //although n is prime, but the bwpc must be larger than 2.
	}else if((n % 2 == 0) || (n % 3 == 0)){
		return 0;
	}else{
		for(i = 5; i*i < n; i += 6)
		{
			if( (n % i == 0) || (n  % (i+2)) == 0 ){
				return 0;
			}
		}
		return 1;
	}
}

/**
 * @Brief:  calculate the best bwpc(bit width) .i.e reg0x96
 * @Algorithm: BaudRate*(div+1)*(bwpc+1)=system clock
 *    simplify the expression: div*bwpc =  constant(z)
 *    bwpc range from 3 to 15.so loop and
 *    get the minimum one decimal point
 * @Return the position of getting the minimum value
 */
static unsigned char g_bwpc = 0;
static unsigned int  g_uart_div = 0;
static void GetBetterBwpc(unsigned int baut_rate)
{
	unsigned char i = 0, j= 0;
	unsigned int primeInt = 0;
	unsigned char primeDec = 0;
	unsigned int D_intdec[13],D_int[13];
	unsigned char D_dec[13];
	unsigned int tmp_sysclk = CLOCK_SYS_CLOCK_1US *1000*1000;
	primeInt = tmp_sysclk/baut_rate;
	primeDec = 10*tmp_sysclk/baut_rate - 10*primeInt;

	/**
	 * calculate the primeInt and check whether primeInt is prime.
	 */
	if(IsPrime(primeInt)){ // primeInt is prime
		primeInt += 1;  //+1 must be not prime. and primeInt must be larger than 2.
	}
	else{
		if(primeDec > 5){ // >5
			primeInt += 1;
			if(IsPrime(primeInt)){
				primeInt -= 1;
			}
		}
	}

	/**
	 * get the best division value and bit width
	 */
	for(i=3;i<=15;i++){
		D_intdec[i-3] = (10*primeInt)/(i+1);////get the LSB
		D_dec[i-3] = D_intdec[i-3] - 10*(D_intdec[i-3]/10);///get the decimal section
		D_int[i-3] = D_intdec[i-3]/10;///get the integer section
	}

	//find the max and min one decimation point
	unsigned char position_min = 0,position_max = 0;
	unsigned int min = 0xffffffff,max = 0x00;
	for(j=0;j<13;j++){
		if((D_dec[j] <= min)&&(D_int[j] != 0x01)){
			min = D_dec[j];
			position_min = j;
		}
		if(D_dec[j]>=max){
			max = D_dec[j];
			position_max = j;
		}
	}

	if((D_dec[position_min]<5) && (D_dec[position_max]>=5)){
		if(D_dec[position_min]<(10-D_dec[position_max])){
			g_bwpc = position_min + 3;
			g_uart_div = D_int[position_min]-1;
		}
		else{
			g_bwpc = position_max + 3;
			g_uart_div = D_int[position_max];
		}
	}
	else if((D_dec[position_min]<5) && (D_dec[position_max]<5)){
		g_bwpc = position_min + 3;
		g_uart_div = D_int[position_min] - 1;
	}
	else{
		g_bwpc = position_max + 3;
		g_uart_div = D_int[position_max];
	}
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
void uart_init(unsigned int BaudRate, UART_ParityTypeDef Parity, UART_StopBitTypeDef StopBit)
{
	/*******************1.config bautrate and timeout********************************/
	GetBetterBwpc(BaudRate);
    reg_uart_ctrl0 = g_bwpc; //set bwpc
    reg_uart_clk_div = (g_uart_div | FLD_UART_CLK_DIV_EN); //set div_clock
	reg_uart_rx_timeout = (g_bwpc+1) * 12; //one byte includes 12 bits at most

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
			return;
		}
	}
	else{ //if not parity,close the parity function
		BM_CLR(reg_uart_ctrl0, FLD_UART_PARITY_EN);  //close parity function
	}

	/******************3.config stop bit**********************************************/
	BM_CLR(reg_uart_ctrl0, FLD_UART_STOP_BIT);
	reg_uart_ctrl0 |= MASK_VAL(FLD_UART_STOP_BIT, StopBit);
}

/**
 * @brief     enable uart DMA mode
 * @param[in] none
 * @return    none
 */
void uart_dma_enable(unsigned char rx_dma_en, unsigned char tx_dma_en)
{
	//enable DMA function of tx and rx
	if(rx_dma_en){
		reg_uart_ctrl0 |= FLD_UART_RX_DMA_EN ;
	}else{
		reg_uart_ctrl0 &= (~FLD_UART_RX_DMA_EN );
	}

	if(tx_dma_en){
		reg_uart_ctrl0  |= FLD_UART_TX_DMA_EN;
	}else{
		reg_uart_ctrl0	&= (~FLD_UART_TX_DMA_EN);
	}
}

/**
 * @brief     config the irq of uart tx and rx
 * @param[in] rx_irq_en - 1:enable rx irq. 0:disable rx irq
 * @param[in] tx_irq_en - 1:enable tx irq. 0:disable tx irq
 * @return    none
 */
void uart_irq_enable(unsigned char rx_irq_en, unsigned char tx_irq_en)
{
	BM_SET(reg_dma0_ctrl, FLD_DMA_WR_MEM); //set DMA0 mode to 0x01 for receive.write to memory
	BM_CLR(reg_dma1_ctrl, FLD_DMA_WR_MEM); //set DMA1 mode to 0x00 for send. read from memory

	if(rx_irq_en){
		reg_dma_chn_irq_msk |= FLD_DMA_UART_RX; //enable uart rx dma interrupt
	}else{
		reg_dma_chn_irq_msk &= ~FLD_DMA_UART_RX; //disable uart rx dma interrupt
	}

	if(tx_irq_en){
		reg_dma_chn_irq_msk |=  FLD_DMA_UART_TX;  //enable uart tx dma interrupt
	}else{
		reg_dma_chn_irq_msk &= ~FLD_DMA_UART_TX;  //disable uart tx dma interrupt
	}

	if(tx_irq_en||rx_irq_en){
		reg_irq_mask |= FLD_IRQ_UART_EN;
	}
	else{
		reg_irq_mask &= ~FLD_IRQ_UART_EN;
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

void uart_rx_buff_init(unsigned char *recAddr, unsigned short recBuffLen){
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
static unsigned char   *tx_buff = 0;
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
unsigned char uart_get_irq_flag(UART_IrqFlagTypeDef UART_Flag)
{
	return (reg_dma_irq_src & UART_Flag) ?1:0;
}

/**
 * @Brief: Only use for DMA mode of UART.(GaoQiu add)
 * @Param:
 * @Return:
 */
void uart_clear_irq_flag(UART_IrqFlagTypeDef UART_Flag)
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
void UART_FlowCtrlPinInit(UART_FlowCtrlPinTypeDef flowCtrlPin)
{
#if(MCU_CORE_TYPE == MCU_CORE_5316)
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
#elif(MCU_CORE_TYPE == MCU_CORE_5317)
	/* CTS Pin Configuration. ------------------------------------------------*/
	if(flowCtrlPin == UART_FLOW_CTRL_CTS_PB2)
	{
		gpio_set_func(GPIO_PB2,AS_UART);//CTS
		GPIOB_AF->RegBits.P2_AF = GPIOB2_UART_CTS;

		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_UART_CTS)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_UART_CTS)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_CTS_PB7)
	{
		gpio_set_func(GPIO_PB7,AS_UART);
		GPIOB_AF->RegBits.P7_AF = GPIOB7_UART_CTS;

		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_UART_CTS)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_UART_CTS)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_CTS_PC2)
	{
		gpio_set_func(GPIO_PC2,AS_UART);
		GPIOC_AF->RegBits.P2_AF = GPIOC2_UART_CTS;

		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_UART_CTS)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_UART_CTS)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}
	}
	/* RTS Pin Configuration. ------------------------------------------------*/
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PB3)
	{
		gpio_set_func(GPIO_PB3,AS_UART);
		GPIOB_AF->RegBits.P3_AF = GPIOB3_UART_RTS;

		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_UART_RTS)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_UART_RTS)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PB6)
	{
		gpio_set_func(GPIO_PB6,AS_UART);
		GPIOB_AF->RegBits.P6_AF = GPIOB6_UART_RTS;

		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_UART_RTS)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_UART_RTS)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);
		}
	}
	else if(flowCtrlPin == UART_FLOW_CTRL_RTS_PC3)
	{
		gpio_set_func(GPIO_PC3,AS_UART);
		GPIOC_AF->RegBits.P3_AF = GPIOC3_UART_RTS;

		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_UART_RTS)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_UART_RTS)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
	}
#endif
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

