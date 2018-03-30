/**
  ******************************************************************************
  * @File: 		 adc_5316.h
  * @CreateDate: 2017-12-22
  * @Author:     qiu.gao@telink-semi.com
  * @Copyright:  (C)2017-2020 telink-semi Ltd.co
  * @Brief:
  ******************************************************************************
  */

#if(MCU_CORE_TYPE == MCU_CORE_5316)

#ifndef __ADC_5316_H
#define __ADC_5316_H

#include "../mcu/register.h"
#include "../common/compatibility.h"
#include "../common/utility.h"

#ifndef ENABLE
	#define ENABLE       1
#endif

#ifndef DISABLE
	#define DISABLE      0
#endif

#define ADC_BATTERY_CHECK_PIN    GPIO_PA7

/* ADC analog register base address define. */
#define ADC_BASE_ADDR               0x80

/* ADC analog register define. */
#define ADC_VREF_ADDR               (ADC_BASE_ADDR + 103)//M[5:4] R[3:2] L[1:0]
enum
{
	ADC_VREF_L_Pos = BIT_RNG(0,1),
	ADC_VREF_R_Pos = BIT_RNG(2,3),
	ADC_VREF_M_Pos = BIT_RNG(4,5),
};

#define ADC_INPUT_CHANNEL_M_ADDR    (ADC_BASE_ADDR + 104)//M_Postive[7:4]  M_Negative[3:0]
#define ADC_INPUT_CHANNEL_L_ADDR    (ADC_BASE_ADDR + 105)//L_Postive[7:4]  L_Negative[3:0]
#define ADC_INPUT_CHANNEL_R_ADDR    (ADC_BASE_ADDR + 106)//R_Postive[7:4]  R_Negative[3:0]
enum//Use for register ADC_BASE_ADDR + 104/105/106
{
	ADC_INPUT_CHANNEL_N_Pos = BIT_RNG(0,3),
	ADC_INPUT_CHANNEL_P_Pos = BIT_RNG(4,7),
};

#define ADC_RES_L_OR_R_ADDR          (ADC_BASE_ADDR + 107)//R[5:4] L[1:0]
#define ADC_RES_M_ADDR		         (ADC_BASE_ADDR + 108)//M[1:0]
enum//Use for register ADC_BASE_ADDR + 107/108
{
	ADC_RES_L_Pos = BIT_RNG(0,1),
	ADC_RES_R_Pos = BIT_RNG(4,5),
	ADC_RES_M_Pos = BIT_RNG(0,1),
};

#define ADC_INPUT_MODE_ADDR           (ADC_BASE_ADDR + 108)//M[6] R[5] L[4]
enum
{
	ADC_INPUT_MODE_L_Pos = BIT(4),
	ADC_INPUT_MODE_R_Pos = BIT(5),
	ADC_INPUT_MODE_M_Pos = BIT(6),
};

#define ADC_STABLE_TIME_L_OR_R_ADDR    (ADC_BASE_ADDR + 109)//R[7:4] L[3:0]
enum
{
	ADC_STABLE_TIME_L_Pos = BIT_RNG(0,3),
	ADC_STABLE_TIME_R_Pos = BIT_RNG(4,7),
};

#define ADC_STABLE_TIME_M_ADDR        (ADC_BASE_ADDR + 110)//M[3:0]
enum
{
	ADC_STABLE_TIME_M_Pos    = BIT_RNG(0,3),
	ADC_STABLE_TIME_RSSI_Pos = BIT_RNG(4,7),
};

#define ADC_CAPTURE_TIME_LEN_M_OR_RNS_ADDRL     (ADC_BASE_ADDR + 111)
#define ADC_CAPTURE_TIME_LEN_M_OR_RNS_ADDRH 	(ADC_BASE_ADDR + 113)//[7:6]
#define ADC_CAPTURE_TIME_LEN_L_OR_R_ADDRL       (ADC_BASE_ADDR + 112)
#define ADC_CAPTURE_TIME_LEN_L_OR_R_ADDRH       (ADC_BASE_ADDR + 113)//[5:4]
#define ADC_SET_STATE_TIME_LEN_ADDR             (ADC_BASE_ADDR + 113)//[3:0]
enum//Use for register ADC_BASE_ADDR+113
{
	ADC_SET_STATE_TIME_LEN_Pos        = BIT_RNG(0,3),
	ADC_CAPTURE_TIME_LEN_L_OR_R_Pos   = BIT_RNG(4,5),
	ADC_CAPTURE_TIME_LEN_M_OR_RNS_Pos = BIT_RNG(6,7),
};

#define ADC_STATE_NUM_ADDR       	  (ADC_BASE_ADDR + 114)//state num[6:4]
enum
{
	ADC_STATE_NUM_Pos = BIT_RNG(4,6),
};

#define ADC_ENABLE_ADDR          	  (ADC_BASE_ADDR + 114)//RNS[3] M[2] R[1] L[0]
enum
{
	ADC_ENABLE_L_Pos   = BIT(0),
	ADC_ENABLE_R_Pos   = BIT(1),
	ADC_ENABLE_M_Pos   = BIT(2),
	ADC_ENABLE_RNS_Pos = BIT(3),
};

#define ADC_CLK_DIV_ADDR   		      (ADC_BASE_ADDR + 116)//[2:0]
enum
{
	ADC_CLK_DIV_Pos = BIT_RNG(0,2),
};

#define ADC_STATE_MACHINE_RST_EN_ADDR (ADC_BASE_ADDR + 116)//[7]
#define ADC_RND_DATA_L_ADDR 		  (ADC_BASE_ADDR + 117)//[7:0]
#define ADC_RND_DATA_H_ADDR 		  (ADC_BASE_ADDR + 118)//[15:8]
#define ADC_DATA_L_ADDR               (ADC_BASE_ADDR + 119)//[7:0]
#define ADC_DATA_H_ADDR               (ADC_BASE_ADDR + 120)//[15:8]
enum//Use for register ADC_BASE_ADDR + 120
{
	ADC_DATA_VALID = BIT(7),
};

#define ADC_VBAT_DIV_ADDR	          (ADC_BASE_ADDR + 121)//Voltage divider:M[5:4] R[3:2] L[1:0]
enum
{
	ADC_VBAT_DIV_Pos = BIT_RNG(2,3),
};

#define ADC_INPUT_VOL_PRESCALE_ADDR   (ADC_BASE_ADDR + 122)//[7:6]
enum
{
	ADC_INPUT_VOL_Prescale_Pos = BIT_RNG(6,7),
};

#define ADC_POWER_ADDR         		  (ADC_BASE_ADDR + 124)//ADC power down/up[5]
enum
{
	ADC_POWER_Pos = BIT(5),
};

#define ADC_RNS_CTRL_ADDR             (ADC_BASE_ADDR + 126)
enum
{
	ADC_RNS_CTRL_RNG_SRC_Pos     = BIT(0),
	ADC_RNS_CTRL_RNG_SRC_SEL_Pos = BIT_RNG(1,2),
};

/* ADC module enum type define. */
typedef enum
{
	ADC_Module_L   = 0x01,
	ADC_Module_R   = 0x02,
	ADC_Module_M   = 0x04,
	ADC_Module_RNS = 0x08,
}eADC_ModuleTypeDef;

/* ADC reference voltage enum type define. */
typedef enum
{
	ADC_Vref_0p6 = 0x00,//Vref = 0.6v
	ADC_Vref_0p9 = 0x01,//Vref = 0.9v
	ADC_Vref_1p2 = 0x02,//Vref = 1.2v
	ADC_Vref_VBat_Div_Off = 0x03,//Vref = VBat/1
	ADC_Vref_VBat_Div_4 = 0x13,//Vref = VBat/4
	ADC_Vref_VBat_Div_3 = 0x23,//Vref = VBat/3
	ADC_Vref_VBat_Div_2 = 0x33,//Vref = VBat/2
}eADC_VrefTypeDef;

/*ADC VBat divider factor enum type define. */
typedef enum
{
	ADC_Vbat_Div_Factor_Off = 0x00,//Vref = VBat/1
	ADC_Vbat_Div_Factor_4 = 0x01,//Vref = VBat/4
	ADC_Vbat_Div_Factor_3 = 0x02,//Vref = VBat/3
	ADC_Vbat_Div_Factor_2 = 0x03,//Vref = VBat/2
}eADC_VbatDivFactorTypeDef;

/*ADC Mode enum type define. */
typedef enum
{
	ADC_Mode_Single,
	ADC_Mode_Diff,
}eADC_ModeTypeDef;

/*ADC input channel enum type define. */
typedef enum
{
	/* Use to set input channel when ADC is single-end mode. */
	ADC_Channel_None = 0x00,//Use for RNS ADC Module.

	ADC_Channel_PA6  = 0x01,
	ADC_Channel_PA7  = 0x02,

	ADC_Channel_PB0  = 0x03,
	ADC_Channel_PB1  = 0x04,
	ADC_Channel_PB2  = 0x05,
	ADC_Channel_PB3  = 0x06,
	ADC_Channel_PB4  = 0x07,
	ADC_Channel_PB5  = 0x08,
	ADC_Channel_PB6  = 0x09,
	ADC_Channel_PB7  = 0x0A,

	/* Use to set Positive/Negative output when ADC is differential mode. */
	ADC_Channel_PGA_P = 0x0B,//Positive
	ADC_Channel_PGA_N = 0x0B,//Negative

	Reserved,

	ADC_Channel_Temp_P = 0x0D,//Positive
	ADC_Channel_Temp_N = 0x0D,//Negative

	ADC_Channel_RSSI_P = 0x0E,//Positive
	ADC_Channel_RSSI_N = 0x0E,//Negative

	ADC_Channel_VBat   = 0x0F,//Positive
	ADC_Channel_GND    = 0x0F,//Negative
}eADC_ChannelTypeDef;

/*ADC resolution enum type define. */
typedef enum
{
	ADC_Resolution_8b  = 0,
	ADC_Resolution_10b = 1,
	ADC_Resolution_12b = 2,
	ADC_Resolution_14b = 3,
}eADC_ResTypeDef;

/*ADC input voltage divider factor enum type define. */
typedef enum
{
	ADC_Input_Vol_Div_Factor_1 = 0,
	ADC_Input_Vol_Div_Factor_2 = 1,
	ADC_Input_Vol_Div_Factor_4 = 2,
	ADC_Input_Vol_Div_Factor_8 = 3,
}eADC_InputVolDivFactorTypeDef;

/*ADC stable time enum type define. */
typedef enum
{
	ADC_Stable_Time_3Cyc  = 0x00,
	ADC_Stable_Time_6Cyc  = 0x01,
	ADC_Stable_Time_9Cyc  = 0x02,
	ADC_Stable_Time_12Cyc = 0x03,
	ADC_Stable_Time_15Cyc = 0x04,
	ADC_Stable_Time_18Cyc = 0x05,
	ADC_Stable_Time_21Cyc = 0x06,
	ADC_Stable_Time_24Cyc = 0x07,
	ADC_Stable_Time_27Cyc = 0x08,
	ADC_Stable_Time_30Cyc = 0x09,
	ADC_Stable_Time_33Cyc = 0x0A,
	ADC_Stable_Time_36Cyc = 0x0B,
	ADC_Stable_Time_39Cyc = 0x0C,
	ADC_Stable_Time_42Cyc = 0x0D,
	ADC_Stable_Time_45Cyc = 0x0E,
	ADC_Stable_Time_48Cyc = 0x0f,
}eADC_StableTimeTypeDef;

void ADC_PowerOn(void);
void ADC_PowerOff(void);

void ADC_ClockSet(u32 ADC_Clk_Hz);
void ADC_ClockSetEx(u8 ADC_ClkDiv);
void ADC_Init(eADC_ModuleTypeDef ADC_Module,
		      eADC_ModeTypeDef   ADC_Mode,
		      eADC_VrefTypeDef   ADC_Vref,
		      eADC_ResTypeDef    ADC_Resolution);
void ADC_VrefSet(eADC_VrefTypeDef ADC_Vref);
void ADC_SingleEndChannelSet(eADC_ModuleTypeDef ADC_Module,eADC_ChannelTypeDef ADC_Channel);
void ADC_DifferChannelSet(eADC_ModuleTypeDef  ADC_Module,
					      eADC_ChannelTypeDef ADC_Channel_P,
					      eADC_ChannelTypeDef ADC_Channel_N);
void ADC_ResolutionSet(eADC_ModuleTypeDef ADC_Module, eADC_ResTypeDef ADC_Res);
void ADC_ModeSet(eADC_ModuleTypeDef ADC_Module, eADC_ModeTypeDef ADC_Mode);
void ADC_StableTime(eADC_ModuleTypeDef ADC_Module, eADC_StableTimeTypeDef ADC_Stabletime);
void ADC_StateNumSet(u8 ADC_StateNum);
void ADC_InputVolDivFactor(eADC_InputVolDivFactorTypeDef ADC_InputVolDivFactor);

void ADC_SetStateTimeSet(u8 ADC_SetTime);
void ADC_CaptureTimeSet(eADC_ModuleTypeDef ADC_Module,u16 ADC_CaptureTime);
void ADC_SampleRateSet(eADC_ModuleTypeDef ADC_Module,u8 ADC_SetTime,u16 ADC_CaptureTime);

void ADC_Cmd(eADC_ModuleTypeDef ADC_Module, u8 ADC_Enable);

u16 ADC_GetConvertValue(void);


/* Battery check */
void ADC_BatteryCheckInit(eADC_ChannelTypeDef ADC_Channel);

#endif /*End of __ADC_5316_H */
#endif/* End of (MCU_CORE_TYPE == MCU_CORE_5316) */

/*----------------------------------File End----------------------------------*/



