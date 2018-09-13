/*
 * dma.h
 *
 *  Created on: 2018-5-31
 *      Author: Administrator
 */

#ifndef DMA_H_
#define DMA_H_


#define DMA_RFRX_LEN_HW_INFO				0
#define DMA_RFRX_OFFSET_HEADER				4
#define DMA_RFRX_OFFSET_RFLEN				5
#define DMA_RFRX_OFFSET_DATA				6



/**
 * @brief     This function resets the DMA module.
 * @param[in] none
 * @return    none
 */
static inline void dma_reset(void)
{
	reg_rst1 |= FLD_RST1_DMA;
	reg_rst1 &= (~FLD_RST1_DMA);
}



static inline void dma_irq_enable(unsigned int msk)
{
	reg_dma_chn_irq_msk |= msk;
}
static inline void dma_irq_disable(unsigned int msk)
{
	reg_dma_chn_irq_msk &= ~msk;
}


/*****************************************************************
chn: see defines of reg_dma_chn_irq_msk in register_8258.h

	enum{
		FLD_DMA_CHN0 =	BIT(0),		FLD_DMA_CHN_UART_RX =	BIT(0),
		FLD_DMA_CHN1 =	BIT(1),		FLD_DMA_CHN_UART_TX =	BIT(1),
		FLD_DMA_CHN2 =	BIT(2),		FLD_DMA_CHN_RF_RX =		BIT(2),
		FLD_DMA_CHN3 =	BIT(3),		FLD_DMA_CHN_RF_TX =		BIT(3),
		FLD_DMA_CHN4 =	BIT(4),		FLD_DMA_CHN_AES_DECO =  BIT(4),
		FLD_DMA_CHN5 =	BIT(5),     FLD_DMA_CHN_AES_CODE =  BIT(5),
		FLD_DMA_CHN7 =	BIT(7),		FLD_DMA_CHN_PWM   	 =	BIT(7),
	};


en = 1: enable
en = 0: disable
 *****************************************************************/

static inline void dma_chn_irq_enable(unsigned char chn, unsigned int en)
{
	reg_dma_irq_status = chn;

	if(en){
		reg_dma_chn_en |= chn;
		reg_dma_chn_irq_msk |= chn;
	}
	else{
		reg_dma_chn_en &= ~chn;
		reg_dma_chn_irq_msk &= ~chn;
	}
}




/**
 * @brief      Clear IRQ status of uart.
 * @param[in]  irq_src - select tx or rx irq.
 * @return     none
 */
static inline void dma_chn_irq_status_clr(unsigned char irq_status)
{
	reg_dma_irq_status = irq_status;
}


/**
 * @brief      Get IRQ status of uart.
 * @param[in]  irq_src - select tx or rx irq.
 * @return     none
 */
static inline unsigned char dma_chn_irq_status_get(void)
{
    return reg_dma_irq_status;
}



#endif /* DMA_H_ */
