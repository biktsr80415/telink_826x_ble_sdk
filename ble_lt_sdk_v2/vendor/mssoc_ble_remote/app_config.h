#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_MSSOC_BLE_REMOTE__


#define CHIP_TYPE				MCU_CORE_MSSOC


/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_SECURITY_ENABLE      1
#define BLE_REMOTE_OTA_ENABLE			0
#define REMOTE_IR_ENABLE				0
#define BATT_CHECK_ENABLE       		0   //enable or disable battery voltage detection
#define BLE_AUDIO_ENABLE				1



////////////////////////// AUDIO CONFIG /////////////////////////////
#if (BLE_AUDIO_ENABLE)
	#define BLE_DMIC_ENABLE					0  //0: Amic   1: Dmic
	#define	ADPCM_PACKET_LEN				128
	#define TL_MIC_ADPCM_UNIT_SIZE			248

	#define	TL_MIC_32K_FIR_16K				0

	#if TL_MIC_32K_FIR_16K
		#define	TL_MIC_BUFFER_SIZE				1984
	#else
		#define	TL_MIC_BUFFER_SIZE				992
	#endif

	#define GPIO_AMIC_BIAS					GPIO_PC4

#endif




//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	0   //proc fast scan when deepsleep back trigged by key press, in case key loss
#define KEYSCAN_IRQ_TRIGGER_MODE		0
#define LONG_PRESS_KEY_POWER_OPTIMIZE	0   //lower power when pressing key without release

//stuck key
#define STUCK_KEY_PROCESS_ENABLE		0
#define STUCK_KEY_ENTERDEEP_TIME		60  //in s

//repeat key
#define KB_REPEAT_KEY_ENABLE			0
#define	KB_REPEAT_KEY_INTERVAL_MS		200
#define KB_REPEAT_KEY_NUM				1
#define KB_MAP_REPEAT					{VK_1, }


#define			CR_VOL_UP				0xf0  ////
#define			CR_VOL_DN				0xf1
#define			CR_VOL_MUTE				0xf2
#define			CR_CHN_UP				0xf3
#define			CR_CHN_DN				0xf4  ////
#define			CR_POWER				0xf5
#define			CR_SEARCH				0xf6
#define			CR_RECORD				0xf7
#define			CR_PLAY					0xf8  ////
#define			CR_PAUSE				0xf9
#define			CR_STOP					0xfa
#define			CR_FAST_BACKWARD		0xfb
#define			CR_FAST_FORWARD			0xfc  ////
#define			CR_HOME					0xfd
#define			CR_BACK					0xfe
#define			CR_MENU					0xff

//special key
#define		 	VOICE					0xc0
#define 		KEY_MODE_SWITCH			0xc1
#define		 	PHY_TEST				0xc2


#define 		IR_VK_0			0x00
#define 		IR_VK_1			0x01
#define 		IR_VK_2			0x02
#define			IR_VK_3			0x03
#define			IR_VK_4			0x04
#define 		IR_VK_5			0x05
#define 		IR_VK_6			0x06
#define 		IR_VK_7			0x07
#define 		IR_VK_8			0x08
#define 		IR_VK_9			0x09

#define 		IR_POWER		0x12
#define			IR_AUDIO_MUTE	0x0d
#define 		IR_NETFLIX		0x0f
#define			IR_BACK			0x0e
#define			IR_VOL_UP		0x0b
#define			IR_VOL_DN		0x0c
#define 		IR_NEXT			0x20
#define 		IR_PREV			0x21
#define			IR_MENU			0x23
#define 		IR_HOME			0x24
#define 		IR_OPER_KEY		0x2e
#define 		IR_INFO			0x2f
#define			IR_REWIND		0x32
#define 		IR_FAST_FOWARD	0x34
#define 		IR_PLAY_PAUSE	0x35
#define			IR_GUIDE		0x41
#define 		IR_UP			0x45
#define			IR_DN			0x44
#define 		IR_LEFT			0x42
#define 		IR_RIGHT		0x43
#define			IR_SEL			0x46
#define 		IR_RED_KEY		0x6b
#define 		IR_GREEN_KEY	0x6c
#define 		IR_YELLOW_KEY	0x6d
#define 		IR_BLUE_KEY		0x6e
#define 		IR_RECORD		0x72
#define 		IR_OPTION		0x73
#define 		IR_STOP			0x74
#define 		IR_SEARCH		0x75
#define 		IR_TEXT			0x76
#define 		IR_VOICE		0x77
#define 		IR_PAUSE		0x78

#define			T_VK_CH_UP		0xd0
#define			T_VK_CH_DN		0xd1


#define			GPIO_LED				GPIO_PC6

#if (REMOTE_IR_ENABLE)  //with IR keymap
		#define 		GPIO_IR_CONTROL			GPIO_PD0

		#define		KB_MAP_NORMAL	{\
						{0, 	1,		2,		3,		4,		5,		6,		}, \
						{7,		8,		9,		10,		11,		12,		13,		}, \
						{14,	15,		16,		17,		18,		19, 	KEY_MODE_SWITCH, }, \
						{21,	22,		23,		24,		25,		26,		27,		}, \
						{28,	29,		VOICE,	31,		32,		33,		34,		}, \
						{35,	36,		37,		38,		39,		40,		41,		}, \
						{42,	43,		44,		45,		46,		47,		48,		}, }

		#define		KB_MAP_BLE	{\
						VK_7,		VK_4,			VK_1,		VK_NONE,			CR_VOL_DN,	CR_VOL_UP,	CR_BACK	, \
						VK_LEFT,	VK_NONE,		VK_NONE,	VK_NONE,			CR_RECORD,	VK_NONE,	CR_POWER, \
						VK_RIGHT,	CR_HOME,		VK_NONE,	CR_FAST_FORWARD,	CR_STOP,	CR_SEARCH,	KEY_MODE_SWITCH , \
						VK_9,	 	VK_6,			VK_3,		VK_NONE,			CR_CHN_DN,	CR_CHN_UP,	CR_MENU , \
						VK_NONE,	CR_VOL_MUTE,	VOICE,		VK_DOWN,			VK_ENTER,	VK_UP,		VK_NONE , \
						VK_NONE,	VK_NONE,		VK_0,		VK_8,				VK_5,		VK_2,		VK_NONE , \
						CR_PAUSE,	CR_PLAY,		VK_NONE,	VK_NONE,			VK_NONE,	VK_NONE,	VK_NONE, }


		#define		KB_MAP_IR	{\
						IR_VK_7,	IR_VK_4,	IR_VK_1,	IR_RED_KEY,	IR_VOL_DN,	IR_VOL_UP,	IR_BACK	, \
						IR_LEFT,	VK_NONE,	VK_NONE,	IR_PREV,	VK_NONE,	VK_MMODE,	IR_POWER, \
						IR_RIGHT,	IR_HOME,	VK_NONE,	IR_NEXT,	IR_STOP,	IR_SEARCH,	KEY_MODE_SWITCH , \
						IR_VK_9,	IR_VK_6,	IR_VK_3,	IR_BLUE_KEY,VK_CH_DN,	VK_CH_UP,	IR_MENU , \
						IR_YELLOW_KEY,VK_NONE,	VK_NONE,	IR_DN,		IR_SEL,		IR_UP,		VK_NONE , \
						VK_NONE,	VK_NONE,	IR_VK_0,	IR_VK_8,	IR_VK_5,	IR_VK_2,	IR_GREEN_KEY , \
						IR_PAUSE,	VK_NONE,	VK_NONE,	VK_NONE,	VK_NONE,	VK_NONE,	VK_NONE, }
#else   //key map

		#define		KB_MAP_NORMAL	{\
						0x00,		VK_2,			0x00,		0x00,				0x00,	 \
						VOICE,		0x00,			0x00,		CR_VOL_UP,			CR_VOL_DN,	 \
						VK_2,		0x00,			0x00,		VK_3,				VK_1,	 \
						VK_5,		0x00,			0x00,		VK_6,				VK_4,	 \
						VK_8,		VK_4,			0x00,		VK_9,				VK_7,	 \
						VK_0,		0x00,			0x00,		0x00,				0x00,	 }

#endif  //end of REMOTE_IR_ENABLE


#define  KB_DRIVE_PINS  {GPIO_PD7, GPIO_PD0, GPIO_PD6, GPIO_PA0, GPIO_PA1}
#define  KB_SCAN_PINS   {GPIO_PC5, GPIO_PB3, GPIO_PB2, GPIO_PB1, GPIO_PA4, GPIO_PD1}

//drive pin as gpio
#define	PD7_FUNC				AS_GPIO
#define	PD0_FUNC				AS_GPIO
#define	PD6_FUNC				AS_GPIO
#define	PA0_FUNC				AS_GPIO
#define	PA1_FUNC				AS_GPIO
//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PD7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD0		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD6		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA0		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA1		MATRIX_ROW_PULL
//drive pin open input to read gpio wakeup level
#define PD7_INPUT_ENABLE		1
#define PD0_INPUT_ENABLE		1
#define PD6_INPUT_ENABLE		1
#define PA0_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1
//scan pin as gpio
#define	PC5_FUNC				AS_GPIO
#define	PB3_FUNC				AS_GPIO
#define	PB2_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO
#define	PA4_FUNC				AS_GPIO
#define	PD1_FUNC				AS_GPIO
//scan  pin need 10K pullup
#define	PULL_WAKEUP_SRC_PC5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB3		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB2		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB1		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA4		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD1		MATRIX_COL_PULL
//scan pin open input to read gpio level
#define PC5_INPUT_ENABLE		1
#define PB3_INPUT_ENABLE		1
#define PB2_INPUT_ENABLE		1
#define PB1_INPUT_ENABLE		1
#define PA4_INPUT_ENABLE		1
#define PD1_INPUT_ENABLE		1





#define		KB_MAP_NUM		KB_MAP_NORMAL
#define		KB_MAP_FN		KB_MAP_NORMAL

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms





///////////////////////////////////// ATT  HANDLER define ///////////////////////////////////////
typedef enum
{
	ATT_H_START = 0,


	//// Gap ////
	/**********************************************************************************************/
	GenericAccess_PS_H, 					//UUID: 2800, 	VALUE: uuid 1800
	GenericAccess_DeviceName_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	GenericAccess_DeviceName_DP_H,			//UUID: 2A00,   VALUE: device name
	GenericAccess_Appearance_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	GenericAccess_Appearance_DP_H,			//UUID: 2A01,	VALUE: appearance
	CONN_PARAM_CD_H,						//UUID: 2803, 	VALUE:  			Prop: Read
	CONN_PARAM_DP_H,						//UUID: 2A04,   VALUE: connParameter


	//// gatt ////
	/**********************************************************************************************/
	GenericAttribute_PS_H,					//UUID: 2800, 	VALUE: uuid 1801
	GenericAttribute_ServiceChanged_CD_H,	//UUID: 2803, 	VALUE:  			Prop: Indicate
	GenericAttribute_ServiceChanged_DP_H,   //UUID:	2A05,	VALUE: service change
	GenericAttribute_ServiceChanged_CCB_H,	//UUID: 2902,	VALUE: serviceChangeCCC


	//// device information ////
	/**********************************************************************************************/
	DeviceInformation_PS_H,					//UUID: 2800, 	VALUE: uuid 180A
	DeviceInformation_pnpID_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_pnpID_DP_H,			//UUID: 2A50,	VALUE: PnPtrs


	//// HID ////
	/**********************************************************************************************/
	HID_PS_H, 								//UUID: 2800, 	VALUE: uuid 1812

	//include
	HID_INCLUDE_H,							//UUID: 2802, 	VALUE: include

	//protocol
	HID_PROTOCOL_MODE_CD_H,					//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	HID_PROTOCOL_MODE_DP_H,					//UUID: 2A4E,	VALUE: protocolMode

	//boot keyboard input report
	HID_BOOT_KB_REPORT_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	HID_BOOT_KB_REPORT_INPUT_DP_H,			//UUID: 2A22, 	VALUE: bootKeyInReport
	HID_BOOT_KB_REPORT_INPUT_CCB_H,			//UUID: 2902, 	VALUE: bootKeyInReportCCC

	//boot keyboard output report
	HID_BOOT_KB_REPORT_OUTPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | write| write_without_rsp
	HID_BOOT_KB_REPORT_OUTPUT_CCB_H,		//UUID: 2A32, 	VALUE: bootKeyOutReport

	//consume report in
	HID_CONSUME_REPORT_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	HID_CONSUME_REPORT_INPUT_DP_H,			//UUID: 2A4D, 	VALUE: reportConsumerIn
	HID_CONSUME_REPORT_INPUT_CCB_H,			//UUID: 2902, 	VALUE: reportConsumerInCCC
	HID_CONSUME_REPORT_INPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_CONSUMER, TYPE_INPUT

	//keyboard report in
	HID_NORMAL_KB_REPORT_INPUT_CD_H,		//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	HID_NORMAL_KB_REPORT_INPUT_DP_H,		//UUID: 2A4D, 	VALUE: reportKeyIn
	HID_NORMAL_KB_REPORT_INPUT_CCB_H,		//UUID: 2902, 	VALUE: reportKeyInInCCC
	HID_NORMAL_KB_REPORT_INPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_KEYBOARD, TYPE_INPUT

	//keyboard report out
	HID_NORMAL_KB_REPORT_OUTPUT_CD_H,		//UUID: 2803, 	VALUE:  			Prop: Read | write| write_without_rsp
	HID_NORMAL_KB_REPORT_OUTPUT_DP_H,  		//UUID: 2A4D, 	VALUE: reportKeyOut
	HID_NORMAL_KB_REPORT_OUTPUT_REF_H, 		//UUID: 2908    VALUE: REPORT_ID_KEYBOARD, TYPE_OUTPUT

	// report map
	HID_REPORT_MAP_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read
	HID_REPORT_MAP_DP_H,					//UUID: 2A4B, 	VALUE: reportKeyIn
	HID_REPORT_MAP_EXT_REF_H,				//UUID: 2907 	VALUE: extService

	//hid information
	HID_INFORMATION_CD_H,					//UUID: 2803, 	VALUE:  			Prop: read
	HID_INFORMATION_DP_H,					//UUID: 2A4A 	VALUE: hidInformation

	//control point
	HID_CONTROL_POINT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: write_without_rsp
	HID_CONTROL_POINT_DP_H,					//UUID: 2A4C 	VALUE: controlPoint


	//// battery service ////
	/**********************************************************************************************/
	BATT_PS_H, 								//UUID: 2800, 	VALUE: uuid 180f
	BATT_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	BATT_LEVEL_INPUT_DP_H,					//UUID: 2A19 	VALUE: batVal
	BATT_LEVEL_INPUT_CCB_H,					//UUID: 2902, 	VALUE: batValCCC


	//// Ota ////
	/**********************************************************************************************/
	OTA_PS_H, 								//UUID: 2800, 	VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,						//UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: otaName



#if (BLE_AUDIO_ENABLE)
	//// Audio ////
	/**********************************************************************************************/
	AUDIO_PS_H, 							//UUID: 2800, 	VALUE: telink audio service uuid

	//mic
	AUDIO_MIC_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	AUDIO_MIC_INPUT_DP_H,					//UUID: telink mic uuid,  VALUE: micData
	AUDIO_MIC_INPUT_CCB_H,					//UUID: 2A19 	VALUE: micDataCCC
	AUDIO_MIC_INPUT_DESC_H,					//UUID: 2901, 	VALUE: micName

	//speaker
	AUDIO_SPEAKER_OUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: write_without_rsp
	AUDIO_SPEAKER_OUT_DP_H,					//UUID: telink speaker uuid,  VALUE: speakerData
	AUDIO_SPEAKEROUT_DESC_H,				//UUID: 2901, 	VALUE: speakerName
#endif



	ATT_END_H,

}ATT_HANDLE;



#define DEBUG_GPIO_ENABLE							0

#if(DEBUG_GPIO_ENABLE)
	//define debug GPIO here according to your hardware

#if 1
	#define GPIO_CHN0							GPIO_PD2
	#define GPIO_CHN1							GPIO_PD3
	#define GPIO_CHN2							GPIO_PD4
	#define GPIO_CHN3							GPIO_PD5
	#define GPIO_CHN4							GPIO_PB4
	#define GPIO_CHN5							GPIO_PB5
	#define GPIO_CHN6							GPIO_PB6
	#define GPIO_CHN7							GPIO_PB7

	#define PD2_OUTPUT_ENABLE					1
	#define PD3_OUTPUT_ENABLE					1
	#define PD4_OUTPUT_ENABLE					1
	#define PD5_OUTPUT_ENABLE					1
	#define PB4_OUTPUT_ENABLE					1
	#define PB5_OUTPUT_ENABLE					1
	#define PB6_OUTPUT_ENABLE					1
	#define PB7_OUTPUT_ENABLE					1
#else
	#define GPIO_CHN0							GPIO_PC0
	#define GPIO_CHN1							GPIO_PC1
	#define GPIO_CHN2							GPIO_PC2
	#define GPIO_CHN3							GPIO_PC3
	#define GPIO_CHN4							GPIO_PC4
	#define GPIO_CHN5							GPIO_PC5
	#define GPIO_CHN6							GPIO_PC6
	#define GPIO_CHN7							GPIO_PC7

	#define PC0_OUTPUT_ENABLE					1
	#define PC1_OUTPUT_ENABLE					1
	#define PC2_OUTPUT_ENABLE					1
	#define PC3_OUTPUT_ENABLE					1
	#define PC4_OUTPUT_ENABLE					1
	#define PC5_OUTPUT_ENABLE					1
	#define PC6_OUTPUT_ENABLE					1
	#define PC7_OUTPUT_ENABLE					1
#endif


	#define DBG_CHN0_LOW		gpio_write(GPIO_CHN0, 0)
	#define DBG_CHN0_HIGH		gpio_write(GPIO_CHN0, 1)
	#define DBG_CHN0_TOGGLE		gpio_toggle(GPIO_CHN0)
	#define DBG_CHN1_LOW		gpio_write(GPIO_CHN1, 0)
	#define DBG_CHN1_HIGH		gpio_write(GPIO_CHN1, 1)
	#define DBG_CHN1_TOGGLE		gpio_toggle(GPIO_CHN1)
	#define DBG_CHN2_LOW		gpio_write(GPIO_CHN2, 0)
	#define DBG_CHN2_HIGH		gpio_write(GPIO_CHN2, 1)
	#define DBG_CHN2_TOGGLE		gpio_toggle(GPIO_CHN2)
	#define DBG_CHN3_LOW		gpio_write(GPIO_CHN3, 0)
	#define DBG_CHN3_HIGH		gpio_write(GPIO_CHN3, 1)
	#define DBG_CHN3_TOGGLE		gpio_toggle(GPIO_CHN3)
	#define DBG_CHN4_LOW		gpio_write(GPIO_CHN4, 0)
	#define DBG_CHN4_HIGH		gpio_write(GPIO_CHN4, 1)
	#define DBG_CHN4_TOGGLE		gpio_toggle(GPIO_CHN4)
	#define DBG_CHN5_LOW		gpio_write(GPIO_CHN5, 0)
	#define DBG_CHN5_HIGH		gpio_write(GPIO_CHN5, 1)
	#define DBG_CHN5_TOGGLE		gpio_toggle(GPIO_CHN5)
	#define DBG_CHN6_LOW		gpio_write(GPIO_CHN6, 0)
	#define DBG_CHN6_HIGH		gpio_write(GPIO_CHN6, 1)
	#define DBG_CHN6_TOGGLE		gpio_toggle(GPIO_CHN6)
	#define DBG_CHN7_LOW		gpio_write(GPIO_CHN7, 0)
	#define DBG_CHN7_HIGH		gpio_write(GPIO_CHN7, 1)
	#define DBG_CHN7_TOGGLE		gpio_toggle(GPIO_CHN7)
#else
	#define DBG_CHN0_LOW
	#define DBG_CHN0_HIGH
	#define DBG_CHN0_TOGGLE
	#define DBG_CHN1_LOW
	#define DBG_CHN1_HIGH
	#define DBG_CHN1_TOGGLE
	#define DBG_CHN2_LOW
	#define DBG_CHN2_HIGH
	#define DBG_CHN2_TOGGLE
	#define DBG_CHN3_LOW
	#define DBG_CHN3_HIGH
	#define DBG_CHN3_TOGGLE
	#define DBG_CHN4_LOW
	#define DBG_CHN4_HIGH
	#define DBG_CHN4_TOGGLE
	#define DBG_CHN5_LOW
	#define DBG_CHN5_HIGH
	#define DBG_CHN5_TOGGLE
	#define DBG_CHN6_LOW
	#define DBG_CHN6_HIGH
	#define DBG_CHN6_TOGGLE
	#define DBG_CHN7_LOW
	#define DBG_CHN7_HIGH
	#define DBG_CHN7_TOGGLE
#endif  //end of DEBUG_GPIO_ENABLE










#endif  //end of __PROJECT_ MSSOC_BLE_REMOTE__



#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
