/*
 * mouse_emi.c
 *
 *  Created on: Feb 14, 2014
 *      Author: xuzhen
 */

#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../common/emi.h"
#include "mouse.h"
#include "mouse_button.h"
#include "mouse_emi.h"
#include "mouse_custom.h"

void mouse_emi_process(mouse_status_t *mouse_status)
{
    static s8   test_chn_idx = 1;    
    static u8   test_mode_sel = 0;
    static u8   flg_emi_init = 0;
    u32 cmd;    

	mouse_button_detect(mouse_status, MOUSE_BTN_LOW);

	cmd = mouse_button_process_emi( &test_chn_idx, &test_mode_sel, 1 );
    cmd |= !flg_emi_init;
    if( !flg_emi_init ){
        flg_emi_init = 1;
    }    
	emi_process( cmd , test_chn_idx, test_mode_sel, mouse_status->pkt_addr, RF_POWER_8dBm );
}

