#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define CHIP_TYPE				CHIP_TYPE_8267



/////////////////// TEST FEATURE SELECTION /////////////////////////////////

//ble link layer test
#define	TEST_ADVERTISING_ONLY							1
#define TEST_SCANNING_ONLY								2
#define TEST_ADVERTISING_IN_CONN_SLAVE_ROLE				3
#define TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE        4
#define TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE	5


#define TEST_SDATA_LENGTH_EXTENSION                     6
#define TEST_MDATA_LENGTH_EXTENSION                     7

//power test
#define TEST_POWER_ADV									10



//smp test
#define TEST_SMP_PASSKEY_ENTRY							20



//other test
#define TEST_USER_BLT_SOFT_TIMER						30
#define TEST_WHITELIST									31
 //phy test
#define TEST_BLE_PHY									32

#define	TEST_EMI										40

//phy test
#define TEST_2M_PHY_CONNECTION                          41

#define INTERNAL_TEST									50



#define FEATURE_TEST_MODE								INTERNAL_TEST




#if (   FEATURE_TEST_MODE == TEST_SCANNING_ONLY || FEATURE_TEST_MODE == TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_SLAVE_ROLE || FEATURE_TEST_MODE==TEST_BLE_PHY \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION \
	 || FEATURE_TEST_MODE == TEST_MDATA_LENGTH_EXTENSION)
	#define BLE_PM_ENABLE								0
#else
	#define BLE_PM_ENABLE								1
#endif

#define USB_ADV_REPORT_TO_PC_ENABLE						1  //display adv report on pc

/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_NONE		0
#define HCI_USE_UART	1
#define HCI_USE_USB		2


#define 		PHYTEST_MODE_THROUGH_2_WIRE_UART		1   //Direct Test Mode through a 2-wire UART interface
#define 		PHYTEST_MODE_OVER_HCI_WITH_USB			2   //Direct Test Mode over HCI(UART hardware interface)
#define 		PHYTEST_MODE_OVER_HCI_WITH_UART			3   //Direct Test Mode over HCI(USB  hardware interface)

#if (FEATURE_TEST_MODE == TEST_BLE_PHY)
	#define BLE_PHYTEST_MODE					     PHYTEST_MODE_OVER_HCI_WITH_UART
	#define MCU_CORE_TYPE                            MCU_CORE_8266
#elif (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)
	#define BLT_SOFTWARE_TIMER_ENABLE      1
#elif (FEATURE_TEST_MODE == TEST_MDATA_LENGTH_EXTENSION) ///now just support 8266 dongle.

	#define MCU_CORE_TYPE                  MCU_CORE_8266

	#define BLE_HOST_SMP_ENABLE             0
	#define UI_BUTTON_ENABLE                1
	#define UI_LED_ENABLE                   1
	#define LL_MASTER_SINGLE_CONNECTION     1


  	  //8266 kma master dongle
	#define PD4_INPUT_ENABLE		1
	#define PD5_INPUT_ENABLE		1
	#define	SW1_GPIO				GPIO_PD5
	#define	SW2_GPIO				GPIO_PD4
	#define PULL_WAKEUP_SRC_PD4		PM_PIN_PULLUP_10K	//btn
	#define PULL_WAKEUP_SRC_PD5		PM_PIN_PULLUP_10K	//btn

	#define	 GPIO_LED_GREEN			GPIO_PC0
	#define	 GPIO_LED_RED			GPIO_PC4
	#define	 GPIO_LED_BLUE			GPIO_PC2
	#define	 GPIO_LED_WHITE			GPIO_PA1

	#define LED_ON_LEVAL 		1 		//gpio output high voltage to turn on led

	#define  PC0_INPUT_ENABLE		0
	#define	 PC0_OUTPUT_ENABLE		1
	#define  PC4_INPUT_ENABLE		0
	#define	 PC4_OUTPUT_ENABLE		1
	#define  PC2_INPUT_ENABLE		0
	#define  PC2_OUTPUT_ENABLE		1
	#define  PA1_INPUT_ENABLE		0
	#define	 PA1_OUTPUT_ENABLE		1
#elif (FEATURE_TEST_MODE == INTERNAL_TEST)

#define RC_BTN_ENABLE 					1
#define BLE_REMOTE_SECURITY_ENABLE 		1

//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	0   //proc fast scan when deepsleep back trigged by key press, in case key loss
#define KEYSCAN_IRQ_TRIGGER_MODE		1
#define LONG_PRESS_KEY_POWER_OPTIMIZE	1   //lower power when pressing key without release

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

#define			T_VK_CH_UP		0xd0
#define			T_VK_CH_DN		0xd1
//special key
#define		 	VOICE					0xc0
#define 		KEY_MODE_SWITCH			0xc1
#define		 	PHY_TEST				0xc2

#define		KB_MAP_NORMAL	{\
			VK_F,	CR_POWER,		VK_A,	VK_G,	CR_HOME,	 \
			VOICE,		VK_E,		VK_B,	CR_VOL_UP,	CR_VOL_DN,	 \
			VK_2,		VK_RIGHT,		VK_C,	VK_3,		VK_1,	 \
			VK_5,		VK_ENTER,		VK_D,	KEY_MODE_SWITCH,		VK_4,	 \
			VK_8,		VK_DOWN,		VK_UP,	VK_9,		VK_7,	 \
			VK_0,	CR_BACK,		VK_LEFT,	CR_VOL_MUTE,		CR_MENU,	}


#define  KB_DRIVE_PINS  {GPIO_PE0, GPIO_PD5, GPIO_PD7, GPIO_PE1, GPIO_PA1}
#define  KB_SCAN_PINS   {GPIO_PC7, GPIO_PA2, GPIO_PB1, GPIO_PA6, GPIO_PA5, GPIO_PD6}

#define	PE0_FUNC				AS_GPIO
#define	PD5_FUNC				AS_GPIO
#define	PD7_FUNC				AS_GPIO
#define	PE1_FUNC				AS_GPIO
#define	PA1_FUNC				AS_GPIO
//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PE0		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD5		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PE1		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA1		MATRIX_ROW_PULL
//drive pin open input to read gpio wakeup level
#define PE0_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define PD7_INPUT_ENABLE		1
#define PE1_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1


#define	PC7_FUNC				AS_GPIO
#define	PA2_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO
#define	PA6_FUNC				AS_GPIO
#define	PA5_FUNC				AS_GPIO
#define	PD6_FUNC				AS_GPIO
//scan  pin need 10K pullup
#define	PULL_WAKEUP_SRC_PC7		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA2		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB1		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA6		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD6		MATRIX_COL_PULL
//scan pin open input to read gpio level
#define PC7_INPUT_ENABLE		1
#define PA2_INPUT_ENABLE		1
#define PB1_INPUT_ENABLE		1
#define PA6_INPUT_ENABLE		1
#define PA5_INPUT_ENABLE		1
#define PD6_INPUT_ENABLE		1

#define		KB_MAP_NUM		KB_MAP_NORMAL
#define		KB_MAP_FN		KB_MAP_NORMAL


#endif


/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000
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
	DeviceInformation_PS_H,					 //UUID: 2800, 	VALUE: uuid 180A
	DeviceInformation_pnpID_CD_H,			 //UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_pnpID_DP_H,			 //UUID: 2A50,	VALUE: PnPtrs

  #if (FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION)
	//// SPP ////
	/**********************************************************************************************/
	SPP_PS_H, 							 //UUID: 2800, 	VALUE: telink spp service uuid

	//server to client
	SPP_SERVER_TO_CLIENT_CD_H,		     //UUID: 2803, 	VALUE:  			Prop: read | Notify
	SPP_SERVER_TO_CLIENT_DP_H,			 //UUID: telink spp s2c uuid,  VALUE: SppDataServer2ClientData
	SPP_SERVER_TO_CLIENT_CCB_H,			 //UUID: 2902, 	VALUE: SppDataServer2ClientDataCCC
	SPP_SERVER_TO_CLIENT_DESC_H,		 //UUID: 2901, 	VALUE: TelinkSPPS2CDescriptor

	//client to server
	SPP_CLIENT_TO_SERVER_CD_H,		     //UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	SPP_CLIENT_TO_SERVER_DP_H,			 //UUID: telink spp c2s uuid,  VALUE: SppDataClient2ServerData
	SPP_CLIENT_TO_SERVER_DESC_H,		 //UUID: 2901, 	VALUE: TelinkSPPC2SDescriptor

	//// Ota ////
	/**********************************************************************************************/
	OTA_PS_H, 							 //UUID: 2800, 	VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,					 //UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,					 //UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_OUT_DESC_H,					 //UUID: 2901, 	VALUE: otaName
  #else

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
	HID_BOOT_KB_REPORT_OUTPUT_DP_H,		    //UUID: 2A32, 	VALUE: bootKeyOutReport

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



	#if FEATURE_TEST_MODE == INTERNAL_TEST
		//// SPP ////
		/**********************************************************************************************/
		SPP_PS_H, 							 //UUID: 2800, 	VALUE: telink spp service uuid

		//server to client
		SPP_SERVER_TO_CLIENT_CD_H,		     //UUID: 2803, 	VALUE:  			Prop: read | Notify
		SPP_SERVER_TO_CLIENT_DP_H,			 //UUID: telink spp s2c uuid,  VALUE: SppDataServer2ClientData
		SPP_SERVER_TO_CLIENT_CCB_H,			 //UUID: 2902, 	VALUE: SppDataServer2ClientDataCCC
		SPP_SERVER_TO_CLIENT_DESC_H,		 //UUID: 2901, 	VALUE: TelinkSPPS2CDescriptor

		//client to server
		SPP_CLIENT_TO_SERVER_CD_H,		     //UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
		SPP_CLIENT_TO_SERVER_DP_H,			 //UUID: telink spp c2s uuid,  VALUE: SppDataClient2ServerData
		SPP_CLIENT_TO_SERVER_DESC_H,		 //UUID: 2901, 	VALUE: TelinkSPPC2SDescriptor
	#endif
   #endif

	ATT_END_H,

}ATT_HANDLE;













#define DEBUG_GPIO_ENABLE							0

#if(DEBUG_GPIO_ENABLE)
	#if (__PROJECT_8266_FEATURE_TEST__)
		//ch0-ch5: B0 A5 E5 F0 F1 E7
		//define debug GPIO here according to your hardware
		#define GPIO_CHN0							GPIO_PB0
		#define GPIO_CHN1							GPIO_PA5
		#define GPIO_CHN2							GPIO_PE5
		#define GPIO_CHN3							GPIO_PF0
		#define GPIO_CHN4							GPIO_PF1
		#define GPIO_CHN5							GPIO_PE7

		#define PB0_OUTPUT_ENABLE					1
		#define PA5_OUTPUT_ENABLE					1
		#define PE5_OUTPUT_ENABLE					1
		#define PF0_OUTPUT_ENABLE					1
		#define PF1_OUTPUT_ENABLE					1
		#define PE7_OUTPUT_ENABLE					1
	#else  //8261/8267/8269
		//define debug GPIO here according to your hardware
		//ch0-ch5: A7 A4 A3 E0 A1 A0
		#define GPIO_CHN0							GPIO_PA7
		#define GPIO_CHN1							GPIO_PA4
		#define GPIO_CHN2							GPIO_PA3
		#define GPIO_CHN3							GPIO_PE0
		#define GPIO_CHN4							GPIO_PA1
		#define GPIO_CHN5							GPIO_PA0

		#define PA7_OUTPUT_ENABLE					1
		#define PA4_OUTPUT_ENABLE					1
		#define PA3_OUTPUT_ENABLE					1
		#define PE0_OUTPUT_ENABLE					1
		#define PA1_OUTPUT_ENABLE					1
		#define PA0_OUTPUT_ENABLE					1
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
#endif  //end of DEBUG_GPIO_ENABLE





/////////////////// PRINT DEBUG INFO ///////////////////////
/* 826x module's pin simulate as a uart tx, Just for debugging */
#define PRINT_DEBUG_INFO                    1//open/close myprintf
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             		1000000 //1M baud rate,should Not bigger than 1M, when system clock is 16M.
#if	(__PROJECT_8261_FEATURE_TEST__ || __PROJECT_8267_FEATURE_TEST__ || __PROJECT_8269_FEATURE_TEST__)
#define DEBUG_INFO_TX_PIN           		GPIO_PC6//G0 for 8267/8269 EVK board(C1T80A30_V1.0)
//#define PC6_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PC6         		PM_PIN_PULLUP_1M
#else//__PROJECT_8266_FEATURE_TEST__
#define DEBUG_INFO_TX_PIN           		GPIO_PD3//G9 for 8266 EVK board(C1T53A20_V2.0)
//#define PD3_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PD3         		PM_PIN_PULLUP_1M
#endif
#endif



#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
