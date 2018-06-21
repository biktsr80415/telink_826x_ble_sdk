#ifndef PWM_H_
#define PWM_H_

#include "register.h"
#include "clock.h"

typedef enum {
	PWM0_ID = 0,
	PWM1_ID,
	PWM2_ID,
	PWM3_ID,
	PWM4_ID,
	PWM5_ID,
}pwm_id;


typedef enum{
	PWM_NORMAL_MODE   = 0x00,
	PWM_COUNT_MODE    = 0x01,
	PWM_IR_MODE       = 0x03,
	PWM_IR_FIFO_MODE  = 0x07,
	PWM_IR_DMA_FIFO_MODE  = 0x0F,
}pwm_mode;



typedef enum{
	PWM_IRQ_PWM0_PNUM =					BIT(0),
	PWM_IRQ_PWM0_IR_DMA_FIFO_DONE =		BIT(1),
	PWM_IRQ_PWM0_FRAME =				BIT(2),
	PWM_IRQ_PWM1_FRAME =				BIT(3),
	PWM_IRQ_PWM2_FRAME =				BIT(4),
	PWM_IRQ_PWM3_FRAME =				BIT(5),
	PWM_IRQ_PWM4_FRAME 	=				BIT(6),
	PWM_IRQ_PWM5_FRAME =				BIT(7),
}PWM_IRQ;



static inline void pwm_set_mode(pwm_id id, pwm_mode mode){
	if(PWM0_ID == id){
		reg_pwm0_mode = mode;  //only PWM0 has count/IR/fifo IR mode
	}
}

//Set PWM clock frequency
static inline void pwm_set_clk(int system_clock_hz, int pwm_clk){
	reg_pwm_clk = (int)system_clock_hz /pwm_clk - 1;
}


static inline void pwm_set_cmp(pwm_id id, unsigned short cmp_tick){
	reg_pwm_cmp(id) = cmp_tick;
}

static inline void pwm_set_cycle(pwm_id id, unsigned short cycle_tick){
	reg_pwm_max(id) = cycle_tick;
}


static inline void pwm_set_cycle_and_duty(pwm_id id, unsigned short cycle_tick, unsigned short cmp_tick){
	reg_pwm_cycle(id) = MASK_VAL(FLD_PWM_CMP, cmp_tick, FLD_PWM_MAX, cycle_tick);
}


static inline void pwm_set_pwm0_shadow_cycle_and_duty(unsigned short cycle_tick, unsigned short cmp_tick)
{
	reg_pwm_tcmp0_shadow = cmp_tick;
	reg_pwm_tmax0_shadow = cycle_tick;
}


static inline void pwm_set_phase(pwm_id id, unsigned short phase){
	reg_pwm_phase(id) = phase;
}


static inline void pwm_set_pulse_num(pwm_id id, unsigned short pulse_num){
	if(PWM0_ID == id){
		reg_pwm_pulse_num = pulse_num;
	}

}

static inline void pwm_start(pwm_id id){
	if(PWM0_ID == id){
		BM_SET(reg_pwm0_enable, BIT(0));
	}
	else{
		BM_SET(reg_pwm_enable, BIT(id));
	}
}

static inline void pwm_stop(pwm_id id){
	if(PWM0_ID == id){
		BM_CLR(reg_pwm0_enable, BIT(0));
	}
	else{
		BM_CLR(reg_pwm_enable, BIT(id));
	}
}

//revert PWMx
static inline void pwm_revert(pwm_id id){
	reg_pwm_invert |= BIT(id);
}


//revert PWMx_N
static inline void pwm_n_revert(pwm_id id){
	reg_pwm_n_invert |= BIT(id);
}

static inline void pwm_polo_enable(pwm_id id, int en){
	if(en){
		BM_SET(reg_pwm_pol, BIT(id));
	}else{
		BM_CLR(reg_pwm_pol, BIT(id));
	}
}


static inline void pwm_set_interrupt_enable(PWM_IRQ irq){
//	if(en){
//		BM_SET(reg_pwm_irq_mask, irq);
//	}else{
//		BM_CLR(reg_pwm_irq_mask, irq);
//	}
	BM_SET(reg_pwm_irq_mask, irq);
}

static inline void pwm_set_interrupt_disable(PWM_IRQ irq){
	BM_CLR(reg_pwm_irq_mask, irq);
}

static inline void pwm_clear_interrupt_status( PWM_IRQ irq){
	reg_pwm_irq_sta = irq;
}










/**********************************************************************************************
 *
 *     PWM FIFO DMA MODE
 *
 *********************************************************************************************/


typedef enum{
	PWM0_PULSE_NORMAL =		0,       // duty cycle and period from TCMP0/TMAX0 					 0x794~0x797
	PWM0_PULSE_SHADOW =		BIT(14), // duty cycle and period from TCMP0_SHADOW / TMAX0_SHADOW   0x7c4~0x7c7
}Pwm0Pulse_SelectDef;


//carrier_en: must 1 or 0
static inline unsigned short pwm_config_dma_fifo_waveform(int carrier_en, Pwm0Pulse_SelectDef pulse,  unsigned short pulse_num)
{
	return  ( carrier_en<<15 | pulse | (pulse_num & 0x3fff) );
}



static inline void pwm_set_dma_address(void * pdat)
{
	reg_dma_pwm_addr = (unsigned short)((unsigned int)pdat);
	reg_dma7_addrHi = 0x04;
	reg_dma_pwm_mode  &= ~FLD_DMA_WR_MEM;
}


static inline void pwm_start_dma_ir_sending(void)
{
	reg_dma_chn_en |= FLD_DMA_CHN_PWM;
	reg_dma_tx_rdy0 |= FLD_DMA_CHN_PWM;
}


static inline void pwm_stop_dma_ir_sending(void)
{
//	reg_dma_tx_rdy0 &= ~FLD_DMA_PWM;

	reg_rst0 = FLD_RST0_PWM;
	sleep_us(20);  //1us <-> 4 byte
	reg_rst0 = 0;
}

#endif /* PWM_H_ */
