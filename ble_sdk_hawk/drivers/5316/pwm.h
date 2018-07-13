#ifndef PWM_H_
#define PWM_H_

#include "register.h"
#include "clock.h"

typedef enum {
	PWM0 = 0x00,
	PWM1,
	PWM2,
	PWM3,
	PWM4,
	PWM5,
}ePWM_IdTypeDef;

typedef enum{
	PWM_SRC_NORMAL = 0x00,   //PWM duty and cycle from TCMP0/TMAX0
	PWM_SRC_SHADOW = BIT(14),//PWM duty and cycle from TCMP0_SHADOW/TMAX0_SHADOW
}ePWM_SrcTypeDef;

typedef enum{
	PWM_MODE_NORMAL      = 0x00,
	PWM_MODE_COUNT       = 0x01,
	PWM_MODE_IR          = 0x03,
	PWM_MODE_IR_FIFO     = 0x07,
	PWM_MODE_IR_DMA_FIFO = 0x0f,
}ePWM_ModeTypeDef;

typedef enum{
	/* PWM Mask0/state0 bit field define. */
	FLD_PWM_IRQ_PWM0_PNUM              = BIT(0),
	FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY = BIT(1),
	FLD_PWM_IRQ_PWM0_FRAME             = BIT(2),
	FLD_PWM_IRQ_PWM1_FRAME             = BIT(3),
	FLD_PWM_IRQ_PWM2_FRAME             = BIT(4),

	/* PWM Mask1/state1 bit field define. */
	FLD_PWM_IRQ_PWM0_IR_FIFO_CNT = 0x80|BIT(0),
}ePWM_IrqTypeDef;

static inline void pwm_set_mode(ePWM_IdTypeDef pwm_id, ePWM_ModeTypeDef mode)
{
	if(pwm_id != PWM0)
		return;
	else
	{
		reg_pwm_mode = MASK_VAL(FLD_PWM_MODE_PWM0, mode);
	}
}

//Set PWM clock frequency
static inline void pwm_set_clk(int system_clock_hz, int pwm_clk){
	reg_rst_clk0 &= ~FLD_RST_PWM;
	reg_clk_en |= FLD_CLK_PWM_EN;
	reg_pwm_clk = (int)system_clock_hz /pwm_clk - 1;
}

static inline void pwm_set_cmp(ePWM_IdTypeDef pwm_id, unsigned short cmp_tick){
	reg_pwm_cmp(pwm_id) = cmp_tick;
}

static inline void pwm_set_cycle(ePWM_IdTypeDef pwm_id, unsigned short cycle_tick){
	reg_pwm_max(pwm_id) = cycle_tick;
}


static inline void pwm_set_cycle_and_duty(ePWM_IdTypeDef pwm_id, unsigned short cycle_tick,
		                                  unsigned short cmp_tick)
{
	reg_pwm_cycle(pwm_id) = MASK_VAL(FLD_PWM_CMP, cmp_tick, FLD_PWM_MAX, cycle_tick);
}

static inline void pwm_set_shadow_cycle_and_duty(ePWM_IdTypeDef pwm_id, unsigned short cycle_tick,
		                                  	  	  unsigned short cmp_tick)
{
	if(pwm_id == PWM0){
		reg_pwm_cmp_shadow = cmp_tick;
		reg_pwm_max_shadow = cycle_tick;
	}
}

/**
 * @Brief:  Only PWM0 have counter mode in 5316/5317 MCU.
 * @Param:  pwm_id -> PWM0
 * @Retval: None.
 */
static inline void pwm_set_pulse_num(ePWM_IdTypeDef pwm_id, unsigned short pulse_num)
{
	if(pwm_id == PWM0)
	{
		reg_pwm_pulse_num = pulse_num;
	}
}

static inline void pwm_start(ePWM_IdTypeDef pwm_id){
	BM_SET(reg_pwm_enable, BIT(pwm_id));
}

static inline void pwm_stop(ePWM_IdTypeDef pwm_id){
	BM_CLR(reg_pwm_enable, BIT(pwm_id));
}

//revert PWMx
static inline void pwm_revert(ePWM_IdTypeDef pwm_id){
	reg_pwm_invert |= BIT(pwm_id);
}


//revert PWMx_N
static inline void pwm_n_revert(ePWM_IdTypeDef pwm_id){
	reg_pwm_n_invert |= BIT(pwm_id);
}

/**
 * @Brief: Set PWM first polarity.
 * @Param:
 * @Retval: None.
 */
static inline void pwm_polarity_set(ePWM_IdTypeDef pwm_id, int polarity){
	if(polarity == 0){
		BM_SET(reg_pwm_polarity, BIT(pwm_id));//First low level.
	}else{
		BM_CLR(reg_pwm_polarity, BIT(pwm_id));//First high level.
	}
}

static inline void pwm_set_fifo_level(unsigned char level)
{
	reg_pwm_fifo_level = level;
}

static inline unsigned char pwm_get_fifo_data_num(void)
{
	return (reg_pwm_fifo_state & 0x0f);
}

static inline bool pwm_fifo_is_empty(void)
{
	return (reg_pwm_fifo_state & BIT(4)) ? 1:0;
}

static inline bool pwm_fifo_is_full(void)
{
	return (reg_pwm_fifo_state & BIT(5)) ? 1:0;
}

static inline void pwm_interrupt_enable(ePWM_IrqTypeDef irq){
	if(irq == FLD_PWM_IRQ_PWM0_IR_FIFO_CNT)
	{
		irq &= 0x7f;
		reg_pwm_irq_mask1 |= irq;
	}
	else
	{
		reg_pwm_irq_mask0 |= irq;
	}

	reg_irq_mask |= FLD_IRQ_PWM_EN;
}

static inline void pwm_interrupt_disable(ePWM_IrqTypeDef irq){
	if(irq == FLD_PWM_IRQ_PWM0_IR_FIFO_CNT)
	{
		irq &= 0x7f;
		reg_pwm_irq_mask1 &= ~(irq);
	}
	else
	{
		reg_pwm_irq_mask0 &= ~irq;
	}

	reg_irq_mask &= ~FLD_IRQ_PWM_EN;
}

static inline unsigned char pwm_get_irq_status(ePWM_IrqTypeDef irq)
{
	if(reg_irq_src & FLD_IRQ_PWM_EN)//must
	{
		if(irq == FLD_PWM_IRQ_PWM0_IR_FIFO_CNT)
		{
			irq &= 0x7f;
			return (reg_pwm_irq_state1 & irq) ? 1:0;
		}

		return (reg_pwm_irq_state0 & irq) ? 1:0;
	}
	return 0;
}

static inline void pwm_clear_irq_status(ePWM_IrqTypeDef irq)
{
	if(irq == FLD_PWM_IRQ_PWM0_IR_FIFO_CNT)
	{
		irq &= 0x7f;
		reg_pwm_irq_state1 |= irq;
	}
	else
	{
		reg_pwm_irq_state0 |= irq;
	}
}

static inline unsigned short pwm_set_waveform_data(unsigned char hasCarrier, ePWM_SrcTypeDef pwmSrc, unsigned int frameNum)
{
	return (((hasCarrier)<<15) | (pwmSrc) | ((frameNum) & 0x3fff));
}

static inline void pwm_dma_ir_data_config(unsigned short* pdat)
{
	reg_dma5_addr = (unsigned int)pdat & 0xffff;

	reg_dma5_ctrl &= ~(FLD_DMA_BUF_SIZE|FLD_DMA_WR_MEM);
	reg_dma5_ctrl |= MASK_VAL(FLD_DMA_BUF_SIZE,0x20);
	//reg_dma5_ctrl  &= ~FLD_DMA_WR_MEM;
}

static inline void pwm_dma_enable(void)
{
	reg_dma_tx_rdy0 |= FLD_DMA_PWM;
}

#endif /* PWM_H_ */
