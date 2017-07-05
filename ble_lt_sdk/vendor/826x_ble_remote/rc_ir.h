/*
 * rc_ir.h
 *
 *  Created on: 2015-12-2
 *      Author: Administrator
 */

#ifndef RC_IR_H_
#define RC_IR_H_


////////////////////////// IR  /////////////////////////////
#define IR_CARRIER_FREQ				38000
#define PWM_CYCLE_VALUE				( CLOCK_SYS_CLOCK_HZ/IR_CARRIER_FREQ )  //16M: 421 tick, f = 16000000/421 = 38004,T = 421/16=26.3125 us
#define PWM_HIGH_VALUE				( PWM_CYCLE_VALUE/3 )   // 1/3 duty
// 1 frame -> 1/38k -> 1000/38 = 26 us

#define IR_PWM_ID					0   //0: PWM0   1:PWM1  ...   5:PWM5


void rc_ir_init(void);
void ir_send_release(void);
//void ir_send_cmd(u8 addr, u8 cmd);



#define IR_HIGH_CARR_TIME			565			// in us
#define IR_HIGH_NO_CARR_TIME		1685
#define IR_LOW_CARR_TIME			560
#define IR_LOW_NO_CARR_TIME			565
#define IR_INTRO_CARR_TIME			9000
#define IR_INTRO_NO_CARR_TIME		4500

#define IR_SWITCH_CODE              0x0d
#define IR_ADDR_CODE                0x00
#define IR_CMD_CODE                 0xbf

#define IR_REPEAT_INTERVAL_TIME     40500
#define IR_REPEAT_NO_CARR_TIME      2250
#define IR_END_TRANS_TIME			563

//#define IR_CARRIER_FREQ				37917//38222
#define IR_CARRIER_DUTY				3
#define IR_LEARN_SERIES_CNT     	160

enum{
    IR_TYPE_KONKA_KONKA=0,
    IR_TYPE_SUNSUNG_TC9012,
    IR_TYPE_HAIER_TC9012,
    IR_TYPE_CHANGHONG_uPD6121,
    IR_TYPE_HiSense_uPD6121,
    IR_TYPE_TCL_RCA,
    IR_TYPE_SKYWORTH_TC9012,
    IR_TYPE_NEC_TIANZUN,
    IR_TYPE_MAX ,
};



typedef struct{
	u32 *time_series;
	u8 type;
	u8 start_high;
	u8 series_cnt;
	u8 code;
}ir_send_ctrl_data_t;

typedef struct{
	//8byte save an index, use 256bytes(1pages)to save 32 key index
	u32 learnkey_flash_addr;
	u32 local_key_code;
}ir_search_index_t;

typedef struct{
	ir_send_ctrl_data_t	data[8];
	u32 sending_start_time;
	u8 index;
	u8 cnt;
	u8 is_sending;
}ir_send_ctrl_t;

typedef struct{
	u8 ir_protocol;
	u8 toshiba_c0flag;
	u8 resv0[2];
	u32 carr_high_tm;
	u32 carr_low_tm;
	u16 series_cnt;
	u16 resv1;
	u8 series_tm[(IR_LEARN_SERIES_CNT/2)*3];
}ir_universal_pattern_t;

typedef struct{
	u8 is_carr;
	u8 ir_protocol;
	u8 toshiba_c0flag;
	u8 learn_timer_started;
	int carr_first;
	u32 carr_switch_start_tm;
	u16 carr_check_cnt;
	u16 series_cnt;
	u32 series_tm[IR_LEARN_SERIES_CNT];
	u32 time_interval;
	u32 last_trigger_tm;
	u32 curr_trigger_tm;
	u32 carr_high_tm;
	u32	carr_low_tm;
	int ir_int_cnt;

}ir_learn_ctrl_t;

extern ir_send_ctrl_t ir_send_ctrl;
extern u8 ir_is_repeat_timer_enable;

void ir_send_switch(u8 addr, u8 cmd);


#ifndef GPIO_IR_LEARN_IN
#define GPIO_IR_LEARN_IN			//
#endif

void ir_record(u32 tm, int pol);
int ir_record_end(void * data);

void ir_learn(void);
void ir_learn_init(void);
void ir_learn_test(void);

void ir_irq_send(void);
void ir_repeat_handle();

#endif /* RC_IR_H_ */
