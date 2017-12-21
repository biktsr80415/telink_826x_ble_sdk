#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#if ( __826x_PTS_TEST__ )

#define CHIP_TYPE										CHIP_TYPE_8267

#define BLE_PM_ENABLE									0

#define USB_ADV_REPORT_TO_PC_ENABLE						0//1  //display adv report on pc

/////////////////////PTS TEST CASES///////////////////////////////////////
#define CONN_DCON_BV01		1
#define CONN_UCON_BV01		2
#define CONN_UCON_BV02		3

#define CONN_CPUP_BV01		4
#define CONN_CPUP_BV02		5
#define CONN_CPUP_BV03		6

#define CONN_TERM_BV01		7

#define L2CAP_LE_CPU_BV01	8
#define L2CAP_LE_CPU_BI02	9

#define GATT_SR_GAN_NV01	10
#define GATT_SR_GAI_NV01	11

#define GAP_BROB_BCST_BV01	12
#define GAP_BROB_BCST_BV02	13

#define GAP_BROB_BCST_BV03	14

#define GAP_DISC_NONN_BV01	15
#define GAP_DISC_NONN_BV02	16
#define GAP_DISC_LIMM_BV03	17
#define GAP_DISC_LIMM_BV04	18
#define GAP_DISC_GENM_BV03	19
#define GAP_DISC_GENM_BV04	20
#define GAP_CONN_NCON_BV01	21
#define GAP_CONN_NCON_BV02	22
#define GAP_CONN_NCON_BV03	23

#define GATT_SR_GAS_BV01	24

//smp concerned
#define SM_MAS_PROT_BV01C   25
#define SM_SLA_PROT_BV02C   26
#define SM_SLA_KDU_BV01C    27
#define SM_MAS_KDU_BV06C    28
#define SM_MAS_SIP_BV02C    29
#define GAP_BOND_BON_BV01C  30
#define GAP_BOND_BON_BV03C  31
#define GAP_BOND_NBON_BV01C 32
#define GAP_BOND_NBON_BV02C 33
#define GAP_BOND_NBON_BV03C 34


#define PTS_TEST_MODE						GAP_BROB_BCST_BV01


////////////////// Ble Role ////////////////////////////////
#define	 BLE_ROLE_AS_SLAVE					1
#define	 BLE_ROLE_AS_MASTER					2

#define  SMP_PENDING						1   //security management

#if (PTS_TEST_MODE == SM_MAS_PROT_BV01C || \
	 PTS_TEST_MODE == SM_MAS_KDU_BV06C || \
	 PTS_TEST_MODE == SM_MAS_SIP_BV02C || \
	 PTS_TEST_MODE == GAP_BOND_NBON_BV01C  || \
	 PTS_TEST_MODE == GAP_BOND_NBON_BV02C)
	#define  CONFIG_BLE_ROLE			        BLE_ROLE_AS_MASTER
	#define  LL_MASTER_SINGLE_CONNECTION        1
	#define  FLASH_ADR_PARING                   0x78000
    #define  UI_BUTTON_ENABLE					1
#else
	#define  CONFIG_BLE_ROLE                    BLE_ROLE_AS_SLAVE

#endif

//PTS test mode belong to SM/BOND test cases, open the micro: SMP_BLE_CERT_TEST .
#if (PTS_TEST_MODE >= SM_MAS_PROT_BV01C && PTS_TEST_MODE <= GAP_BOND_NBON_BV03C)
	#define  SMP_BLE_CERT_TEST					1
#else
    #define  SMP_BLE_CERT_TEST					0
#endif


///////////////////// UI gpio /////////////////////////////
#if (UI_BUTTON_ENABLE)//8267 EVK
#define PD2_INPUT_ENABLE		1
#define PC7_INPUT_ENABLE		1
#define	SW2_GPIO				GPIO_PC7
#define	SW1_GPIO				GPIO_PD2
//PC7 1m pullup not very stable, so we use 10k pullup
//#define PULL_WAKEUP_SRC_PC7     PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PC7     PM_PIN_PULLUP_10K	//btn
#define PULL_WAKEUP_SRC_PD2     PM_PIN_PULLUP_1M  	//btn
#endif




/////////////////// DEBUG  /////////////////////////////////
//826x's pin simulate as a uart tx, Just for debugging
#define PRINT_DEBUG_INFO               		1	//open/close myprintf



/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  					CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  				(CONFIG_BLE_ROLE * 16000000)
//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE						XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE				0
#define WATCHDOG_INIT_TIMEOUT				500  //ms





//////////////////// PRINT DEBUG INFO ///////////////////////
#ifndef PRINT_DEBUG_INFO
#define PRINT_DEBUG_INFO                    0
#endif
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             		(CONFIG_BLE_ROLE * 1000000) //1M baud rate,should Not bigger than 1M, when system clock is 16M.
#if	(__826x_PTS_TEST__)
#define DEBUG_INFO_TX_PIN           		GPIO_PC6//G0 for 8267/8269 EVK board(C1T80A30_V1.0)
//#define PC6_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PC6         		PM_PIN_PULLUP_1M
#else//__PROJECT_8266__
#define DEBUG_INFO_TX_PIN           		GPIO_PD3//G9 for 8266 EVK board(C1T53A20_V2.0)
//#define PD3_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PD3         		PM_PIN_PULLUP_1M
#endif
#endif




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
	#define DBG_CHN0_HIGH		gpio_write(GPIO_CHN0, 0)
	#define DBG_CHN0_TOGGLE		gpio_toggle(GPIO_CHN0)
	#define DBG_CHN1_LOW		gpio_write(GPIO_CHN1, 0)
	#define DBG_CHN1_HIGH		gpio_write(GPIO_CHN1, 0)
	#define DBG_CHN1_TOGGLE		gpio_toggle(GPIO_CHN1)
	#define DBG_CHN2_LOW		gpio_write(GPIO_CHN2, 0)
	#define DBG_CHN2_HIGH		gpio_write(GPIO_CHN2, 0)
	#define DBG_CHN2_TOGGLE		gpio_toggle(GPIO_CHN2)
	#define DBG_CHN3_LOW		gpio_write(GPIO_CHN3, 0)
	#define DBG_CHN3_HIGH		gpio_write(GPIO_CHN3, 0)
	#define DBG_CHN3_TOGGLE		gpio_toggle(GPIO_CHN3)
	#define DBG_CHN4_LOW		gpio_write(GPIO_CHN4, 0)
	#define DBG_CHN4_HIGH		gpio_write(GPIO_CHN4, 0)
	#define DBG_CHN4_TOGGLE		gpio_toggle(GPIO_CHN4)
	#define DBG_CHN5_LOW		gpio_write(GPIO_CHN5, 0)
	#define DBG_CHN5_HIGH		gpio_write(GPIO_CHN5, 0)
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



#include "../common/default_config.h"

#endif//end of __826x_PTS_TEST__
/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
