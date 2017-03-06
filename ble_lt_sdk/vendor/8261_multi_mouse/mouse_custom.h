
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
#define M_HW_VBAT_CHN    GPIO_PB7					//A/D Module
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
	u16	vid;		//vendor id
	u16 gid;
    
	u32 did;		//0x04~0x07 device id
	
	u8	cap;		//0x08 crystal CAP setting
	
    u8  tx_power;   	 //0x09
    u8  tx_power_paring; //0x0a
    u8  tx_power_emi;    //0x0b
    
	u8	wheel_dir;	//0x0c
	u8	sns_motion;
	u8	sns_dir;
	u8	sns_hw_cpi;
    
	u8	sns_dir_idx_re[4];      //0x10	
	u8	sns_cpi_idx_re[4];      //0x14
	
    //custom_slp_cfg_t slp_cfg;   //0x18
    u8  slp_mode;
    u8  slp_no_dongle;    
    u16  slp_tick;              //time to enter deep sleep mode, 1 seonds base
    
	custom_cpi_cfg_t sns_cpi;   //0x1c
    u8	ui_level;		        //0x12	
	u8	paring_only_pwon;       //0x23
    
    custom_btn_ui_t btn_ui;     //0x24

    u8  cpi_2_btn_ctrl;         //0x28
    u8	slp_btn_blink;          //0x29    0: sensor sleep, blinky every button, else: sensor sleep, no-blinky every button
    u8  led_pairing_end_mode;
    u8  led_cpi_mode;


    u8 bat_reuse_mb;            //0x44
    u8 tx_power_sync;        //0x45
    
    u8 no_ov_rd;            //0x46
    u8 c_rsvd;              //0x47

    mouse_led_cfg_t led_cfg[E_LED_RSVD+1];    //0x2c, 30 34 38 3c 40
	mouse_hw_t cust_ms_hw; 	 //0x48 mouse hw_define
#if 0
typedef struct {
    u32 button[6];      //left: 0x0, right:0x4, middle:0x8, FB:0xc, 
                        //BB:0x10,   DPI  :0x14
    u32 led_cntr;       //0x18
    u32 cfg_1_r[2];     //cfg_1_r_0: 0x1c,
                        //cfg_1_r_1: 0x20
                        
    u32 cfg_2_r[2];     //cfg_2_r_0: 0x24,
                        //cfg_2_r_1: 0x28

    u32 gpio_level;     //0x2c -bit[0-5], button pull up/down ; bit 6 led on level; bit 8, 9 sensor direction pull up/down

    u32 vbat_channel;	//0x30

    u32 wheel[2];		//wheel_0: 0x34, wheel_1: 0x38

    u32 sensor_data;	//0x3c
    u32 sensor_sclk;    //0x40
    u32 sensor_int;		//0x44
}mouse_hw_t;
#endif
} custom_cfg_t;

extern custom_cfg_t    *p_custom_cfg;
extern custom_cpi_cfg_t mouse_cpi;
extern custom_btn_ui_t  mouse_btn_ui;

//extern led_cfg_t mouse_led_cfg[];
#define mouse_tx_power         ( (p_custom_cfg->tx_power == 0xff) ? RF_POWER_8dBm : p_custom_cfg->tx_power )
#define mouse_tx_paring_power  ( (p_custom_cfg->tx_power_paring == 0xff) ? RF_POWER_4dBm : p_custom_cfg->tx_power_paring )
#define mouse_tx_emi_power     ( (p_custom_cfg->tx_power_emi == 0xff) ? RF_POWER_8dBm : p_custom_cfg->tx_power_emi )

#if	MOUSE_RF_CUSTOM
#define mouse_cust_tx_power         (p_custom_cfg->tx_power)
#define mouse_cust_tx_power_paring  ( (p_custom_cfg->tx_power_paring == 0xff) ? RF_POWER_4dBm : p_custom_cfg->tx_power_paring )
#define mouse_cust_tx_power_sync  ( (p_custom_cfg->tx_power_sync == 0xff) ? mouse_status->tx_power : p_custom_cfg->tx_power_sync )
#define mouse_cust_tx_power_emi         ( (p_custom_cfg->tx_power_emi == 0xff) ? RF_POWER_8dBm : p_custom_cfg->tx_power_emi )
#else
#define mouse_cust_tx_power_paring		RF_POWER_m28dBm
#define mouse_cust_tx_power_sync		mouse_status->tx_power
#define mouse_cust_tx_power_emi			RF_POWER_8dBm
#endif


#define sensor_motion_detct         (p_custom_cfg->cust_ms_hw.sensor_int)  //motion_pin定制值为0 表示硬件上无motion脚
#define sensor_no_overflow_rd         (p_custom_cfg->no_ov_rd == 0)

#define mouse_ui_level              ((p_custom_cfg->ui_level == 5) ? 5 : 0)
#define mouse_paring_only_pwon      (p_custom_cfg->paring_only_pwon)
#define mouse_cust_2_btn_cpi        (p_custom_cfg->cpi_2_btn_ctrl)
#define mouse_cust_paring_only_pwon (p_custom_cfg->paring_only_pwon)
#define mouse_cust_led_cpi_mode_2   (p_custom_cfg->led_cpi_mode == U8_MAX)
#define mouse_cust_led_cpi_mode_1   (p_custom_cfg->led_cpi_mode == 1)

#define CUST_MCU_SLEEP_EN 	        (p_custom_cfg->slp_mode & 1)
#define CUST_SENSOR_SLEEP_EN        (p_custom_cfg->slp_mode & 2)


#define QUICK_SLEEP_EN  	        (p_custom_cfg->slp_no_dongle)
#define SENSOR_ON_EVERY_BTN         (!p_custom_cfg->slp_btn_blink)
#define GET_HOST_ACCESS_CODE_FLASH_OTP  (p_custom_cfg->gid)


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
