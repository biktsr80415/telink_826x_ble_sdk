/*
 * rc_ir.c
 *
 *  Created on: 2015-12-2
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/ble_ll.h"

#include "rc_ir.h"

#if (__PROJECT_BLE_REMOTE__)

enum{
	IR_SEND_TYPE_TIME_SERIES,
	IR_SEND_TYPE_HALF_TIME_SERIES,
	IR_SEND_TYPE_BYTE,
};


ir_send_ctrl_t ir_send_ctrl;
u32 ir_byte_time[2][2];

static u32 ir_is_repeat_time;

u8  g_u8KeyType = IR_TYPE_NEC_TIANZUN;
u8 ir_is_operating = 0;

u8 ir_is_repeat_timer_enable;
static u8 ir_send_irq_idx;
static u8 ir_send_start_high;
static u8 last_cmd = 0xff;
static u8 ir_search_index_next_addr;


static ir_learn_ctrl_t ir_learn_ctrl;
static ir_search_index_t ir_index_data;
static ir_universal_pattern_t	ir_flash_universal_data1;
static ir_universal_pattern_t	ir_flash_universal_data2;
void ir_irq_send(void);
void ir_nec_send_repeat(void);
void ir_tc9012_send_repeat(u8 addr1);
void ir_rca_send_repeat(void);
void ir_upd6121_send_repeat(void);
void ir_konka_send_repeat(void);
void ir_tc9012_send(u8 addr1, u8 addr2,u8 local_data_code);
void ir_upd6121_send(u8 addr1, u8 addr2,u8 local_data_code);
void ir_konka_send(u8 addr,u8 local_data_code);
void ir_rca_send(u8 addr1, u8 addr2,u8 local_data_code);
void ir_nec_send(u8 addr1, u8 addr2, u8 cmd);

static inline void timer_enable_timer(int i){
	SET_FLD(reg_tmr_ctrl8, BIT(i * 3));
}

static inline void timer_disable_timer(int i){
	CLR_FLD(reg_tmr_ctrl8, BIT(i * 3));
}

static inline void timer_set_timeout(int i, u32 t){
	reg_tmr_tick(i) = 0;
	reg_tmr_capt(i) = (u32)t;
}


static int ir_send_repeat_timer(void *data){
#if 1
	ir_nec_send_repeat();  //proc nec only now
#else
    if(g_u8KeyType == IR_TYPE_SKYWORTH_TC9012)
	    ir_tc9012_send_repeat(0x0E);
    else if(g_u8KeyType == IR_TYPE_CHANGHONG_uPD6121)
	    ir_upd6121_send_repeat();
    else if(g_u8KeyType == IR_TYPE_KONKA_KONKA)
	    ir_konka_send_repeat();
    else if(g_u8KeyType == IR_TYPE_SUNSUNG_TC9012)
	    ir_tc9012_send_repeat(0x07);
    else if(g_u8KeyType == IR_TYPE_HiSense_uPD6121)
	    ir_upd6121_send_repeat();
    else if(g_u8KeyType == IR_TYPE_HAIER_TC9012)
	    ir_tc9012_send_repeat(0x18);
    else if(g_u8KeyType == IR_TYPE_TCL_RCA)
	    ir_rca_send_repeat();
    else
	    ir_nec_send_repeat();
#endif
	return 0;
}

//static ev_time_event_t ir_repeat_timer = {ir_send_repeat_timer};

void ir_send_ctrl_clear(void){
	ir_send_ctrl.is_sending = ir_send_ctrl.cnt = ir_send_ctrl.index = 0;
}
void ir_send_add_series_item(u32 *time_series, u8 series_cnt, u8 start_high){
	u8 i = ir_send_ctrl.cnt;
	ir_send_ctrl.data[i].type = IR_SEND_TYPE_TIME_SERIES;
	ir_send_ctrl.data[i].time_series = time_series;
	ir_send_ctrl.data[i].series_cnt = series_cnt;
	ir_send_ctrl.data[i].start_high = start_high;
	++ir_send_ctrl.cnt;
}

void ir_send_add_byte_item(u8 code, u8 start_high){
	u8 i = ir_send_ctrl.cnt;
	ir_send_ctrl.data[i].type = IR_SEND_TYPE_BYTE;
	ir_send_ctrl.data[i].start_high = start_high;
	ir_send_ctrl.data[i].code = code;
	++ir_send_ctrl.cnt;
}
void ir_send_add_half_byte_item(u8 code, u8 start_high){
	u8 i = ir_send_ctrl.cnt;
	ir_send_ctrl.data[i].type = IR_SEND_TYPE_HALF_TIME_SERIES;
	ir_send_ctrl.data[i].start_high = start_high;
	ir_send_ctrl.data[i].code = code;
	++ir_send_ctrl.cnt;
}


static inline void ir_set_repeat_timer(){
	if(!ir_is_repeat_timer_enable){
        ir_is_repeat_time = clock_time();
		ir_is_repeat_timer_enable = 1;

		//set timer2 cap 108 ms
		reg_irq_mask |= FLD_IRQ_TMR2_EN;
		reg_tmr2_tick = 0;
		reg_tmr2_capt = CLOCK_SYS_CLOCK_1US * 1000 * 108 ;
		reg_tmr_ctrl |= FLD_TMR2_EN;
	}
}
static inline void ir_release_repeat_timer(){
	if(ir_is_repeat_timer_enable){
		ir_is_repeat_timer_enable = 0;
		reg_irq_mask &= ~FLD_IRQ_TMR2_EN;
		reg_tmr_ctrl  &=  ~FLD_TMR2_EN;  //stop timer2
	}
}

void ir_send_ctrl_start(int need_repeat){
	if(need_repeat){
		ir_set_repeat_timer();
	}else{
		ir_release_repeat_timer();
	}
	ir_send_irq_idx = 0;
	ir_send_ctrl.index = 0;
	ir_send_ctrl.is_sending = 1;
	ir_send_ctrl.sending_start_time = clock_time();

	reg_irq_mask |= FLD_IRQ_TMR1_EN;

	ir_irq_send();
}

void ir_send_release(void){
	last_cmd = 0xff;
	ir_release_repeat_timer();
}

static inline void ir_send_start_next_item(void){
	ir_send_irq_idx = 0;
	reg_tmr1_tick = 0;
	reg_tmr1_capt = (u32)(CLOCK_SYS_CLOCK_1US / 2);
}

static inline u32 ir_get_byte_time(u8 code, u8 index){
	u8 b = index / 2;
	u8 r = index & 1;
	if(code & BIT(b)){
		if(!r){
			return ir_byte_time[0][0];
		}else{
			return ir_byte_time[0][1];
		}
	}else{
		if(!r){
			return ir_byte_time[1][0];
		}else{
			return ir_byte_time[1][1];
		}
	}
}

#if (BLE_IR_ENABLE)
_attribute_ram_code_
#endif
void ir_irq_send(void){
	//static u32 ir_send_cmd_start_time;
	u32 t = 0;
	u8 end = 0;

	u8 i = ir_send_ctrl.index;
	if(IR_SEND_TYPE_TIME_SERIES == ir_send_ctrl.data[i].type){
		if(ir_send_irq_idx < ir_send_ctrl.data[i].series_cnt){
			if(ir_send_ctrl.index == 0){
				if(ir_send_irq_idx < ir_send_ctrl.data[i].series_cnt){
			        t = ir_send_ctrl.data[i].time_series[ir_send_irq_idx];
				}else{
					end = 1;
				}
			}
			else{
		        t = ir_send_ctrl.data[i].time_series[ir_send_irq_idx];
			}
		}
		else{
			end = 1;
		}
	}
#if 0
	else if(IR_SEND_TYPE_HALF_TIME_SERIES == ir_send_ctrl.data[i].type){
		if(ir_send_irq_idx < 8){
			t = ir_send_ctrl.data[i].time_series[ir_send_irq_idx];
		}else{
			end = 1;
		}
	}
#endif
	else{
		if(ir_send_irq_idx < 16){
			t = ir_get_byte_time(ir_send_ctrl.data[i].code, ir_send_irq_idx);
		}else{
			end = 1;
		}
	}

	if(end){
		++ir_send_ctrl.index;
		if(ir_send_ctrl.index < ir_send_ctrl.cnt){
			ir_send_start_next_item();
			return;
		}
		else{
			ir_send_ctrl.is_sending = 0;
			reg_irq_mask &= ~FLD_IRQ_TMR1_EN;
			pwmm_stop(IR_PWM_ID);

			timer_disable_timer(1);
			return;
		}
	}
	else if(0 == ir_send_irq_idx){		// start
		//ir_send_cmd_start_time = clock_time();
		ir_send_start_high = ir_send_ctrl.data[i].start_high;
		timer_enable_timer(1);
	}

	if(ir_send_start_high){
		pwmm_start(IR_PWM_ID);
	}
	else{
		pwmm_stop(IR_PWM_ID);
	}

	timer_set_timeout(1, t);

	++ir_send_irq_idx;
	ir_send_start_high = !ir_send_start_high;

}

#if (BLE_IR_ENABLE)
_attribute_ram_code_
#endif
void ir_repeat_handle(){
    if(ir_is_repeat_timer_enable){
       // if(clock_time_exceed(ir_is_repeat_time, 108*1000)){
           // ir_is_repeat_time = clock_time();
            ir_send_repeat_timer(0);
        //}
    }
}

int ir_is_sending()
{
	if(ir_send_ctrl.is_sending && clock_time_exceed(ir_send_ctrl.sending_start_time, 300*1000))
	{	//  in case timer irq missing
		ir_send_ctrl.is_sending = 0;
	}
	return ir_send_ctrl.is_sending;
}

int ir_sending_check(){
	u8 r = irq_disable();
	if(ir_is_sending()){
		irq_restore(r);
		return 1;
	}
	ir_send_ctrl.is_sending = 1;
	irq_restore(r);
	return 0;
}

//////////////TC9012//////////////////////////////////
#define IR_INTRO_CARR_TIME_TC9012          4500
#define IR_INTRO_NO_CARR_TIME_TC9012       4500
#define IR_SYN_CARR_TIME_TC9012            560
#define IR_REPEAT_CARR_TIME_TC9012         4500
#define IR_REPEAT_NO_CARR_TIME_TC9012      4500  // 自己定义的 要修改
#define IR_REPEAT_LOW_CARR_TIME_TC9012     560
#define IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_0  1690
#define IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_1  560

#define IR_HIGH_CARR_TIME_TC9012	        560
#define IR_HIGH_NO_CARR_TIME_TC9012		1690
#define IR_LOW_CARR_TIME_TC9012			560
#define IR_LOW_NO_CARR_TIME_TC9012         560

void ir_tc9012_send(u8 addr1, u8 addr2,u8 local_data_code){
	if(ir_sending_check()){
		return;
	}

    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US, IR_INTRO_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US};
    ir_byte_time[0][0]=IR_HIGH_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1]=IR_HIGH_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0]=IR_LOW_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1]=IR_LOW_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
	//pwm_set(IR_PWM_ID, IR_CARRIER_FREQ, 0, IR_CARRIER_DUTY);
	//pwm_set(PWMID_LED1, PMW_MAX_TICK, PMW_MAX_TICK);        //@@@@@@@@@@@@
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
	ir_send_add_byte_item(addr1, 1);
	ir_send_add_byte_item(addr2, 1);
	ir_send_add_byte_item(local_data_code, 1);
	ir_send_add_byte_item(~local_data_code, 1);
	ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
	ir_send_ctrl_start(1);

}
void ir_tc9012_send_repeat(u8 addr1){
	if(ir_sending_check()){
		return;
	}
	static u32 ir_times[] = {IR_REPEAT_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US, IR_REPEAT_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US,
        IR_REPEAT_LOW_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US,IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_0*CLOCK_SYS_CLOCK_1US,IR_REPEAT_LOW_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US};
    if((addr1)&(0x01))
        ir_times[3]=IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_1*CLOCK_SYS_CLOCK_1US;
    else
        ir_times[3]=IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_0*CLOCK_SYS_CLOCK_1US;
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
	ir_send_ctrl_start(1);
}

//////////////uPD6121//////////////////////////////////
#define IR_INTRO_CARR_TIME_UPD6121          9000
#define IR_INTRO_NO_CARR_TIME_UPD6121       4500
#define IR_SYN_CARR_TIME_UPD6121            560
#define IR_REPEAT_CARR_TIME_UPD6121         9000  // user define
#define IR_REPEAT_NO_CARR_TIME_UPD6121      2250  // user define
#define IR_REPEAT_LOW_CARR_TIME_UPD6121            560

#define IR_HIGH_CARR_TIME_UPD6121	        560
#define IR_HIGH_NO_CARR_TIME_UPD6121		1690
#define IR_LOW_CARR_TIME_UPD6121			560
#define IR_LOW_NO_CARR_TIME_UPD6121         560
void ir_upd6121_send(u8 addr1, u8 addr2,u8 local_data_code){
	if(ir_sending_check()){
		return;
	}
    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US, IR_INTRO_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US};
    ir_byte_time[0][0]=IR_HIGH_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1]=IR_HIGH_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0]=IR_LOW_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1]=IR_LOW_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
	//pwm_set(IR_PWM_ID, IR_CARRIER_FREQ, 0, IR_CARRIER_DUTY);
    //pwm_set(PWMID_LED1, PMW_MAX_TICK, PMW_MAX_TICK);   //@@@@@@@@@@@
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
	ir_send_add_byte_item(addr1, 1);
	ir_send_add_byte_item(addr2, 1);
	ir_send_add_byte_item(local_data_code, 1);
	ir_send_add_byte_item(~local_data_code, 1);
	ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
	ir_send_ctrl_start(1);
}
void ir_upd6121_send_repeat(void){
	if(ir_sending_check()){
		return;
	}
	static u32 ir_times[] = {IR_REPEAT_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US, IR_REPEAT_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US, IR_REPEAT_LOW_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US};
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
	ir_send_ctrl_start(1);
}
//////////////konka//////////////////////////////////
#define IR_INTRO_CARR_TIME_KONKA          3000
#define IR_INTRO_NO_CARR_TIME_KONKA       3000
#define IR_SYN_CARR_TIME_KONKA            2500
#define IR_REPEAT_NO_CARR_TIME_KONKA      160  // user define
#define IR_REPEAT_LOW_CARR_TIME_KONKA            560

#define IR_HIGH_CARR_TIME_KONKA	        500
#define IR_HIGH_NO_CARR_TIME_KONKA		2500
#define IR_LOW_CARR_TIME_KONKA			500
#define IR_LOW_NO_CARR_TIME_KONKA         1500
void ir_konka_send(u8 addr,u8 local_data_code){
	if(ir_sending_check()){
		return;
	}
    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US, IR_INTRO_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US};
    ir_byte_time[0][0]=IR_HIGH_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1]=IR_HIGH_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0]=IR_LOW_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1]=IR_LOW_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
	//pwm_set(IR_PWM_ID, IR_CARRIER_FREQ, 0, IR_CARRIER_DUTY);
    //pwm_set(PWMID_LED1, PMW_MAX_TICK, PMW_MAX_TICK);   //@@@@@@@@@@@
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
	ir_send_add_byte_item(addr, 1);
	ir_send_add_byte_item(local_data_code, 1);
	ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
	ir_send_ctrl_start(0);
}
void ir_konka_send_repeat(void){
	if(ir_sending_check()){
		return;
	}
	static u32 ir_times[] = {IR_INTRO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US, IR_REPEAT_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US, IR_REPEAT_LOW_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US};
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
	ir_send_ctrl_start(0);
}
//////////////RCA//////////////////////////////////
#define IR_INTRO_CARR_TIME_RCA          4000
#define IR_INTRO_NO_CARR_TIME_RCA       4000
#define IR_SYN_CARR_TIME_RCA            500
#define IR_REPEAT_CARR_TIME_RCA         4000
#define IR_REPEAT_NO_CARR_TIME_RCA      4000  // user define
#define IR_REPEAT_LOW_CARR_TIME_RCA_RCA            500

#define IR_HIGH_CARR_TIME_RCA	        500
#define IR_HIGH_NO_CARR_TIME_RCA		2000
#define IR_LOW_CARR_TIME_RCA			500
#define IR_LOW_NO_CARR_TIME_RCA         1000

#define IR_CARRIER_RCA_FREQ             56000

void ir_rca_send(u8 addr1, u8 addr2,u8 local_data_code){
	if(ir_sending_check()){
		return;
	}
    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US, IR_INTRO_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US};
    ir_byte_time[0][0]=IR_HIGH_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1]=IR_HIGH_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0]=IR_LOW_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1]=IR_LOW_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
	//pwm_set(IR_PWM_ID, IR_CARRIER_RCA_FREQ, 0, IR_CARRIER_DUTY);
    //pwm_set(PWMID_LED1, PMW_MAX_TICK, PMW_MAX_TICK);   //@@@@@@@@@@@@@
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
	ir_send_add_half_byte_item(addr1, 1);
	ir_send_add_byte_item(local_data_code, 1);
	ir_send_add_half_byte_item(addr2, 1);
	ir_send_add_byte_item(~local_data_code, 1);
	ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
	ir_send_ctrl_start(0);
}
void ir_rca_send_repeat(void){
	if(ir_sending_check()){
		return;
	}
	static u32 ir_times[] = {IR_REPEAT_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US, IR_REPEAT_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US, IR_REPEAT_LOW_CARR_TIME_RCA_RCA * CLOCK_SYS_CLOCK_1US};
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
	ir_send_ctrl_start(1);
}

/////////////  NEC  protocol  /////////////////////////////////////////////
#define IR_INTRO_CARR_TIME_NEC          9000
#define IR_INTRO_NO_CARR_TIME_NEC       4500
#define IR_END_TRANS_TIME_NEC           563  // user define
#define IR_REPEAT_CARR_TIME_NEC          9000
#define IR_REPEAT_NO_CARR_TIME_NEC          2250
#define IR_REPEAT_LOW_CARR_TIME_NEC			560

#define IR_HIGH_CARR_TIME_NEC	        560
#define IR_HIGH_NO_CARR_TIME_NEC		1690
#define IR_LOW_CARR_TIME_NEC			560
#define IR_LOW_NO_CARR_TIME_NEC         560
void ir_nec_send_repeat(void){
	if(ir_sending_check()){
		return;
	}

	static u32 ir_times[] = {IR_REPEAT_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US, IR_REPEAT_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US, IR_REPEAT_LOW_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US};
	ir_send_ctrl_clear();
	ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
	ir_send_ctrl_start(1);
}

#define NEC_FRAME_CYCLE				(108*1000)
#define GD25Q40B_FLASH_PAGE_SIZE 		0x100 //256bytes
#define IR_LEARN_MAX_INTERVAL			(WATCHDOG_INIT_TIMEOUT * CLOCK_SYS_CLOCK_1MS)
#define IR_STORED_INDEX_ADDRESS			(0xa000)
#define IR_STORED_SERIES_ADDRESS 		(0xa200)
#define IR_LEARN_NONE_CARR_MIN			(200*CLOCK_SYS_CLOCK_1US)//old is 80
#define IR_LEARN_CARR_GLITCH_MIN		(3*CLOCK_SYS_CLOCK_1US)
#define IR_LEARN_CARR_MIN				(7 * CLOCK_SYS_CLOCK_1US)
#define IR_CARR_CHECK_CNT				10
#define IR_LEARN_START_MINLEN			(300*CLOCK_SYS_CLOCK_1US)
#define IR_MAX_INDEX_TABLE_LEN			32
#define NEC_LEAD_CARR_MIN_INTERVAL			(8700*CLOCK_SYS_CLOCK_1US)
#define NEC_LEAD_CARR_MAX_INTERVAL			(9300*CLOCK_SYS_CLOCK_1US)
#define NEC_LEAD_NOCARR_MIN_INTERVAL		(4200*CLOCK_SYS_CLOCK_1US)
#define NEC_LEAD_NOCARR_MAX_INTERVAL		(4800*CLOCK_SYS_CLOCK_1US)
#define TOSHIBA_LEAD_MIN_INTERVAL			(4200*CLOCK_SYS_CLOCK_1US)
#define TOSHIBA_LEAD_MAX_INTERVAL			(4800*CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_CARR_MIN_INTERVAL		(2100*CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_CARR_MAX_INTERVAL		(2700*CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_NOCARR_MIN_INTERVAL		(900*CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_NOCARR_MAX_INTERVAL		(1500*CLOCK_SYS_CLOCK_1US)
#define IR_NEC_TYPE							1
#define IR_TOSHIBA_TYPE 					2
#define IR_FRAXEL_TYPE						3
#define IR_HIGH_LOW_MIN_INTERVAL			(1000*CLOCK_SYS_CLOCK_1US)
#define IR_HIGH_LOW_MAX_INTERVAL			(2000*CLOCK_SYS_CLOCK_1US)
#define TC9012_FRAME_CYCLE					(108*1000)
#define FRAXEL_LEVEL_NUM 					19
#define NEC_TOSHIBA_LEVEL_NUM 				67



void ir_get_index_addr(void){
	u8 i = 0;
	 //32*8 = 256 , one page size.
	static ir_search_index_t indexTable[IR_MAX_INDEX_TABLE_LEN];
	flash_read_page(IR_STORED_INDEX_ADDRESS, sizeof(indexTable), (u8*)(indexTable));
	for(i = 0;i < IR_MAX_INDEX_TABLE_LEN;i++){
		if(ismemf4((void*)(&indexTable[i]), sizeof(ir_search_index_t))){
			// Invalid data
			break;
		}
	}
	ir_search_index_next_addr = i;
}

void ir_nec_send(u8 addr1, u8 addr2, u8 cmd){
	/*
	if(ir_find_learnkey_data(cmd)==0){
		ir_learn_send(cmd);
	}else*/
	{
		if(last_cmd != cmd)
		{
			if(ir_sending_check())
			{
				return;
			}
		static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US, IR_INTRO_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US};
		static u32 ir_stop_bit_times[] = {IR_END_TRANS_TIME_NEC * CLOCK_SYS_CLOCK_1US};
	        ir_byte_time[0][0]=IR_HIGH_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;
	        ir_byte_time[0][1]=IR_HIGH_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;
	        ir_byte_time[1][0]=IR_LOW_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;
	        ir_byte_time[1][1]=IR_LOW_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;

			last_cmd = cmd;
			ir_send_ctrl_clear();
			ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
			ir_send_add_byte_item(addr1, 1);
			ir_send_add_byte_item(addr2, 1);
			ir_send_add_byte_item(cmd, 1);
			ir_send_add_byte_item(~cmd, 1);
			ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
			ir_send_ctrl_start(1);
		}
	}
}
int ir_record_end(void *data){
	if( ir_learn_ctrl.series_cnt < IR_LEARN_SERIES_CNT ){
		++ir_learn_ctrl.series_cnt;//plus the last carrier.
	}
	/*
	gpio_clr_interrupt(GPIO_IR_LEARN_IN);
	ir_write_universal_data(&ir_index_data);
	ev_emit_event_syn(EV_IR_LEARN_STOP, 0);
	*/
	return -1;
}
int a_ir_record=0;
int a_ir_record1=0;
int a_ir_record2=0;
int a_ir_record3=0;
int a_ir_record4=0;
int a_ir_record5=0;
int a_ir_record6=0;
int a_ir_record7=0;
void ir_record1(u32 tm, u32 pol)
{
    ++a_ir_record;
    ++ir_learn_ctrl.ir_int_cnt;
    a_ir_record1=ir_learn_ctrl.ir_int_cnt;
    ir_learn_ctrl.curr_trigger_tm = tm;
	if( ir_learn_ctrl.series_cnt >= IR_LEARN_SERIES_CNT )
	{
		return;
	}
    if(ir_learn_ctrl.ir_int_cnt > 1){
        ++a_ir_record5;

        ir_learn_ctrl.time_interval = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm;
        // record carrier time
        if((ir_learn_ctrl.time_interval < IR_LEARN_NONE_CARR_MIN) && ir_learn_ctrl.time_interval > IR_LEARN_CARR_GLITCH_MIN){   // removing glitch  // receiving carrier
            ++a_ir_record7;
            if(!ir_learn_ctrl.is_carr){
				ir_learn_ctrl.carr_first = ir_learn_ctrl.time_interval;
                if(ir_learn_ctrl.series_cnt > 0){  //  Do not record leading none-carrier
                	ir_learn_ctrl.series_tm[(ir_learn_ctrl.series_cnt)] = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.carr_switch_start_tm
                        - ir_learn_ctrl.carr_first;
					++ir_learn_ctrl.series_cnt;
                }
                ir_learn_ctrl.carr_switch_start_tm = ir_learn_ctrl.curr_trigger_tm;
                ir_learn_ctrl.is_carr = 1;
            }
			else{
				ir_learn_ctrl.series_tm[(ir_learn_ctrl.series_cnt)] = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.carr_switch_start_tm;
						//+ (ir_learn_ctrl.ir_series_data.carr_low_tm + ir_learn_ctrl.ir_series_data.carr_high_tm);
   			}

            //  to calulate duty-cycle of the carrier
            if(ir_learn_ctrl.carr_check_cnt < IR_CARR_CHECK_CNT){
                //&& ir_learn_ctrl.time_interval < IR_LEARN_CARR_MAX && ir_learn_ctrl.time_interval > IR_LEARN_CARR_MIN){
                ++ir_learn_ctrl.carr_check_cnt;
                // we are receiving carrier
                if(pol){
                    if(ir_learn_ctrl.time_interval < ir_learn_ctrl.carr_high_tm || 0 == ir_learn_ctrl.carr_high_tm)     //  record the shortest cycle
					ir_learn_ctrl.carr_high_tm = ir_learn_ctrl.time_interval;
                }else{
                    if(ir_learn_ctrl.time_interval < ir_learn_ctrl.carr_low_tm || 0 == ir_learn_ctrl.carr_low_tm)   //  record the shortest cycle
                        ir_learn_ctrl.carr_low_tm = ir_learn_ctrl.time_interval;
                }
            }
        }else{
            // record carrier time
            ++a_ir_record4;
            if(ir_learn_ctrl.is_carr){
                ++a_ir_record2;
                // It decrease one cycle with actually calculate, so add it.
               ir_learn_ctrl.series_tm[(ir_learn_ctrl.series_cnt)]= (ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.carr_switch_start_tm)
                    + ir_learn_ctrl.carr_first;
				if(ir_learn_ctrl.series_tm[0] < IR_LEARN_START_MINLEN){
					memset4(&ir_learn_ctrl, 0, sizeof(ir_learn_ctrl));
                    ++a_ir_record3;
					return;
				}
				else{
					#ifndef WIN32
	                if(0 == ir_learn_ctrl.learn_timer_started){
	                   ir_learn_ctrl.learn_timer_started = 1;
	                    // to stop learning after 200 ms
	                   // static ev_time_event_t record_end = {ir_record_end};
	                  //  ev_on_timer(&record_end, (200*1000));

	                }
					#endif
				}

                ++ir_learn_ctrl.series_cnt;

				ir_learn_ctrl.carr_switch_start_tm = ir_learn_ctrl.last_trigger_tm;
			}
            ir_learn_ctrl.is_carr = 0;
        }
    }
    ir_learn_ctrl.last_trigger_tm = ir_learn_ctrl.curr_trigger_tm;

}


void rc_ir_init(void)
{
	//ir_get_index_addr();
	PWM0_CFG_GPIO_A0();
	pwmm_clk(CLOCK_SYS_CLOCK_HZ);  //set pwm_clk as sys_clk(16000000)
	pwmm_set_mode(IR_PWM_ID, PWM_NORMAL_MODE);  //pwm0 count mode
	pwmm_set_duty(IR_PWM_ID, PWM_CYCLE_VALUE, PWM_HIGH_VALUE);  //set cycle and high
	pwmm_set_phase(IR_PWM_ID, 0);   //no phase at pwm beginning
}


#endif
