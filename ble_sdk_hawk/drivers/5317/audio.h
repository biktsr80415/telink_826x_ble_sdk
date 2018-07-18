#ifndef _AUDIO_H
#define _AUDIO_H

#include "driver_config.h"
#include "register.h"

#if 1

enum{
	SAR_ADC_CLK_EN = BIT(7)
};
#define    	SET_ADC_CLK_EN()				do{\
												unsigned char val=ReadAnalogReg(0x80);\
												val |= (SAR_ADC_CLK_EN);\
												WriteAnalogReg(0x80,val);\
											}while(0)
enum{
	ADC_CLK_DIV = BIT_RNG(0,2)
};

#define    	SET_ADC_CLK(v)					do{\
												unsigned char val=ReadAnalogReg(0x80+116);\
												val &= (~ADC_CLK_DIV);\
												val |= (((unsigned char)(v))&0x07);\
												WriteAnalogReg(0x80+116,val);\
											}while(0)
enum{
	ADC_NORMAL_MODE			= 0,
	ADC_RNS_MODE			= 1,
};
#define    	SET_ADC_MODE(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= 0xef;\
												WriteAnalogReg(0x80+124,val|(v<<4));\
											}while(0)

enum{
	ADC_POWER_ON 			= BIT(5),
	LEFT_CHN_PGA_POWER_ON 	= BIT(6),
};

#define    	SET_ADC_POWER_ON(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= ((unsigned char)(~(v)));\
												WriteAnalogReg(0x80+124,val);\
											}while(0)

#define    	SET_LEFT_GAIN_BIAS(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+124,val);\
											}while(0)


#define		SET_ADC_VBAT_DIVIDER(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+121);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+121,val);\
											}while(0)


#define		SET_ADC_ITRIM_PREAMP(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+122);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+122,val);\
											}while(0)

#define		SET_ADC_ITRIM_VREFBUF(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+122);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+122,val);\
											}while(0)

#define		SET_ADC_ITRIM_VCMBUF(v)		    do{\
												unsigned char val=ReadAnalogReg(0x80+122);\
												val &= 0xcf;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+122,val);\
											}while(0)


#define		SET_ADC_PRESCALER(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+122);\
												val &= 0x3f;\
												val |= (((unsigned char)(v)<<6)&0xc0);\
												WriteAnalogReg(0x80+122,val);\
											}while(0)

#define    	SET_LEFT_BOOST_BIAS(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+123);\
												val &= 0xcf;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+123,val|0x01);\
											}while(0)


#define		SET_ADC_LEFT_VREF(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+103);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+103,val);\
											}while(0)




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


#define		SET_ADC_LEFT_RES(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+107);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+107,val);\
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



#define		SET_ADC_LEFT_TSAMP_CYCLE(v)		do{\
												unsigned char val=ReadAnalogReg(0x80+109);\
												val &= 0xf0;\
												val |= (((unsigned char)(v))&0x0f);\
												WriteAnalogReg(0x80+109,val);\
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

#define		SET_PGA_GAIN_VALUE(v)			do{\
												unsigned char val = (((unsigned char)(v))&0x3f);\
												WRITE_REG8(0xb55,val|0x80);\
												WRITE_REG8(0xb56,0x01);\
												WRITE_REG8(0xb56,0x00);\
											}while(0)
/*ADC  CONFIG  END*/
enum{
	AUDIO_OUTPUT_OFF		= 0,
	AUDIO_OUTPUT_MONO_MODE 	= BIT(0),
	AUDIO_I2S_PLAYER 		= BIT(1),
	AUDIO_SDM_PLAYER 		= BIT(2),
	AUDIO_ISO_PLAYER 		= BIT(3),
	AUDIO_I2S_RECORDER 		= BIT(4),
	AUDIO_I2S_INTERFACE 	= BIT(5),
	AUDIO_GRP 				= BIT(6),
	AUDIO_HPF 				= BIT(7),
};

#define		SET_AUDIO_OUTPUT_CTRL(v)		do{\
												unsigned char val=0;\
												val |= ((unsigned char)(v));\
												WRITE_REG8(0x560,val);\
											}while(0)
enum{
	AUDIO_DFIFO0_IN 		= BIT(0),
	AUDIO_DFIFO1_IN 		= BIT(1),
	AUDIO_DFIFO2_IN 		= BIT(2),
	AUDIO_DFIFO0_OUT 		= BIT(3),
	AUDIO_DFIFO0_L_INT		= BIT(4),
	AUDIO_DFIFO0_H_INT		= BIT(5),
	AUDIO_DFIFO1_H_INT		= BIT(6),
	AUDIO_DFIFO2_H_INT		= BIT(7),
};

#define		REG_AUDIO_DFIFO_MODE	REG_ADDR8(0xb10)


enum{
	AUDIO_DMIC0_DATA_IN_RISING = BIT(0),
	AUDIO_DMIC1_DATA_IN_RISING = BIT(1),
	AUDIO_CIC_BYPASS	   	   = BIT(4),
};

#define		REG_AUDIO_DFIFO_AIN		REG_ADDR8(0xb11)

enum{
	AUDIO_INPUT_USB,
	AUDIO_INPUT_I2S,
	AUDIO_INPUT_AMIC,
	AUDIO_INPUT_DMIC,
};

#define		SET_AUDIO_INPUT_CTRL(v)		do{\
											unsigned char val=READ_REG8(0xb11);\
											val &= 0xf3;\
											val |= ((unsigned char)(v)<<2);\
											WRITE_REG8(0xb11,val);\
										}while(0)
enum{
	DEC_RATIO    = BIT_RNG(0,3),
	DEC_SHIFT    = BIT_RNG(4,6), //the value not larger than 5
};

#define		REG_AUDIO_DEC_RATIO			REG_ADDR8(0xb12)
#define		SET_AUDIO_DEC_RATIO(v)		do{\
											REG_AUDIO_DEC_RATIO &= ~DEC_RATIO;\
											REG_AUDIO_DEC_RATIO |= ((v)&0x0f);\
										}while(0)

#define		SET_AUDIO_DEC_SHIFT(v)		do{\
											REG_AUDIO_DEC_RATIO &= ~DEC_SHIFT;\
											REG_AUDIO_DEC_RATIO |= ((v)&0xf0);\
										}while(0)


enum{
	AUDIO_LNR_VALID_SEL	= BIT(0),
	AUDIO_CIC_MODE  	= BIT(3)
};

#define		REG_AUDIO_DEC_MODE					REG_ADDR8(0xb35)

typedef enum{
	HPF_ALC_LPF_ON = 0,
	HPF_ADJ = BIT_RNG(0,3),
	HPF_OFF = BIT(4),
	ALC_OFF = BIT(5),
	LPF_OFF = BIT(6),
	DOUBLE_DOWN_SAMPLING_ON = BIT(7)
}Audio_ALC_HPF_LPF_Typedef;

#define		REG_AUDIO_ALC_HPF_LPF_CTRL			REG_ADDR8(0xb40)
#define		SET_HPF_SHIFT(v)					do{\
													unsigned char val=READ_REG8(0xb40);\
													val &=((unsigned char)(~HPF_ADJ));\
													val |= ((unsigned char)(v)&0x0f);\
													WRITE_REG8(0xb40,val);\
												}while(0)
enum{
	MIN_VOLUME_IN_DIGITAL_MODE 	= BIT_RNG(0,5),
	PGA_GAIN_IN_ANALOG_MODE		= BIT_RNG(0,6),
	AUTO_MODE_SEL				= BIT(7)
};

#define		SET_AUDIO_ALC_LEFT_CHN_MIN_VOL(v)	do{\
													unsigned char val=READ_REG8(0xb41);\
													val &=((unsigned char)(~(MIN_VOLUME_IN_DIGITAL_MODE|AUTO_MODE_SEL)));\
													val |= ((unsigned char)(v)&0x3f);\
													WRITE_REG8(0xb41,val);\
												}while(0)

#define		REG_AUDIO_DFIFO0_LOW_THRESHOLD		REG_ADDR8(0xb0c)
#define		REG_AUDIO_DFIFO0_HIGH_THRESHOLD		REG_ADDR8(0xb0d)

/*available for user and developer*/
typedef enum {
	AMIC,
	DMIC,
	I2S_IN,
}AudioInput_Typedef;

typedef enum{
	OFF=0,
	AMIC_ON=2,
	DMIC_ON=3,
}AudioMicS_Typedef;
/**
 *
 */
typedef enum {
	R1,R2,R3,R4,R5,R6,R7,R8,R16,R32,R64,R128,
}Audio_SampRateTypeDef;

typedef enum{
	EN_NONE=0,
	EN_GRP=BIT(6),
	EN_HPF=BIT(7),
	EN_HPF_GRP=BIT(7)|BIT(6),
}AudioModuleEn_Typedef;

typedef enum{
	SDM,
	I2S_OUT,
}AudioOutput_Typedef;

/* Audio rate define */
typedef enum{
	AUDIO_8K,
	AUDIO_16K,
	AUDIO_22K,
	AUDIO_32K,
	AUDIO_44K,
	AUDIO_48K,
	AUDIO_96K,
	RATE_SIZE,
}AUDIO_RateTypeDef;


void audio_amic_init(AUDIO_RateTypeDef Audio_Rate);
void audio_dmic_init (AUDIO_RateTypeDef Audio_Rate,unsigned int MicBufSize);
/**
*
*	@brief	   sdm setting function, enable or disable the sdm output, configure SDM output paramaters
*
*	@param[in]	audio_out_en - audio output enable or disable set, '1' enable audio output; '0' disable output
*	@param[in]	sample_rate - audio sampling rate, such as 16K,32k etc.
*	@param[in]	sdm_clk -	  SDM clock, default to be 8Mhz
*	@param[in]  fhs_source - the parameter is CLOCK_SYS_TYPE. avoid CLOCK_SYS_TYPE to be modified to other word.such as SYS_TYPE etc.
*
*	@return	none
*/
void audio_sdm_output_set(AudioInput_Typedef InType,AUDIO_RateTypeDef Audio_Rate, unsigned char audio_out_en);

#else

typedef struct{
	unsigned char  CIC;
	unsigned char  setLen;
	unsigned short captureLen;
}AUDIO_SampleRateTypdef;

/* Audio input source define */
typedef enum{
	AUDIO_Input_RSVD = 0x00,
	AUDIO_Input_I2S  = 0x01,
	AUDIO_Input_AMIC = 0x02,
	AUDIO_Input_DMIC = 0x03,
}eAUDIO_InputTypeDef;

/* Dec filter down sample rate define */
typedef enum{
	AUDIO_DecDownRatio_1 = 0x00,
	AUDIO_DecDownRatio_2 = 0x01,
	AUDIO_DecDownRatio_3 = 0x02,
	AUDIO_DecDownRatio_4 = 0x03,
	AUDIO_DecDownRatio_5 = 0x04,
	AUDIO_DecDownRatio_6 = 0x05,
	AUDIO_DecDownRatio_7 = 0x06,
	AUDIO_DecDownRatio_8 = 0x07,

	AUDIO_DecDownRatio_16  = 0x08,
	AUDIO_DecDownRatio_32  = 0x09,
	AUDIO_DecDownRatio_64  = 0x0A,
	AUDIO_DecDownRatio_128 = 0x0B,
	AUDIO_DecDownRatio_256 = 0x0C,

}eAUDIO_DecDownRatioTypeDef;

/* Audio Biquad filter define */
typedef enum{
	AUDIO_Biquad_None = 0x00,//disable filter0 and filter1
	AUDIO_Biquad_Filter0 = 0x10,
	AUDIO_Biquad_Filter1 = 0x20,
	AUDIO_Biquad_Filter0_And_1 = 0x30,
}eAUDIO_BiquadTypeDef;

/* Audio output define */
typedef enum{
	AUDIO_Output_Off           = 0x00,
	AUDIO_Output_I2S_Player    = BIT(1),
	AUDIO_Output_SDM_Player    = BIT(2),
	AUDIO_Output_I2S_Recorder  = BIT(4),
	AUDIO_Output_I2S_Interface = BIT(5),
	AUDIO_Output_GRP           = BIT(6),
	AUDIO_Output_HPF           = BIT(7),
}eAUDIO_OutputTypeDef;

/* Audio rate define */
typedef enum{
	AUDIO_Rate_16K = 0x00,
	AUDIO_Rate_22K = 0x01,
	AUDIO_Rate_32K = 0x02,
	AUDIO_Rate_44K = 0x03,
	AUDIO_Rate_48K = 0x04,
	AUDIO_Rate_96K = 0x05,
}eAUDIO_RateTypeDef;


void AUDIO_AmicInit(eAUDIO_RateTypeDef AUDIO_Rate, short *audioBuf, unsigned int bytesLen);
#endif

#endif/* End of _AUDIO_H */
