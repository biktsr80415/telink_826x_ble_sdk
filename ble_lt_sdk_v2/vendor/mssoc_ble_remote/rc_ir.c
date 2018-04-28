/*
 * rc_ir.c
 *
 *  Created on: 2015-12-2
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "rc_ir.h"
#include "app_config.h"


#if (REMOTE_IR_ENABLE)




/////////////  NEC  protocol  /////////////////////////////////////////////
//start
#define IR_INTRO_CARR_TIME_NEC          9000
#define IR_INTRO_NO_CARR_TIME_NEC       4500
//stop
#define IR_END_TRANS_TIME_NEC           563  // user define
//repeat
#define IR_REPEAT_CARR_TIME_NEC         9000
#define IR_REPEAT_NO_CARR_TIME_NEC      2250
#define IR_REPEAT_LOW_CARR_TIME_NEC		560
//data "1"
#define IR_HIGH_CARR_TIME_NEC	        560
#define IR_HIGH_NO_CARR_TIME_NEC		1690
//data "0"
#define IR_LOW_CARR_TIME_NEC			560
#define IR_LOW_NO_CARR_TIME_NEC         560



int ir_sending_check(void);




void ir_nec_send(u8 addr1, u8 addr2, u8 cmd)
{

	if(ir_send_ctrl.last_cmd != cmd)
	{
		if(ir_sending_check())
		{
			return;
		}


		ir_send_ctrl.last_cmd = cmd;

	//// set waveform input in sequence //////

		pwm_set_waveform_input_1(WAVEFORM2_ID);  //waveform2 is for start bit

		u32 data = (~cmd)<<24 | cmd<<16 | addr2<<8 | addr1;
		for(int i=0;i<32;i++){
			if(data & BIT(i)){
				pwm_set_waveform_input_1(WAVEFORM1_ID); //waveform1 is for logic_1
			}
			else{
				pwm_set_waveform_input_1(WAVEFORM0_ID); //waveform0 is for logic_0
			}
		}

		pwm_set_waveform_input_1(WAVEFORM3_ID); //waveform2 is for stop bit


		if(1){  //need repeat
			ir_send_ctrl.repeat_enable = 1;  //need repeat signal
		}
		else{  //no need repeat
			ir_send_ctrl.repeat_enable = 0;  //no need repeat signal
		}


		reg_pwm0_fifo_mode_irq_sta = FLD_IRQ_PWM0_FIFO_MODE_STOP;   //clear  fifo mode stop irq status
		reg_pwm0_fifo_mode_irq_mask |= FLD_IRQ_PWM0_FIFO_MODE_STOP; //enable fifo mode stop irq mask


		ir_send_ctrl.is_sending = IR_SENDING_DATA;

		ir_send_ctrl.sending_start_time = clock_time();
		pwm_start(PWM0_ID);


	}
}









int ir_is_sending()
{
	if(ir_send_ctrl.is_sending && clock_time_exceed(ir_send_ctrl.sending_start_time, 300*1000))
	{
		ir_send_ctrl.is_sending = IR_SENDING_NONE;
		pwm_stop(PWM0_ID);

	}

	return ir_send_ctrl.is_sending;
}

int ir_sending_check(void)
{
	u8 r = irq_disable();
	if(ir_is_sending()){
		irq_restore(r);
		return 1;
	}
	irq_restore(r);
	return 0;
}



void ir_send_release(void){
	u8 r = irq_disable();

	ir_send_ctrl.last_cmd = 0xff;

	if(ir_send_ctrl.is_sending == IR_SENDING_REPEAT){
		pwm_stop(PWM0_ID);
	}

	ir_send_ctrl.is_sending = IR_SENDING_NONE;

	reg_pwm0_fifo_mode_irq_sta = FLD_IRQ_PWM0_FIFO_MODE_STOP;   //clear  fifo mode stop irq status
	reg_pwm0_fifo_mode_irq_mask &= ~FLD_IRQ_PWM0_FIFO_MODE_STOP; //disable fifo mode stop irq mask


	irq_restore(r);
}








#if (REMOTE_IR_ENABLE)
_attribute_ram_code_
#endif
void rc_ir_irq_prc(void)
{

	if(reg_pwm0_fifo_mode_irq_sta & FLD_IRQ_PWM0_FIFO_MODE_STOP){
		reg_pwm0_fifo_mode_irq_sta = FLD_IRQ_PWM0_FIFO_MODE_STOP; //clear irq status

		if(ir_send_ctrl.repeat_enable){

			if(ir_send_ctrl.is_sending == IR_SENDING_DATA){
				ir_send_ctrl.is_sending = IR_SENDING_REPEAT;

				u32 tick_2_repeat_sysClockTimer16M = 110*CLOCK_16M_SYS_TIMER_CLK_1MS - (clock_time() - ir_send_ctrl.sending_start_time);
				u32 tick_2_repeat_sysTimer = (tick_2_repeat_sysClockTimer16M*CLOCK_SYS_CLOCK_1US>>4);

				pwm_config_waveform_carrier_num(WAVEFORM6_HEAD, (tick_2_repeat_sysTimer + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);

				pwm_set_waveform_input_1(WAVEFORM6_ID);

				for(int i=0;i<10;i++){
					pwm_set_waveform_input_2(WAVEFORM4_ID, WAVEFORM5_ID); //add 10 repeat here
				}

				pwm_start(PWM0_ID);
			}
			else if(ir_send_ctrl.is_sending == IR_SENDING_REPEAT){

				for(int i=0;i<10;i++){
					pwm_set_waveform_input_2(WAVEFORM4_ID, WAVEFORM5_ID); //add 10 repeat here
				}

				pwm_start(PWM0_ID);

			}
		}
		else{
			ir_send_release();
		}

	}
}





void rc_ir_init(void)
{

//only pwm0 support fifo mode
	gpio_set_func(GPIO_PB0, AS_PWM0);
	pwm_set_mode(PWM0_ID, PWM_IR_FIFO_MODE);
	pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning

	//config carrier: 38k, 1/3 duty
	pwm_set_cycle_and_duty(PWM0_ID, PWM_CARRIER_CYCLE_TICK,  PWM_CARRIER_HIGH_TICK );


//waveform0:  logic_0, 560 us carrier,  560 us low
	pwm_config_waveform_carrier_num(WAVEFORM0_HEAD, (560 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM0_HEAD, 1);
	pwm_config_waveform_carrier_num(WAVEFORM0_TAIL, (560 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM0_TAIL, 0);


//waveform1:  logic_1, 560 us carrier,  1690 us low
	pwm_config_waveform_carrier_num(WAVEFORM1_HEAD, (560 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM1_HEAD, 1);
	pwm_config_waveform_carrier_num(WAVEFORM1_TAIL, (1690 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM1_TAIL, 0);


//waveform2:  start bit, 9000 us carrier,  4500 us low
	pwm_config_waveform_carrier_num(WAVEFORM2_HEAD, (9000 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM2_HEAD, 1);
	pwm_config_waveform_carrier_num(WAVEFORM2_TAIL, (4500 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM2_TAIL, 0);


//waveform3:  stop bit,  560 us carrier, 500 us low
	pwm_config_waveform_carrier_num(WAVEFORM3_HEAD, (560 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM3_HEAD, 1);

	pwm_config_waveform_carrier_num(WAVEFORM3_TAIL, (500 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM3_TAIL, 0);



//waveform4:  repeat signal  first part,  9000 us carrier, 2250 us low
	pwm_config_waveform_carrier_num(WAVEFORM4_HEAD, (9000 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM4_HEAD, 1);
	pwm_config_waveform_carrier_num(WAVEFORM4_TAIL, (2250 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM4_TAIL, 0);

//waveform5:  repeat signal  second part,  560 us carrier, 99190 us low(110 ms - 9000us - 2250us - 560us = 99190 us)
	pwm_config_waveform_carrier_num(WAVEFORM5_HEAD, (560 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM5_HEAD, 1);
	pwm_config_waveform_carrier_num(WAVEFORM5_TAIL, (99190 * CLOCK_SYS_CLOCK_1US + PWM_CARRIER_HALF_CYCLE_TICK)/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM5_TAIL, 0);



//waveform6:  when data end, the interval between data end and repeat signal, we add this waveform to make sure the repeat signal 110ms timing
	pwm_config_waveform_carrier_num(WAVEFORM6_HEAD, 0);  //this timing will update in fifo stop irq
	pwm_set_waveform_carrier_en(WAVEFORM6_HEAD, 0);
	pwm_config_waveform_carrier_num(WAVEFORM6_TAIL, 1);
	pwm_set_waveform_carrier_en(WAVEFORM6_TAIL, 0);



//add fifo stop irq, when all waveform send over, this irq will triggers
	//enable system irq PWM
	reg_irq_mask |= FLD_IRQ_SW_PWM_EN;

	//enable pwm0 fifo stop irq
	reg_pwm0_fifo_mode_irq_sta = FLD_IRQ_PWM0_FIFO_MODE_STOP; //clear irq status
	//reg_pwm0_fifo_mode_irq_mask |= FLD_IRQ_PWM0_FIFO_MODE_STOP;


	ir_send_ctrl.last_cmd = 0xff; //must
}








#endif
