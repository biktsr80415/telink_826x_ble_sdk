
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

//for fw version configure setting, please use FW_VER_CFG in fw_rel_ver.h 
#include "fw_rel_ver.h"

#define NV_CFG_DATA_OFFSET  (0x6000)
//build option: debug support via printer or not
#define BUILD_DEBUG_PRINTER    1   // 1 or 0; enable printer debug support or not
#define BUILD_TRIAL_PRODUCT     1   // 1 or 0; trial production build or not

//hw-prj selection: mutul exclusive
//project specific define, please make prj select macro are mutul exclusive
#define CODE_PRJ_KU1322     	1 //Chicony KU1322 prj
#define CODE_PRJ_TLK1          0  //telink SH, TP's white kbd hw
#define CODE_PRJ_TLK2         0  //for telink Montry's kbd layout


#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known 

#define __LOG_RT_ENABLE__		1
#define	__SWIRE2USB__			1

//the printer interface macro define, used by the C code
#if (BUILD_DEBUG_PRINTER != 0)
#define	USB_PRINTER		       1 //1: enable to easy debug, 0: disable for release
#else
#define	USB_PRINTER		       0 //1: enable to easy debug, 0: disable for release
#endif

//final product or trial prodct
#if (BUILD_TRIAL_PRODUCT != 0)
#define	PRODUCT_TRIAL		       1 //1: enable to easy debug, 0: disable for release
#else
#define	PRODUCT_TRIAL		       0 //1: enable to easy debug, 0: disable for release
#endif

#define CHIP_TYPE				CHIP_TYPE_3522		// 3520-48
#define APPLICATION_DONGLE		1			// or else APPLICATION_DEVICE
#define DONLGE_MONITOR_MODE		0			// by pass mode,  no sending pkts

#define	FLOW_NO_OS				1
#define	DID_AUTO_GEN			1
#ifndef OTP_PROGRAM
#define	OTP_PROGRAM				0
#endif

//each prj's keyboard hw mapping layout has a unique number to identify
#define   KBD_HW_MAP_TLK1              1
#define   KBD_HW_MAP_TLk2              2
#define   KBD_HW_MAP_TLkORI1              3
#define   KBD_HW_MAP_KU1322              11
#define   KBD_HW_MAP_KT1309              12
#define   KBD_HW_MAP_KTXXX                13

#if (CODE_PRJ_KU1322)  // mainly for hw layout and module difference

//////////// product  Infomation  //////////////////////////////
#define ID_VENDOR			0x04f2			// for report
// If ID_PRODUCT left undefined, it will default to be combination of ID_PRODUCT_BASE and the USB config USB_SPEAKER_ENABLE/USB_MIC_ENABLE...
#define ID_PRODUCT_BASE			0x1322
#define ID_PRODUCT				(ID_PRODUCT_BASE+((USB_PRINTER != 0)<<2)+(PRODUCT_TRIAL != 0))

#define STRING_VENDOR			L"Chicony"
#define STRING_PRODUCT			L"Fujitsu slim Keyboard with Touchpad"
#define STRING_SERIAL			L"TLSC3520"

#define IRQ_HOST_ENABLE			0
#define PS2_IF_SUPPORT			0
#define SAR_ADC_CODE_EN			0
#define KB_LED_ENABLE			0
#define KBD_HW_MAP_TYPE         KBD_HW_MAP_KU1322
#define PS2_SLAVE_PWR_NEED		1

//LED io cfg
#define CFG_LED_NUM             GPIO_PA0
#define CFG_LED_CAPS            GPIO_PA1
#define CFG_LED_SCR             GPIO_PA2
#define CFG_LED_SCR_PCBV2       GPIO_PE1
#define CFG_PCB_V_DETECT        GPIO_PE0

#define LVL_LED_NUM				0
#define LVL_LED_CAPS			0
#define LVL_LED_SCR				0
//ps2 io cfg
#define CFG_PS2_CLK             GPIO_PD7
#define CFG_PS2_DATA			GPIO_PD6
#define CFG_PS2_PWR				GPIO_PD2

//////////// TouchPad //////////////////////////////
#define TOUCHPAD_IRQSRC			GPIO_PD6
#define TOUCHPAD_IRQLVL			0
#define TOUCHPAD_RESET			GPIO_PD7
#define TOUCHPAD_CTRL			GPIO_PD2
#define TOUCHPAD_IF_SUPPORT		1

#elif (CODE_PRJ_TLK1)

//////////// product  Infomation  //////////////////////////////
#define ID_VENDOR				0x248a			// for report
// If ID_PRODUCT left undefined, it will default to be combination of ID_PRODUCT_BASE and the USB config USB_SPEAKER_ENABLE/USB_MIC_ENABLE...
#define ID_PRODUCT_BASE			0x0a07
#define ID_PRODUCT				(ID_PRODUCT_BASE+USB_PRINTER)

#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"Chicony Keyboard"
#define STRING_SERIAL			L"TLSC3520"

#define IRQ_HOST_ENABLE			0
#define PS2_IF_SUPPORT                       0
#define SAR_ADC_CODE_EN                   0
#define KB_LED_ENABLE                        1
#define KBD_HW_MAP_TYPE             KBD_HW_MAP_TLK1
#define PS2_SLAVE_PWR_NEED              0

//LED io cfg
#define CFG_LED_NUM             GPIO_PA0
#define CFG_LED_CAPS             GPIO_PA1
#define CFG_LED_SCR             GPIO_PA2
#define CFG_LED_SCR_PCBV2       GPIO_PE1

//ps2 io cfg
#define CFG_PS2_CLK             GPIO_PD6
#define CFG_PS2_DATA             GPIO_PD5

#endif

//make telink debug tool work
#if (BUILD_DEBUG_PRINTER)
#undef ID_VENDOR
#define ID_VENDOR			0x248a
#endif

#define PID_SEL_LOW       0x1323
#define PID_SEL_HIGH       0x1322

#if (PS2_IF_SUPPORT)
void ps2_irqcapture_handler(void);
#endif

#if(IRQ_HOST_ENABLE)
void host_irq_handler(void);
#endif

//////////// debug  /////////////////////////////////
#define __MOUSE_SIMU__  	0
#define __KEYBOARD_SIMU__  	0

/////////////////// MODULE /////////////////////////////////

#define MODULE_PM_ENABLE		0
#define MODULE_ETH_ENABLE		0
#define	MODULE_AUDIO_ENABLE		0
#define	MODULE_RF_ENABLE	0
#define	MODULE_USB_ENABLE		0
#define	MODULE_KEYBOARD_ENABLE	0

///////////////////  Hardware  //////////////////////////////

/////////////////// Clock  /////////////////////////////////
#if  0
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	30000000
#define USB_CLK_SRC_PLL            1   //disable this line if NOT using pll as USB sys clk src
#else
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_OSC	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	48000000
#endif

/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500		//  in ms
///////////////////  interrupt  //////////////////////////////

///////////////////  GPIO  /////////////////////////////////
//  only need to define those are not default
//  all gpios are default to be output disabled, input disabled, output to 0, output strength to 1

#define		PA2_IE		1
#define		PA2_OEN		1
#define		PA2_DO		0
#define		PA2_MODE	1

//#define     PE3_DO		0
//#define 	PE3_OEN     1
/// unused IO, to pass compilation
#define	GPIO_CK				0
#define	GPIO_CN				0
#define	GPIO_DI				0
#define	GPIO_DO				0

///////////////////  ADC  /////////////////////////////////


///////////////////  Keyboard //////////////////////////////
#define KB_PWR_TIME_INTERVAL		(2000*1000)


///////////////////  Audio  /////////////////////////////////
#define MIC_RESOLUTION_BIT		16
#define MIC_SAMPLE_RATE			16000
#define MIC_CHANNLE_COUNT		1
#define	MIC_ENOCDER_ENABLE		0

///////////////////  POWER MANAGEMENT  //////////////////

#define PM_ACTIVE_SUSPEND_WAKEUP_TIME  	300		// in ms
#define PM_USB_WAKEUP_TIME  			12		// in ms
#define PM_ENTER_DEEPSLEEP_TIME			600		// in MS

#define PM_SUSPEND_WAKEUP_GPIO_PIN  	0
#define PM_SUSPEND_WAKEUP_GPIO_LEVEL  	1
#define PM_SUSPEND_WAKEUP_FUNC_PIN 		0
#define PM_SUSPEND_WAKEUP_FUNC_LEVEL 	1
/*
the should be the combination of the followings:
DEEPSLEEP_WAKEUP_PIN_GPIO0 to DEEPSLEEP_WAKEUP_PIN_GPIO3
DEEPSLEEP_WAKEUP_PIN_ANA01 to DEEPSLEEP_WAKEUP_PIN_ANA12
*/
#define PM_DEEPSLEEP_WAKEUP_PIN 		0
#define PM_DEEPSLEEP_WAKEUP_LEVEL 		0

///////////////////  USB   /////////////////////////////////
#define	USB_PRINTER_ENABLE 		USB_PRINTER	//
#define	USB_SPEAKER_ENABLE 		0
#define	USB_MIC_ENABLE 			0
#define	USB_MOUSE_ENABLE 		1
#define	USB_KEYBOARD_ENABLE 	1
#define USB_HOTKEY_ENABLE		1
#define	USB_SOMATIC_ENABLE      0   //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#define USB_CUSTOM_HID_REPORT	1
#define IRQ_USB_EP0_ENABLE		MODULE_USB_ENABLE

////////////////  ethernet /////////////////	
#define ETH_PHY_RST_GPIO			GPIO_GP0
#define GPIO0_OUTPUT_ENABLE			1

/////////////////// set default   ////////////////

#include "../common/default_config.h"


#define PS2_RPT_M_INIT  0
#define PS2_RPT_M_REL  1     //relative mode
#define PS2_RPT_M_ABS  2    //abs mode

//user controlled nv ram section define:
//led control, 0xbc00
extern unsigned char nv_kbd_led_sta;
extern unsigned char nv_pcb_info;
extern unsigned short nv_rst_cnt; //0xbc02
extern unsigned short nv_pid_sel;


//ps2 debug control, 0xbc10
extern unsigned int ps2_dbg_ctl;
//ps2 debug control data, for cmd 0x36, addr fixed as 0xbc14
extern unsigned char dbg_mswr_data;

//for cmd 0x46, dbg_mswr_rsv1 is cmds wait time cfg; dbg_mswr_rsv2 for cmd len;
extern unsigned char dbg_mswr_rsv1; //0xbc15; as cmd wait cfg
extern unsigned char dbg_mswr_rsv2; //0xbc16; as cmd len;
extern unsigned char dbg_mswr_rsv3; //0xbc17; used as op mode discriptor

//0xbc18, consecutive ps2 wr cmd(0x46) data buffer;
extern unsigned char dbg_mswr_cmds[24]; 
extern unsigned char dbg_mswr_cmdres[64];  //0xbc30

extern unsigned char nv_ps2_raw_data[8]; //0xbc70
extern unsigned short nv_ps2_raw_flag;     //0xbc78
extern unsigned short nv_ps2_rbuf_cnt;     //0xbc7a
extern unsigned short nv_ps2_rbuf_idx;     //0xbc7c
extern unsigned int nv_ps2_rbuf[128];


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

