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
    int i;
    u32 level;
    u32 spec_pin;
	for(i=0;i<MAX_MOUSE_BUTTON;i++){
#if(!MOUSE_BUTTON_GPIO_REUSABILITY)  //no gpio reusability
		spec_pin = mouse_hw->button[i];
		level = (mouse_hw->gpio_level_button[i] == U8_MAX);  //0xff：pullup   others:pulldown
		gpio_btn_valid[i] =	level ? 0 : spec_pin;  //1:低有效  0:高有效
		gpio_btn_all |= spec_pin;
		//gpio_setup_up_down_resistor(spec_pin, level );		//已经10K上拉了
#endif


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

static inline void mouse_button_process_cpi_2_btn(u32 *button){
    static u16 btn_cpi_cnt = 0;
    if( mouse_cust_2_btn_cpi ){
        if( (*button & mouse_btn_ui.cpi_2_btn) == mouse_btn_ui.cpi_2_btn )
            btn_cpi_cnt++;
        else
            btn_cpi_cnt = 0;

        if( btn_cpi_cnt > mouse_btn_ui.cpi_2_btn_time){
            *button |= FLAG_BUTTON_DPI;
        }
    }
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
#if(MOUSE_LED_MODULE_EN & 0)
        led_cfg_t mouse_led_test_0 = { 2, 4, 3, 0};
        mouse_led_setup( mouse_led_test_0 );
#endif
    }
    else if ( button == 0 )
        *dbg_mode &= ~STATE_TEST_0_BIT;
    
    if( (test_mode == BTN_INTO_SPECL_MODE)&& !(button & FLAG_BUTTON_LEFT) && (button & FLAG_BUTTON_RIGHT)  )
        *dbg_mode |= STATE_TEST_EMI_BIT;
    else
        *dbg_mode &= ~STATE_TEST_EMI_BIT;
    
    if( (test_mode == BTN_INTO_SPECL_MODE) && (button & FLAG_BUTTON_LEFT) && !(button & FLAG_BUTTON_RIGHT) ){
        *dbg_mode |= STATE_TEST_V_BIT;
#if(MOUSE_LED_MODULE_EN & 0)
        led_cfg_t mouse_led_test_v = { 8, 16, 3, 0};
        mouse_led_setup( mouse_led_test_v );
#endif
    }
    else
        *dbg_mode &= ~STATE_TEST_V_BIT;
#endif
}

extern int SysMode;
extern u16 ble_swith_time_thresh;
extern u16 switch_mode_start_flg;
extern led_cfg_t led_cfg[];
extern led_cfg_t led_cpi[];
static u8 test_mode_pending;
#if 0
_attribute_ram_code_
#endif
u32 mouse_button_process(mouse_status_t * mouse_status)
{   
    u32 button = button_last;
    static u16 btn_lr_cnt = 0;
    static u16 thresh_cnt = 0;

    thresh_cnt = (SysMode == RF_1M_BLE_MODE) ? ble_swith_time_thresh : _2P4G_MODE_SWITCH_CNT;

    if ( button_pre || button_last ){
        if( (button & FLAG_BUTTON_LEFT) && (button & FLAG_BUTTON_RIGHT) )
            btn_lr_cnt++;
        else
            btn_lr_cnt = 0;
    
        mouse_button_process_cpi_2_btn( &button );
    }

    if(btn_lr_cnt > thresh_cnt){
    	btn_lr_cnt = 0;
    	device_led_setup(led_cfg[0]);
    	u8 ana_reg1 = analog_read(DEEP_ANA_REG1);
    	if(!SysMode){
    		ana_reg1 |= BIT(4);
    	}
    	else{
    		ana_reg1 &= ~BIT(4);
    	}
    	analog_write(DEEP_ANA_REG1, ana_reg1);
    	switch_mode_start_flg = 1;
    }

	if( (switch_mode_start_flg == 2) && !DEVICE_LED_BUSY){
		irq_disable();
		REG_ADDR8(0x6f) = 0x20;
		while(1);
	}


    if (button_pre != button) {            //new event
#if 1
        if( button & FLAG_BUTTON_DPI ){
        	++mouse_status->cpi;
            mouse_status->cpi = (mouse_status->cpi < mouse_cpi.sns_cpi_sgmt) ? mouse_status->cpi : 0;

            mouse_sensor_set_cpi( &mouse_status->cpi );

            device_led_setup(led_cpi[mouse_status->cpi]);
#if(MOUSE_LED_MODULE_EN & 0)
            mouse_led_setup( mouse_led_cpi_cfg_cust(mouse_status->cpi) );
#endif
        }
#endif
        u32 multi_seq = (btn_lr_cnt > 800) || mouse_ui_level;
        u32 paring_any_time = !mouse_cust_paring_only_pwon && device_never_linked && !test_mode_pending;
        //test_mode_pending |= mouse_button_special_seq( \
            button, multi_seq, (mouse_status->mouse_mode == STATE_POWERON), paring_any_time ); 
        
    	//mouse_button_process_test_mode( &mouse_status->mouse_mode, &mouse_status->dbg_mode, button, test_mode_pending);

       if( !button )
            test_mode_pending = 0;

    }

	button_pre = button;

}

u32 mouse_button_process_emi(s8 *chn_idx, u8 *test_mode_sel, u32 btn_pro_end)
{ 
    u32 cmd = 0;    
    if (button_pre != button_last) {     //new event
        cmd = 0x80;
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
u32 mouse_button_detect(mouse_status_t  * mouse_status, u32 detect_level)
{
    static u8 btn_cur = 0;
    static u32 btn_changed = 0;			//once button be pressed/released, button_changed value is 1

    u32 pull_level = 0;
    u32 spec_pin = 0;
    int i = 0;
    for ( i = MAX_MOUSE_BUTTON - 1; i >= 0; i-- ){
        spec_pin = mouse_status->hw_define->button[i];
        pull_level = MOUSE_BTN_HIGH && mouse_status->hw_define->gpio_level_button[i];         
        if ( pull_level != detect_level ){
            if( !gpio_read(spec_pin) ^ !pull_level ){
                btn_cur |= (1<<i);
            }
        }
    }

    if ( detect_level )
        return 0;

    static u8 btn_real;
#if 0
    u32 debouce_len = (mouse_status->mouse_mode == STATE_POWERON) ? 0 : 3;
    btn_real = mouse_button_debounce( btn_cur, button_last, debouce_len );
#endif

    btn_real = btn_cur;

    btn_cur = 0;

    //mask button event on power-on
    static u8 btn_power_on_mask = 0xff;
    if ( !btn_real )
        btn_power_on_mask = 0xff;



    if( switch_mode_start_flg == 1 ){
    	switch_mode_start_flg = 2;
    	mouse_status->data->btn = 0;
    }
    else{

    	mouse_status->data->btn = btn_real & btn_power_on_mask;

    }
    
    btn_changed = (button_last != btn_real) ? 1 : 0;

    button_last = btn_real;
    //return button_last;
    return btn_changed;
}

u32 mouse_button_pull_and_detect(mouse_status_t  * mouse_status){
    mouse_button_pull(mouse_status, MOUSE_BTN_LOW);
    WaitUs(100);
    mouse_button_detect(mouse_status, MOUSE_BTN_HIGH);
    mouse_button_pull(mouse_status, MOUSE_BTN_HIGH);    
    WaitUs(100);
    return mouse_button_detect(mouse_status, MOUSE_BTN_LOW);
}

