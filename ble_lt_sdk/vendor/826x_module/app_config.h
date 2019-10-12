/********************************************************************************************************
 * @file     app_config.h
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if		(__PROJECT_8261_MODULE__)
	#define CHIP_TYPE				CHIP_TYPE_8261
#elif   (__PROJECT_8267_MODULE__)
	#define CHIP_TYPE				CHIP_TYPE_8267
#elif   (__PROJECT_8269_MODULE__)
	#define CHIP_TYPE				CHIP_TYPE_8269
#elif   (__PROJECT_8266_MODULE__)
	#define CHIP_TYPE				CHIP_TYPE_8266
#endif



/////////////////////HCI ACCESS OPTIONS/////////////////////
#define HCI_USE_UART	1
#define HCI_USE_USB		0
#define HCI_ACCESS		HCI_USE_UART
//#define HCI_ACCESS		HCI_USE_USB



/////////////////// MODULE /////////////////////////////////
#define BLE_MODULE_SECURITY_ENABLE          1
#define BLE_MODULE_PM_ENABLE				1
#define TELIK_SPP_SERVICE_ENABLE			1
#define BLE_MODULE_APPLICATION_ENABLE		1
#define BLE_MODULE_INDICATE_DATA_TO_MCU		1
#define BATT_CHECK_ENABLE       			0   //enable or disable battery voltage detection
#define SIG_PROC_ENABLE 					0   //To known if the Master accepted or rejected Connection_Parameters_Update or not!

/////////////////// DEBUG  /////////////////////////////////
//826x module's pin simulate as a uart tx, Just for debugging
#define PRINT_DEBUG_INFO               		0	//open/close myprintf





//8267 EVK board
/////////////////// button pin /////////////////////////////////
#if SMP_BUTTON_ENABLE
// KEY1		GPIO_PD2
// KEY2		GPIO_PC7
#define MAX_BTN_SIZE			2
#define BTN_VALID_LEVEL			0
//                              KEY1-GPIO_PD2   KEY2-GPIO_PC7
#define BTN_PINS				{GPIO_PD2, GPIO_PC7}
#define BTN_MAP					{1, 2}
#define	PULL_WAKEUP_SRC_PC7		PM_PIN_PULLUP_10K
#define	PULL_WAKEUP_SRC_PD2		PM_PIN_PULLUP_10K
#define PC7_INPUT_ENABLE		1
#define PD2_INPUT_ENABLE		1
#endif


#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_ALL
#define 	MY_DIRECT_ADV_TMIE					2000000  //us


/////////////////// led pin /////////////////////////////////
#if	(__PROJECT_8261_MODULE__ || __PROJECT_8267_MODULE__ || __PROJECT_8269_MODULE__)//8267/8269 EVK board(C1T80A30_V1.0)
#define RED_LED                             GPIO_PD5
#define WHITE_LED     						GPIO_PD6
#define GREEN_LED    		    			GPIO_PD7
#define BLUE_LED                			GPIO_PB4
#else//__PROJECT_8266_MODULE__ //8266 EVK board(C1T53A20_V2.0)
#define RED_LED                             GPIO_PC4
#define WHITE_LED     						GPIO_PA1
#define GREEN_LED    		    			GPIO_PC0
#define BLUE_LED                			GPIO_PC2
#endif
#define ON            						1
#define OFF           						0



//////////////////// PRINT DEBUG INFO ///////////////////////
#ifndef PRINT_DEBUG_INFO
#define PRINT_DEBUG_INFO                    0
#endif
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             		1000000 //1M baud rate,should Not bigger than 1M, when system clock is 16M.
#if	(__PROJECT_8261_MODULE__ || __PROJECT_8267_MODULE__ || __PROJECT_8269_MODULE__)
#define DEBUG_INFO_TX_PIN           		GPIO_PC6//G0 for 8267/8269 EVK board(C1T80A30_V1.0)
//#define PC6_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PC6         		PM_PIN_PULLUP_1M
#else//__PROJECT_8266_MODULE__
#define DEBUG_INFO_TX_PIN           		GPIO_PD3//G9 for 8266 EVK board(C1T53A20_V2.0)
//#define PD3_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PD3         		PM_PIN_PULLUP_1M
#endif
#endif



//////////////////////////// MODULE PM GPIO	/////////////////////////////////
#if	(__PROJECT_8261_MODULE__ || __PROJECT_8267_MODULE__ || __PROJECT_8269_MODULE__)
#define GPIO_WAKEUP_MODULE					GPIO_PB5   //P4N;mcu wakeup module //P4N for 8267/8269 EVK board(C1T80A30_V1.1)
#define	PB5_FUNC							AS_GPIO
#define PB5_INPUT_ENABLE					1
#define	PB5_OUTPUT_ENABLE					0
#define	PB5_DATA_OUT						0
#define GPIO_WAKEUP_MODULE_HIGH				gpio_setup_up_down_resistor(GPIO_PB5, PM_PIN_PULLUP_10K);
#define GPIO_WAKEUP_MODULE_LOW				gpio_setup_up_down_resistor(GPIO_PB5, PM_PIN_PULLDOWN_100K);

#define GPIO_WAKEUP_MCU						GPIO_PB1   //P2N;module wakeup mcu  //P2N for 8267/8269 EVK board(C1T80A30_V1.0)
#define	PB1_FUNC							AS_GPIO
#define PB1_INPUT_ENABLE					1
#define	PB1_OUTPUT_ENABLE					1
#define	PB1_DATA_OUT						0
#define GPIO_WAKEUP_MCU_HIGH				do{gpio_set_output_en(GPIO_PB1, 1); gpio_write(GPIO_PB1, 1);}while(0)
#define GPIO_WAKEUP_MCU_LOW					do{gpio_set_output_en(GPIO_PB1, 1); gpio_write(GPIO_PB1, 0);}while(0)
#define GPIO_WAKEUP_MCU_FLOAT				do{gpio_set_output_en(GPIO_PB1, 0); gpio_write(GPIO_PB1, 0);}while(0)
#else//__PROJECT_8266_MODULE__
#define GPIO_WAKEUP_MODULE					GPIO_PE4   //mcu wakeup module //G3 for 8266 EVK board(C1T53A20_V2.0)
#define	PE4_FUNC							AS_GPIO
#define PE4_INPUT_ENABLE					1
#define	PE4_OUTPUT_ENABLE					0
#define	PE4_DATA_OUT						0
#define GPIO_WAKEUP_MODULE_HIGH				gpio_setup_up_down_resistor(GPIO_PE4, PM_PIN_PULLUP_10K);
#define GPIO_WAKEUP_MODULE_LOW				gpio_setup_up_down_resistor(GPIO_PE4, PM_PIN_PULLDOWN_100K);

#define GPIO_WAKEUP_MCU						GPIO_PE5   //module wakeup mcu //G2 for 8266 EVK board(C1T53A20_V2.0)
#define	PE5_FUNC							AS_GPIO
#define PE5_INPUT_ENABLE					1
#define	PE5_OUTPUT_ENABLE					1
#define	PE5_DATA_OUT						0
#define GPIO_WAKEUP_MCU_HIGH				do{gpio_set_output_en(GPIO_PE5, 1); gpio_write(GPIO_PE5, 1);}while(0)
#define GPIO_WAKEUP_MCU_LOW					do{gpio_set_output_en(GPIO_PE5, 1); gpio_write(GPIO_PE5, 0);}while(0)
#define GPIO_WAKEUP_MCU_FLOAT				do{gpio_set_output_en(GPIO_PE5, 0); gpio_write(GPIO_PE5, 0);}while(0)
#endif



/////////////////////// POWER OPTIMIZATION  AT SUSPEND ///////////////////////
//notice that: all setting here aims at power optimization ,they depends on
//the actual hardware design.You should analyze your hardware board and then
//find out the io leakage

//shut down the input enable of some gpios, to lower io leakage at suspend state
//for example:  #define PA2_INPUT_ENABLE   0



///////////// avoid ADC module current leakage (when module on suspend status) //////////////////////////////
#define ADC_MODULE_CLOSED               BM_CLR(reg_adc_mod, FLD_ADC_CLK_EN)  // adc clk disable
#define ADC_MODULE_ENABLE               BM_SET(reg_adc_mod, FLD_ADC_CLK_EN)  // adc clk open



/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000



//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal



/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



#if	(__PROJECT_8261_MODULE__ || __PROJECT_8267_MODULE__ || __PROJECT_8269_MODULE__)
/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PB0_DATA_OUT					1

#if 0  //debug GPIO
	#define	PD0_FUNC							AS_GPIO
	#define PD0_INPUT_ENABLE					0
	#define	PD0_OUTPUT_ENABLE					1
	#define DBG_CHN0_LOW						gpio_write(GPIO_PD0, 0)
	#define DBG_CHN0_HIGH						gpio_write(GPIO_PD0, 1)
	#define DBG_CHN0_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PD0), GPIO_PD0 & 0xff);


	#define	PD1_FUNC							AS_GPIO
	#define PD1_INPUT_ENABLE					0
	#define	PD1_OUTPUT_ENABLE					1
	#define DBG_CHN1_LOW						gpio_write(GPIO_PD1, 0)
	#define DBG_CHN1_HIGH						gpio_write(GPIO_PD1, 1)
	#define DBG_CHN1_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PD1), GPIO_PD1 & 0xff);


	#define	PC5_FUNC							AS_GPIO
	#define PC5_INPUT_ENABLE					0
	#define	PC5_OUTPUT_ENABLE					1
	#define DBG_CHN2_LOW						gpio_write(GPIO_PC5, 0)
	#define DBG_CHN2_HIGH						gpio_write(GPIO_PC5, 1)
	#define DBG_CHN2_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PC5), GPIO_PC5 & 0xff);


	#define	PC6_FUNC							AS_GPIO
	#define PC6_INPUT_ENABLE					0
	#define	PC6_OUTPUT_ENABLE					1
	#define DBG_CHN3_LOW						gpio_write(GPIO_PC6, 0)
	#define DBG_CHN3_HIGH						gpio_write(GPIO_PC6, 1)
	#define DBG_CHN3_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PC6), GPIO_PC6 & 0xff);
#endif

#else//__PROJECT_8266_MODULE__


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


#if (TELIK_SPP_SERVICE_ENABLE)
	//// TELIK_SPP ////
	/**********************************************************************************************/
	SPP_PS_H, 								//UUID: 2800, 	VALUE: telink audio service uuid

	//Server2Client
	SPP_Server2Client_INPUT_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	SPP_Server2Client_INPUT_DP_H,			//UUID: TELIK_SPP_Server2Client uuid,  VALUE: SppDataServer2ClientData
	SPP_Server2Client_INPUT_CCB_H,			//UUID: 2902 	VALUE: SppDataServer2ClientDataCCC
	SPP_Server2Client_INPUT_DESC_H,			//UUID: 2901, 	VALUE: TelinkSPPS2CDescriptor

	//Client2Server
	SPP_Client2Server_OUT_CD_H,				//UUID: 2803, 	VALUE:  			Prop: Read | write_without_rsp
	SPP_Client2Server_OUT_DP_H,				//UUID: TELIK_SPP_Client2Server uuid,  VALUE: SppDataClient2ServerData
	SPP_Client2Server_DESC_H,				//UUID: 2901, 	VALUE: TelinkSPPC2SDescriptor
#endif


	//// Ota ////
	/**********************************************************************************************/
	OTA_PS_H, 								//UUID: 2800, 	VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,						//UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: otaName


	ATT_END_H,

}ATT_HANDLE;






/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

