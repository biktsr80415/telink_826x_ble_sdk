#pragma once
#ifndef _ADC_H
#define _ADC_H

#include "driver_config.h"
#include "pm.h"
#include "bsp.h"
#include "analog.h"
#include "register.h"
#include "gpio.h"
#include "clock.h"


#if 1

static inline void	adc_reset_adc_module(void)
{
	reg_rst2 = FLD_RST2_ADC;
	reg_rst2 = 0;
}

static inline void adc_enable_clk_24m_to_sar_adc(unsigned int en)  //1: enable;  0: disable
{
	if(en){
		analog_write(0x80, analog_read(0x80) | FLD_CLK_24M_TO_SAR_EN);
	}
	else{
		analog_write(0x80, analog_read(0x80) & ~FLD_CLK_24M_TO_SAR_EN);
	}
}







/**************************************************************************************
afe_0xE7
    BIT<1:0>  adc_vrefl, Select VREF for left channel
    BIT<3:2>  adc_vrefr, Select VREF for right channel
    BIT<5:4>  adc_vrefm, Select VREF for Misc channel
			0x0: 0.6V
			0x1: 0.9V
 	 	 	0x2: 1.2V
 	 	 	0x3: VBAT/N (N=2/3/4, when afe_0xF9<3:2> is set as 0x3/0x2/0x1).

	notice that: when choose ADC_VREF_VBAT_N, must use adc_set_vref_vbat_divider to config N(1/2/3/4)

afe_0xF9
    BIT<3:2>  adc_vbat_div
      	  	  Vbat divider select (see adc_vref)
					0x0: OFF
					0x1: VBAT/4
					0x2: VBAT/3
					0x3: VBAT/2
 *************************************************************************************/
#define anareg_adc_vref			0xE7
enum{
	FLD_ADC_VREF_CHN_L = 		BIT_RNG(0,1),
	FLD_ADC_VREF_CHN_R = 		BIT_RNG(2,3),
	FLD_ADC_VREF_CHN_M = 		BIT_RNG(4,5),
};

//ADC reference voltage
typedef enum{
	ADC_VREF_0P6V,
	ADC_VREF_0P9V,
	ADC_VREF_1P2V,
	ADC_VREF_VBAT_N,  //here N(1/2/3/4) is configed by interface adc_set_vref_vbat_divider
}ADC_RefVolTypeDef;


#define anareg_adc_vref_vbat_div		0xF9
enum{
	FLD_ADC_VREF_VBAT_DIV = 		BIT_RNG(2,3),
	FLD_ADC_SEL_ATB       = 		BIT_RNG(4,5),
};

typedef enum{
	ADC_VBAT_DIVIDER_OFF,
	ADC_VBAT_DIVIDER_1F4,
	ADC_VBAT_DIVIDER_1F3,
	ADC_VBAT_DIVIDER_1F2
}ADC_VbatDivTypeDef;

typedef enum{
	ADC_SEL_ATB_NONE = 0,
	ADC_SEL_ATB_0,
	ADC_SEL_ATB_1,
	ADC_SEL_ATB_2
}ADC_SelAtbTypeDef;

static inline void adc_set_atb(ADC_SelAtbTypeDef stat)
{
	analog_write(anareg_adc_vref_vbat_div, ((analog_read(anareg_adc_vref_vbat_div)&(~FLD_ADC_SEL_ATB)) | (stat<<4)) );
}

static inline void adc_set_vref_chn_left(ADC_RefVolTypeDef v_ref)
{
	analog_write(anareg_adc_vref, ((analog_read(anareg_adc_vref)&(~FLD_ADC_VREF_CHN_L)) | (v_ref)) );
}

static inline void adc_set_vref_chn_right(ADC_RefVolTypeDef v_ref)
{
	analog_write(anareg_adc_vref, ((analog_read(anareg_adc_vref)&(~FLD_ADC_VREF_CHN_R)) | (v_ref<<2) ));
}

static inline void adc_set_vref_chn_misc(ADC_RefVolTypeDef v_ref)
{
	analog_write(anareg_adc_vref, ((analog_read(anareg_adc_vref)&(~FLD_ADC_VREF_CHN_M)) | (v_ref<<4)) );
}


/**
 * @brief      This function select Vbat voltage divider
 * @param[in]  vbat_div - enum variable of Vbat Vbat voltage divider.
 * @return     none
 */
static inline void adc_set_vref_vbat_divider(ADC_VbatDivTypeDef vbat_div)
{
	analog_write (anareg_adc_vref_vbat_div, (analog_read(anareg_adc_vref_vbat_div)&(~FLD_ADC_VREF_VBAT_DIV)) | (vbat_div<<2) );
}



/**************************************************************************************
afe_0xE8
    BIT<3:0>  Select negative input for Misc channel
    BIT<7:4>  Select positive input for Misc channel

afe_0xE9
    BIT<3:0>  Select negative input for left channel
    BIT<7:4>  Select positive input for left channel

afe_0xEA
    BIT<3:0>  Select negative input for right channel
    BIT<7:4>  Select positive input for right channel

	negative input:
		0x0: No input
		0x1: B<0>
		0x2: B<1>
		...
		0x8: B<7>
		0x9: C<4>
		0xa: C<5>
		0xb: pga_n<0> (PGA left-channel negative output)
		0xc: pga_n<1> (PGA right-channel negative output)
		0xd: tempsensor_n (Temperature sensor negative output)
		0xe: rsvd  //spec "GND" is wrong
		0xf: Ground

	positive input:
		0x0: No input
		0x1: B<0>
		0x2: B<1>
		...
		0x8: B<7>
		0x9: C<4>
		0xa: C<5>
		0xb: pga_p<0> (PGA left-channel positive output)
		0xc: pga_p<1> (PGA right-channel positive output)
		0xd: tempsensor_p (Temperature sensor positive output)
		0xe: rsvd
		0xf: Vbat (Battery voltage)
 *************************************************************************************/
#define anareg_adc_ain_chn_misc			0xE8
#define anareg_adc_ain_chn_left			0xE9
#define anareg_adc_ain_chn_right		0xEA

enum{
	FLD_ADC_AIN_NEGATIVE = 		BIT_RNG(0,3),
	FLD_ADC_AIN_POSITIVE = 		BIT_RNG(4,7),
};


/**
 *ADC analog input channel
 */
/*ADC analog negative input channel selection enum*/
typedef enum {
	NOINPUTN,
	RSVD_A6N,
	A7N,
	B0N,
	B1N,
	B2N,
	B3N,
	B4N,
	B5N,
	B6N,
	B7N,
	PGA0N,
	PGA1N,
	TEMSENSORN,
	RSVD_N,
	GND,
}ADC_InputNchTypeDef;

/*ADC analog positive input channel selection enum*/
typedef enum {
	NOINPUTP,
	RSVD_A6P,
	A7P,
	B0P,
	B1P,
	B2P,
	B3P,
	B4P,
	B5P,
	B6P,
	B7P,
	PGA0P,
	PGA1P,
	TEMSENSORP,
	RSVD_P,
	VBAT,
}ADC_InputPchTypeDef;


static inline void adc_set_ain_negative_chn_misc(ADC_InputNchTypeDef v_ain)
{
	analog_write (anareg_adc_ain_chn_misc, (analog_read(anareg_adc_ain_chn_misc)&(~FLD_ADC_AIN_NEGATIVE)) | (v_ain) );
}

static inline void adc_set_ain_positive_chn_misc(ADC_InputPchTypeDef v_ain)
{
	analog_write (anareg_adc_ain_chn_misc, (analog_read(anareg_adc_ain_chn_misc)&(~FLD_ADC_AIN_POSITIVE)) | (v_ain<<4) );
}

static inline void adc_set_ain_negative_chn_left(ADC_InputNchTypeDef v_ain)
{
	analog_write (anareg_adc_ain_chn_left, (analog_read(anareg_adc_ain_chn_left)&(~FLD_ADC_AIN_NEGATIVE)) | (v_ain) );
}

static inline void adc_set_ain_positive_chn_left(ADC_InputPchTypeDef v_ain)
{
	analog_write (anareg_adc_ain_chn_left, (analog_read(anareg_adc_ain_chn_left)&(~FLD_ADC_AIN_POSITIVE)) | (v_ain<<4) );
}

static inline void adc_set_ain_negative_chn_right(ADC_InputNchTypeDef v_ain)
{
	analog_write (anareg_adc_ain_chn_right, (analog_read(anareg_adc_ain_chn_right)&(~FLD_ADC_AIN_NEGATIVE)) | (v_ain) );
}

static inline void adc_set_ain_positive_chn_right(ADC_InputPchTypeDef v_ain)
{
	analog_write (anareg_adc_ain_chn_right, (analog_read(anareg_adc_ain_chn_right)&(~FLD_ADC_AIN_POSITIVE)) | (v_ain<<4) );
}

/**************************************************************************************
afe_0xEB
    BIT<1:0>  adc_resl, Set resolution for left channel
    BIT<5:4>  adc_resr, Set resolution for right channel

afe_0xEC
    BIT<1:0>  adc_resm, Set resolution for Misc channel

			 If RNS mode is set, resolution is set to 1. Otherwise ADC mode setting is:
				0x0: 8bits
				0x1: 10bits
				0x2: 12bits
				0x3: 14bits

	BIT<3:2>  Reserved
	BIT<4>    adc_en_diffl,  Select single-end or differential input mode for left channel.
	BIT<5>    adc_en_diffr,  Select single-end or differential input mode for right channel.
	BIT<6>    adc_en_diffm,  Select single-end or differential input mode for Misc channel.
	BIT<7>    Reserved
 *************************************************************************************/
#define anareg_adc_res_l_r			0xEB
enum{
	FLD_ADC_RES_L = 		BIT_RNG(0,1),
	FLD_ADC_RES_R = 		BIT_RNG(4,5),
};

#define anareg_adc_res_m			0xEC
enum{
	FLD_ADC_RES_M 		 = 	BIT_RNG(0,1),
	FLD_ADC_EN_DIFF_CHN_L	 =  BIT(4),
	FLD_ADC_EN_DIFF_CHN_R	 =  BIT(5),
	FLD_ADC_EN_DIFF_CHN_M	 =  BIT(6),
};

typedef enum{
	RES8,
	RES10,
	RES12,
	RES14
}ADC_ResTypeDef;


static inline void adc_set_resolution_chn_left(ADC_ResTypeDef v_res)
{
	analog_write(anareg_adc_res_l_r, (analog_read(anareg_adc_res_l_r)&(~FLD_ADC_RES_L)) | (v_res) );
}

static inline void adc_set_resolution_chn_right(ADC_ResTypeDef v_res)
{
	analog_write(anareg_adc_res_l_r, (analog_read(anareg_adc_res_l_r)&(~FLD_ADC_RES_R)) | (v_res<<4) );
}

static inline void adc_set_resolution_chn_misc(ADC_ResTypeDef v_res)
{
	analog_write(anareg_adc_res_m, (analog_read(anareg_adc_res_m)&(~FLD_ADC_RES_M)) | (v_res) );
}



typedef enum{
	SINGLE_ENDED_MODE = 0,  //single-ended mode
	DIFFERENTIAL_MODE = 1,  //differential mode
}ADC_InputModeTypeDef;

static inline void adc_set_input_mode_chn_left(ADC_InputModeTypeDef m_input)
{
	if(m_input){  //differential mode
		analog_write(anareg_adc_res_m, (analog_read(anareg_adc_res_m) | FLD_ADC_EN_DIFF_CHN_L ));
	}
	else{  //single-ended mode
		analog_write(anareg_adc_res_m, (analog_read(anareg_adc_res_m) & (~FLD_ADC_EN_DIFF_CHN_L)));
	}
}

static inline void adc_set_input_mode_chn_right(ADC_InputModeTypeDef m_input)
{
	if(m_input){  //differential mode
		analog_write(anareg_adc_res_m, analog_read(anareg_adc_res_m) | FLD_ADC_EN_DIFF_CHN_R );
	}
	else{  //single-ended mode
		analog_write(anareg_adc_res_m, analog_read(anareg_adc_res_m) & (~FLD_ADC_EN_DIFF_CHN_R) );
	}
}

static inline void adc_set_input_mode_chn_misc(ADC_InputModeTypeDef m_input)
{
	if(m_input){  //differential mode
		analog_write(anareg_adc_res_m, analog_read(anareg_adc_res_m) | FLD_ADC_EN_DIFF_CHN_M );
	}
	else{  //single-ended mode
		analog_write(anareg_adc_res_m, analog_read(anareg_adc_res_m) & (~FLD_ADC_EN_DIFF_CHN_M) );
	}
}

/**************************************************************************************
afe_0xED
    BIT<3:0>  adc_tsampl, Number of ADC clock cycles in sampling phase for left channel to stabilize the input before sampling
    BIT<7:4>  adc_tsampr, Number of ADC clock cycles in sampling phase for right channel to stabilize the input before sampling

afe_0xEE
    BIT<3:0>  adc_tsampm, Number of ADC clock cycles in sampling phase for Misc channel to stabilize the input before sampling

			0x0: 3 cycles (RNS mode default)
			0x1: 6 cycles
			0x2: 9 cycles
			0x3: 12 cycles
			......
			0xf: 48 cycles
 *************************************************************************************/
#define anareg_adc_tsmaple_l_r			0xED
enum{
	FLD_ADC_TSAMPLE_CYCLE_CHN_L = 		BIT_RNG(0,3),
	FLD_ADC_TSAMPLE_CYCLE_CHN_R = 		BIT_RNG(4,7),
};

#define anareg_adc_tsmaple_m			0xEE
enum{
	FLD_ADC_TSAMPLE_CYCLE_CHN_M = 		BIT_RNG(0,3),
};

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


/**
 * @brief      This function sets ADC sample time(the number of adc clocks for each sample)
 * @param[in]  adcST - enum variable of adc sample time.
 * @return     none
 */
static inline void adc_set_tsample_cycle_chn_left(ADC_SampCycTypeDef adcST)
{
	analog_write(anareg_adc_tsmaple_l_r, (analog_read(anareg_adc_tsmaple_l_r)&(~FLD_ADC_TSAMPLE_CYCLE_CHN_L)) | (adcST) );
}

static inline void adc_set_tsample_cycle_chn_right(ADC_SampCycTypeDef adcST)
{
	analog_write(anareg_adc_tsmaple_l_r, (analog_read(anareg_adc_tsmaple_l_r)&(~FLD_ADC_TSAMPLE_CYCLE_CHN_R)) | (adcST<<4) );
}

static inline void adc_set_tsample_cycle_chn_misc(ADC_SampCycTypeDef adcST)
{
	analog_write(anareg_adc_tsmaple_m, (analog_read(anareg_adc_tsmaple_m)&(~FLD_ADC_TSAMPLE_CYCLE_CHN_M)) | (adcST) );
}

/**************************************************************************************
afe_0xEF<7:0>  	r_max_mc[7:0]
afe_0xF0<7:0>	r_max_c[7:0]
afe_0xF1
		<3:0>   r_max_s
		<5:4>   r_max_c[9:8]
		<7:6>   r_max_mc[9:8]

	r_max_mc[9:0]: serves to set length of ¡°capture¡± state for RNS and Misc channel.
	r_max_c[9:0]:  serves to set length of ¡°capture¡± state for left and right channel.
	r_max_s:       serves to set length of ¡°set¡± state for left, right and Misc channel.

	Note: State length indicates number of 24M clock cycles occupied by the state.
 *************************************************************************************/
#define    anareg_r_max_mc	        	0xEF
#define    anareg_r_max_c       		0xF0
#define    anareg_r_max_s			    0xF1

enum{								 //ana_EF
	FLD_R_MAX_MC0	= BIT_RNG(0,7),
};

enum{								 //ana_F0
	FLD_R_MAX_C0	= BIT_RNG(0,7),
};

enum{                                //ana_F1
	FLD_R_MAX_S		= BIT_RNG(0,3),
	FLD_R_MAX_C1	= BIT_RNG(4,5),
	FLD_R_MAX_MC1	= BIT_RNG(6,7),
};


static inline void adc_set_length_set_state(unsigned char r_max_s)
{
	analog_write(anareg_r_max_s, (analog_read(anareg_r_max_s)&(~FLD_R_MAX_S)) | (r_max_s) );
}

static inline void adc_set_length_capture_state_for_chn_misc_rns(unsigned short r_max_mc)
{
	analog_write(anareg_r_max_mc,  (r_max_mc & 0x0ff));
	analog_write(anareg_r_max_s,  ((analog_read(anareg_r_max_s)&(~FLD_R_MAX_MC1)) | (r_max_mc>>8)<<6 ));
}

static inline void adc_set_length_capture_state_for_chn_left_right(unsigned short r_max_c)
{
	analog_write(anareg_r_max_c,  r_max_c & 0xff);
	analog_write(anareg_r_max_s,  (analog_read(anareg_r_max_s)&(~FLD_R_MAX_C1)) | (r_max_c>>8)<<4 );
}

/***************************************************************************************
afe_0xF2
    BIT<0>   r_en_left    Enable left  channel. 1: enable;  0: disable
    BIT<1>   r_en_right   Enable right channel. 1: enable;  0: disable
	BIT<2>   r_en_Misc    Enable Misc  channel. 1: enable;  0: disable
	BIT<3>   r_en_rns	  Enable RNS sampling.  1: enable;  0: disable
	BIT<6:4> r_max_scnt   Set total length for sampling state machine (i.e. max state index)
 *************************************************************************************/
#define anareg_adc_chn_en			0xF2
enum{
	FLD_ADC_CHN_EN_L	= BIT(0),
	FLD_ADC_CHN_EN_R	= BIT(1),
	FLD_ADC_CHN_EN_M	= BIT(2),
	FLD_ADC_CHN_EN_RNS	= BIT(3),
	FLD_ADC_MAX_SCNT	= BIT_RNG(4,6),
};

typedef enum{
	ADC_LEFT_CHN 	= BIT(0),
	ADC_RIGHT_CHN	= BIT(1),
	ADC_MISC_CHN	= BIT(2),
	ADC_RNS_CHN 	= BIT(3),
}ADC_ChTypeDef;

static inline void adc_set_chn_enable(ADC_ChTypeDef ad_ch)
{
	analog_write(anareg_adc_chn_en, (analog_read(anareg_adc_chn_en)&0xf0) | ad_ch );
}

static inline void adc_set_max_state_cnt(unsigned char s_cnt)
{
	analog_write(anareg_adc_chn_en, (analog_read(anareg_adc_chn_en)&(~FLD_ADC_MAX_SCNT)) | ((s_cnt&0x07)<<4) );
}

static inline void adc_set_chn_enable_and_max_state_cnt(ADC_ChTypeDef ad_ch, unsigned char s_cnt)
{
	analog_write(anareg_adc_chn_en, ad_ch | ((s_cnt&0x07)<<4) );
}

/**************************************************************************************
afe_0xF4
    BIT<2:0>  adc_clk_div
      	  	  ADC clock(derive from external 24M crystal)
      	  	  ADC clk frequency = 24M/(adc_clk_div + 1)
    BIT<7:3>  reserved
 *************************************************************************************/
#define anareg_adc_clk_div				0xF4
enum{
	FLD_ADC_CLK_DIV = BIT_RNG(0,2)
};



//adc sample clk = 24M/(1+div)    div: 0~7
static inline void adc_set_sample_clk(unsigned char div)
{
//	analog_write(anareg_adc_clk_div, (analog_read(anareg_adc_clk_div)&(~FLD_ADC_CLK_DIV)) | (div & 0x07) );
	analog_write(anareg_adc_clk_div,  div & 0x07 );
}



/**************************************************************************************
afe_0xF5<7:0>  	rng[7:0]   		Read only, random number [7:0]
afe_0xF6<7:0>	rng[15:8]  		Read only, random number [15:8]
afe_0xF7<7:0>   adc_dat[7:0]  	Read only, Misc adc dat[7:0]
afe_0xF8<7:0>   adc_dat[15:8]  	Read only
								[7]: vld, ADC data valid status bit (This bit will be set as 1 at the end of capture state to indicate the ADC data is valid, and will be cleared when set state starts.)
								[6:0]:  Misc adc_dat[14:8]

 *************************************************************************************/
#define    anareg_adc_rng_l        		0xF5
#define    anareg_adc_rng_h        		0xF6
#define    anareg_adc_misc_l        	0xF7
#define    anareg_adc_misc_h        	0xF8

enum{
	FLD_ADC_MISC_DATA   = BIT_RNG(0,6),
	FLD_ADC_MISC_VLD    = BIT(7),
};

#define		CHECK_ADC_MISC_STATUS		((analog_read(anareg_adc_misc_h) & FLD_ADC_MISC_VLD)?1:0)


/**************************************************************************************
afe_0xFA
		BIT<1:0>  adc_itrim_preamp, Comparator preamp bias current trimming
		BIT<3:2>  adc_itrim_vrefbuf, Vref buffer bias current trimming
		BIT<5:4>  adc_itrim_vcmbuf, Vref buffer bias current trimming

		BIT<7:6>  Analog input pre-scaling select
					sel_ai_scale[1:0]: scaling factor
							0x0: 1
							0x1: 1/2
							0x2: 1/4
							0x3: 1/8

 *************************************************************************************/
#define    anareg_ain_scale        		0xFA
enum{
	FLD_ADC_ITRIM_PREAMP 	= BIT_RNG(0,1),
	FLD_ADC_ITRIM_VREFBUF	= BIT_RNG(2,3),
	FLD_ADC_ITRIM_VCMBUF	= BIT_RNG(4,5),

	FLD_SEL_AIN_SCALE 		= BIT_RNG(6, 7),
};

typedef enum{
	ADC_PRESCALER_1,
	ADC_PRESCALER_1F2,
	ADC_PRESCALER_1F4,
	ADC_PRESCALER_1F8
}ADC_PreScalingTypeDef;


typedef enum{
	 ADC_CUR_TRIM_PER75,
	 ADC_CUR_TRIM_PER100,
	 ADC_CUR_TRIM_PER125,
	 ADC_CUR_TRIM_PER150
}CUrrent_TrimTypeDef;

static inline void adc_set_itrim_preamp(CUrrent_TrimTypeDef bias)
{
	analog_write(anareg_ain_scale, (analog_read(anareg_ain_scale)&(~FLD_ADC_ITRIM_PREAMP)) | (bias<<0) );
}

static inline void adc_set_itrim_vrefbuf(CUrrent_TrimTypeDef bias)
{
	analog_write(anareg_ain_scale, (analog_read(anareg_ain_scale)&(~FLD_ADC_ITRIM_VREFBUF)) | (bias<<2) );
}

static inline void adc_set_itrim_vcmbuf(CUrrent_TrimTypeDef bias)
{
	analog_write(anareg_ain_scale, (analog_read(anareg_ain_scale)&(~FLD_ADC_ITRIM_VCMBUF)) | (bias<<4) );
}


/**************************************************************************************
afe_0xFB
    BIT<5:4>  pga_itrim_boost_l
    BIT<7:6>  pga_itrim_boost_r
    			0x0 :   75%
				0x1 :   100%
				0x2 :   125%
				0x3 :   150%

afe_0xFC
    BIT<1:0>  pga_itrim_gain_l
    BIT<3:2>  pga_itrim_gain_r
    			0x0 :   75%
				0x1 :   100%
				0x2 :   125%
				0x3 :   150%

    BIT<4>	  adc_mode
    					0: normal mode
						1: rsvd (RNS mode)
	BIT<5>	  adc_pd, Power down ADC
						1: Power down
						0: Power up
	BIT<6>    pga_pd_l, power down left channel pga
						1: Power down
						0: Power up
	BIT<7>    pga_pd_r, power down right channel pga
						1: Power down
						0: Power up
 *************************************************************************************/
enum{                                              //ana_0xFB
	FLD_PGA_CAP_TRIM_EN_L		= BIT(0),
	FLD_PGA_CAP_TRIM_EN_R		= BIT(1),
	FLD_PGA_ITRIM_BOOST_L		= BIT_RNG(4,5),
	FLD_PGA_ITRIM_BOOST_R		= BIT_RNG(6,7),
};

#define anareg_adc_pga_ctrl				0xFC
enum{                                              //ana_0xFC
	FLD_PGA_ITRIM_GAIN_L		= BIT_RNG(0,1),
	FLD_PGA_ITRIM_GAIN_R		= BIT_RNG(2,3),
	FLD_ADC_MODE				= BIT(4),     //0 for  normal mode
	FLD_SAR_ADC_POWER_DOWN 		= BIT(5),
	FLD_POWER_DOWN_PGA_CHN_L 	= BIT(6),
	FLD_POWER_DOWN_PGA_CHN_R 	= BIT(7),
};

typedef enum{
	NORMAL_MODE      = 0,
	RNS_MODE         = BIT(4),
}ADCModeTypeDef;


/**
 * Name     :adc_set_mode
 * Function :Set the source and mode of the random number generator
 * Input    :unsigned char stat
 *          :RNG_SrcTypeDef|RNG_UpdataTypeDef
 * return   :None
 */
static inline void adc_set_mode(ADCModeTypeDef adc_m)
{
	analog_write (anareg_adc_pga_ctrl, (analog_read(anareg_adc_pga_ctrl)&(~FLD_ADC_MODE)) | adc_m);

}

//on _off = 1 : power on   sar_adc
//on _off = 0 : power off  sar_adc
static inline void adc_power_on_sar_adc(unsigned char on_off)
{
	analog_write (anareg_adc_pga_ctrl, (analog_read(anareg_adc_pga_ctrl)&(~FLD_SAR_ADC_POWER_DOWN)) | (!on_off)<<5  );
}

//on _off = 1 : power on   pga_left_chn
//on _off = 0 : power off  pga_left_chn
static inline void pga_left_chn_power_on(unsigned char on_off)
{
	analog_write (anareg_adc_pga_ctrl, (analog_read(anareg_adc_pga_ctrl)&(~FLD_POWER_DOWN_PGA_CHN_L)) | (!on_off)<<6 );
}

//on _off = 1 : power on   pga_right_chn
//on _off = 0 : power off  pga_right_chn
static inline void pga_right_chn_power_on(unsigned char on_off)
{
	analog_write (anareg_adc_pga_ctrl, (analog_read(anareg_adc_pga_ctrl)&(~FLD_POWER_DOWN_PGA_CHN_R)) | (!on_off)<<7 );
}

typedef enum{
	GAIN_STAGE_BIAS_PER75 = 0,
	GAIN_STAGE_BIAS_PER100,
	GAIN_STAGE_BIAS_PER125,
	GAIN_STAGE_BIAS_PER150,
}Gain_BiasTypeDef;

static inline void adc_set_left_boost_bias(Gain_BiasTypeDef bias)
{
	analog_write(0xFB, (analog_read(0xFB)&(~FLD_PGA_ITRIM_BOOST_L)) | (bias<<4) | FLD_PGA_CAP_TRIM_EN_L );
}

static inline void adc_set_right_boost_bias(Gain_BiasTypeDef bias)
{
	analog_write(0xFB, (analog_read(0xFB)&(~FLD_PGA_ITRIM_BOOST_R)) | (bias<<6) |  FLD_PGA_CAP_TRIM_EN_R);
}

static inline void adc_set_left_gain_bias(Gain_BiasTypeDef bias)
{
	analog_write(0xFC, (analog_read(0xFC)&(~FLD_PGA_ITRIM_GAIN_L)) | (bias) );
}

static inline void adc_set_right_gain_bias(Gain_BiasTypeDef bias)
{
	analog_write(0xFC, (analog_read(0xFC)&(~FLD_PGA_ITRIM_GAIN_R)) | (bias<<2) );
}

/********************************	configure set state	  ****************************/

/********************************	configure capture state	  ****************************/

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

void adc_set_state_length(unsigned short R_max_mc, unsigned short R_max_c,unsigned char R_max_s);

void adc_set_ref_voltage(ADC_ChTypeDef ch_n, ADC_RefVolTypeDef v_ref);

void adc_set_resolution(ADC_ChTypeDef ch_n, ADC_ResTypeDef v_res);

void adc_set_tsample_cycle(ADC_ChTypeDef ch_n, ADC_SampCycTypeDef adcST);

void adc_set_input_mode(ADC_ChTypeDef ch_n,  ADC_InputModeTypeDef m_input);

void adc_set_ain_channel_differential_mode(ADC_ChTypeDef ch_n, ADC_InputPchTypeDef InPCH,ADC_InputNchTypeDef InNCH);

void adc_set_ain_pre_scaler(ADC_PreScalingTypeDef v_scl);

unsigned short ADC_SampleValueGet(void);


#else
/**
  ******************************************************************************
  * @File: 		 adc.h
  * @CreateDate: 2018-6-13
  * @Author:     qiu.gao@telink-semi.com
  * @Copyright:  (C)2018-2020 telink-semi Ltd.co
  * @Brief:
  ******************************************************************************
  */
#ifndef ENABLE
	#define ENABLE       1
#endif

#ifndef DISABLE
	#define DISABLE      0
#endif

#define ADC_BATTERY_CHECK_PIN    GPIO_PB3
#define ADC_BATTERY_VOL_OK       0x00
#define ADC_BATTERY_VOL_LOW      0x01

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

void ADC_ClockSet(unsigned int ADC_Clk_Hz);
void ADC_ClockSetEx(unsigned char ADC_ClkDiv);
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
void ADC_StateNumSet(unsigned char ADC_StateNum);
//void ADC_InputVolDivFactor(eADC_InputVolDivFactorTypeDef ADC_InputVolDivFactor);
void ADC_InputVolDivFactor(eADC_VrefTypeDef ADC_Vref, eADC_InputVolDivFactorTypeDef ADC_InputVolDivFactor);

void ADC_SetStateTimeSet(unsigned char ADC_SetTime);
void ADC_CaptureTimeSet(eADC_ModuleTypeDef ADC_Module,unsigned short ADC_CaptureTime);
void ADC_SampleRateSet(eADC_ModuleTypeDef ADC_Module,unsigned char ADC_SetTime,unsigned short ADC_CaptureTime);

void ADC_Cmd(eADC_ModuleTypeDef ADC_Module, unsigned char ADC_Enable);

unsigned short ADC_GetConvertValue(void);

#endif

#endif/* End of ADC_H */
/*----------------------------------File End----------------------------------*/



