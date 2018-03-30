/**************************************************************************************************
  Filename:       	adc.h
  Author:			junjun.xu@telink-semi.com
  Created Date:	2016/06/05

  Description:    This file contains the enum variable defination details used for adc function


**************************************************************************************************/


#ifndef 	ADC_H_
#define 	ADC_H_

#include "../mcu/register.h"
#include "../common/compatibility.h"
#include "../common/utility.h"



enum{
	SAR_ADC_CLK_EN = BIT(6)
};
#define    	SET_ADC_CLK_EN()				do{ analog_write(0x82, analog_read(0x82) | SAR_ADC_CLK_EN);}while(0)



enum{
	ADC_CLK_DIV = BIT_RNG(0,2)
};

#define    	SET_ADC_CLK(v)					do{ analog_write(0x80+116, (analog_read(0x80+116)&0xf8) | (v)); }while(0)




#define    REG_ADC_RNGL        			ReadAnalogReg(0x80+117)
#define    REG_ADC_RNGH        			ReadAnalogReg(0x80+118)
#define    REG_ADC_MISCL        		ReadAnalogReg(0x80+119)
#define    REG_ADC_MISCH        		ReadAnalogReg(0x80+120)

enum{
	ADC_MISC_DATA   = BIT_RNG(0,6),
	ADC_MISC_VLD    = BIT(7),
};
#define		CHECK_ADC_MISC_STATUS		((REG_ADC_MISCH & ADC_MISC_VLD)?1:0)

enum{
	LEFT_CHN_GAIN_STAGE		= BIT_RNG(0,1),
	RIGHT_CHN_GAIN_STAGE	= BIT_RNG(2,3),
	ADC_RNS_MODE			= BIT(4),
	ADC_POWER_ON 			= BIT(5),
	LEFT_CHN_PGA_POWER_ON 	= BIT(6),
	RIGHT_CHN_PGA_POWER_ON 	= BIT(7),
};

#define    	SET_ADC_POWER_ON(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= ((unsigned char)(~(v)));\
												WriteAnalogReg(0x80+124,val);\
											}while(0)
enum{
	GAIN_STAGE_BIAS_PER75,
	GAIN_STAGE_BIAS_PER100,
	GAIN_STAGE_BIAS_PER125,
	GAIN_STAGE_BIAS_PER150,
};

#define    	SET_LEFT_GAIN_BIAS(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+124,val);\
											}while(0)

#define    	SET_RIGHT_GAIN_BIAS(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+124,val);\
											}while(0)

#define    	SET_LEFT_BOOST_BIAS(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+123);\
												val &= 0xfc;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+123,val|0x01);\
											}while(0)

#define    	SET_RIGHT_BOOST_BIAS(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+123);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<6)&0xc0);\
												WriteAnalogReg(0x80+123,val|0x04);\
											}while(0)



typedef enum{
	ADC_LEFT_CHN 	= BIT(0),
	ADC_RIGHT_CHN	= BIT(1),
	ADC_MISC_CHN	= BIT(2),
	ADC_RNS_CHN 	= BIT(3),
}ADC_ChTypeDef;

#define  	SET_ADC_CHN_EN(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+114);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+114,val);\
											}while(0)
enum{
	ADC_MAX_SCNT	= BIT_RNG(4,6),
};

#define  	SET_ADC_MAX_SCNT(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+114);\
												val &= (~ADC_MAX_SCNT);\
												val |= (((unsigned char)(v)<<4)&0xf0);\
												WriteAnalogReg(0x80+114,val);\
											}while(0)
/********************************	configure set state	  ****************************/
enum{
	ADC_SET_MAX_S	=BIT_RNG(0,3)
};

#define 	SET_ADC_SET_LEN(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+113);\
												val &= (~ADC_SET_MAX_S);\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+113,val);\
											}while(0)

enum{
	ADC_LEFT_CHN_MODE	= BIT(4),
	ADC_RIGTH_CHN_MODE	= BIT(5),
	ADC_MISC_CHN_MODE	= BIT(6),
};

#define 	SET_ADC_CHN_DIFF_EN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+108);\
												val &= 0x8f;\
												val |= (((unsigned char)(v))&0x70);\
												WriteAnalogReg(0x80+108,val);\
											}while(0)

#define 	SET_ADC_CHN_SINGLE_EN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+108);\
												val &= (unsigned char)(~(v));\
												WriteAnalogReg(0x80+108,val);\
											}while(0)

/**
 *ADC analog input channel
 */
/*ADC analog negative input channel selection enum*/
typedef enum {
	NOINPUTN,
	B0N,
	B1N,
	B2N,
	B3N,
	B4N,
	B5N,
	B6N,
	B7N,
	C4N,
	C5N,
	PGA0N,
	PGA1N,
	TEMSENSORN,
	RSVD_N,
	GND,
}ADC_InputNchTypeDef;

/*ADC analog positive input channel selection enum*/
typedef enum {
	NOINPUTP,
	B0P,
	B1P,
	B2P,
	B3P,
	B4P,
	B5P,
	B6P,
	B7P,
	C4P,
	C5P,
	PGA0P,
	PGA1P,
	TEMSENSORP,
	RSVD_P,
	VBAT,
}ADC_InputPchTypeDef;

#define 	SET_ADC_MISC_N_CHN_AIN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+104);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+104,val);\
											}while(0)

#define 	SET_ADC_MISC_P_CHN_AIN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+104);\
												val &= 0x0f;\
												val |= (((unsigned char)(v)<<4)&0xf0);\
												WriteAnalogReg(0x80+104,val);\
											}while(0)


#define 	SET_ADC_LEFT_N_CHN_AIN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+105);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+105,val);\
											}while(0)


#define 	SET_ADC_LEFT_P_CHN_AIN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+105);\
												val &= 0x0f;\
												val |= (((unsigned char)(v)<<4)&0xf0);\
												WriteAnalogReg(0x80+105,val);\
											}while(0)


#define 	SET_ADC_RIGHT_N_CHN_AIN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+106);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+106,val);\
											}while(0)


#define 	SET_ADC_RIGHT_P_CHN_AIN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+106);\
												val &= 0x0f;\
												val |= (((unsigned char)(v)<<4)&0xf0);\
												WriteAnalogReg(0x80+106,val);\
											}while(0)

typedef enum{
	ADC_PRESCALER_1,
	ADC_PRESCALER_1F2,
	ADC_PRESCALER_1F4,
	ADC_PRESCALER_1F8
}ADC_PreScalingTypeDef;
#define		SET_ADC_BANDGAP_ON()			do{\
												unsigned char val=ReadAnalogReg(0x06);\
												val &= 0xef;\
												WriteAnalogReg(0x06,val);\
											}while(0)

#define		SET_ADC_PRESCALER(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+122);\
												val &= 0x3f;\
												val |= (((unsigned char)(v)<<6)&0xc0);\
												WriteAnalogReg(0x80+122,val);\
											}while(0)




typedef enum{
	ADC_VBAT_DIVIDER_OFF,
	ADC_VBAT_DIVIDER_1F4,
	ADC_VBAT_DIVIDER_1F3,
	ADC_VBAT_DIVIDER_1F2
}ADC_VbatDivTypeDef;

#define		SET_ADC_VBAT_DIVIDER(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+121);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+121,val);\
											}while(0)
/**
 *  ADC reference voltage
 */
typedef enum{
	ADC_VREF_0P6V,
	ADC_VREF_0P9V,
	ADC_VREF_1P2V,
	ADC_VREF_VBAT_N,
}ADC_RefVolTypeDef;

#define		SET_ADC_LEFT_VREF(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+103);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+103,val);\
											}while(0)

#define		SET_ADC_RIGHT_VREF(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+103);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+103,val);\
											}while(0)

#define		SET_ADC_MISC_VREF(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+103);\
												val &= 0xcf;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+103,val);\
											}while(0)

typedef enum{
	RES8,
	RES10,
	RES12,
	RES14
}ADC_ResTypeDef;

#define		SET_ADC_LEFT_RES(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+107);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+107,val);\
											}while(0)

#define		SET_ADC_RIGHT_RES(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+107);\
												val &= 0xcf;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+107,val);\
											}while(0)

#define		SET_ADC_MISC_RES(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+108);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+108,val);\
											}while(0)

typedef enum{
	SAMPLING_CYCLES_3,
	SAMPLING_CYCLES_6,
	SAMPLING_CYCLES_9,
	SAMPLING_CYCLES_12,
	SAMPLING_CYCLES_15,
	SAMPLING_CYCLES_18,
	SAMPLING_CYCLES_21,
	SAMPLING_CYCLES_24,
	SAMPLING_CYCLES_27,
	SAMPLING_CYCLES_30,
	SAMPLING_CYCLES_33,
	SAMPLING_CYCLES_36,
	SAMPLING_CYCLES_39,
	SAMPLING_CYCLES_42,
	SAMPLING_CYCLES_45,
	SAMPLING_CYCLES_48,
}ADC_SampCycTypeDef;

#define		SET_ADC_LEFT_TSAMP_CYCLE(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+109);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+109,val);\
											}while(0)

#define		SET_ADC_RIGHT_TSAMP_CYCLE(v)	do{\
												unsigned char val=ReadAnalogReg(0x80+109);\
												val &= 0x0f;\
												val |= (((unsigned char)(v)<<4)&0xf0);\
												WriteAnalogReg(0x80+109,val);\
											}while(0)

#define		SET_ADC_MISC_TSAMP_CYCLE(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+110);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+110,val);\
											}while(0)

/********************************	configure capture state	  ****************************/
enum{
	ADC_MAX_MC0		= BIT_RNG(0,7),
	ADC_MAX_MC1		= BIT_RNG(6,7)
};
#define		SET_ADC_M_RNS_CAPTURE_LEN(v)	do{\
												unsigned char val=ReadAnalogReg(0x80+113);\
												val &= 0x3f;\
												val |= (((unsigned char)((v)>>8)<<6)&0xc0);\
												WriteAnalogReg(0x80+111,(unsigned char)(v)&0xff);\
												WriteAnalogReg(0x80+113,val);\
											}while(0)

enum{
	ADC_MAX_C0		= BIT_RNG(0,7),
	ADC_MAX_C1		= BIT_RNG(4,5)
};
#define		SET_ADC_L_R_CAPTURE_LEN(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+113);\
												val &= 0xcf;\
												val |= (((unsigned char)((v)>>8)<<4)&0x30);\
												WriteAnalogReg(0x80+112,(unsigned char)(v)&0xff);\
												WriteAnalogReg(0x80+113,val);\
											}while(0)

typedef enum{
	NORMAL_MODE      = 0,
	RNS_MODE         = BIT(4),
}ADCModeTypeDef;

typedef enum {
	SAR_ADC_RNG_MODE   = 0,
	R_RNG_MODE_0       = 2,
	R_RNG_MODE_1       = 3,
	ADC_DAT12_RNG_MODE = 4,
	ADC_DAT5_RNG_MODE  = 6,
}RNG_SrcTypeDef;

typedef enum {
	READ_UPDATA        = BIT(3),
	CLOCLK_UPDATA      = BIT(4),
}RNG_UpdataTypeDef;


enum{
 ADC_CUR_TRIM_PER75,
 ADC_CUR_TRIM_PER100,
 ADC_CUR_TRIM_PER125,
 ADC_CUR_TRIM_PER150
};
#define  SET_ADC_ITRIM_PREAMP(v)   do{\
            unsigned char val=ReadAnalogReg(0x80+122);\
            val &= 0xfc;\
            val |= (((unsigned char)(v))&0x03);\
            WriteAnalogReg(0x80+122,val);\
           }while(0)

#define  SET_ADC_ITRIM_VREFBUF(v)  do{\
            unsigned char val=ReadAnalogReg(0x80+122);\
            val &= 0xf3;\
            val |= (((unsigned char)(v)<<2)&0x0c);\
            WriteAnalogReg(0x80+122,val);\
           }while(0)

#define  SET_ADC_ITRIM_VCMBUF(v)      do{\
            unsigned char val=ReadAnalogReg(0x80+122);\
            val &= 0xcf;\
            val |= (((unsigned char)(v)<<4)&0x30);\
            WriteAnalogReg(0x80+122,val);\
           }while(0)


#endif


