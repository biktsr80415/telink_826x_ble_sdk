/*
 * app_pwm.c
 *
 *  Created on: 2017-8-25
 *      Author: Administrator
 */

#include "tl_common.h"
#include "drivers.h"


#if (DRIVER_TEST_MODE == TEST_PWM)


//normal mode
#define  TEST_PWM_NORMAL_MODE_1				1
#define  TEST_PWM_NORMAL_MODE_2				2
#define  TEST_PWM_NORMAL_MODE_3				3

//fifo mode
#define  TEST_PWM0_FIFO_MODE_1				4





#define  TEST_PWM_SELECT					TEST_PWM0_FIFO_MODE_1







#define IR_CARRIER_FREQ				38000  	// 1 frame -> 1/38k -> 1000/38 = 26 us
#define PWM_CARRIER_CYCLE_TICK		( CLOCK_SYS_CLOCK_HZ/IR_CARRIER_FREQ )  //16M: 421 tick, f = 16000000/421 = 38004,T = 421/16=26.3125 us
#define PWM_CARRIER_HIGH_TICK		( PWM_CARRIER_CYCLE_TICK/3 )   // 1/3 duty





/*********************************************************************************
    PWM0   :  PB0.  PB5.  PC2.	PD0.  PD4.
    PWM1   :  PB1.  PC0.  PD1.
    PWM2   :  PB2.  PD2.
    PWM3   :  PB3.  PD3.
    PWM4   :  PB4.  PC0.  PD4.
    PWM5   :  PB5.  PC1.  PD5.
    PWM0_N :  PA0.  PB1.  PB4.	PD5.
    PWM1_N :  PB0.  PC3.
    PWM2_N :  PC4.
    PWM3_N :  PC5.
    PWM4_N :  PB6.
    PWM5_N :  PB7.
 *********************************************************************************/

void app_pwm_test(void)
{
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);


#if (TEST_PWM_SELECT == TEST_PWM_NORMAL_MODE_1)  //test PWMx (0~5)   normal mode
	//PB0 PWM0  1ms cycle  1/2 duty
	gpio_set_func(GPIO_PB0, AS_PWM0);
	pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM0_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (500 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM0_ID);


	//PB1 PWM1  1ms cycle  1/3 duty
	gpio_set_func(GPIO_PB1, AS_PWM1);
	pwm_set_mode(PWM1_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM1_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM1_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (333 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM1_ID);


	//PB2 PWM2   1ms cycle  1/4 duty
	gpio_set_func(GPIO_PB2, AS_PWM2);
	pwm_set_mode(PWM2_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM2_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM2_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (250 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM2_ID);


	//PB3 PWM3  1ms cycle  1/5 duty
	gpio_set_func(GPIO_PB3, AS_PWM3);
	pwm_set_mode(PWM3_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM3_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM3_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (200 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM3_ID);

	//PB4 PWM4  1ms cycle  2/3 duty
	gpio_set_func(GPIO_PB4, AS_PWM4);
	pwm_set_mode(PWM4_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM4_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM4_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (667 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM4_ID);


	//PB5 PWM5  1ms cycle  3/4 duty
	gpio_set_func(GPIO_PB5, AS_PWM5);
	pwm_set_mode(PWM5_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM5_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM5_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (750 * CLOCK_SYS_CLOCK_1US) );
	pwm_start(PWM5_ID);

#elif (TEST_PWM_SELECT == TEST_PWM_NORMAL_MODE_2)  //test PWMx and PWMx_N(0~2)   normal mode

	//PB5 PWM0     1ms cycle  1/3 duty
	//PA0 PWM0_N   1ms cycle  2/3 duty
	gpio_set_func(GPIO_PB5, AS_PWM0);
	gpio_set_func(GPIO_PA0, AS_PWM0_N);
	pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM0_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (333 * CLOCK_SYS_CLOCK_1US) );


	//PC0 PWM1     1ms cycle  1/4 duty
	//PB0 PWM1_N   1ms cycle  3/4 duty
	gpio_set_func(GPIO_PC0, AS_PWM1);
	gpio_set_func(GPIO_PB0, AS_PWM1_N);
	pwm_set_mode(PWM1_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM1_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM1_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (250 * CLOCK_SYS_CLOCK_1US) );


	//PD2 PWM2     1ms cycle  1/5 duty
	//PC4 PWM2_N   1ms cycle  4/5 duty
	gpio_set_func(GPIO_PD2, AS_PWM2);
	gpio_set_func(GPIO_PC4, AS_PWM2_N);
	pwm_set_mode(PWM2_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM2_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM2_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (200 * CLOCK_SYS_CLOCK_1US) );


	pwm_start(PWM0_ID);
	pwm_start(PWM1_ID);
	pwm_start(PWM2_ID);


#elif (TEST_PWM_SELECT == TEST_PWM_NORMAL_MODE_3)  //test PWMx and PWMx_N(3~5)   normal mode

	//PD3 PWM3     1ms cycle  1/3 duty
	//PC5 PWM3_N   1ms cycle  2/3 duty
	gpio_set_func(GPIO_PD3, AS_PWM3);
	gpio_set_func(GPIO_PC5, AS_PWM3_N);
	pwm_set_mode(PWM3_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM3_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM3_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (333 * CLOCK_SYS_CLOCK_1US) );


	//PC0 PWM4     1ms cycle  1/4 duty
	//PB6 PWM4_N   1ms cycle  3/4 duty
	gpio_set_func(GPIO_PC0, AS_PWM4);
	gpio_set_func(GPIO_PB6, AS_PWM4_N);
	pwm_set_mode(PWM4_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM4_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM4_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (250 * CLOCK_SYS_CLOCK_1US) );


	//PC1 PWM5     1ms cycle  1/5 duty
	//PB7 PWM5_N   1ms cycle  4/5 duty
	gpio_set_func(GPIO_PC1, AS_PWM5);
	gpio_set_func(GPIO_PB7, AS_PWM5_N);
	pwm_set_mode(PWM5_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM5_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM5_ID, (u16) (1000 * CLOCK_SYS_CLOCK_1US),  (u16) (200 * CLOCK_SYS_CLOCK_1US) );


	pwm_start(PWM3_ID);
	pwm_start(PWM4_ID);
	pwm_start(PWM5_ID);


#elif (TEST_PWM_SELECT == TEST_PWM0_FIFO_MODE_1)

	sleep_us(1000000);

	//only pwm0 support fifo mode
	gpio_set_func(GPIO_PB0, AS_PWM0);
	pwm_set_mode(PWM0_ID, PWM_IR_FIFO_MODE);
	pwm_set_phase(PWM0_ID, 0);   //no phase at pwm beginning

	//config carrier: 38k, 1/3 duty
	pwm_set_cycle_and_duty(PWM0_ID, PWM_CARRIER_CYCLE_TICK,  PWM_CARRIER_HIGH_TICK );


//waveform0:  9 ms carrier,  4.5 ms low
	pwm_config_waveform_carrier_num(WAVEFORM0_HEAD, 9000 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM0_HEAD, 1);

	pwm_config_waveform_carrier_num(WAVEFORM0_TAIL, 4500 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM0_TAIL, 0);


//waveform1:  560 us carrier,  560 us low
	pwm_config_waveform_carrier_num(WAVEFORM1_HEAD, 560 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM1_HEAD, 1);

	pwm_config_waveform_carrier_num(WAVEFORM1_TAIL, 560 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM1_TAIL, 0);


//waveform2:  560 us carrier,  1690 us low
	pwm_config_waveform_carrier_num(WAVEFORM2_HEAD, 560 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM2_HEAD, 1);

	pwm_config_waveform_carrier_num(WAVEFORM2_TAIL, 1690 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM2_TAIL, 0);


//waveform3:  263 us carrier,  500 us low
	pwm_config_waveform_carrier_num(WAVEFORM3_HEAD, 263 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM3_HEAD, 1);

	pwm_config_waveform_carrier_num(WAVEFORM3_TAIL, 500 * CLOCK_SYS_CLOCK_1US/PWM_CARRIER_CYCLE_TICK);
	pwm_set_waveform_carrier_en(WAVEFORM3_TAIL, 0);


	//set input: waveform0, waveform1, waveform2, waveform3, waveform0 in sequence
	pwm_set_waveform_input_4(WAVEFORM0_ID, WAVEFORM1_ID, WAVEFORM2_ID, WAVEFORM3_ID);
	pwm_set_waveform_input_1(WAVEFORM0_ID);


//add fifo stop irq, when all waveform send over, this irq will triggers
	//enable mcu global irq
	 irq_enable();

	//enable system irq PWM
	reg_irq_mask |= FLD_IRQ_SW_PWM_EN;

	//enable pwm0 fifo stop irq
	reg_pwm0_fifo_mode_irq_sta = FLD_IRQ_PWM0_FIFO_MODE_STOP; //clear irq status
	reg_pwm0_fifo_mode_irq_mask |= FLD_IRQ_PWM0_FIFO_MODE_STOP;


//PWM0 fifo mode begin
	DBG_CHN0_HIGH;  //debug
	pwm_start(PWM0_ID);

#else


#endif

}




_attribute_ram_code_ void app_pwm_irq_test_proc(void)
{

	if(reg_pwm0_fifo_mode_irq_sta & FLD_IRQ_PWM0_FIFO_MODE_STOP){
		reg_pwm0_fifo_mode_irq_sta = FLD_IRQ_PWM0_FIFO_MODE_STOP; //clear irq status
		DBG_CHN0_LOW;  //debug
	}
}




#endif
