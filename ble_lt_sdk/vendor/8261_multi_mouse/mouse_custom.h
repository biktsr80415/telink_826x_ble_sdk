
#pragma once
#include "../../proj/config/user_config.h"

#include "mouse.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


#ifndef MOUSE_CUSTOM_FULL_FUNCTION
#define MOUSE_CUSTOM_FULL_FUNCTION      1
#endif

#ifndef MOUSE_HW_CUS
#define MOUSE_HW_CUS      1
#endif

#ifndef MOUSE_SW_CUS
#define MOUSE_SW_CUS      1
#endif

#ifndef MOUSE_WKUP_SENSOR_SIM
#define MOUSE_WKUP_SENSOR_SIM      1
#endif

#ifndef MOUSE_BTNUI_CUS
#define MOUSE_BTNUI_CUS      1
#endif

#ifndef MOUSE_SENSOR_CPI_CUS
#define MOUSE_SENSOR_CPI_CUS	1
#endif


#define		MOUSE_DEVICE_ID_ADDRESS		0x1e000



#ifndef M_HW_BTN_LEFT
#define M_HW_BTN_LEFT   GPIO_PA1
#endif

#ifndef M_HW_BTN_RIGHT
#define M_HW_BTN_RIGHT  GPIO_PE3
#endif

#ifndef M_HW_BTN_MIDL
#define M_HW_BTN_MIDL   GPIO_PE2
#endif

#ifndef M_HW_BTN_FB
#define M_HW_BTN_FB     GPIO_PC4
#endif

#ifndef M_HW_BTN_BB
#define M_HW_BTN_BB    GPIO_PC5
#endif

#ifndef M_HW_BTN_CPI
#define M_HW_BTN_CPI    GPIO_PC3
#endif

#ifndef M_HW_LED_CTL
#define M_HW_LED_CTL    GPIO_PA0
#endif

#ifndef M_HW_GPIO_LEVEL_LEFT
#define M_HW_GPIO_LEVEL_LEFT   U8_MAX
#endif

#ifndef M_HW_GPIO_LEVEL_RIGHT
#define M_HW_GPIO_LEVEL_RIGHT   U8_MAX
#endif

#ifndef M_HW_GPIO_LEVEL_MIDL
#define M_HW_GPIO_LEVEL_MIDL   U8_MAX
#endif

#ifndef M_HW_GPIO_LEVEL_FB
#define M_HW_GPIO_LEVEL_FB   U8_MAX
#endif

#ifndef M_HW_GPIO_LEVEL_BB
#define M_HW_GPIO_LEVEL_BB   U8_MAX
#endif

#ifndef M_HW_GPIO_LEVEL_CPI
#define M_HW_GPIO_LEVEL_CPI   U8_MAX
#endif

#ifndef M_HW_GPIO_LEVEL_LED
#define M_HW_GPIO_LEVEL_LED   U8_MAX
#endif

#ifndef M_HW_VBAT_CHN
#define M_HW_VBAT_CHN    GPIO_PB7			// A/D Module
#endif

#ifndef M_HW_CFG_1_DIR_0
#define M_HW_CFG_1_DIR_0  0					//No used
#endif

#ifndef M_HW_CFG_1_DIR_1
#define M_HW_CFG_1_DIR_1  0					//No used
#endif

#ifndef M_HW_CFG_2_DIR_0
#define M_HW_CFG_2_DIR_0  0					//No used
#endif

#ifndef M_HW_CFG_2_DIR_1
#define M_HW_CFG_2_DIR_1  0					//No used
#endif


#ifndef M_HW_WHEEL_Z0
#define M_HW_WHEEL_Z0   GPIO_PE1
#endif

#ifndef M_HW_WHEEL_Z1
#define M_HW_WHEEL_Z1   GPIO_PE0
#endif

#ifndef M_HW_SNS_DATA
#define M_HW_SNS_DATA   GPIO_PB6
#endif

#ifndef M_HW_SNS_CLK
#define M_HW_SNS_CLK    GPIO_PB1
#endif

#ifndef M_HW_SNS_MOT_PIN
#define M_HW_SNS_MOT_PIN    GPIO_PC2
#endif


#ifndef CUST_DIR_IDX_RE_0
#define CUST_DIR_IDX_RE_0   0xff
#endif

#ifndef CUST_DIR_IDX_RE_1
#define CUST_DIR_IDX_RE_1   0xff
#endif

#ifndef CUST_DIR_IDX_RE_2
#define CUST_DIR_IDX_RE_2   0xff
#endif

#ifndef CUST_DIR_IDX_RE_3
#define CUST_DIR_IDX_RE_3   0xff
#endif

enum{
	E_LED_POWER_ON = 0,
    E_LED_PAIRING,
    E_LED_PAIRING_END,
    E_LED_BAT_LOW,
    E_LED_CPI,
    E_LED_RSVD,
};

typedef struct{
	u8	sns_cpi_dft;	
	u8  sns_cpi_sgmt;	
	u8  sns_cpi_tbl[4];	
} custom_cpi_cfg_t;

typedef struct{
    u8  mode_switch_ui;          //0x20:0xff for auto; else for paring/emi key define
    u8  emi_ui;             //0xff for auto; else for paring/emi key define
    u8  cpi_2_btn;
    u8  cpi_2_btn_time;
} custom_btn_ui_t;


typedef struct{
	u16	vid;			//vendor id
	u16 gid;
    
	u32 did;			//0x04~0x07 device id
	
	u8	cap;			//0x08 crystal CAP setting
    u8  tx_power;   	 //0x09
    u8  tx_power_paring; //0x0a
    u8  tx_power_emi;    //0x0b
    
	u8	wheel_dir;		//0x0c
    u8  no_ov_rd;
	u8	sns_dir;
	u8	sns_hw_cpi;		//0x0f

	u8	sns_dir_idx_re[4];      //0x10	
	u8	sns_cpi_idx_re[4];      //0x14
    
	custom_cpi_cfg_t sns_cpi;   //0x1c
    custom_btn_ui_t btn_ui;     //0x24

	mouse_hw_t cust_ms_hw; 	 //0x48 mouse hw_define
} custom_cfg_t;

extern custom_cfg_t    *p_custom_cfg;
extern custom_cpi_cfg_t mouse_cpi;
extern custom_btn_ui_t  mouse_btn_ui;

//extern led_cfg_t mouse_led_cfg[];
#define mouse_tx_power         ( (p_custom_cfg->tx_power == 0xff) ? RF_POWER_8dBm : p_custom_cfg->tx_power )
#define mouse_tx_paring_power  ( (p_custom_cfg->tx_power_paring == 0xff) ? RF_POWER_4dBm : p_custom_cfg->tx_power_paring )
#define mouse_tx_emi_power     ( (p_custom_cfg->tx_power_emi == 0xff) ? RF_POWER_8dBm : p_custom_cfg->tx_power_emi )

#define sensor_motion_detct         (p_custom_cfg->cust_ms_hw.sensor_int)  //motion_pin定制值为0 表示硬件上无motion脚
#define sensor_no_overflow_rd         (p_custom_cfg->no_ov_rd == 0)



void mouse_custom_init ( mouse_status_t *pStatus );

#if(MOUSE_LED_MODULE_EN & 0)
#if MOUSE_CUSTOM_FULL_FUNCTION
static inline led_cfg_t mouse_led_cpi_cfg_cust( u32 cpi ){
     if ( mouse_cust_led_cpi_mode_1 ){                
         led_cfg_t mouse_led_cpi_cfg = {0, 0, 0, 0};
         mouse_led_cpi_cfg.over_wrt = mouse_led_cfg[E_LED_CPI].over_wrt;
         mouse_led_cpi_cfg.repeat_count = 1 << cpi;
         mouse_led_cpi_cfg.on_time = mouse_led_cfg[E_LED_CPI].on_time >> cpi;
         mouse_led_cpi_cfg.off_time = mouse_led_cfg[E_LED_CPI].off_time >> cpi;
         return mouse_led_cpi_cfg;
     }   
     else{
         if( mouse_cust_led_cpi_mode_2 )
            mouse_led_cfg[E_LED_CPI].repeat_count = cpi + 1;                
         return mouse_led_cfg[E_LED_CPI];
    }
}

static inline led_cfg_t mouse_led_pairing_end_cfg_cust( u32 pairing_end ){
     if ( pairing_end == p_custom_cfg->led_pairing_end_mode ){         
         mouse_led_cfg[E_LED_PAIRING_END].on_time = mouse_led_cfg[E_LED_PAIRING_END].off_time;
     }
     return mouse_led_cfg[E_LED_PAIRING_END];
}

#else
static inline led_cfg_t mouse_led_cpi_cfg_cust( u32 cpi ) {}
static inline led_cfg_t mouse_led_pairing_end_cfg_cust( u32 pairing_end ) {}
#endif

#endif
