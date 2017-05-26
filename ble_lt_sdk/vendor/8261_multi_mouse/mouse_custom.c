#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"


#include "mouse_info.h"
#include "mouse.h"
#include "mouse_rf.h"
#include "mouse_button.h"
#include "mouse_wheel.h"
#include "mouse_sensor.h"
#include "mouse_rf.h"
#include "mouse_custom.h"

custom_cfg_t   *p_custom_cfg;


#if(TELINK_MOUSE_DEMO)
const u32 m_hw_def_dft[] = {
    M_HW_BTN_LEFT,
    M_HW_BTN_RIGHT,
    M_HW_BTN_MIDL,
    M_HW_BTN_BB,
    M_HW_BTN_FB,
    M_HW_BTN_CPI,
    M_HW_LED_CTL,    

    M_HW_GPIO_LEVEL_LEFT | (M_HW_GPIO_LEVEL_RIGHT<<8) | (M_HW_GPIO_LEVEL_MIDL<<16) | (M_HW_GPIO_LEVEL_BB<<24),
    M_HW_GPIO_LEVEL_FB   | (M_HW_GPIO_LEVEL_CPI<<8)   | (M_HW_GPIO_LEVEL_LED<<16)  | (M_HW_VBAT_CHN<<24),

    M_HW_CFG_1_DIR_0,
    M_HW_CFG_1_DIR_1,
    M_HW_CFG_2_DIR_0,
    M_HW_CFG_2_DIR_1,
    

    M_HW_WHEEL_Z0,
    M_HW_WHEEL_Z1,
    M_HW_SNS_DATA,    
    M_HW_SNS_CLK,
    M_HW_SNS_MOT_PIN,
};
#else
const u32 m_hw_def_dft[] = {

	GPIO_PA1,				//LEFT
	GPIO_PE3,				//RIGHT
	GPIO_PE2,				//MIDDLE
	GPIO_PA0,				//BOUND		SWITCH BLE / 2.4G MODE

	GPIO_PC4,				//LED_RED
	GPIO_PC5,				//LED_GREEN
	GPIO_PD2,				//LED_BLUE
	GPIO_PB7,				//V_BAT

	M_HW_GPIO_LEVEL_LEFT | (M_HW_GPIO_LEVEL_RIGHT<<8) | (M_HW_GPIO_LEVEL_MIDL<<16) | (0xff<<24),

	GPIO_PE1,				//WHEEL1
	GPIO_PE0,				//WHEEL2
	GPIO_PD3,				//WHEEL3

	GPIO_PA7,
	GPIO_PB4,
	GPIO_PA4,
	GPIO_PB6,
	GPIO_PB5,
};
#endif

#define SENSOR_IDX_CLOCK_3  0
#define SENSOR_IDX_CLOCK_6  1
#define SENSOR_IDX_CLOCK_9  2
#define SENSOR_IDX_CLOCK_12 3


const u8 custom_tbl_dir[4] = {
	SENSOR_DIRECTION_CLOCK_3,
	SENSOR_DIRECTION_CLOCK_6,
	SENSOR_DIRECTION_CLOCK_9,
	SENSOR_DIRECTION_CLOCK_12
};

const u8 custom_dir_idx_re_def[4] = {
    CUST_DIR_IDX_RE_0,
    CUST_DIR_IDX_RE_1,
    CUST_DIR_IDX_RE_2,
    CUST_DIR_IDX_RE_3 
};

custom_cpi_cfg_t mouse_cpi = {
    1,  //default cpi_idx
    3,  //segment: 3
    0,  //cpi_800
    2,  //cpi_1200
    3,  //cpi_1600
    1,  //optional
};


custom_cpi_cfg_t mouse_4_sgmt_cpi = {
		1,
		4,
		22,				// cpi_800 =  22*38 = 836, step = 38
		32,				// cpi_1200 = 32*38 = 1216
		42,				// cpi_1600 = 42*38 = 1596
		63,				// cpi_2400 = 63*38 = 2394
};


custom_btn_ui_t mouse_btn_ui = {
    ( FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT ),                       //pairing ui
    ( FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE ),  //emi ui
    ( FLAG_BUTTON_MIDDLE | FLAG_BUTTON_RIGHT ),                       //cpi-2-btn ui, should not be the same with pairing ui
    U8_MAX,                                                         //cpi-2-btn time
};


#if 1//MOUSE_CUSTOM_FULL_FUNCTION    
void mouse_custom_re_get( u8 *p_dst, u8 *p_src_0, u8 *p_src_1, u32 len ){
    int i;
    for( i = 0; i < len; i++ ){
        *p_dst = (*p_src_1 == U8_MAX) ? *p_src_0 : *p_src_1;
        *p_src_0++;
        *p_src_1++;
        *p_dst++;
    }
}

void mouse_custom_re_get_4( u32 *p_dst, u32 *p_src_0, u32 *p_src_1, u32 len ){
    int i;
    for( i = 0; i < len; i++ ){
        *p_dst = (*p_src_1 == U32_MAX) ? *p_src_0 : *p_src_1;
        *p_src_0++;
        *p_src_1++;
        *p_dst++;
    }
}
#else
static inline void mouse_custom_re_get( u8 *p_dst, u8 *p_src_0, u8 *p_src_1, u32 len ) {}
static inline void mouse_custom_re_get_4( u32 *p_dst, u32 *p_src_0, u32 *p_src_1, u32 len ) {}
#endif

//cfg_init	to£ºval_c   =        0    |   1    |   2    |      3
//--------------------------------------------------------------
//	0		cust_addr_0 =        8    |   a    |   4    |      6
//	1		cust_addr_1 =        7    |   9    |   3    |      5
//	2		cust_addr_2 =        6    |   8    |   2    |      4
//	3		cust_addr_3 =        5    |   7    |   1    |      3
//--------------------------------------------------------------
u8 custom_cfg_re_define( u8 cfg, u8* p_cfg_re_def ){
	u32 val_c = ( *(p_cfg_re_def + cfg) == U8_MAX ) ? cfg : \
		( ( ( cfg + *(p_cfg_re_def+cfg) ) >> 1 ) & 3 );
	return val_c;
}

//Pin-1 Pin-0  index    dir
//  1      1       0    clk_3
//  1      0       1    clk_6
//  0      1       2    clk_9
//  0      0       3    clk_12
u32 mouse_custom_cfg_r ( u32 *dir_r ){
    u32 dir_idx = 0;
	//Pin_1/Pin_0 internal pull up, and get input level

#if(0)
	gpio_setup_up_down_resistor( dir_r[0], 1 );
	gpio_setup_up_down_resistor( dir_r[1], 1 );
#else
    gpio_write(dir_r[0], 1);
    gpio_write(dir_r[1], 1);
#endif

	WaitUs(10);   //can change to mcu stall later, if needs
	//should disable output_en ?
	//gpio_set_output_en(dir_r[0], 0);
	//gpio_set_output_en(dir_r[1], 0);
	gpio_set_input_en(dir_r[0], 1);
	gpio_set_input_en(dir_r[1], 1);
	if( gpio_read(dir_r[0]) )
		dir_idx |= BIT(0);
	if( gpio_read(dir_r[1]) )
		dir_idx |= BIT(1);
	//Pin_1/Pin_0 recovery from direction detection setting
#if(0)
	gpio_setup_up_down_resistor( dir_r[0], PM_PIN_UP_DOWN_FLOAT );
	gpio_setup_up_down_resistor( dir_r[1], PM_PIN_UP_DOWN_FLOAT );
#else
	gpio_write(dir_r[0], 0);
	gpio_write(dir_r[1], 0);
#endif

	return  (3 - dir_idx);
}


#if(TELINK_MOUSE_DEMO)
u8 mouse_custom_sensor_dir_init (mouse_hw_t *pHW){
    u32 idx_dir;
	idx_dir = mouse_custom_cfg_r(pHW->cfg_1_r);
    if ( p_custom_cfg->sns_dir == U8_MAX ){		//default is "hardware config sensor direction"
        idx_dir = mouse_custom_cfg_r(pHW->cfg_1_r);
        idx_dir = custom_cfg_re_define( idx_dir, custom_dir_idx_re_def );
    }
    else{
        idx_dir = p_custom_cfg->sns_dir;		//can custom "soft config default dpi"
    }
    idx_dir = custom_cfg_re_define( idx_dir, p_custom_cfg->sns_dir_idx_re);
	return custom_tbl_dir[idx_dir&3];
}
#endif

extern rf_packet_pairing_t	pkt_mouse_pairing;

void mouse_custom_init ( mouse_status_t *pStatus ){

	for(int i = 0; i<3; i++){
		p_custom_cfg = (custom_cfg_t *)(MOUSE_DEVICE_ID_ADDRESS - i*0x100);
		if(p_custom_cfg->cap != 0){
			break;
		}
	}

    if(*(volatile u16 *)(MOUSE_DEVICE_ID_ADDRESS) != U16_MAX){
		rf_set_access_code0 (rf_access_code_16to32(*(volatile u16 *)(MOUSE_DEVICE_ID_ADDRESS)));
	}

	/*****************   set up did(device id)    ********************/

    u32 *mouse_did = (u32 *)(MOUSE_DEVICE_ID_ADDRESS);
    if(*mouse_did != U32_MAX){
    	pkt_mouse_pairing.did =  *(mouse_did + 1);
    	pkt_mouse.did = pkt_mouse_pairing.did;
    }

	/**************  Load hardware config, cpi config, button ui config, and led config *************/
#if 1
	mouse_custom_re_get_4( pStatus->hw_define, m_hw_def_dft, &p_custom_cfg->cust_ms_hw, sizeof(mouse_hw_t)>>2 );
    mouse_custom_re_get( &mouse_cpi, &mouse_cpi, &p_custom_cfg->sns_cpi, sizeof(custom_cpi_cfg_t) );
    mouse_custom_re_get( &mouse_btn_ui, &mouse_btn_ui, &p_custom_cfg->btn_ui, sizeof(custom_btn_ui_t) );

#else
	mouse_custom_re_get_4( pStatus->hw_define, m_hw_def_dft, m_hw_def_dft, sizeof(mouse_hw_t)>>2 );

    mouse_custom_re_get( &mouse_cpi, &mouse_cpi, &mouse_cpi, sizeof(custom_cpi_cfg_t) );

    mouse_custom_re_get( &mouse_btn_ui, &mouse_btn_ui, &mouse_btn_ui, sizeof(custom_btn_ui_t) );

#endif
	/**************  Get Wheel direction **************/
	pStatus->wheel_dir = p_custom_cfg->wheel_dir == U8_MAX ? 1 : -1;
    
	/*************	 Get sensor direction ************/

#if(0)			//sensor direction not be customed
    pStatus->sensor_dir = mouse_custom_sensor_dir_init ( pStatus->hw_define );
    if( p_custom_cfg->sns_hw_cpi == 0 ){		    //can custom "hardware config default dpi"
        pStatus->cpi = mouse_custom_cfg_r( pStatus->hw_define->cfg_2_r );
        pStatus->cpi = custom_cfg_re_define(pStatus->cpi, p_custom_cfg->sns_cpi_idx_re );
    }
    else{
        pStatus->cpi = mouse_cpi.sns_cpi_dft;	//default is "soft config default dpi"

    }

#else
    pStatus->sensor_dir = SENSOR_DIRECTION_CLOCK_3;
    pStatus->cpi = mouse_cpi.sns_cpi_dft;	//default is "soft config default dpi"

#endif
}

