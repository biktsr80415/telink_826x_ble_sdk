#include "audio.h"
#include "pga.h"
#include "adc.h"
#include "irq.h"
#include "register.h"
#include "dfifo.h"

#if 1

/**
 *
 */
#define DFIFO_BUFF_ADDR    			READ_REG16(0xb00)
#define DFIFO_BUFF_SIZE    			READ_REG8(0xb02)

#define REG_CLK2_EN     			REG_ADDR8(0x65)


/*******************************************************************
 *
 */
enum{
	I2S_STEP    = BIT_RNG(0,6),
	I2S_CLK_EN  = BIT(7),
};

#define     REG_I2S_STEP          	READ_REG8(0x67)
#define		SET_I2S_STEP(v)			do{\
										REG_I2S_STEP &= (~I2S_STEP);\
										REG_I2S_STEP |= (unsigned char)(v)&0x7F;\
										REG_I2S_STEP |= I2S_CLK_EN;\
									}while(0)
enum{
	I2S_MODE    = BIT_RNG(0,7),
};

#define     REG_I2S_MODE          	READ_REG8(0x68)
#define		SET_I2S_MODE(v)			do{	\
										REG_I2S_MODE = (unsigned char)(v);\
									}while(0)

#define		SET_I2S_PIN_EN()		do{	\
										unsigned short val  = READ_REG8(0x59e);\
										val &= 0xc3;\
										WRITE_REG8(0x59e,val);\
										val = READ_REG16(0x5ae);\
										val &= 0xf00f;\
										val |= 0x0550;\
										WRITE_REG16(0x5ae,val);\
									}while(0)


/*******************************************************************
 *
 */
#define     REG_DMIC_STEP          	READ_REG8(0x6c)
enum{
	DMIC_STEP = BIT_RNG(0,6),
	DMIC_CLK_EN = BIT(7),
};
#define     SET_DMIC_STEP(v)		do{\
										REG_DMIC_STEP &= (~DMIC_STEP);\
										REG_DMIC_STEP |= (v)&0x7f;\
										REG_DMIC_STEP |= DMIC_CLK_EN;\
									}while(0)

/*******************************************************************
 *
 */
#define     REG_DMIC_MODE           READ_REG8(0x6d)
enum{
	DMIC_MODE = BIT_RNG(0,7),
};

#define     SET_DMIC_MODE(v)		do{\
										REG_DMIC_MODE = (unsigned char)(v);\
									}while(0)

#define     SET_DMIC_PIN_EN()		do{\
										unsigned char val= READ_REG8(0x586);\
										val &= 0xe7;\
										WRITE_REG8(0x586,val);\
										val = READ_REG8(0x5a8);\
										val &= 0x3f;\
										WRITE_REG8(0x5a8,val|0x40);\
										val = READ_REG8(0x5a9);\
										val &= 0xfc;\
										WRITE_REG8(0x5a9,val|0x01);\
									}while(0)

#define		SET_SDM_PIN_EN()		do{\
										unsigned char val= READ_REG8(0x58e);\
										val &= 0xf3;\
										WRITE_REG8(0x58e,val);\
										val= READ_REG8(0x5aa);\
										val &= 0x0f;\
										WRITE_REG8(0x5aa,val|0x50);\
									}while(0)

enum{
	SDM_PWM_MULTIPLY2		= BIT(0),
	SDM_PWM_ENABLE			= BIT(1),
	SDM_LINER_INTERPOLATE_EN= BIT(2),
	SDM_LEFT_SHAPING_EN 	= BIT(5),
	SDM_RIGTH_SHAPING_EN 	= BIT(6)
};
#define		REG_SDM_PWM_CTRL		 	REG_ADDR8(0x563)




#define		SET_SDM_PN_SHAPING(v)  do{\
										unsigned char val= READ_REG8(0x563);\
										val &= 0xdf;\
										val |=(unsigned char)(v);\
										WRITE_REG8(0x563,val);\
									}while(0)


#define		REG_ASCL_TUNE				REG_ADDR32(0x564)

enum{
	PN1_LEFT_CHN_BITS 	= BIT_RNG(0,4),
	PN2_LEFT_CHN_EN	  	= BIT(5),
	PN1_LEFT_CHN_EN   	= BIT(6),
};

#define		REG_PN1_LEFT				REG_ADDR8(0x568)
#define		SET_PN1_LEFT_BITS(v)		do{\
											unsigned char val= REG_ADDR8(0x568);\
											val &= 0xe0;\
											val |=(unsigned char)(v&0x1f);\
											WRITE_REG8(0x568,val);\
										}while(0)

enum{
	PN2_LEFT_CHN_BITS 	= BIT_RNG(0,4),
	PN2_RIGHT_CHN_EN	= BIT(5),
	PN1_RIGHT_CHN_EN   	= BIT(6),
};

#define		REG_PN2_LEFT				REG_ADDR8(0x569)

#define		SET_PN2_LEFT_BITS(v)		do{\
											unsigned char val= REG_ADDR8(0x569);\
											val &= 0xe0;\
											val |=(unsigned char)(v&0x1f);\
											WRITE_REG8(0x569,val);\
										}while(0)
enum{
	PN1_RIGHT_CHN_BITS 	= BIT_RNG(0,4),
	EXCHANGE_SDM_DATA_EN= BIT(5),
};

#define		REG_PN1_RIGHT				REG_ADDR8(0x56a)

#define		SET_PN1_RIGHT_BITS(v)		do{\
											unsigned char val= REG_ADDR8(0x56a);\
											val &= 0xe0;\
											val |=(unsigned char)(v&0x1f);\
											WRITE_REG8(0x56a,val);\
										}while(0)

enum{
	PN2_RIGHT_CHN_BITS 		= BIT_RNG(0,4),
	SDM_LEFT_CHN_CONST_EN	= BIT(5),
	SDM_RIGHT_CHN_CONST_EN	= BIT(6),
};

#define		REG_SDM_PN_GENREATOR		REG_ADDR8(0x56b)
#define		REG_PN2_RIGHT				REG_ADDR8(0x56b)

#define		SET_PN2_RIGHT_BITS(v)		do{\
											unsigned char val= REG_ADDR8(0x56b);\
											val &= 0xe0;\
											val |=(unsigned char)(v&0x1f);\
											WRITE_REG8(0x56b,val);\
										}while(0)

enum{
	INCREASE_STEP = BIT_RNG(0,3),
	DECREASE_STEP = BIT_RNG(4,7),
};

#define		ALC_VOL_STEP				REG_ADDR8(0xb4c)

enum{
	VOL_TICK_L    = BIT_RNG(0,7),
	VOL_TICK_H    = BIT_RNG(0,7),
};

#define		ALC_VOL_TICK				REG_ADDR16(0xb50)

/**
 *
 */
#define     ALC_VOL_REG         		READ_REG8(0xb06)
enum{
	MANUAL_VOL_VAL  = BIT_RNG(0,6),
	MANUAL_VOL_EN   = BIT(7),
};
#define     SET_MANUAL_VOL(v) 			do{\
											ALC_VOL_REG &= ~MANUAL_VOL_VAL;\
											ALC_VOL_REG &= ~MANUAL_VOL_EN;\
											ALC_VOL_REG |= ((v)&0x7f);\
										 }while(0)
/**
 *
 */
#define    SDM_VOL_CTRL         READ_REG8(0x562)
enum{
	SDM_VOL_ADD_QUARTER  = BIT(0),
	SDM_VOL_ADD_HALF     = BIT(1),
	SDM_SHIFT_LEFT       = BIT_RNG(2,6),
	SDM_MUTE_EN          = BIT(7),
};
#define    SET_SDM_VOL(v)	do{\
								BIT_CLR(SDM_VOL_CTRL,BIT_RNG(0,6));\
								SDM_VOL_CTRL |= ((v)&0x7f);\
							}while(0)


unsigned char AMIC_ADC_SampleLength[3] = {0xf0/*96K*/,0xab/*132K*/,0x75/*192K*/};

unsigned char DMIC_CLK_Mode[RATE_SIZE] = {47/*8	k	1021.277*/,	47/*16k	1021.277*/,
										  34/*22k	1411.765*/,	47/*32k	1021.277*/,
										  34/*44k	1411.765*/,	32/*48k	1500	*/,
										  32/*96k	1500	*/};

unsigned char DMIC_CIC_Rate[RATE_SIZE] = {0x23/*8k 7.978723		CIC_MODE 0*/,	0x22/*16k 15.95745	CIC_MODE 0*/,
										  0x32/*22k 22.05882	CIC_MODE 0*/,	0x11/*32k 31.91489	CIC_MODE 0*/,
										  0x11/*44k 44.11765	CIC_MODE 0*/,	0x11/*48k 46.875	CIC_MODE 0*/,
										  0x00/*96k 93.75 		CIC_MODE 0*/};

unsigned char AMIC_CIC_Rate[RATE_SIZE] = {0xab/*8k  96/12	*/,	0x85/*16k 96/6	*/,
										  0x85/*22k 132/6	*/,	0x42/*32k 96/3	*/,
										  0x42/*44k 132/3	*/,	0x31/*48k 96/2	*/,
										  0x20/*96k			*/};

/*  matrix used under condition: I2S = 2 MHz  */
unsigned long ASDM_Rate_Matching[RATE_SIZE] = {0x00832001/*8k */,0x01063001/*16k*/,
											   0x01687001/*22k*/,0x020C5001/*32k*/,
											   0x02D4D001/*44k*/,0x03127001/*48k*/,
											   0x0624d001/*96k*/};

/*  matrix used under condition: I2S = 2 MHz  */
unsigned long DSDM_Rate_Matching[RATE_SIZE] = {0x00820001/*8 k*/,0x01058001/*16k*/,
											   0x01697001/*22k*/,0x020AF001/*32k*/,
											   0x02D2E001/*44k*/,0x03000001/*48k*/,
											   0x06000001/*96k*/};

/**
 * @brief     set the mic buffer's address and size
 * @param[in] Pro_M - the first address of SRAM buffer to store MIC data.
 * @param[in] HPF_ADJ - the size of pbuff.
 * @return    none
 */
void audio_set_alc_hpf_lpf(Audio_ALC_HPF_LPF_Typedef Pro_M,unsigned char HPF_ADJ)
{
	REG_AUDIO_ALC_HPF_LPF_CTRL  &= 0x00;
	REG_AUDIO_ALC_HPF_LPF_CTRL  |= (HPF_ADJ&0x0f);
	REG_AUDIO_ALC_HPF_LPF_CTRL  |= Pro_M;
}
/**
 * @Brief:  Audio initialiaztion.
 * @Param:  AUDIO_Rate ->
 * @Param:  audioBuf   -> pointer point to buffer of audio data.
 * @Param:  bytesLen   -> byte size of buffer of audio data.
 * @Return: None.
 */
void audio_amic_init(AUDIO_RateTypeDef Audio_Rate)
{
	/*******1.ADC setting for analog audio sample**************************/
	set_ana_ldo_trim(LDO_OUT_VOLT_TRIM_1P9);
	adc_set_atb(ADC_SEL_ATB_1);//防止低温抖动
	adc_reset_adc_module();   //reset whole digital adc module
	adc_enable_clk_24m_to_sar_adc(1);  //enable signal of 24M clock to sar adc

	adc_set_sample_clk(5); //adc sample clk= 24M/(1+5)=4M

	//adc state machine state cnt 2( "set" stage and "capture" state for left channel)
	adc_set_max_state_cnt(0x02);

	//set "capture state" length for misc channel: 240
	//set "set state" length for misc channel: 10
	//adc state machine  period  = 24M/250 = 96K
	adc_set_length_set_state(8);									//max_s
	if((Audio_Rate == AUDIO_44K)||(Audio_Rate == AUDIO_22K))
	{
		adc_set_length_capture_state_for_chn_left_right(AMIC_ADC_SampleLength[1]);	//max_c	132K
	}
	else
	{
		adc_set_length_capture_state_for_chn_left_right(AMIC_ADC_SampleLength[2]);	//max_c	192K
	}

	adc_set_chn_enable(ADC_LEFT_CHN);    								//left channel enable
	adc_set_input_mode(ADC_LEFT_CHN, DIFFERENTIAL_MODE);  				//left channel differential mode
	adc_set_ain_channel_differential_mode(ADC_LEFT_CHN, PGA0P, PGA0N);  //left channel positive and negative data in

	adc_set_ref_voltage(ADC_LEFT_CHN, ADC_VREF_1P2V);					//left channel vref
	adc_set_resolution(ADC_LEFT_CHN, RES14);							//left channel resolution
	adc_set_tsample_cycle(ADC_LEFT_CHN, SAMPLING_CYCLES_3);				//left channel tsample

	adc_set_ain_pre_scaler(ADC_PRESCALER_1);                            //ain pre scaler none

	adc_set_itrim_preamp(ADC_CUR_TRIM_PER75);
	adc_set_itrim_vrefbuf(ADC_CUR_TRIM_PER100);
	adc_set_itrim_vcmbuf(ADC_CUR_TRIM_PER100);

	//PGA0 left  1: B4/B5；0:A7/B0 ,
	analog_write(anareg_adc_pga_sel_vin, MASK_VAL(FLD_PGA_SEL_VIN_LEFT_P, PGA_AIN_A7_B0));

	adc_set_left_boost_bias(GAIN_STAGE_BIAS_PER75);

	analog_write (anareg_adc_pga_ctrl, MASK_VAL( FLD_PGA_ITRIM_GAIN_L, GAIN_STAGE_BIAS_PER100,
												 FLD_ADC_MODE, 0,
												 FLD_SAR_ADC_POWER_DOWN, 0,
												 FLD_POWER_DOWN_PGA_CHN_L, 0));

	WriteAnalogReg(0xfe,0x05);					//0xfe default value is 0xe5,for output audio, mast claer 0xfe<7:5>

	adc_power_on_sar_adc(1);   //power on sar adc

	PGA_SetGain(PGA_PreGain_26dB,PGA_PostGain_0dB);

	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//Enable HPF, Enable LPF, Disable ALC, Enable double_down_sampling
	reg_audio_alc_hpf_lpf_en = MASK_VAL( FLD_AUD_IN_HPF_SFT,    0x0b,   //different pcb may set different value.
										 FLD_AUD_IN_HPF_BYPASS, 0,
										 FLD_AUD_IN_ALC_BYPASS, 1,
										 FLD_AUD_IN_LPF_BYPASS, 0,
										 FLD_DOUBLE_DOWN_SAMPLING_ON, 1);
	//ALC mode select digital mode
	reg_audio_alc_cfg &= ~FLD_AUDIO_ALC_ANALOG_AGC_EN;
	//ALC left channel select manual regulate, and set volume
	reg_audio_alc_vol_l = MASK_VAL( FLD_AUDIO_ALC_VOL_VALUE,  0x24,
									FLD_AUDIO_ALC_AUTO_MODE_EN, 0);


	//2. Dfifo setting
	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()
	reg_dfifo_mode = FLD_AUD_DFIFO0_IN;

	//amic input, mono mode, enable decimation filter
	reg_audio_input_select = MASK_VAL(FLD_AUDIO_DMIC_CLK_RISING_EDGE,AUDIO_DMIC_DATA_IN_FALLING_EDGE,
									  FLD_AUDIO_DMIC_DISABLE,1,
									  FLD_AUDIO_INPUT_SELECT, AUDIO_INPUT_AMIC,
									  FLD_AUDIO_DEC_DISABLE, 0);

	reg_audio_dec_mode |= FLD_AUD_LNR_VALID_SEL | FLD_AUD_CIC_MODE;
	reg_audio_dec = 0x65;  // 96k/3 = 32k, down sampling to 16K by set core_b40<7>
}


void  audio_dmic_init (AUDIO_RateTypeDef Audio_Rate,unsigned int MicBufSize)
{
	/*******1.Dmic setting for audio input**************************/
	reg_audio_ctrl = 0x00;
//	REG_AUDIO_DEC_RATIO = DMIC_CIC_Rate[Audio_Rate];
	REG_AUDIO_DEC_RATIO = 0x3a;//32K
	//write_reg8(0x800b40,0xfb);
	REG_AUDIO_DFIFO_MODE = AUDIO_DFIFO0_IN|AUDIO_DFIFO0_OUT|AUDIO_DFIFO0_L_INT;
	REG_CLK2_EN |= FLD_CLK2_DFIFO_EN; 		//enable the DFIFO clock

	REG_AUDIO_DFIFO0_LOW_THRESHOLD = (MicBufSize/64)-1;
	REG_AUDIO_DFIFO0_HIGH_THRESHOLD= (MicBufSize/64*3)-1;

	SET_AUDIO_INPUT_CTRL(AUDIO_INPUT_DMIC);
	SET_AUDIO_ALC_LEFT_CHN_MIN_VOL(0x28);

	SET_DMIC_PIN_EN();
	SET_DMIC_STEP(0x82);						//set dmic step  0x6c<6:0>  <7>enable
	SET_DMIC_MODE(DMIC_CLK_Mode[Audio_Rate]);   //set dmic mode  0x6d<7:0>

	reg_audio_alc_hpf_lpf_en = MASK_VAL( FLD_AUD_IN_HPF_SFT,    0x0b,   //different pcb may set different value.
										 FLD_AUD_IN_HPF_BYPASS, 0,
										 FLD_AUD_IN_ALC_BYPASS, 0,
										 FLD_AUD_IN_LPF_BYPASS, 0,
										 FLD_DOUBLE_DOWN_SAMPLING_ON, 1);//down sample 1:enable -> /2; 0:disable
}

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
void audio_sdm_output_set(AudioInput_Typedef InType,AUDIO_RateTypeDef Audio_Rate, unsigned char audio_out_en)
{
	if(audio_out_en)
	{
		SET_SDM_PIN_EN();
		SET_SDM_PN_SHAPING(0);
		REG_SDM_PWM_CTRL &= (unsigned char)(~(SDM_PWM_MULTIPLY2|SDM_PWM_ENABLE|SDM_LINER_INTERPOLATE_EN));
		SET_I2S_STEP(1);
		SET_I2S_MODE(24);
		if(InType==AMIC)
		{
			REG_ASCL_TUNE = ASDM_Rate_Matching[Audio_Rate];
		}
		else
		{
			REG_ASCL_TUNE = DSDM_Rate_Matching[Audio_Rate];
		}

		SET_PN1_LEFT_BITS(6);
		REG_PN1_LEFT &= (unsigned char)(~(PN2_LEFT_CHN_EN|PN1_LEFT_CHN_EN));
		SET_PN2_LEFT_BITS(6);
		REG_PN2_LEFT &= (unsigned char)(~(PN2_RIGHT_CHN_EN|PN1_RIGHT_CHN_EN));

		REG_PN1_RIGHT &= (unsigned char)(~(EXCHANGE_SDM_DATA_EN));
		REG_PN2_RIGHT &= (unsigned char)(~(SDM_LEFT_CHN_CONST_EN));

	    reg_audio_ctrl = FLD_AUDIO_SDM_PLAYER_EN;
	}
	else
	{
		reg_audio_ctrl = 0x00;
	}
}


#else

/* Audio rate table */
AUDIO_SampleRateTypdef audioSampleRate[]={
	{0x85, 0x0a, 0x00f0},/*Audio_Rate:16k = ADC_Rate/(5+1); ADC_Rate:96K  */
	{0x85, 0x0a, 0x00ab},/*Audio_Rate:22k = ADC_Rate/(5+1); ADC_Rate:132K */
	{0x42, 0x0a, 0x00f0},/*Audio_Rate:32k = ADC_Rate/(2+1); ADC_Rate:96K  0x42 */
	{0x42, 0x0a, 0x00ab},/*Audio_Rate:44k = ADC_Rate/(2+1); ADC_Rate:132K */
	{0x31, 0x0a, 0x00f0},/*Audio_Rate:48k = ADC_Rate/(1+1); ADC_Ra6e:96K  */
	{0x20, 0x0a, 0x00f0},/*Audio_Rate:96k = ADC_Rate/(0+1); ADC_Ra6e:96K  */
};

/**
 * @Brief:  Set audio input source.
 * @Param:  AUDIO_Input ->
 * @Return: None.
 */
void AUDIO_SetAudioInput(eAUDIO_InputTypeDef AUDIO_Input)
{
	reg_audio_input_select &= ~FLD_AUDIO_INPUT_SELECT;
	reg_audio_input_select |= (AUDIO_Input << 2);
}

/**
 * @Brief:  Set Dec filter down ratio.
 * @Param:  AUDIO_DecDownRatio ->
 * @Return: None.
 */
void AUDIO_SetDecDownRatio(eAUDIO_DecDownRatioTypeDef AUDIO_DecDownRatio)
{
	reg_audio_dec &= ~FLD_AUDIO_DEC_DOWN_RATIO;
	reg_audio_dec |= (AUDIO_DecDownRatio & 0x0f);
}

/**
 * @Brief:  Set shift of Dec filter.
 * @Param:  shiftValue ->
 * @Return: None.
 */
void AUDIO_SetDecShift(unsigned char shiftValue)
{
	reg_audio_dec &= ~FLD_AUDIO_DEC_SHIFT;
	reg_audio_dec |= (shiftValue << 4);
}

/**
 * @Brief:  Set shift and down ratio of Dec filter.
 * @Param:  value ->
 * @Return: None.
 */
void AUDIO_SetDecDownRateAndShift(unsigned char value)
{
	reg_audio_dec = value;
}

/**
 * @Brief:  Enable/Disable Dec filter module.
 * @Param:  enable -> can be ENABLE / DISABLE.
 * @Return: None.
 */
void AUDIO_SetDecFilter(unsigned char enable)
{
	if(enable){
		reg_audio_input_select &= ~FLD_AUDIO_DEC_DISABLE;
	}else{
		reg_audio_input_select |= FLD_AUDIO_DEC_DISABLE;
	}
}

/**
 * @Brief:  Set Dec filter module.
 * @Param:  AUDIO_DecDownRatio ->
 * @Param:  shiftValue         ->
 * @Param:  enable             -> can be ENABLE / DISABLE.
 * @Return: None.
 */
void AUDIO_SetDecFilterEx(eAUDIO_DecDownRatioTypeDef AUDIO_DecDownRatio,
                          unsigned char shiftValue, unsigned char enable)
{
	AUDIO_SetDecDownRatio(AUDIO_DecDownRatio);
	AUDIO_SetDecShift(shiftValue);
	AUDIO_SetDecFilter(enable);
}

/**
 * @Brief:  Enable/Disable Low Pass Filter(LPF).
 * @Param:  enable  -> can be ENABLE / DISABLE.
 * @Return: None.
 */
void AUDIO_SetLPF(unsigned char enable)
{
	if(enable){
		reg_audio_alc_hpf_lpf_en &= ~FLD_AUDIO_LPF_DISABLE;
	}else{
		reg_audio_alc_hpf_lpf_en |= FLD_AUDIO_LPF_DISABLE;
	}
}

/**
 * @Brief:  Enable/Disable down sample module.
 * @Param:  enable -> can be ENABLE / DISABLE.
 * @Return: None.
 */
void AUDIO_SetDownSample(unsigned char enable)
{
	if(enable){
		reg_audio_alc_hpf_lpf_en |= FLD_AUDIO_DOWN_SAMPLE_ENABLE;//Down ratio: 1/2
	}else{
		reg_audio_alc_hpf_lpf_en &= ~FLD_AUDIO_DOWN_SAMPLE_ENABLE;//Down ratio: 1
	}
}

/**
 * @Brief:  Enable/Disable High Pass Filter(HPF).
 * @Param:  shift  ->
 * @Param:  enable -> can be ENABLE / DISABLE.
 * @Return: None.
 */
void AUDIO_SetHPF(unsigned char shift, unsigned char enable)
{
	if(enable){
		reg_audio_alc_hpf_lpf_en &= ~(FLD_AUDIO_HPF_ADJUST|FLD_AUDIO_HPF_DISABLE);
		reg_audio_alc_hpf_lpf_en |= (shift & 0x0f);
	}else{
		reg_audio_alc_hpf_lpf_en |= FLD_AUDIO_HPF_DISABLE;
	}
}

/**
 * @Brief:  Enable/Disable ALC moudle.
 * @Param:  enable -> can be ENABLE / DISABLE.
 * @Return: None.
 */
void AUDIO_SetALC(unsigned char enable)
{
	if(enable){
		reg_audio_alc_hpf_lpf_en &= ~FLD_AUDIO_ALC_DISABLE;
	}else{
		reg_audio_alc_hpf_lpf_en |= FLD_AUDIO_ALC_DISABLE;
	}
}

/**
 * @Brief:  Enable/Disable Biquad moudle.
 * @Param:  AUDIO_Biquad  ->
 * @Param:  shiftValue    ->
 * @Return: None.
 */
void AUDIO_SetBiquad(eAUDIO_BiquadTypeDef AUDIO_Biquad,unsigned char shiftValue)
{
	unsigned char temp = (shiftValue & 0x07);
	temp |= (unsigned char)AUDIO_Biquad;
	reg_audio_biquad_cfg = temp;
}

/**
 * @Brief:  Set Audio ouput path.
 * @Param:  AUDIO_Output ->
 * @Return: None.
 */
void AUDIO_SetAudioOutputPath(eAUDIO_OutputTypeDef AUDIO_Output)
{
	reg_audio_ctrl = (AUDIO_Output | FLD_AUDIO_MONO_MODE);
}

/**
 * @Brief:  Audio initialiaztion.
 * @Param:  AUDIO_Rate ->
 * @Param:  audioBuf   -> pointer point to buffer of audio data.
 * @Param:  bytesLen   -> byte size of buffer of audio data.
 * @Return: None.
 */
void AUDIO_AmicInit(eAUDIO_RateTypeDef AUDIO_Rate, short *audioBuf, unsigned int bytesLen)
{
	/* Audio initialization --------------------------------------------------*/
	//Set Audio Output path
	AUDIO_SetAudioOutputPath(AUDIO_Output_Off);

	/* DFIFO Initialization */
	CLOCK_EN->ClkEn2.Bit.DFIFO_Bit = 1;
	dfifo_set_mode(DFIFO_Mode_FIFO0_Input);//DFIFO_Mode_FIFO0_Output/DFIFO_Mode_FIFO0_Low_Irq_EN
	dfifo_config_dfifo0(audioBuf, bytesLen);

	//Set audio input source
	AUDIO_SetAudioInput(AUDIO_Input_AMIC);

	//Set decimation filter
	AUDIO_SetDecDownRateAndShift(audioSampleRate[AUDIO_Rate].CIC);
	AUDIO_SetDecFilter(ENABLE);

	//Set LPF/Down Sample/HPF/ALC
	AUDIO_SetLPF(DISABLE);
	AUDIO_SetDownSample(ENABLE);
	AUDIO_SetHPF(0x06,DISABLE);//0x0b 这个值取决于PCB版
	AUDIO_SetALC(DISABLE);

	//reg_audio_alc_cfg &= ~FLD_AUDIO_ALC_ANALOG_AGC_EN;
	//reg_audio_alc_vol_l = MASK_VAL(FLD_AUDIO_ALC_AUTO_MODE_EN,0,FLD_AUDIO_ALC_VOL_VALUE,0x26);

	//Set Biquad filter
	//AUDIO_SetBiquad(AUDIO_Biquad_None,0x00);

	/* ADC initialization ----------------------------------------------------*/
	ADC_ClockSet(3000000);//3MHz

	//Clear ADC Enable register.
	analog_write(ADC_ENABLE_ADDR,0x00);//Must

	ADC_VrefSet(ADC_Vref_0p9);
	ADC_InputVolDivFactor(ADC_Vref_0p9, ADC_Input_Vol_Div_Factor_1);
	ADC_ModeSet(ADC_Module_L,ADC_Mode_Diff);
	ADC_DifferChannelSet(ADC_Module_L,ADC_Channel_PGA_P,ADC_Channel_PGA_N);
	ADC_DifferChannelSet(ADC_Module_M, ADC_Channel_None,ADC_Channel_None);
	ADC_ResolutionSet(ADC_Module_L,ADC_Resolution_14b);
	ADC_SetStateTimeSet(ADC_Stable_Time_6Cyc);
	ADC_SampleRateSet(ADC_Module_L|ADC_Module_M,audioSampleRate[AUDIO_Rate].setLen,
								   audioSampleRate[AUDIO_Rate].captureLen);
	ADC_Cmd(ADC_Module_L, ENABLE);

	/* PGA Settings  ---------------------------------------------------------*/
	PGA_PowerOn();
	PGA_SetInputChannel(PGA_Input_Channel_PA7PB0);

	//Set pga_itrim_boost_l as 100%
	unsigned char temp = analog_read(0x80+123);
	temp &= 0xcf;
	temp |= (0x01<<4);
	analog_write(0x80+123,temp|0x01);

	//WriteAnalogReg(0x80+125,0x00);//PGA set PAG channel PA7 PB0
	WriteAnalogReg(0x80+126,0x05);//bit(7)
	PGA_SetGain(PGA_PreGain_26dB,PGA_PostGain_3dB);//29dB
}
#endif



