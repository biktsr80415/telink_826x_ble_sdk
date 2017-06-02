/*
 * mouse_button.c
 *
 *  Created on: Feb 12, 2014
 *      Author: xuzhen
 */

#include "../../proj/tl_common.h"
#include "../common/rf_frame.h"
#include "../../proj_lib/pm.h"
#include "../common/blt_led.h"
#include "mouse.h"
#include "mouse_custom.h"
#include "mouse_rf.h"
#include "mouse_custom.h"
#include "mouse_button.h"
#include "trace.h"

static u8 button_last;  //current button
static u8 button_pre;   //history button

#if(!MOUSE_BUTTON_GPIO_REUSABILITY)
	static u32 gpio_btn_all;
	u32 gpio_btn_valid[MAX_MOUSE_BUTTON];
#endif

void mouse_button_init(mouse_hw_t *mouse_hw)
{
    u32 level;
    u32 spec_pin;

	for(int i=0;i<MAX_MOUSE_BUTTON;i++){
		spec_pin = mouse_hw->button[i];
		level = (mouse_hw->gpio_level_button[i] == U8_MAX);  //0xff：pullup   others:pulldown
		gpio_btn_valid[i] =	level ? 0 : spec_pin;  //1:低有效  0:高有效
		gpio_btn_all |= spec_pin;
		//gpio_setup_up_down_resistor(spec_pin, level );		//已经10K上拉了

#if MOUSE_GPIO_FULL_RE_DEF
		gpio_set_func(spec_pin,AS_GPIO);
		gpio_set_output_en(spec_pin, 0);
		gpio_set_input_en(spec_pin, 1);
#endif
	}
}

const u32	button_seq_button_middle_click2 =	// LRM - LR - LRM - LR
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE) << 24) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT) << 16) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE) << 8) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT) << 0);

const u32	button_seq_paring =	                // LR - LRM - LR
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | 0x80) << 16) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE) << 8) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT) << 0);

const u32	button_seq_emi =                    // LRM - LR - LRM - LR
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE | 0x80) << 24) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT) << 16) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE) << 8) |
		((FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT) << 0);

static inline u32 mouse_button_special_seq( u32 button, u32 multi_seq, u32 power_on, u32 paring_anytime ){
    static u32  button_seq = 0;
    static u32  button_seq1 = 0;
    static u32  button_seq2 = 0;    
    u32 specail_seq;
    u32 seq = 0;
    u32  mouse_pairing_ui = mouse_btn_ui.mode_switch_ui;
    u32  mouse_emi_ui = mouse_btn_ui.emi_ui;

    if( multi_seq ){
        if( power_on )
            button |= 0x80;
        button_seq = (button_seq << 8) | (button_seq1>>24);
        button_seq1 = (button_seq1 << 8) | (button_seq2>>24);
        button_seq2 = (button_seq2 << 8) | button;
        specail_seq = (button_seq_button_middle_click2 == button_seq1)\
               && (button_seq_button_middle_click2 == button_seq2);
        mouse_pairing_ui = button_seq_paring;
        mouse_emi_ui = button_seq_emi;
    }
    else{
        button_seq = button;
        specail_seq = power_on;
    }

    if( specail_seq ){
        if( button_seq == mouse_pairing_ui ){
            seq = BTN_INTO_PAIRING;
    	}
    	else if (button_seq == mouse_emi_ui) {
           seq = BTN_INTO_EMI;
    	}
        else if ( button_seq == button_seq1 ){
            seq = BTN_INTO_SPECL_MODE;
            button_seq2 = 0xff;     //next spcail sequence must be the same with the last one
        }
    }
    else if ( paring_anytime && (button_seq == mouse_pairing_ui) ){
        seq = BTN_INTO_PAIRING;
    }
    
    return seq;
}

static inline void mouse_button_process_test_mode( u8 *mouse_mode, u8 *dbg_mode, u32 button, u32 test_mode ){
    if( (test_mode == BTN_INTO_PAIRING) && !button ) {
        *mouse_mode = STATE_PAIRING;
	}
    else if( (test_mode == BTN_INTO_EMI) && !button ) {            
       	*mouse_mode = STATE_EMI;
    }
#if MOUSE_BUTTON_FULL_FUNCTION    
    if( (test_mode == BTN_INTO_SPECL_MODE) && (button & FLAG_BUTTON_LEFT) && (button & FLAG_BUTTON_RIGHT) ){
        *dbg_mode |= STATE_TEST_0_BIT;
    }
    else if ( button == 0 )
        *dbg_mode &= ~STATE_TEST_0_BIT;
    
    if( (test_mode == BTN_INTO_SPECL_MODE)&& !(button & FLAG_BUTTON_LEFT) && (button & FLAG_BUTTON_RIGHT)  )
        *dbg_mode |= STATE_TEST_EMI_BIT;
    else
        *dbg_mode &= ~STATE_TEST_EMI_BIT;
    
    if( (test_mode == BTN_INTO_SPECL_MODE) && (button & FLAG_BUTTON_LEFT) && !(button & FLAG_BUTTON_RIGHT) ){
        *dbg_mode |= STATE_TEST_V_BIT;
    }
    else
        *dbg_mode &= ~STATE_TEST_V_BIT;
#endif
}

extern u8 adv_type_switch;
extern u8 mouse_get_pre_info_from_master;
extern u8 SysMode;
extern u16 BLE_MODE_SWITCH_THRESH;
extern u16 switch_mode_start_flg;
extern led_cfg_t led_cfg[];
extern led_cfg_t led_cpi[];

inline u32 mouse_button_process(mouse_status_t * mouse_status)
{   

    static u16 btn_lr_cnt = 0;
    static u16 thresh_cnt = 0;

    static u16 btn_d_cnt = 0;
    static u8  btn_emi_cnt = 0;

    //button_last: current button
    //button_pre: previous button

    if( switch_mode_start_flg == 1 ){
    	switch_mode_start_flg = 2;
    }

    if ( !button_last && (adv_type_switch != 1) && (switch_mode_start_flg != 2)){
    	btn_lr_cnt=0;
    	btn_d_cnt = 0;
    	button_pre = button_last;

    	return 0;				//there is no button be pressed
    }
    else{
    	if(  SysMode == RF_1M_BLE_MODE  ){		//BLE MODE, BUTTON PROCESS
    		if(button_last == FLAG_BUTTON_MIDDLE){
    		    if((btn_d_cnt++ > 270) && !mouse_get_pre_info_from_master){
    		    	adv_type_switch = 1;
    		    	device_led_setup(led_cfg[LED_NON_DIR_ADV]);
    		    }
    		}
    		else{
    			btn_d_cnt=0;
    		}
    	}
    	else{									//2P4G MODE
    		if(mouse_status->mouse_mode != STATE_EMI && (button_last == (FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT |  FLAG_BUTTON_MIDDLE)) ){
    			if(++btn_emi_cnt > 0x7f){
    				mouse_status->mouse_mode = STATE_EMI;
    				return 0;
    			}
    		}
    		else{
    			btn_emi_cnt = 0;
    		}

    	}

    	//if((button_last & FLAG_BUTTON_LEFT) && (button_last & FLAG_BUTTON_RIGHT))		//mode switch
    	if(button_last == (FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT))
    		btn_lr_cnt++;
    	else
    		btn_lr_cnt=0;

        if((button_pre != button_last) && (button_last & FLAG_BUTTON_DPI)) {            //new event
            ++mouse_status->cpi;
            mouse_status->cpi = (mouse_status->cpi < mouse_cpi.sns_cpi_sgmt) ? mouse_status->cpi : 0;
            mouse_sensor_set_cpi( &mouse_status->cpi );

            //device_led_setup(led_cpi[mouse_status->cpi]);
            device_led_setup(led_cfg[mouse_status->cpi]);
        }
    	button_pre = button_last;
    }

    thresh_cnt = (SysMode == RF_1M_BLE_MODE) ? BLE_MODE_SWITCH_THRESH : NORMAL_MODE_SWITCH_THRESH;

    if( (btn_lr_cnt > thresh_cnt) && !switch_mode_start_flg){
    	switch_mode_start_flg = 1;
    	device_led_setup(led_cfg[LED_SWITCH_MODE]);
    }



	if( ((switch_mode_start_flg == 2) || adv_type_switch == 1) && !DEVICE_LED_BUSY){
    	u8 ana_reg1 = analog_read(DEEP_ANA_REG4);

    	if(switch_mode_start_flg == 2){
    		if(!SysMode){
    			ana_reg1 |= BIT(2);
    		}
    		else{
    			ana_reg1 &= ~BIT(2);
    		}
    	}
    	else{
    		ana_reg1 |= BIT(3);
    	}
    	analog_write(DEEP_ANA_REG4, ana_reg1);
		irq_disable();
		REG_ADDR8(0x6f) = 0x20;
		while(1);
	}
}

u32 mouse_button_process_emi(s8 *chn_idx, u8 *test_mode_sel, u32 btn_pro_end)
{ 
    u32 cmd = 0;    
    if (button_pre != button_last) {     //new event
        cmd = 0x80;

        if(!button_pre && (button_last == FLAG_BUTTON_MIDDLE)){
        	*test_mode_sel = (*test_mode_sel+1) & 3;  //mode change: carrier, cd, rx, tx
        }
        else if(!button_pre && (button_last == FLAG_BUTTON_LEFT)){
        	*chn_idx += 1;               //channel up
        }
        else if(!button_pre && (button_last == FLAG_BUTTON_RIGHT)) {
               *chn_idx -= 1;            //channel down
        }
        else {
        	cmd &= 0x0f;
        }

        if (*chn_idx < 0) {
        	*chn_idx = 2;
        }
        else if (*chn_idx > 2) {
            *chn_idx = 0;
       }


        if( btn_pro_end )
            button_pre = button_last;
    }

#if 0
        if (!(button_pre & FLAG_BUTTON_MIDDLE) && (button_last & FLAG_BUTTON_MIDDLE)) {                
            *test_mode_sel = (*test_mode_sel+1) & 3;  //mode change: carrier, cd, rx, tx
        }
        else if (!(button_pre & FLAG_BUTTON_LEFT) && (button_last & FLAG_BUTTON_LEFT)) {
            *chn_idx += 1;               //channel up
        }
        else if (!(button_pre & FLAG_BUTTON_RIGHT) && (button_last & FLAG_BUTTON_RIGHT)) {                
            *chn_idx -= 1;               //channel down
        }
        else {
            cmd &= 0x0f;
        }
        
        if (*chn_idx < 0) {
            *chn_idx = 2;
        }
        else if (*chn_idx > 2) {
            *chn_idx = 0;
        }
    }
    if( btn_pro_end )
        button_pre = button_last;
#endif
    return cmd;
}

static inline u8 mouse_button_debounce(u8 btn_cur, u8 btn_last, u8 debouce_len){
    static u8 s_btn_cnt = 0;
	if(	s_btn_cnt >= debouce_len && btn_last != btn_cur ){
		btn_last = btn_cur;
		s_btn_cnt = 0;
	}else{
		if(btn_last != btn_cur)
			s_btn_cnt ++;
		else
			s_btn_cnt = 0;
	}
    return btn_last;
}

void mouse_button_pull(mouse_status_t  * mouse_status, u32 prepare_level ){
    u32 pull_level = 0;
    u32 spec_pin = 0;
    int i = 0;
    for ( i = MAX_MOUSE_BUTTON - 1; i >= 0; i-- ){
        spec_pin = mouse_status->hw_define->button[i];
        pull_level = MOUSE_BTN_HIGH && mouse_status->hw_define->gpio_level_button[i];
        if ( pull_level == prepare_level ){
            gpio_setup_up_down_resistor( spec_pin, pull_level );            
        }
    }
}


/// \param detect_level - 0: detect low after button been pullup,  else: detect high after button been pulldown
/// \mouse should detect high, then detect low
inline u8 mouse_button_detect(mouse_status_t  * mouse_status, u32 detect_level)
{
    static u8 btn_cur = 0;
    static u8 btn_real;

    if ( detect_level )
        return 0;

    u32 pull_level = 0;
    u32 spec_pin = 0;
    for ( int i = MAX_MOUSE_BUTTON - 1; i >= 0; i-- ){
//        spec_pin = mouse_status->hw_define->button[i];
//        pull_level = MOUSE_BTN_HIGH && mouse_status->hw_define->gpio_level_button[i];
    	spec_pin = mouse_hw.button[i];
    	pull_level = MOUSE_BTN_HIGH; //&& mouse_status->hw_define->gpio_level_button[i];

        if ( pull_level != detect_level ){
            if( !gpio_read(spec_pin) ^ !pull_level ){
                btn_cur |= (1<<i);
            }
        }
    }

    //mask button event on power-on
//    static u8 btn_power_on_mask = 0xff;
//    if ( !btn_cur )
//        btn_power_on_mask = 0xff;

    mouse_status->data->btn = btn_cur & 0xff;
    u8 button_change = 0;
    button_change = (button_last != btn_cur) ? 1 : 0;

//    if( !button_last && (btn_cur ^ button_last) ){
//    	button_change = 1;					//button has been pressed
//    }
//    else if(!btn_real && (btn_cur ^ button_last)){
//    	button_change = 0;					//buttn has been released
//    }
//    else if(btn_real && !(btn_cur ^ button_last) ){
//    	button_change = 2;					//button hold on
//    }
    button_last = btn_cur;
    btn_cur = 0;
    return button_change;
}

u32 mouse_button_pull_and_detect(mouse_status_t  * mouse_status){
    mouse_button_pull(mouse_status, MOUSE_BTN_LOW);
    WaitUs(100);
    mouse_button_detect(mouse_status, MOUSE_BTN_HIGH);
    mouse_button_pull(mouse_status, MOUSE_BTN_HIGH);    
    WaitUs(100);
    return mouse_button_detect(mouse_status, MOUSE_BTN_LOW);
}

