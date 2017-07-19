/*
 * rc_ir.c
 *
 *  Created on: 2015-12-2
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/pm.h"
#include "../common/blt_led.h"

#include "rc_ir.h"

#if (REMOTE_IR_ENABLE)

enum {
    IR_SEND_TYPE_TIME_SERIES,
    IR_SEND_TYPE_HALF_TIME_SERIES,
    IR_SEND_TYPE_BYTE,
};

enum {
    IR_LEARN_DISABLE = 0x00,
    IR_LEARN_WAIT_KEY,
    IR_LEARN_KEY,
    IR_LEARN_FINISH,
};

const static led_cfg_t g_ir_led[] = {
        {250, 250, 0x02, 0x05},    // Enter IR learning
        {500, 500, 0x01, 0x04},    // Timeout for wait for key
        {250, 250, 0x06, 0x04},    // Timeout for IR learning
        {250, 250, 0x07, 0x04},    // Timeout for IR learning
        {250, 250, 0x08, 0x04},    // Timeout for IR learning
        {250, 250, 0x09, 0x04},    // Timeout for IR learning
};

u32 g_ir_learn_tick;
u32 ir_byte_time[2][2];
ir_send_ctrl_t ir_send_ctrl;

u8 g_ir_is_repeat_timer_enable;
static u8 g_ir_proto_type = IR_TYPE_NEC_TIANZUN;

static u8 g_ir_addr;
static u8 g_last_cmd;
static u8 g_ir_errcnt;
static u8 g_ir_learn_state;
static u8 g_ir_send_irq_idx;
static u8 g_ir_send_start_high;
static u8 g_ir_search_index_next_addr;

static u32 g_ir_is_repeat_time;

static ir_learn_ctrl_t ir_learn_ctrl;
static ir_search_index_t ir_index_data;
static ir_universal_pattern_t ir_flash_universal_data1;
static ir_universal_pattern_t ir_flash_universal_data2;

extern u32 learn_keycode;
extern const u8 kb_map_ir[49];

void ir_irq_send(void);
void ir_nec_send_repeat(u8 addr1);
void ir_tc9012_send_repeat(u8 addr1);
void ir_rca_send_repeat(u8 addr1);
void ir_upd6121_send_repeat(u8 addr1);
void ir_konka_send_repeat(u8 addr1);
void ir_tc9012_send(u8 addr1, u8 addr2, u8 local_data_code);
void ir_upd6121_send(u8 addr1, u8 addr2, u8 local_data_code);
void ir_konka_send(u8 addr1, u8 addr2, u8 local_data_code);
void ir_rca_send(u8 addr1, u8 addr2, u8 local_data_code);
void ir_nec_send(u8 addr1, u8 addr2, u8 cmd);
void ir_learn_send(u8 local_data_code);

static int ir_find_learnkey_data(u8 key_value);
static void ir_disable_keyboard();
static void ir_restore_keyboard();

const static ir_send_func_t ir_send_array[] = {
        ir_konka_send, ir_tc9012_send, ir_tc9012_send, ir_upd6121_send,
        ir_upd6121_send, ir_rca_send, ir_tc9012_send, ir_nec_send
};

const static ir_send_release_func_t ir_send_release_array[] = {
        ir_konka_send_repeat, ir_tc9012_send_repeat, ir_tc9012_send_repeat, ir_upd6121_send_repeat,
        ir_upd6121_send_repeat, ir_rca_send_repeat, ir_tc9012_send_repeat, ir_nec_send_repeat
};

static inline void timer_enable_timer(int i)
{
    SET_FLD(reg_tmr_ctrl8, BIT(i * 3));
}

static inline void timer_disable_timer(int i)
{
    CLR_FLD(reg_tmr_ctrl8, BIT(i * 3));
}

static inline void timer_set_timeout(int i, u32 t)
{
    reg_tmr_tick(i) = 0;
    reg_tmr_capt(i) = (u32)t;
}


static int ir_send_repeat_timer(void *data)
{
    if (ir_send_release_array[g_ir_proto_type])
        ir_send_release_array[g_ir_proto_type](g_ir_addr);
    return 0;
}

void ir_send_cmd(u8 addr1, u8 addr2, u8 cmd)
{
    if (ir_find_learnkey_data(cmd) == 0) {
        ir_learn_send(cmd);
        return;
    }

    if (g_ir_proto_type >= IR_TYPE_MAX)
        g_ir_proto_type = IR_TYPE_NEC_TIANZUN;

    if (ir_send_array[g_ir_proto_type])
        ir_send_array[g_ir_proto_type](addr1, addr2, cmd);
}

void ir_send_ctrl_clear(void)
{
    ir_send_ctrl.is_sending = ir_send_ctrl.cnt = ir_send_ctrl.index = 0;
}

void ir_send_add_series_item(u32 *time_series, u8 series_cnt, u8 start_high)
{
    u8 i = ir_send_ctrl.cnt;
    ir_send_ctrl.data[i].type = IR_SEND_TYPE_TIME_SERIES;
    ir_send_ctrl.data[i].time_series = time_series;
    ir_send_ctrl.data[i].series_cnt = series_cnt;
    ir_send_ctrl.data[i].start_high = start_high;
    ++ ir_send_ctrl.cnt;
}

void ir_send_add_byte_item(u8 code, u8 start_high)
{
    u8 i = ir_send_ctrl.cnt;
    ir_send_ctrl.data[i].type = IR_SEND_TYPE_BYTE;
    ir_send_ctrl.data[i].start_high = start_high;
    ir_send_ctrl.data[i].code = code;
    ++ ir_send_ctrl.cnt;
}

void ir_send_add_half_byte_item(u8 code, u8 start_high)
{
    u8 i = ir_send_ctrl.cnt;
    ir_send_ctrl.data[i].type = IR_SEND_TYPE_HALF_TIME_SERIES;
    ir_send_ctrl.data[i].start_high = start_high;
    ir_send_ctrl.data[i].code = code;
    ++ ir_send_ctrl.cnt;
}

static inline void ir_set_repeat_timer()
{
    if (g_ir_is_repeat_timer_enable)
        return;

    g_ir_is_repeat_time = clock_time();
    g_ir_is_repeat_timer_enable = 1;

    //set timer2 cap 108 ms
    reg_irq_mask |= FLD_IRQ_TMR2_EN;
    reg_tmr2_tick = 0;
    reg_tmr2_capt = CLOCK_SYS_CLOCK_1US * 1000 * 108 ;
    reg_tmr_ctrl |= FLD_TMR2_EN;
}

static inline void ir_release_repeat_timer()
{
    if (!g_ir_is_repeat_timer_enable)
        return;

    g_ir_is_repeat_timer_enable = 0;
    reg_irq_mask &= ~FLD_IRQ_TMR2_EN;
    reg_tmr_ctrl &= ~FLD_TMR2_EN;  //stop timer2
}

void ir_send_ctrl_start(int need_repeat)
{
    if (need_repeat) {
        ir_set_repeat_timer();
    } else {
        ir_release_repeat_timer();
    }

    g_ir_send_irq_idx = 0;
    ir_send_ctrl.index = 0;
    ir_send_ctrl.is_sending = 1;
    ir_send_ctrl.sending_start_time = clock_time();

    reg_irq_mask |= FLD_IRQ_TMR1_EN;

    ir_irq_send();
}

void ir_send_release(void)
{
    g_last_cmd = 0xff;
    ir_release_repeat_timer();
}

static inline void ir_send_start_next_item(void)
{
    g_ir_send_irq_idx = 0;
    reg_tmr1_tick = 0;
    reg_tmr1_capt = (u32)(CLOCK_SYS_CLOCK_1US / 2);
}

static inline u32 ir_get_byte_time(u8 code, u8 index)
{
    u8 b = index / 2;
    u8 r = index & 1;

    return ir_byte_time[code & BIT(b) ? 0 : 1][r ? 1 : 0];
}

_attribute_ram_code_ void ir_irq_send(void){
    //static u32 ir_send_cmd_start_time;
    u32 t = 0;
    u8 end = 0;

    u8 i = ir_send_ctrl.index;
    if (IR_SEND_TYPE_TIME_SERIES == ir_send_ctrl.data[i].type) {
        if (g_ir_send_irq_idx < ir_send_ctrl.data[i].series_cnt) {
            if (ir_send_ctrl.index == 0) {
                if (g_ir_send_irq_idx < ir_send_ctrl.data[i].series_cnt) {
                    t = ir_send_ctrl.data[i].time_series[g_ir_send_irq_idx];
                } else {
                    end = 1;
                }
            } else {
                t = ir_send_ctrl.data[i].time_series[g_ir_send_irq_idx];
            }
        } else {
            end = 1;
        }
    }
#if 0
    else if(IR_SEND_TYPE_HALF_TIME_SERIES == ir_send_ctrl.data[i].type){
        if(g_ir_send_irq_idx < 8){
            t = ir_send_ctrl.data[i].time_series[g_ir_send_irq_idx];
        }else{
            end = 1;
        }
    }
#endif
    else {
        if (g_ir_send_irq_idx < 16) {
            t = ir_get_byte_time(ir_send_ctrl.data[i].code, g_ir_send_irq_idx);
        } else {
            end = 1;
        }
    }

    if (end) {
        ++ ir_send_ctrl.index;
        if (ir_send_ctrl.index < ir_send_ctrl.cnt) {
            ir_send_start_next_item();
            return;
        } else {
            ir_send_ctrl.is_sending = 0;
            reg_irq_mask &= ~FLD_IRQ_TMR1_EN;
            pwmm_stop(IR_PWM_ID);

            timer_disable_timer(1);
            return;
        }
    } else if (0 == g_ir_send_irq_idx) {  // start
        //ir_send_cmd_start_time = clock_time();
        g_ir_send_start_high = ir_send_ctrl.data[i].start_high;
        timer_enable_timer(1);
    }

    if (g_ir_send_start_high) {
        pwmm_start(IR_PWM_ID);
    } else {
        pwmm_stop(IR_PWM_ID);
    }

    timer_set_timeout(1, t);

    ++ g_ir_send_irq_idx;
    g_ir_send_start_high = !g_ir_send_start_high;

}

_attribute_ram_code_ void ir_repeat_handle()
{
    if (!g_ir_is_repeat_timer_enable)
        return;

    ir_send_repeat_timer(0);
}

u8 ir_is_sending()
{
    if (ir_send_ctrl.is_sending &&
        clock_time_exceed(ir_send_ctrl.sending_start_time, 300 * 1000)) {
        // in case timer irq missing
        ir_send_ctrl.is_sending = 0;
    }

    return ir_send_ctrl.is_sending;
}

u8 ir_sending_check()
{
    u8 r = irq_disable();
    if (ir_is_sending()) {
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

#define IR_HIGH_CARR_TIME_TC9012           560
#define IR_HIGH_NO_CARR_TIME_TC9012        1690
#define IR_LOW_CARR_TIME_TC9012            560
#define IR_LOW_NO_CARR_TIME_TC9012         560

void ir_tc9012_send(u8 addr1, u8 addr2, u8 local_data_code)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US,
            IR_INTRO_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US};
    ir_byte_time[0][0] = IR_HIGH_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1] = IR_HIGH_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0] = IR_LOW_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1] = IR_LOW_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US;
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

    g_ir_addr = addr1;
}

void ir_tc9012_send_repeat(u8 addr1)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_times[] = {IR_REPEAT_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US,
        IR_REPEAT_NO_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US,
        IR_REPEAT_LOW_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US,
        IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_0*CLOCK_SYS_CLOCK_1US,
        IR_REPEAT_LOW_CARR_TIME_TC9012 * CLOCK_SYS_CLOCK_1US};

    if(addr1 & 0x01)
        ir_times[3] = IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_1 * CLOCK_SYS_CLOCK_1US;
    else
        ir_times[3] = IR_REPEAT_LOW_NO_CARR_TIME_TC9012_S0_0 * CLOCK_SYS_CLOCK_1US;

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
#define IR_REPEAT_LOW_CARR_TIME_UPD6121     560

#define IR_HIGH_CARR_TIME_UPD6121           560
#define IR_HIGH_NO_CARR_TIME_UPD6121        1690
#define IR_LOW_CARR_TIME_UPD6121            560
#define IR_LOW_NO_CARR_TIME_UPD6121         560

void ir_upd6121_send(u8 addr1, u8 addr2,u8 local_data_code)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US,
            IR_INTRO_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US};

    ir_byte_time[0][0] = IR_HIGH_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1] = IR_HIGH_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0] = IR_LOW_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1] = IR_LOW_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US;

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

    g_ir_addr = addr1;
}

void ir_upd6121_send_repeat(u8 addr1)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_times[] = {IR_REPEAT_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_NO_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_LOW_CARR_TIME_UPD6121 * CLOCK_SYS_CLOCK_1US};

    ir_send_ctrl_clear();
    ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
    ir_send_ctrl_start(1);
}

//////////////konka//////////////////////////////////
#define IR_INTRO_CARR_TIME_KONKA          3000
#define IR_INTRO_NO_CARR_TIME_KONKA       3000
#define IR_SYN_CARR_TIME_KONKA            2500
#define IR_REPEAT_NO_CARR_TIME_KONKA      160  // user define
#define IR_REPEAT_LOW_CARR_TIME_KONKA     560

#define IR_HIGH_CARR_TIME_KONKA           500
#define IR_HIGH_NO_CARR_TIME_KONKA        2500
#define IR_LOW_CARR_TIME_KONKA            500
#define IR_LOW_NO_CARR_TIME_KONKA         1500

void ir_konka_send(u8 addr1, u8 addr2, u8 local_data_code)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US,
            IR_INTRO_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US};

    ir_byte_time[0][0] = IR_HIGH_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1] = IR_HIGH_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0] = IR_LOW_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1] = IR_LOW_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US;

    //pwm_set(IR_PWM_ID, IR_CARRIER_FREQ, 0, IR_CARRIER_DUTY);
    //pwm_set(PWMID_LED1, PMW_MAX_TICK, PMW_MAX_TICK);   //@@@@@@@@@@@
    ir_send_ctrl_clear();
    ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
    ir_send_add_byte_item(addr1, 1);
    ir_send_add_byte_item(local_data_code, 1);
    ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
    ir_send_ctrl_start(0);

    g_ir_addr = addr1;
}

void ir_konka_send_repeat(u8 addr1)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_times[] = {IR_INTRO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_NO_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_LOW_CARR_TIME_KONKA * CLOCK_SYS_CLOCK_1US};

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
#define IR_REPEAT_LOW_CARR_TIME_RCA_RCA 500

#define IR_HIGH_CARR_TIME_RCA           500
#define IR_HIGH_NO_CARR_TIME_RCA        2000
#define IR_LOW_CARR_TIME_RCA            500
#define IR_LOW_NO_CARR_TIME_RCA         1000

#define IR_CARRIER_RCA_FREQ             56000

void ir_rca_send(u8 addr1, u8 addr2,u8 local_data_code)
{
    if(ir_sending_check()){
        return;
    }

    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US,
            IR_INTRO_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_SYN_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US};

    ir_byte_time[0][0] = IR_HIGH_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1] = IR_HIGH_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0] = IR_LOW_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1] = IR_LOW_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US;

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

    g_ir_addr = addr1;
}

void ir_rca_send_repeat(u8 addr1)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_times[] = {IR_REPEAT_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_NO_CARR_TIME_RCA * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_LOW_CARR_TIME_RCA_RCA * CLOCK_SYS_CLOCK_1US};

    ir_send_ctrl_clear();
    ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
    ir_send_ctrl_start(1);
}

/////////////  NEC  protocol  /////////////////////////////////////////////
#define IR_INTRO_CARR_TIME_NEC          9000
#define IR_INTRO_NO_CARR_TIME_NEC       4500
#define IR_END_TRANS_TIME_NEC           563  // user define
#define IR_REPEAT_CARR_TIME_NEC         9000
#define IR_REPEAT_NO_CARR_TIME_NEC      2250
#define IR_REPEAT_LOW_CARR_TIME_NEC     560

#define IR_HIGH_CARR_TIME_NEC           560
#define IR_HIGH_NO_CARR_TIME_NEC        1690
#define IR_LOW_CARR_TIME_NEC            560
#define IR_LOW_NO_CARR_TIME_NEC         560

void ir_nec_send_repeat(u8 addr1)
{
    if (ir_sending_check()) {
        return;
    }

    static u32 ir_times[] = {IR_REPEAT_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US,
            IR_REPEAT_LOW_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US};
    ir_send_ctrl_clear();
    ir_send_add_series_item(ir_times, ARRAY_SIZE(ir_times), 1);
    ir_send_ctrl_start(1);
}

#define NEC_FRAME_CYCLE                 (108 * 1000)
#define GD25Q40B_FLASH_PAGE_SIZE        (0x100)
#define IR_LEARN_MAX_INTERVAL           (WATCHDOG_INIT_TIMEOUT * CLOCK_SYS_CLOCK_1MS)
#define IR_STORED_INDEX_ADDRESS         (0x78000)
#define IR_STORED_SERIES_ADDRESS        (0x7A000)
#define IR_LEARN_NONE_CARR_MIN          (200 * CLOCK_SYS_CLOCK_1US)//old is 80
#define IR_LEARN_CARR_GLITCH_MIN        (3 * CLOCK_SYS_CLOCK_1US)
#define IR_LEARN_CARR_MIN               (7 * CLOCK_SYS_CLOCK_1US)
#define IR_CARR_CHECK_CNT               (10)
#define IR_LEARN_START_MINLEN           (300 * CLOCK_SYS_CLOCK_1US)
#define IR_MAX_INDEX_TABLE_LEN          (32)
#define NEC_LEAD_CARR_MIN_INTERVAL      (8700 * CLOCK_SYS_CLOCK_1US)
#define NEC_LEAD_CARR_MAX_INTERVAL      (9300 * CLOCK_SYS_CLOCK_1US)
#define NEC_LEAD_NOCARR_MIN_INTERVAL    (4200 * CLOCK_SYS_CLOCK_1US)
#define NEC_LEAD_NOCARR_MAX_INTERVAL    (4800 * CLOCK_SYS_CLOCK_1US)
#define TOSHIBA_LEAD_MIN_INTERVAL       (4200 * CLOCK_SYS_CLOCK_1US)
#define TOSHIBA_LEAD_MAX_INTERVAL       (4800 * CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_CARR_MIN_INTERVAL   (2100 * CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_CARR_MAX_INTERVAL   (2700 * CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_NOCARR_MIN_INTERVAL (900 * CLOCK_SYS_CLOCK_1US)
#define FRAXEL_LEAD_NOCARR_MAX_INTERVAL (1500 * CLOCK_SYS_CLOCK_1US)

#define IR_NEC_TYPE                     1
#define IR_TOSHIBA_TYPE                 2
#define IR_FRAXEL_TYPE                  3
#define IR_HIGH_LOW_MIN_INTERVAL        (1000 * CLOCK_SYS_CLOCK_1US)
#define IR_HIGH_LOW_MAX_INTERVAL        (2000 * CLOCK_SYS_CLOCK_1US)
#define TC9012_FRAME_CYCLE              (108 * 1000)
#define FRAXEL_LEVEL_NUM                19
#define NEC_TOSHIBA_LEVEL_NUM           67

//////////////////////////////////////////////////
static int ir_write_universal_data(ir_search_index_t *ir_index_data)
{
    u8 ir_start_cnt = 0;

    if (g_ir_search_index_next_addr >= IR_MAX_INDEX_TABLE_LEN) {
        ++ g_ir_errcnt;
        //it's full, don't store any more.
        return -1;
    }

    memset((void *)&ir_flash_universal_data1, 0, sizeof(ir_universal_pattern_t));

    ir_flash_universal_data1.series_cnt = ir_learn_ctrl.series_cnt;
    ir_flash_universal_data1.carr_high_tm = ir_learn_ctrl.carr_high_tm;
    ir_flash_universal_data1.carr_low_tm = ir_learn_ctrl.carr_low_tm;

    #if 0
    //because use timer int to send,so don't need to judge if the time interval more than watchdog timeout.
    foreach (i,ir_flash_universal_data1.series_cnt) {
        if (ir_learn_ctrl.series_tm[i] >= IR_LEARN_MAX_INTERVAL) {
            // time interval shouldn't be more than watchdog timeout time
            ++ g_ir_errcnt;
            return -1;
        }
    }
    #endif
    if (ir_flash_universal_data1.series_cnt <= IR_LEARN_SERIES_CNT / 2) {
        foreach(i,ir_flash_universal_data1.series_cnt) {
            if (ir_learn_ctrl.series_tm[i] > NEC_LEAD_CARR_MIN_INTERVAL &&
                ir_learn_ctrl.series_tm[i] < NEC_LEAD_CARR_MAX_INTERVAL &&
                ir_learn_ctrl.series_tm[i + 1] > NEC_LEAD_NOCARR_MIN_INTERVAL &&
                ir_learn_ctrl.series_tm[i + 1] < NEC_LEAD_NOCARR_MAX_INTERVAL) {
                ir_flash_universal_data1.ir_protocol = IR_NEC_TYPE;
                ir_learn_ctrl.series_tm[i] = (9000 * CLOCK_SYS_CLOCK_1US);
                ir_learn_ctrl.series_tm[i + 1] = (4500 * CLOCK_SYS_CLOCK_1US);
                ir_start_cnt = i + 2;
                break;
            } else if (ir_learn_ctrl.series_tm[i] > TOSHIBA_LEAD_MIN_INTERVAL &&
                ir_learn_ctrl.series_tm[i] < TOSHIBA_LEAD_MAX_INTERVAL &&
                ir_learn_ctrl.series_tm[i + 1] > TOSHIBA_LEAD_MIN_INTERVAL &&
                ir_learn_ctrl.series_tm[i + 1] < TOSHIBA_LEAD_MAX_INTERVAL) {
                ir_flash_universal_data1.ir_protocol = IR_TOSHIBA_TYPE;
                ir_learn_ctrl.series_tm[i] = (4500 * CLOCK_SYS_CLOCK_1US);
                ir_learn_ctrl.series_tm[i + 1] = (4500 * CLOCK_SYS_CLOCK_1US);
                ir_start_cnt = i + 2;
                break;
            } else if (ir_learn_ctrl.series_tm[i] > FRAXEL_LEAD_CARR_MIN_INTERVAL &&
                ir_learn_ctrl.series_tm[i] < FRAXEL_LEAD_CARR_MAX_INTERVAL &&
                ir_learn_ctrl.series_tm[i + 1] > FRAXEL_LEAD_NOCARR_MIN_INTERVAL &&
                ir_learn_ctrl.series_tm[i + 1] < FRAXEL_LEAD_NOCARR_MAX_INTERVAL) {
                ir_flash_universal_data1.ir_protocol = IR_FRAXEL_TYPE;
                ir_learn_ctrl.series_tm[i] = (2400 * CLOCK_SYS_CLOCK_1US);
                ir_learn_ctrl.series_tm[i + 1] = (1200 * CLOCK_SYS_CLOCK_1US);
                ir_start_cnt = i + 2;
                break;
            } else {
                //do nothing
            }
        }
    }

    if (ir_flash_universal_data1.ir_protocol == IR_TOSHIBA_TYPE) {
        ir_flash_universal_data1.series_cnt = NEC_TOSHIBA_LEVEL_NUM;
        if (ir_learn_ctrl.series_tm[ir_start_cnt + 1] > (ir_learn_ctrl.series_tm[ir_start_cnt] << 1))
            ir_flash_universal_data1.toshiba_c0flag = 1;
        else
            ir_flash_universal_data1.toshiba_c0flag = 0;
    }

    if (ir_flash_universal_data1.ir_protocol == IR_FRAXEL_TYPE)
        ir_flash_universal_data1.series_cnt = FRAXEL_LEVEL_NUM;
    if (ir_flash_universal_data1.ir_protocol == IR_NEC_TYPE)
        ir_flash_universal_data1.series_cnt = NEC_TOSHIBA_LEVEL_NUM;
    if (ir_flash_universal_data1.ir_protocol != 0) {
        for (int i = ir_start_cnt; i < ir_flash_universal_data1.series_cnt; i ++) {
            if (ir_learn_ctrl.series_tm[i] < IR_HIGH_LOW_MIN_INTERVAL) {
                ir_learn_ctrl.series_tm[i] = (560 * CLOCK_SYS_CLOCK_1US);
            } else if (ir_learn_ctrl.series_tm[i] < IR_HIGH_LOW_MAX_INTERVAL) {
                if (ir_flash_universal_data1.ir_protocol == IR_NEC_TYPE)
                    ir_learn_ctrl.series_tm[i] = (1680 * CLOCK_SYS_CLOCK_1US);
                else
                    ir_learn_ctrl.series_tm[i] = (1690 * CLOCK_SYS_CLOCK_1US);
            } else {
            }
        }
    }
    //transfer 4bytes to 3bytes before saving.
    foreach (i,ir_flash_universal_data1.series_cnt) {
        if (i < IR_LEARN_SERIES_CNT / 2) {
            ir_flash_universal_data1.series_tm[i * 3] = ((ir_learn_ctrl.series_tm[i]) & 0x0000FF);
            ir_flash_universal_data1.series_tm[i * 3 + 1] = (((ir_learn_ctrl.series_tm[i]) & 0x00FF00) >> 8);
            ir_flash_universal_data1.series_tm[i * 3 + 2] = (((ir_learn_ctrl.series_tm[i]) & 0xFF0000) >> 16);
        } else {
            ir_flash_universal_data2.series_tm[(i-IR_LEARN_SERIES_CNT / 2) * 3] = ((ir_learn_ctrl.series_tm[i]) & 0x0000FF);
            ir_flash_universal_data2.series_tm[(i-IR_LEARN_SERIES_CNT / 2) * 3 + 1] = (((ir_learn_ctrl.series_tm[i]) & 0x00FF00) >> 8);
            ir_flash_universal_data2.series_tm[(i-IR_LEARN_SERIES_CNT / 2) * 3 + 2] = (((ir_learn_ctrl.series_tm[i]) & 0xFF0000) >> 16);
        }
    }

    // Use one page to save one key series data.
    flash_write_page((IR_STORED_SERIES_ADDRESS + GD25Q40B_FLASH_PAGE_SIZE * g_ir_search_index_next_addr),
            sizeof(ir_universal_pattern_t), (u8*)(&ir_flash_universal_data1));
    ir_index_data->learnkey_flash_addr = IR_STORED_SERIES_ADDRESS + GD25Q40B_FLASH_PAGE_SIZE * g_ir_search_index_next_addr;
    ir_index_data->local_key_code = learn_keycode;

    // Store index data
    flash_write_page((IR_STORED_INDEX_ADDRESS + sizeof(ir_search_index_t) * g_ir_search_index_next_addr),
            sizeof(ir_search_index_t), (u8*)(ir_index_data));
    ++ g_ir_search_index_next_addr;

    if (ir_flash_universal_data1.series_cnt > IR_LEARN_SERIES_CNT / 2) {
        //use two page to save if serial cnt > 80
        flash_write_page((IR_STORED_SERIES_ADDRESS + GD25Q40B_FLASH_PAGE_SIZE * g_ir_search_index_next_addr),
                sizeof(ir_universal_pattern_t), (u8*)(&ir_flash_universal_data2));
        flash_write_page((IR_STORED_INDEX_ADDRESS + sizeof(ir_search_index_t) * g_ir_search_index_next_addr),
                sizeof(ir_search_index_t), (u8*)(ir_index_data));
        ++ g_ir_search_index_next_addr;
    }

    return 0;
}

void ir_get_index_addr(void)
{
    u8 i = 0;
     //32*8 = 256 , one page size.
    ir_search_index_t indexTable[IR_MAX_INDEX_TABLE_LEN];
    flash_read_page(IR_STORED_INDEX_ADDRESS, sizeof(indexTable), (u8*)(indexTable));
    for (i = 0; i < IR_MAX_INDEX_TABLE_LEN; i ++) {
        if (ismemf4((void*)(&indexTable[i]), sizeof(ir_search_index_t))) {
            // Invalid data
            break;
        }
    }
    g_ir_search_index_next_addr = i;
}

static int ir_find_learnkey_data(u8 key_value)
{
    int i;
    ir_search_index_t indexTable[IR_MAX_INDEX_TABLE_LEN]; //32*8 = 256 , one page size.

    if (g_ir_search_index_next_addr >= IR_MAX_INDEX_TABLE_LEN) {
        g_ir_search_index_next_addr = IR_MAX_INDEX_TABLE_LEN;
    }

    if (g_ir_search_index_next_addr == 0)
        return -1;

    flash_read_page(IR_STORED_INDEX_ADDRESS, sizeof(indexTable), (u8*)(indexTable));
    // Search serial data flash addr from the newest one by the key_value
    for (i = (g_ir_search_index_next_addr - 1); i >= 0; i --) {
        if (indexTable[i].local_key_code == key_value) {
            flash_read_page(indexTable[i].learnkey_flash_addr,
                    sizeof(ir_universal_pattern_t), (u8*)(&ir_flash_universal_data1));
            if (ir_flash_universal_data1.series_cnt > IR_LEARN_SERIES_CNT / 2) {
                flash_read_page(((indexTable[i].learnkey_flash_addr) + GD25Q40B_FLASH_PAGE_SIZE),
                        sizeof(ir_universal_pattern_t), (u8*)(&ir_flash_universal_data2));
            }
            return 0;
        }
    }
    return -1;
}

void ir_learn_send(u8 local_data_code)
{
    u32 carrier_cycle = 0;
    if (ir_sending_check()) {
        return;
    }

    if (ir_find_learnkey_data(local_data_code)) {
        return;//not found
    }

    memset(&ir_learn_ctrl, 0, sizeof(ir_learn_ctrl));
    //record protocol and c0flag in order to diff repeat code.
    ir_learn_ctrl.ir_protocol = ir_flash_universal_data1.ir_protocol;
    ir_learn_ctrl.toshiba_c0flag = ir_flash_universal_data1.toshiba_c0flag;

    foreach (i,ir_flash_universal_data1.series_cnt) {
        if (i < IR_LEARN_SERIES_CNT / 2) {
            ir_learn_ctrl.series_tm[i] = ir_flash_universal_data1.series_tm[i * 3] |
                (ir_flash_universal_data1.series_tm[i * 3 + 1] << 8) |
                (ir_flash_universal_data1.series_tm[i * 3 + 2] << 16);
        } else {
            ir_learn_ctrl.series_tm[i] = ir_flash_universal_data2.series_tm[(i - IR_LEARN_SERIES_CNT / 2) * 3] |
                (ir_flash_universal_data2.series_tm[(i-IR_LEARN_SERIES_CNT / 2) * 3 + 1] << 8) |
                (ir_flash_universal_data2.series_tm[(i-IR_LEARN_SERIES_CNT / 2) * 3 + 2] << 16);
        }
    }

    if (ir_learn_ctrl.ir_protocol == 0) {
        pwmm_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
        carrier_cycle = ir_flash_universal_data1.carr_high_tm + ir_flash_universal_data1.carr_low_tm;
        pwmm_set_duty(IR_PWM_ID, carrier_cycle, ir_flash_universal_data1.carr_high_tm);  //set cycle and high
        pwmm_set_phase(IR_PWM_ID, 0);   //no phase at pwm beginning
    } else {
        pwmm_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
        pwmm_set_duty(IR_PWM_ID, PWM_CYCLE_VALUE, PWM_HIGH_VALUE);  //set cycle and high
        pwmm_set_phase(IR_PWM_ID, 0);   //no phase at pwm beginning
    }

    ir_send_ctrl_clear();
    ir_send_add_series_item(ir_learn_ctrl.series_tm, ir_flash_universal_data1.series_cnt, 1);
    if ((ir_learn_ctrl.ir_protocol == IR_NEC_TYPE) ||
        (ir_learn_ctrl.ir_protocol == IR_TOSHIBA_TYPE)) {
        ir_send_ctrl_start(1);
    } else {
        ir_send_ctrl_start(0);
    }
}


void ir_nec_send(u8 addr1, u8 addr2, u8 cmd)
{
    if (ir_sending_check() || g_last_cmd == cmd) {
        return;
    }

    static u32 ir_lead_times[] = {IR_INTRO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US,
            IR_INTRO_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US};
    static u32 ir_stop_bit_times[] = {IR_END_TRANS_TIME_NEC * CLOCK_SYS_CLOCK_1US};

    ir_byte_time[0][0] = IR_HIGH_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[0][1] = IR_HIGH_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][0] = IR_LOW_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;
    ir_byte_time[1][1] = IR_LOW_NO_CARR_TIME_NEC * CLOCK_SYS_CLOCK_1US;

    g_last_cmd = cmd;
    ir_send_ctrl_clear();
    ir_send_add_series_item(ir_lead_times, ARRAY_SIZE(ir_lead_times), 1);
    ir_send_add_byte_item(addr1, 1);
    ir_send_add_byte_item(addr2, 1);
    ir_send_add_byte_item(cmd, 1);
    ir_send_add_byte_item(~cmd, 1);
    ir_send_add_series_item(ir_stop_bit_times, ARRAY_SIZE(ir_stop_bit_times), 1);
    ir_send_ctrl_start(1);

    g_ir_addr = addr1;
}

int ir_record_end(void *data)
{
    if (ir_learn_ctrl.series_cnt < IR_LEARN_SERIES_CNT) {
        ++ ir_learn_ctrl.series_cnt;//plus the last carrier.
    }

    gpio_clr_interrupt(GPIO_IR_LEARN_IN);
    ir_write_universal_data(&ir_index_data);

    //device_led_setup(g_ir_led[IR_LEARN_FINISH - 1]);

    return -1;
}

void ir_record(u32 tm, int pol)
{
    ++ ir_learn_ctrl.ir_int_cnt;
    ir_learn_ctrl.curr_trigger_tm = tm;

    if (ir_learn_ctrl.series_cnt >= IR_LEARN_SERIES_CNT) {
        return;
    }

    if (ir_learn_ctrl.ir_int_cnt > 1) {
        ir_learn_ctrl.time_interval = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm;
        // record carrier time
        if (ir_learn_ctrl.time_interval < IR_LEARN_NONE_CARR_MIN &&
            ir_learn_ctrl.time_interval > IR_LEARN_CARR_GLITCH_MIN) {   // removing glitch  // receiving carrier
            if (!ir_learn_ctrl.is_carr) {
                ir_learn_ctrl.carr_first = ir_learn_ctrl.time_interval;
                if (ir_learn_ctrl.series_cnt > 0) {  //  Do not record leading none-carrier
                    ir_learn_ctrl.series_tm[(ir_learn_ctrl.series_cnt)] =
                            ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.carr_switch_start_tm - ir_learn_ctrl.carr_first;
                    ++ ir_learn_ctrl.series_cnt;
                }
                ir_learn_ctrl.carr_switch_start_tm = ir_learn_ctrl.curr_trigger_tm;
                ir_learn_ctrl.is_carr = 1;
            } else {
                ir_learn_ctrl.series_tm[(ir_learn_ctrl.series_cnt)] = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.carr_switch_start_tm;
                        //+ (ir_learn_ctrl.ir_series_data.carr_low_tm + ir_learn_ctrl.ir_series_data.carr_high_tm);
            }

            if (ir_learn_ctrl.carr_check_cnt < IR_CARR_CHECK_CNT) {
                //&& ir_learn_ctrl.time_interval < IR_LEARN_CARR_MAX && ir_learn_ctrl.time_interval > IR_LEARN_CARR_MIN){
                ++ ir_learn_ctrl.carr_check_cnt;
                // we are receiving carrier
                if (pol) {
                    if (ir_learn_ctrl.time_interval < ir_learn_ctrl.carr_high_tm ||
                        0 == ir_learn_ctrl.carr_high_tm)     //  record the shortest cycle
                        ir_learn_ctrl.carr_high_tm = ir_learn_ctrl.time_interval;
                } else {
                    if (ir_learn_ctrl.time_interval < ir_learn_ctrl.carr_low_tm ||
                        0 == ir_learn_ctrl.carr_low_tm)   //  record the shortest cycle
                        ir_learn_ctrl.carr_low_tm = ir_learn_ctrl.time_interval;
                }
            }
        } else {
            // record carrier time
            if (ir_learn_ctrl.is_carr) {
                // It decrease one cycle with actually calculate, so add it.
               ir_learn_ctrl.series_tm[(ir_learn_ctrl.series_cnt)] =
                       (ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.carr_switch_start_tm) + ir_learn_ctrl.carr_first;
                if (ir_learn_ctrl.series_tm[0] < IR_LEARN_START_MINLEN){
                    memset(&ir_learn_ctrl, 0, sizeof(ir_learn_ctrl));
                    return;
                } else {
                    //#ifndef WIN32
                    #if 0
                    if (0 == ir_learn_ctrl.learn_timer_started) {
                        ir_learn_ctrl.learn_timer_started = 1;
                        device_led_setup(g_ir_led[5]);
                        ir_record_end(&ir_index_data);
                    }
                    #endif
                }

                ++ ir_learn_ctrl.series_cnt;

                ir_learn_ctrl.carr_switch_start_tm = ir_learn_ctrl.last_trigger_tm;
            }
            ir_learn_ctrl.is_carr = 0;
        }
    }
    ir_learn_ctrl.last_trigger_tm = ir_learn_ctrl.curr_trigger_tm;
}

_attribute_ram_code_ void ir_learn_irq_handler(void)
{
    if (g_ir_learn_state != IR_LEARN_KEY)
        return;
    reg_irq_src |= IR_LEARN_INTERRUPT_MASK;
    ir_record(clock_time(), 1);  // IR Learning
}

void ir_exit_learn(void)
{
    //irq_clr_mask(FLD_IRQ_GPIO_EN);
    gpio_clr_interrupt(GPIO_IR_LEARN_IN);
    //recover IR_out and IR_control to default mode.
    gpio_set_func(GPIO_IR_OUT, AS_PWM);
    gpio_set_output_en(GPIO_IR_CONTROL, 1);
    gpio_write(GPIO_IR_CONTROL, 1);

    #if(MODULE_FRAXEL_ENABLE)
    //Recover other gpio interrupt
    reg_irq_src = 0xffffffff;
    gpio_set_interrupt(GPIO_FRAXEL_PIN0, 0);
    gpio_set_interrupt(GPIO_FRAXEL_PIN1, 0);
    #endif
    g_ir_learn_state = IR_LEARN_FINISH;
    g_ir_learn_tick = clock_time();
    device_led_setup(g_ir_led[4]);
    ir_record_end(&ir_index_data);
    ir_restore_keyboard();
}

void ir_check_tick()
{
#define IR_LEARN_WAIT_KEY_TIMEOUT    (10000000)
#define IR_LEARN_KEY_TIMEOUT         (15000000)

    if (g_ir_learn_state == IR_LEARN_DISABLE ||
        g_ir_learn_state == IR_LEARN_FINISH)
        return;

    if (g_ir_learn_state == IR_LEARN_WAIT_KEY &&
        clock_time_exceed(g_ir_learn_tick, IR_LEARN_WAIT_KEY_TIMEOUT)) {
        // learn start, but no key is pressed
        g_ir_learn_state = IR_LEARN_DISABLE;
        g_ir_learn_tick = clock_time();
        device_led_setup(g_ir_led[IR_LEARN_WAIT_KEY + g_ir_learn_state]);
        ir_restore_keyboard();
        return;
    }

    if (g_ir_learn_state == IR_LEARN_KEY &&
        clock_time_exceed(g_ir_learn_tick, IR_LEARN_KEY_TIMEOUT)) {
        ir_exit_learn();
        g_ir_learn_tick = clock_time();
    }
}

static void ir_disable_keyboard()
{
    u32 pin[] = KB_DRIVE_PINS;
    for (u8 i = 0; i < ARRAY_SIZE(pin); i ++) {
        gpio_set_wakeup(pin[i], 1, 0);       // drive pin core(gpio) suspend disable
        cpu_set_gpio_wakeup(pin[i], 1, 0);  // drive pin pad deepsleep disable
    }
    reg_irq_src |= IR_LEARN_INTERRUPT_MASK;
}

static void ir_restore_keyboard()
{
    u32 pin[] = KB_DRIVE_PINS;
    for (u8 i = 0; i < ARRAY_SIZE(pin); i ++) {
        gpio_set_wakeup(pin[i], 1, 1);       // drive pin core(gpio) suspend wakeup high
        cpu_set_gpio_wakeup(pin[i], 1, 1);  // drive pin pad deepsleep wakeup high
    }
    reg_irq_src |= IR_LEARN_INTERRUPT_MASK;
    rc_ir_init();
}

void ir_learn(const kb_data_t *kb_data)
{
    u8 key;

    ir_check_tick();

    if (!kb_data)
        return;

    do {  // check to start IR learning
        if (kb_data->cnt != 2)
            break;
        if (g_ir_learn_state != IR_LEARN_DISABLE &&
            g_ir_learn_state != IR_LEARN_FINISH)
            break;

        u8 key0 = kb_data->keycode[0];
        u8 key1 = kb_data->keycode[1];
        key0 = kb_map_ir[key0];
        key1 = kb_map_ir[key1];

        if ((key0 == IR_LEARN_L && key1 == IR_LEARN_R) ||
            (key0 == IR_LEARN_R && key1 == IR_LEARN_L)) {
            ir_start_learn();
            return;
        }
    } while (0);

    if (g_ir_learn_state == IR_LEARN_DISABLE ||
        g_ir_learn_state == IR_LEARN_FINISH)
        return;

    if (kb_data->cnt > 1)  // skip multiple keys
        return;

    do {
        if (kb_data->cnt == 0)
            break;
        /*
         * new key pressed
         */
        key = kb_data->keycode[0];
        key = kb_map_ir[key];

        /*
         * skip the keys used for control IR learning
         */
        if (key == IR_LEARN_L || key == IR_LEARN_R)
            return;
        if (key == IR_LEARN_STOP) {
            ir_exit_learn();
            return;
        }

        if (learn_keycode != 0 && (learn_keycode & 0xFF) != key) {
            // new key
            device_led_setup(g_ir_led[3]);
            ir_record_end(&ir_index_data);
        }

        ir_start_learn();
        g_ir_learn_state = IR_LEARN_KEY;
        learn_keycode = key;
        ir_disable_keyboard();
    } while (0);

}

u8 ir_learning()
{
    return g_ir_learn_state == IR_LEARN_WAIT_KEY || g_ir_learn_state == IR_LEARN_KEY;
}

void ir_start_learn(void)
{
    if (g_ir_learn_state != IR_LEARN_DISABLE &&
        g_ir_learn_state != IR_LEARN_FINISH)
        return;

    g_ir_learn_state = IR_LEARN_WAIT_KEY;
    g_ir_learn_tick = clock_time();

    #if(MODULE_FRAXEL_ENABLE)
    //clear other gpio interrup to avoid blocked by other interrupt
    reg_irq_src = 0xffffffff;
    gpio_clr_interrupt(GPIO_FRAXEL_PIN0);
    gpio_clr_interrupt(GPIO_FRAXEL_PIN1);
    #endif

    // Init data
    memset(&ir_learn_ctrl, 0, sizeof(ir_learn_ctrl));

    // To output ircontrol and irout low, then ir receive can work.
    gpio_set_func(GPIO_IR_OUT, AS_GPIO);
    gpio_set_input_en(GPIO_IR_OUT, 0);
    gpio_set_output_en(GPIO_IR_OUT, 1);
    gpio_write(GPIO_IR_OUT, 0);

    gpio_set_func(GPIO_IR_CONTROL, AS_GPIO);
    gpio_set_input_en(GPIO_IR_CONTROL, 0);
    gpio_set_output_en(GPIO_IR_CONTROL, 1);
    gpio_write(GPIO_IR_CONTROL, 0);

    gpio_set_func(GPIO_IR_LEARN_IN, AS_GPIO);
    gpio_set_input_en(GPIO_IR_LEARN_IN, 1);
    gpio_set_output_en(GPIO_IR_LEARN_IN, 0);

    // Enable interrupt.
    reg_irq_src |= IR_LEARN_INTERRUPT_MASK;
    gpio_set_wakeup(GPIO_IR_LEARN_IN, 0, 1);
#if 0
    gpio_set_interrupt(GPIO_IR_LEARN_IN, 1);
    gpio_setup_up_down_resistor(GPIO_IR_LEARN_IN, PM_PIN_PULLUP_10K);
#endif
    gpio_core_irq_enable_all(1);
    irq_set_mask(IR_LEARN_INTERRUPT_MASK);

    device_led_setup(g_ir_led[IR_LEARN_WAIT_KEY - 1]);

}

void rc_ir_init(void)
{
    PWM0_CFG_GPIO_A0();
    pwmm_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);  //set pwm_clk as sys_clk(16000000)
    pwmm_set_mode(IR_PWM_ID, PWM_NORMAL_MODE);  //pwm0 count mode
    pwmm_set_duty(IR_PWM_ID, PWM_CYCLE_VALUE, PWM_HIGH_VALUE);  //set cycle and high
    pwmm_set_phase(IR_PWM_ID, 0);   //no phase at pwm beginning

    // Only with IR Send function, So the GPIO_IR_CONTROL need to output high
    gpio_set_output_en(GPIO_IR_CONTROL, 1);
    gpio_write(GPIO_IR_CONTROL, 1);

    memset(&ir_learn_ctrl, 0, sizeof(ir_learn_ctrl));
    g_last_cmd = 0xff;
    ir_get_index_addr();
}

#endif
