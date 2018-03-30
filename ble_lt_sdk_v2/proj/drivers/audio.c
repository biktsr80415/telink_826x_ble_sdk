
/**************************************************************************************************
  Filename:       	audio.c
  Author:			junjun.xu@telink-semi.com
  Created Date:	2016/06/06

  Description:    This file contains the audio driver functions for the Telink 8267.


**************************************************************************************************/

#include "../../proj/tl_common.h"
#include "audio.h"
#include "pga.h"
#include "adc.h"





unsigned char AMIC_ADC_SampleLength[2] = {0xf0/*96K*/,0xab/*132K*/};

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




























enum {
	AUD_DMIC,
	AUD_AMIC,
};

#define SET_PFM(v)     do{\
                           BM_CLR(reg_adc_period_chn0,FLD_ADC_CHNM_PERIOD);\
	              	   	   reg_adc_period_chn0 |= MASK_VAL(FLD_ADC_CHNM_PERIOD,v);\
                       }while(0)
#define SET_PFL(v)     do{\
                           BM_CLR(reg_adc_period_chn12,FLD_ADC_CHNLR_PERIOD);\
                           reg_adc_period_chn12 |= MASK_VAL(FLD_ADC_CHNLR_PERIOD,v);\
                       }while(0)
/**
 * @brief     configure the mic buffer's address and size
 * @param[in] pbuff - the first address of SRAM buffer to store MIC data.
 * @param[in] size_buff - the size of pbuff.
 * @return    none
 */
void audio_config_mic_buf(signed short* pbuff,unsigned char size_buff)
{
	reg_dfifo0_addr = pbuff;
	reg_dfifo0_size = (size_buff>>4)-1;
}
/**
 * @brief     configure the SDM buffer's address and size
 * @param[in] pbuff - the first address of buffer SDM read data from.
 * @param[in] size_buff - the size of pbuff.
 * @return    none
 */
void audio_config_sdm_buf(signed short* pbuff, unsigned char size_buff)
{
	reg_aud_base_adr = pbuff;
	reg_aud_buff_size = (size_buff>>4)-1;
}

/****
* brief: audio amic initial function. configure ADC corresponding parameters. set hpf,lpf and decimation ratio.
* param[in] mode_flag -- '1' differ mode ; '0' signal end mode
* param[in] misc_sys_tick -- system ticks of adc misc channel.
* param[in] l_sys_tick -- system tick of adc left channel CLOCK_SYS_TYPE
* param[in]  fhs_source - the parameter is CLOCK_SYS_TYPE. avoid CLOCK_SYS_TYPE to be modified to other word.such as SYS_TYPE etc.
*
* return none
*/
void audio_amic_init(AudioRate_Typedef Audio_Rate)
{

	//Dfifo setting
	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock
	reg_audio_dfifo_mode = FLD_AUD_DFIFO0_IN;


	//amic input, mono mode, enable decimation filter
#if 0
	reg_audio_dfifo_ain &= ~AIDIO_DECIMATION_FILTER_BYPASS;
	reg_audio_dfifo_ain |= AUDIO_INPUT_MONO_MODE;
	SET_AUDIO_INPUT_CTRL(AUDIO_INPUT_AMIC);
#else
	reg_audio_dfifo_ain = MASK_VAL( FLD_AUD_INPUT_SELECT, AUDIO_INPUT_AMIC, \
									FLD_AUD_INPUT_MONO_MODE, 1, \
									FLD_AUD_DECIMATION_FILTER_BYPASS, 0);
#endif



	reg_audio_dec_mode  &= ~FLD_AUD_CIC_MODE;
	reg_audio_dec_mode  |= FLD_AUD_LNR_VALID_SEL;
	reg_audio_dec_ratio = AMIC_CIC_Rate[Audio_Rate];



	/*******2.ADC setting for analog audio sample**************************/
	SET_ADC_CLK_EN();										//enable 24M clk to SAR ADC
//	SET_ADC_CLK(2);											//adc_clk= 24M/(1+2)=8M
	SET_ADC_CLK(5);  										//adc_clk= 24M/(1+5)=4M
	SET_ADC_M_RNS_CAPTURE_LEN(0x0f);						//max_mc

	if((Audio_Rate == AUDIO_44K)||(Audio_Rate == AUDIO_22K))
	{
		SET_ADC_L_R_CAPTURE_LEN(AMIC_ADC_SampleLength[1]);	//max_c	132K
	}
	else
	{
		SET_ADC_L_R_CAPTURE_LEN(AMIC_ADC_SampleLength[0]);	//max_c	96K
	}


	SET_ADC_SET_LEN(0x0a);									//max_s
	SET_ADC_CHN_EN(ADC_LEFT_CHN);
	SET_ADC_MAX_SCNT(0x02);

	SET_ADC_BANDGAP_ON();

//	SET_ADC_LEFT_VREF(ADC_VREF_0P6V);
	SET_ADC_LEFT_VREF(ADC_VREF_0P9V);

	SET_ADC_LEFT_N_CHN_AIN(PGA0N);
	SET_ADC_LEFT_P_CHN_AIN(PGA0P);
	SET_ADC_LEFT_RES(RES14);
	SET_ADC_CHN_DIFF_EN(ADC_LEFT_CHN_MODE);
	SET_ADC_LEFT_TSAMP_CYCLE(SAMPLING_CYCLES_6);

	SET_ADC_VBAT_DIVIDER(ADC_VBAT_DIVIDER_1F2);
	SET_ADC_ITRIM_PREAMP(ADC_CUR_TRIM_PER100);
	SET_ADC_ITRIM_VREFBUF(ADC_CUR_TRIM_PER125);
	SET_ADC_ITRIM_VCMBUF(ADC_CUR_TRIM_PER100);
	SET_ADC_PRESCALER(ADC_PRESCALER_1);				//0x80+122  = 0x15? 0x00


#if 1 //B3
	SET_PGA_LEFT_P_AIN(PGA_AIN_C1);
	SET_PGA_LEFT_N_AIN(PGA_AIN_C1);
	SET_PGA_RIGHT_P_AIN(PGA_AIN_C0);
	SET_PGA_RIGHT_N_AIN(PGA_AIN_C0);
#else
	SET_PGA_LEFT_P_AIN(PGA_AIN_C0);
	SET_PGA_LEFT_N_AIN(PGA_AIN_C1);
#endif
	SET_LEFT_BOOST_BIAS(GAIN_STAGE_BIAS_PER125);
	SET_LEFT_GAIN_BIAS(GAIN_STAGE_BIAS_PER125);
	SET_ADC_POWER_ON(ADC_POWER_ON|LEFT_CHN_PGA_POWER_ON);
	WriteAnalogReg(0x80+126,0x05);					//0x80+126  = 0x05
	SET_PGA_GAIN_FIX_VALUE(0);



	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//enable hpf, enable lpf, anable alc, disable double_down_sampling
	reg_aud_hpf_alc = MASK_VAL( FLD_AUD_IN_HPF_SFT,  0x0b,   //different pcb may set different value.
							    		   FLD_AUD_IN_HPF_BYPASS, 0, \
							               FLD_AUD_IN_ALC_BYPASS, 0, \
							               FLD_AUD_IN_LPF_BYPASS, 0, \
							               FLD_DOUBLE_DOWN_SAMPLING_ON, 0);
	//alc mode select digital mode
	reg_aud_alc_cfg &= ~FLD_AUD_ALC_ANALOG_MODE_EN;
	//alc left channel select manual regulate, and set volume
	reg_aud_alc_vol_l_chn = MASK_VAL( FLD_AUD_ALC_MIN_VOLUME_IN_DIGITAL_MODE,  0x24, \
									  FLD_AUD_ALC_DIGITAL_MODE_AUTO_REGULATE_EN, 0);




#if 0
	unsigned char tmp_shift;
	unsigned char adc_mode;


	/***judge which clock is the source of FHS. three selection: 1--PLL 2--RC32M 3--16Mhz crystal oscillator***/
	switch(fhs_source){
	case CLOCK_TYPE_PLL:
		adc_mode = 192; //FHS is 192Mhz
		break;
	case CLOCK_TYPE_OSC:
		adc_mode = 32;  //FHS is RC_32Mhz
		break;
	case CLOCK_TYPE_PAD:
		adc_mode = 16;  //FHS is 16Mhz crystal oscillator
		break;
	}

	/***configure adc module. adc clock = FHS*adc_step/adc_mode ***/
	reg_adc_step_l  = 0x04;  //set the step 0x04,adc clock 4Mhz
	reg_adc_mod_l   = adc_mode;
	reg_adc_clk_en |= FLD_ADC_MOD_H_CLK; //enable adc clock

	/***set resolution,reference voltage,sample cycle***/
	BM_CLR(reg_adc_ref,FLD_ADC_REF_L);
	reg_adc_ref      |= MASK_VAL(FLD_ADC_REF_L,RV_AVDD);       //1.set reference voltage
	BM_CLR(reg_adc_res_lr,FLD_ADC_RESOLUTION_SEL);
	reg_adc_res_lr   |= MASK_VAL(FLD_ADC_RESOLUTION_SEL,RES14); //2.set resolution
	BM_CLR(reg_adc_tsamp_lr,FLD_ADC_SAMPLE_TIME);
	reg_adc_tsamp_lr |= MASK_VAL(FLD_ADC_SAMPLE_TIME,S_3);      //3.set sample cycle
	
	if(mode_flag == DIFF_MODE){        //different mode 
		BM_CLR(reg_adc_chn_l_sel,FLD_ADC_CHN_SEL|FLD_ADC_DIFF_CHN_SEL|FLD_ADC_DATA_FORMAT);
		reg_adc_chn_l_sel |= MASK_VAL(FLD_ADC_CHN_SEL,AUD_PGAVOM,FLD_ADC_DIFF_CHN_SEL,AUD_PGAVOPM,FLD_ADC_DATA_FORMAT,1);
	}
	else{ //adc is single end mode
		BM_CLR(reg_adc_chn_l_sel,FLD_ADC_DIFF_CHN_SEL);
		reg_adc_chn_l_sel |= MASK_VAL(FLD_ADC_DIFF_CHN_SEL,AUD_SINGLEEND);
	}
	reg_adc_chn_l_sel |= FLD_ADC_DATA_FORMAT;                  //4.signed adc data
	BM_CLR(reg_dfifo_ana_in,FLD_DFIFO_MIC_ADC_IN|FLD_DFIFO_AUD_INPUT_MONO);
	reg_dfifo_ana_in |= MASK_VAL(FLD_DFIFO_MIC_ADC_IN,AUD_AMIC,FLD_DFIFO_AUD_INPUT_MONO,3); //select AMIC,enable dfifo and wptr

	SET_PFM(misc_sys_tick); //set system tick of misc channel
	SET_PFL(left_sys_tick); //set system tick of left channel

	/**mono,left channel, adc done signal:falling,enable audio output**/
	reg_adc_ctrl = MASK_VAL(FLD_ADC_AUD_DATAPATH_EN,1,FLD_ADC_CHNL_AUTO_EN,1,FLD_ADC_AUD_MODE,MONO_AUDIO,\
			                FLD_ADC_DONE_SIGNAL,AUD_ADC_DONE_FALLING);
		
	/***decimation/down sample[3:0]Decimation rate [6:4]decimation shift select(0~5)***/
	switch(d_samp&0x0f){
	case R1:
		tmp_shift = 0x01;
		break;
	case R2:
	case R3:
		tmp_shift = 0x02;
		break;
	case R4:
	case R5:
		tmp_shift = 0x03;
		break;
	case R6:
		tmp_shift = 0x04;
		break;
	default:
		tmp_shift = 0x05;
		break;
	}
	reg_dfifo_scale = MASK_VAL(FLD_DFIFO2_DEC_CIC,d_samp,FLD_DFIFO0_DEC_SCALE,tmp_shift);
	/***************HPF setting[3:0]HPF shift [4]bypass HPF [5]bypass ALC [6]bypass LPF********/
	BM_CLR(reg_aud_hpf_alc,FLD_AUD_IN_HPF_SFT);
	reg_aud_hpf_alc |= MASK_VAL(FLD_AUD_IN_HPF_SFT,0x09);//different pcb may set different value.
	/***************ALC Volume[5:0]manual volume [6]0:manual 1:auto**************************/
	reg_aud_alc_vol = MASK_VAL(FLD_AUD_MANUAL_VOLUME,0x24,FLD_AUD_VOLUME_CTRL_MODE,AUD_VOLUME_MANUAL);//0x1c is the level of volume.0x1c

#endif

}

/**
 * @brief     audio DMIC init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * @param[in] dmic_speed - set the DMIC speed. such as 1 indicate 1M and 2 indicate 2M.
 * @param[in] d_samp - set the decimation. ie div_speed.
 * @param[in]  fhs_source - the parameter is CLOCK_SYS_TYPE. avoid CLOCK_SYS_TYPE to be modified to other word.such as SYS_TYPE etc.
 *
 * @return    none.
 */
void audio_dmic_init(unsigned char dmic_speed, enum audio_deci_t d_samp,unsigned char fhs_source)
{
//	unsigned char adc_mode;
//	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock.
//
//	/***judge which clock is the source of FHS. three selection: 1--PLL 2--RC32M 3--16Mhz crystal oscillator***/
//	switch(fhs_source){
//	case CLOCK_TYPE_PLL:
//		adc_mode = 192 - 4; //FHS is 192Mhz
//		break;
//	case CLOCK_TYPE_OSC:
//		adc_mode = 32;  //FHS is RC_32Mhz
//		break;
//	case CLOCK_TYPE_PAD:
//		adc_mode = 16;  //FHS is 16Mhz crystal oscillator
//		break;
//	}
//	/***config the pin dmic_sda and dmic_scl***/
//	gpio_set_func(GPIO_DMIC_DI,AS_DMIC);  //disable DI gpio function
//	gpio_set_func(GPIO_DMIC_CK,AS_DMIC);  //disable CK gpio function
//	gpio_set_input_en(GPIO_DMIC_DI,1);    //enable DI input
//	gpio_set_input_en(GPIO_DMIC_CK,1);    //enable CK input
////	reg_gpio_config_func0 |= FLD_DMIC_DI_PWM0; //enable PA0 as dmic pin
//	/***configure DMIC clock ***/
//	BM_CLR(reg_dmic_step,FLD_DMIC_STEP);
//	reg_dmic_step |= MASK_VAL(FLD_DMIC_STEP,dmic_speed);
//	reg_dmic_mod = adc_mode;
//	reg_dmic_step |= FLD_DMIC_CLK_EN; // enable dmic clock
//
//	BM_CLR(reg_dfifo_ana_in,FLD_DFIFO_MIC_ADC_IN|FLD_DFIFO_AUD_INPUT_MONO);
//	reg_dfifo_ana_in |= MASK_VAL(FLD_DFIFO_MIC_ADC_IN,AUD_DMIC,FLD_DFIFO_AUD_INPUT_MONO,3); //select AMIC,enable dfifo and wptr
//
//	/***************decimation/down sample[3:0]Decimation rate [6:4]decimation shift select(0~5)***/
//	switch(d_samp){
//	case R32:
//		reg_dfifo_scale = MASK_VAL(FLD_DFIFO2_DEC_CIC,d_samp,FLD_DFIFO0_DEC_SCALE,0x02);
//		break;
//	case R64:
//		reg_dfifo_scale = MASK_VAL(FLD_DFIFO2_DEC_CIC,d_samp,FLD_DFIFO0_DEC_SCALE,0x05);
//		break;
//	default:
//		reg_dfifo_scale = MASK_VAL(FLD_DFIFO2_DEC_CIC,d_samp,FLD_DFIFO0_DEC_SCALE,0x05);
//		break;
//	}
//	/***************HPF setting[3:0]HPF shift [4]bypass HPF [5]bypass ALC [6]bypass LPF*************/
//	BM_CLR(reg_aud_hpf_alc,FLD_AUD_IN_HPF_SFT);
//	reg_aud_hpf_alc |= MASK_VAL(FLD_AUD_IN_HPF_SFT,0x05);//different pcb may set different value.
//	/***************ALC Volume[5:0]manual volume [6]0:manual 1:auto*********************************/
//	reg_aud_alc_vol = 0x30;  //0x24
//
//	/*************enable HPF ,ALC and disable LPF***/
//	BM_CLR(reg_aud_hpf_alc,FLD_AUD_IN_HPF_BYPASS|FLD_AUD_IN_ALC_BYPASS|FLD_AUD_IN_LPF_BYPASS);//open hpf,alc,lpf
//	reg_aud_hpf_alc |= FLD_AUD_IN_LPF_BYPASS; //close lpf
}
/************************************************************************************
*
*	@brief	audio input set function, select analog audio input channel, start the filters
*
*	@param	adc_ch:	if audio input as signle end mode, should identify an analog audio signal input channel, enum variable of ADCINPUTCH
*
*	@return	none
*/
//void audio_amic_input_set(enum audio_input_t adc_ch){
//	unsigned char tem;
//	if(adc_ch == PGA_CH){ //this selection is diff mode. the input is pga.
//		pgaInit();
//		preGainAdjust(DB20); //set pre pga gain to 20db
//		postGainAdjust(DB9); //set post pga gain to 0db
//	}
//	else{                 //this selection is signed end
//		reg_adc_chn_l_sel &= (~FLD_ADC_CHN_SEL);
//		reg_adc_chn_l_sel |= adc_ch;
//	}
//	//configure filter and volume control register
//	BM_CLR(reg_aud_hpf_alc,FLD_AUD_IN_HPF_BYPASS|FLD_AUD_IN_ALC_BYPASS|FLD_AUD_IN_LPF_BYPASS);//open hpf,alc,lpf
//	reg_aud_hpf_alc |= FLD_AUD_IN_LPF_BYPASS; //close lpf
//}
/**
*	@brief		reg0x30[1:0] 2 bits for fine tuning, divider for slow down sample rate
*	@param[in]	fine_tune - unsigned char fine_tune,range from 0 to 3
*	@return	    none
*/
void audio_finetune_sample_rate(unsigned char fine_tune)
{
    //if(fine_tune>3) return;
	BM_CLR(reg_adc_period_chn0,FLD_ADC_PHASE_TICK);
	fine_tune &= 0x03;
    reg_adc_period_chn0 |= MASK_VAL(FLD_ADC_PHASE_TICK,fine_tune);////reg0x30[1:0] 2 bits for fine tuning
}

/**
 *  @brief      tune decimation shift .i.e register 0xb04 in datasheet.
 *  @param[in]  deci_shift - range from 0 to 5.
 *  @return     none
 */
unsigned char audio_tune_deci_shift(unsigned char deci_shift)
{
	if(deci_shift > 5)
	{
		return 0;
	}
	BM_CLR(reg_dfifo_scale,FLD_DFIFO0_DEC_SCALE);
	reg_dfifo_scale |= MASK_VAL(FLD_DFIFO0_DEC_SCALE,deci_shift);
	return 1;
}
/**
 *   @brief       tune the HPF shift .i.e register 0xb05 in datasheet.
 *   @param[in]   hpf_shift - range from 0 to 0x0f
 *   @return      none
 */
 unsigned char audio_tune_hpf_shift(unsigned char hpf_shift)
 {
	if(hpf_shift > 0x0f){
		return 0;
	}
	BM_CLR(reg_aud_hpf_alc,FLD_AUD_IN_HPF_SFT);
	reg_aud_hpf_alc |= MASK_VAL(FLD_AUD_IN_HPF_SFT,hpf_shift);
	return 1;
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
void audio_sdm_output_set(unsigned char audio_out_en,int sample_rate,unsigned char sdm_clk,unsigned char fhs_source)
{
#if 0
	unsigned char adc_mode;
	if(audio_out_en){
		/***enable SDM pins(sdm_n,sdm_p)***/
//		gpio_set_func(GPIO_SDMP,AS_SDM);  //disable gpio function
//		gpio_set_func(GPIO_SDMN,AS_SDM);  //disable gpio function
//		gpio_set_input_en(GPIO_SDMP,1);   //in sdk, require to enable input.because the gpio_init() function in main() function.
//		gpio_set_input_en(GPIO_SDMN,1);   //in sdk, require to enable input
		reg_gpio_config_func4 &= (~(FLD_DMIC_CK_RX_CLK|FLD_I2S_DI_RX_DAT));//enable dmic function of SDMP and SDMM

		/***judge which clock is the source of FHS. three selection: 1--PLL 2--RC32M 3--16Mhz crystal oscillator***/
		switch(fhs_source){
		case CLOCK_TYPE_PLL:
			adc_mode = 192; //FHS is 192Mhz
			break;
		case CLOCK_TYPE_OSC:
			adc_mode = 32;  //FHS is RC_32Mhz
			break;
		case CLOCK_TYPE_PAD:
			adc_mode = 16;  //FHS is 16Mhz crystal oscillator
			break;
		}

		/***configure sdm clock; ***/
		BM_CLR(reg_i2s_step,FLD_I2S_STEP);
		reg_i2s_step |= MASK_VAL(FLD_I2S_STEP,sdm_clk,FLD_I2S_CLK_EN,1);
		reg_i2s_mod   = adc_mode;
		reg_clk_en2  |= FLD_CLK2_AUD_EN;	//enable audio clock

		/***configure interpolation ratio***/
		reg_ascl_step = AUD_SDM_STEP (sample_rate, sdm_clk*1000000);
		/***2.Enable PN generator as dither control, clear bit 2, 3, 6***/
		BM_CLR(reg_aud_ctrl,FLD_AUD_ENABLE|FLD_AUD_SDM_PLAY_EN|FLD_AUD_PN_SHAPPING_BYPASS|FLD_AUD_SHAPING_EN|FLD_AUD_CONST_VAL_INPUT_EN);
		reg_aud_ctrl |= MASK_VAL(FLD_AUD_PN2_GENERATOR_EN,1,FLD_AUD_PN1_GENERATOR_EN,1);
		
		reg_aud_pn1 = 0x08;  //PN generator 1 bits used
		reg_aud_pn2 = 0x08;  //PN generator 1 bits used

		//enable audio and sdm player.
		reg_aud_ctrl |= (FLD_AUD_ENABLE|FLD_AUD_SDM_PLAY_EN);
	}
	else{
		BM_CLR(reg_aud_ctrl,FLD_AUD_SDM_PLAY_EN);  //close sdm player
	}
#endif
}

/**
*	@brief	    set audio volume level
*	@param[in]	input_out_sel - select the tune channel, '1' tune ALC volume; '0' tune SDM output volume
*	@param[in]	volume_level - volume level
*	@return	    none
*/
void audio_volume_tune(unsigned char input_out_sel, unsigned char volume_level)
{
//	if(input_out_sel){
//		BM_CLR(reg_aud_alc_vol,FLD_AUD_MANUAL_VOLUME);
//		reg_aud_alc_vol |= MASK_VAL(FLD_AUD_MANUAL_VOLUME,volume_level);//the low six bits is volume level bits
//	}
//	else{
//		reg_aud_vol_ctrl = volume_level;
//	}
}

/*************************************************************
*
*	@brief	automatically gradual change volume
*
*	@param[in]	vol_step - volume change step, the high part is decrease step while the low part is increase step
*			    gradual_interval - volume increase interval
*
*	@return	none
*/
void audio_volume_step_adjust(unsigned char vol_step,unsigned short gradual_interval)
{
	reg_aud_vol_step = vol_step;
	BM_CLR(reg_aud_tick_interval,FLD_AUD_ALC_VOL_TICK_L|FLD_AUD_ALC_VOL_TICK_H);
	reg_aud_tick_interval |= MASK_VAL(FLD_AUD_ALC_VOL_TICK_L,(gradual_interval&0xff),FLD_AUD_ALC_VOL_TICK_H,((gradual_interval>>8)&0x3f));
}


