#ifndef PWM_H_
#define PWM_H_



typedef enum {
	PWM0_ID = 0,
	PWM1_ID,
	PWM2_ID,
	PWM3_ID,
	PWM4_ID,
	PWM5_ID,
}pwm_id;





static inline void pwm_set_mode(pwm_id id, int mode){
	if(PWM0_ID == id){
		reg_pwm_mode = mode;  //only PWM0 has count/IR/fifo IR mode
	}
}

//Set PWM clock frequency
static inline void pwm_set_clk(int system_clock_hz, int pwm_clk){
	reg_pwm_clk = (int)system_clock_hz /pwm_clk - 1;
}


static inline void pwm_set_cmp(pwm_id id, u16 cmp_tick){
	reg_pwm_cmp(id) = cmp_tick;
}

static inline void pwm_set_cycle(pwm_id id, u16 cycle_tick){
	reg_pwm_max(id) = cycle_tick;
}


static inline void pwm_set_cycle_and_duty(pwm_id id, u16 cycle_tick, u16 cmp_tick){
	reg_pwm_cycle(id) = MASK_VAL(FLD_PWM_CMP, cmp_tick, FLD_PWM_MAX, cycle_tick);
}


static inline void pwm_set_phase(pwm_id id, u16 phase){
	reg_pwm_phase(id) = phase;
}


static inline void pwm_set_pulse_num(pwm_id id, u16 pulse_num){
	if(PWM0_ID == id){
		reg_pwm_pulse_num = pulse_num;
	}

}

static inline void pwm_start(pwm_id id){
	BM_SET(reg_pwm_enable, BIT(id));
}

static inline void pwm_stop(pwm_id id){
	BM_CLR(reg_pwm_enable, BIT(id));
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


typedef enum{
	WAVEFORM0_ID = 0x00,
	WAVEFORM1_ID = 0x01,
	WAVEFORM2_ID = 0x02,
	WAVEFORM3_ID = 0x03,
	WAVEFORM4_ID = 0x04,
	WAVEFORM5_ID = 0x05,
	WAVEFORM6_ID = 0x06,
	WAVEFORM7_ID = 0x07,
}WAVEFORM_ID;


typedef enum{
	WAVEFORM0_TAIL = 0x00,
	WAVEFORM0_HEAD = 0x01,
	WAVEFORM1_TAIL = 0x02,
	WAVEFORM1_HEAD = 0x03,
	WAVEFORM2_TAIL = 0x04,
	WAVEFORM2_HEAD = 0x05,
	WAVEFORM3_TAIL = 0x06,
	WAVEFORM3_HEAD = 0x07,
	WAVEFORM4_TAIL = 0x08,
	WAVEFORM4_HEAD = 0x09,
	WAVEFORM5_TAIL = 0x0a,
	WAVEFORM5_HEAD = 0x0b,
	WAVEFORM6_TAIL = 0x0c,
	WAVEFORM6_HEAD = 0x0d,
	WAVEFORM7_TAIL = 0x0e,
	WAVEFORM7_HEAD = 0x0f,
}WAVEFORM_TypeDef;





static inline void pwm_set_waveform_carrier_en(WAVEFORM_TypeDef wf_type, int en)
{
	u16 mask = BIT((wf_type^0x01));
	if(en){
		BM_SET(reg_pwm0_waveform_carrier_en, mask);
	}
	else{
		BM_CLR(reg_pwm0_waveform_carrier_en, mask);
	}
}


static inline void pwm_config_waveform_carrier_num(WAVEFORM_TypeDef wf_type, u16 carrier_num)
{
	reg_pwm0_waveform_carrier_pulse_num(wf_type) = carrier_num;
}



static inline void pwm_set_waveform_input_1(WAVEFORM_ID wf_id)
{
	reg_pwm0_fifo_input = (0xfff0 | wf_id);
}

static inline void pwm_set_waveform_input_2(WAVEFORM_ID wf_id1, WAVEFORM_ID wf_id2)
{
	reg_pwm0_fifo_input = (0xff00 | wf_id2<<4 | wf_id1);
}

static inline void pwm_set_waveform_input_3(WAVEFORM_ID wf_id1, WAVEFORM_ID wf_id2, WAVEFORM_ID wf_id3)
{
	reg_pwm0_fifo_input = (0xf000 | wf_id3<<8 | wf_id2<<4 | wf_id1);
}

static inline void pwm_set_waveform_input_4(WAVEFORM_ID wf_id1, WAVEFORM_ID wf_id2, WAVEFORM_ID wf_id3, WAVEFORM_ID wf_id4)
{
	reg_pwm0_fifo_input = (wf_id4<<12 | wf_id3<<8 | wf_id2<<4 | wf_id1);
}





#endif /* PWM_H_ */
