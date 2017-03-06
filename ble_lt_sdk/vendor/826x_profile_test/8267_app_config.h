#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_826X_PROFILE_TEST__


#define CHIP_TYPE				CHIP_TYPE_8267


/////////////////// MODULE /////////////////////////////////
#define PM_ENABLE				            0
#define TELIK_SPP_SERVICE_ENABLE			1
#define BLE_MODULE_APPLICATION_ENABLE		1

/////////////////////for test by tuyf 16-9-01//////////////////////////////////
#define RED_LED                 GPIO_PD5
#define GREEN_LED     			GPIO_PD6
#define WHITE_LED    		    GPIO_PD7
#define BLUE_LED                GPIO_PB4

#define ON            1
#define OFF           0

//key
#define KEY1                    GPIO_PD2
#define KEY2                    GPIO_PC7

#define PD2_FUNC 				AS_GPIO
#define PD2_INPUT_ENABLE		1
#define PULL_WAKEUP_SRC_PD2		PM_PIN_PULLUP_1M

#define PC7_FUNC 				AS_GPIO
#define PC7_INPUT_ENABLE		1
#define PULL_WAKEUP_SRC_PC7		PM_PIN_PULLUP_1M

////////////////////////////////////////////////////////



//////////////////////////////// service ATT table test /////////////////////////////////////
#define BAS_DIS_SCPS_TPS 					    0//BS_DIS_SCPS_TPS
#define PXP            							0//PXP profile
#define BLS            							0//BLS
#define BLP            							0//BLP
#define HRS            							0//HRS
#define HRP            							0//HRP
#define WSS            							0//WSS
#define WSP            							0//WSP
#define CTS            							1//CTS

#include "../../proj/common/types.h"
//time info packet structure, should transfer to time service later
typedef struct {
	u16 year;
	u8 month;
	u8 day;
	u8 hours;
	u8 minutes;
	u8 seconds;
} time_packet;

//weight measurement data structure
typedef struct {
	u8 wmFlag;
	u16 wmWeight;
	time_packet timeInf;
	u8 userID;
	u16 wmBMI;
	u16 wmHeight;
} weight_measure_packet;

//Blood Pressure Measurement data structure
typedef struct {
	u8 bpmFlag;
	short bpmSys;        //Systolic
	short bpmSysDiastoli;//Diastoli
	short bpmMAR;        //Mean Arterial Pressure
	time_packet timeInf; //weight scale defined
	short pulseRate;     //pulse Rate
	u8 userID;
	u16 measurementStatus;
} blood_pressure_measure_packet;

//heart rate measurement data structure
typedef struct {
	u8 hrmFlag;
	u16 hrVal;        //Heart Rate Measurement Value
	u16 eryexd;//Energy Expended
	u16 rr_interval; //RR-Interval
} heart_rate_measurement_packet;

/////////////////////// POWER OPTIMIZATION  AT SUSPEND ///////////////////////
//notice that: all setting here aims at power optimization ,they depends on
//the actual hardware design.You should analyze your hardware board and then
//find out the io leakage

//shut down the input enable of some gpios, to lower io leakage at suspend state
//for example:  #define PA2_INPUT_ENABLE   0



/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PB0_DATA_OUT					1

#endif
/////////////////// set default   ////////////////
#define	PULL_WAKEUP_SRC_PB1		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB2		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB3		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB6		PM_PIN_PULLDOWN_100K
#define PB1_INPUT_ENABLE		1
#define PB2_INPUT_ENABLE		1
#define PB3_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1



#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
