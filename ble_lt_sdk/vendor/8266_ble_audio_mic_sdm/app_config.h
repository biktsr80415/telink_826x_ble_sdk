#if 1
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if		(__PROJECT_8266_AUDIO_MIC_SDM__)
	#define CHIP_TYPE				CHIP_TYPE_8266
#elif   (__PROJECT_8267_AUDIO_MIC_SDM__)
	#define CHIP_TYPE				CHIP_TYPE_8267
#elif   (__PROJECT_8269_AUDIO_MIC_SDM__)
	#define CHIP_TYPE				CHIP_TYPE_8269
#endif



/////////////////// MODULE /////////////////////////////////
#define MODULE_PM_ENABLE				   0
#define	MODULE_AUDIO_ENABLE				   1
#define	TELINK_AUDIO_SERVICE_ENABLE        1
#define BLE_DMIC_ENABLE                    1
/////////////////// PRINT DEBUG INFO ///////////////////////
/* 826x module's pin simulate as a uart tx, Just for debugging */
#define PRINT_DEBUG_INFO                    1//open/close myprintf
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             		1000000 //1M baud rate,should Not bigger than 1M, when system clock is 16M.
#if	( __PROJECT_8267_AUDIO_MIC_SDM__ || __PROJECT_8269_AUDIO_MIC_SDM__)
#define DEBUG_INFO_TX_PIN           		GPIO_PC6//G0 for 8267/8269 EVK board(C1T80A30_V1.0)
//#define PC6_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PC6         		PM_PIN_PULLUP_1M
#else//__PROJECT_8266_AUDIO_MIC_SDM__
#define DEBUG_INFO_TX_PIN           		GPIO_PD3//G9 for 8266 EVK board(C1T53A20_V2.0)
//#define PD3_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PD3         		PM_PIN_PULLUP_1M
#endif
#endif

//////////////////// AUDIO CONFIG //////////////////////////
//8266 Ӳ��MIC����Ϊ16K��ÿ��16K��Sample��ÿms 16��sample���������16bit(2bytes)
//15.5ms����15.5*16=248��sample����496bytes��ԭ������(pcm����)������496bytes��
//pcm������pcm to adpcmת����ѹ����1/4,ѹ���������Ϊ124bytes������4bytes��ͷ�����ݣ���128bytes��

#define ADPCM_PACKET_LEN 				128//ѹ�����͸�BLE master�豸������ѹ������С
#define TL_MIC_ADPCM_UNIT_SIZE 			248//15.5ms����248��sample
#define TL_SDM_BUFFER_SIZE 				1024//
#define TL_MIC_32K_FIR_16K 				0

#if TL_MIC_32K_FIR_16K//MIC Ӳ��ʹ��32K ��������FIR �������16K �ٶȽ����ݷ���buffer ʱ���������Ϊÿ��sample ���4 bytes ����
#define TL_MIC_BUFFER_SIZE 				1984//��ѹ��ʱֻ��ǰ����bytes ��Ϊһ��16 bit ��ԭ�����ݣ�������bytes ����������ʱbuffer size �ӱ���TL_MIC_BUFFER_SIZE Ϊ1984��
#else//16k����
#define TL_MIC_BUFFER_SIZE 				992//�����buffer ����Ϊ�ܹ��洢2 ��ѹ�����ݣ�Ҳ����496 ��sample����Ӧ992bytes
#endif

/////////////////// DMIC GPIO /////////////////////
//----Digital MIC------
//  DMICCLK(DCK)-PE1
//  DMICDI(DDI) -PE2
//---------SDM---------
//  SDMP        -PE4
//  SDMN        -PE5
#define  PE1_FUNC				AS_DMIC
#define  PE2_FUNC				AS_DMIC
#define  PE2_INPUT_ENABLE		1
#define	 PE1_OUTPUT_ENABLE		1

#define  PE4_FUNC				AS_SDM
#define  PE5_FUNC				AS_SDM
#define  PE4_OUTPUT_ENABLE		1
#define	 PE5_OUTPUT_ENABLE		1

/////////////////// button GPIO   /////////////////
#define	BUTTON_RECORD			GPIO_PD4 //SW2
#define	BUTTON_PLAY				GPIO_PD5 //SW1
#define PD4_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define PULL_WAKEUP_SRC_PD4		PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PD5		PM_PIN_PULLUP_1M	//btn

//////////////////// led gpio /////////////////////
#if	(__PROJECT_8267_AUDIO_MIC_SDM__ || __PROJECT_8269_AUDIO_MIC_SDM__ )//8267/8269 EVK board(C1T80A30_V1.0)
#define RED_LED                             GPIO_PD5
#define WHITE_LED     						GPIO_PD6
#define GREEN_LED    		    			GPIO_PD7
#define BLUE_LED                			GPIO_PB4
#define  PD5_INPUT_ENABLE					0
#define	 PD5_OUTPUT_ENABLE					1
#define  PD6_INPUT_ENABLE					0
#define	 PD6_OUTPUT_ENABLE					1
#define  PD7_INPUT_ENABLE					0
#define  PD7_OUTPUT_ENABLE					1
#define  PB4_INPUT_ENABLE					0
#define	 PB4_OUTPUT_ENABLE					1
#else//__PROJECT_8266_AUDIO_MIC_SDM__ //8266 EVK board(C1T53A20_V2.0)
#define RED_LED                             GPIO_PA5
#define WHITE_LED     						GPIO_PB0
#define GREEN_LED    		    			GPIO_PA1
#define BLUE_LED                			GPIO_PA6
#define  PA5_INPUT_ENABLE					0
#define	 PA5_OUTPUT_ENABLE					1
#define  PB0_INPUT_ENABLE					0
#define	 PB0_OUTPUT_ENABLE					1
#define  PA1_INPUT_ENABLE					0
#define  PA1_OUTPUT_ENABLE					1
#define  PA6_INPUT_ENABLE					0
#define	 PA6_OUTPUT_ENABLE					1
#endif
#define ON            						1
#define OFF           						0

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	32000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms


/////////////////// set default   ////////////////
#include "../common/default_config.h"


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
#endif
