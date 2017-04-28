
/**************************************************************************************************
  Filename:       	audio.c
  Author:			junjun.xu@telink-semi.com
  Created Date:	2016/06/06

  Description:    This file contains the audio driver functions for the Telink 8267.


**************************************************************************************************/


#include "../../proj/tl_common.h"
#include "audio.h"
#include "pga.h"

#if( (CHIP_TYPE == MCU_CORE_8261) ||  (CHIP_TYPE == MCU_CORE_8267) ||  (CHIP_TYPE == MCU_CORE_8269) )

#define SET_PFM(v)     write_reg16(0x800030,(v<<2)&0x0fff)
#define SET_PFL(v)     write_reg16(0x800032,v)
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
* param[in] l_sys_tick -- system tick of adc left channel
*/
void audio_amic_init(enum audio_mode_t mode_flag,unsigned short misc_sys_tick, unsigned short left_sys_tick,enum audio_deci_t d_samp)
{
	unsigned char fhsBL, fhsBH,adc_mode,tmp_shift;

	reg_clk_en2 |= FLD_CLK2_DIFIO_EN; //enable dfifo clock
	fhsBH = reg_fhs_sel & 0x01;       //register 0x70[0]
	fhsBL = reg_clk_sel & 0x80;       //register 0x66[7], 0x66[7] and 0x70[0] decide the source of FHS.

	/***judge which source clock is the source of FHS. three selection: 1--PLL 2--RC32M 3--16Mhz crystal oscillator***/
	if((0==fhsBL)&&(0==fhsBH)){
		adc_mode = 192; //fhs is 192Mhz
		WriteAnalogReg(0x05,ReadAnalogReg(0x05) & 0x7f); //power up pll LDO
	}
	else if((0x80==fhsBL)&&(0==fhsBH)){
		adc_mode = 32; //FHS is RC_32Mhz
		WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0xfb); //Power up 32MHz RC oscillator
	}
	else{
		adc_mode = 16; //FHS is 16Mhz crystal oscillator
		WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0xf7); //power up 16MHz crystal oscillator
	}

	/***configure adc module. adc clock = FHS*adc_step/adc_mode ***/
	reg_adc_step_l &= 0x00;
	reg_adc_step_l  = 0x04;  //set the step 0x04,adc clock 4Mhz
	reg_adc_mod_l  &= 0x00;
	reg_adc_mod_l   = adc_mode;
	WriteAnalogReg(0x06, ReadAnalogReg(0x06) & 0xfe); //power on sar
	reg_adc_clk_en |= FLD_ADC_MOD_H_CLK; //enable adc clock

	/***set resolution,reference voltage,sample cycle***/
	reg_adc_ref &= (~FLD_ADC_REF_L);	
	reg_adc_ref |= (RV_1P428<<2);      //1.set reference voltage
	
	reg_adc_chn_l_sel |= BIT(7);       //2.signed adc data
	
	reg_adc_res_lr &= (~BIT_RNG(0,2));
	reg_adc_res_lr |= RES14;           //3.set resolution
	
	reg_adc_tsamp_lr &= (~BIT_RNG(0,2)); 
	reg_adc_tsamp_lr |= S_3;           //4.set sample cycle
	
	if(mode_flag == DIFF_MODE){        //different mode 
		reg_adc_chn_l_sel &= (~FLD_ADC_CHN_SEL);
		reg_adc_chn_l_sel |= AUD_PGAVOM;
		reg_adc_chn_l_sel &= (~BIT_RNG(5,6));
		reg_adc_chn_l_sel |= (AUD_PGAVOPM<<5);
	}
	else{ //adc is single end mode
		reg_adc_chn_l_sel &= (~BIT_RNG(5,6));
		reg_adc_chn_l_sel |= (AUD_SINGLEEND<<5);
	}
	reg_dfifo_ana_in |= (FLD_DFIFO_MIC_ADC_IN|FLD_DFIFO_AUD_INPUT_MONO);
	SET_PFM(misc_sys_tick); //set system tick of misc channel
	SET_PFL(left_sys_tick); //set system tick of left channel
	/**mono,left channel, adc done signal:falling,enable audio output**/
	reg_adc_ctrl = 0x00; 
	reg_adc_ctrl = (FLD_ADC_AUD_DATAPATH_EN|FLD_ADC_CHNL_AUTO_EN|MONO_AUDIO|AUD_ADC_DONE_FALLING);
		
	/***decimation/down sample[3:0]Decimation rate [6:4]decimation shift select(0~5)***/
	#if TL_MIC_32K_FIR_16K
		switch(d_samp&0x0f){
		case R2:
			tmp_shift = 0x01;
			break;
		case R3:
			tmp_shift = 0x02;
			break;
		default:
			tmp_shift = 0x01;
			break;
		}
	#else
		switch(d_samp&0x0f){
		case R3:
			tmp_shift = 0x01;
			break;
		case R5:
			tmp_shift = 0x03;
			break;
		case R6:
			tmp_shift = 0x04;
			break;
		default:
			tmp_shift = 0x04;
			break;
		}
	#endif
	reg_dfifo_scale = 0x00;
	reg_dfifo_scale |= d_samp;
	reg_dfifo_scale |= (tmp_shift<<4);
	/***************HPF setting[3:0]HPF shift [4]bypass HPF [5]bypass ALC [6]bypass LPF********/
	reg_aud_hpf_alc &= (~BIT_RNG(0,3));
	reg_aud_hpf_alc |= 0x09; //different pcb may set different value.
	/***************ALC Volume[5:0]manual volume [6]0:manual 1:auto**************************/
	reg_aud_alc_vol = 0x1c;  //set volume level and enable manual mode
}

/**
 * @brief     audio DMIC init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * @param[in] dmic_speed - set the DMIC speed. such as 1 indicate 1M and 2 indicate 2M.
 * @param[in] d_samp - set the decimation. ie div_speed.
 * @return    none.
 */
void audio_dmic_init(unsigned char dmic_speed, enum audio_deci_t d_samp)
{
	unsigned char tmp_shift = 0;
	unsigned char fhsBL,fhsBH,adc_mode;
	reg_clk_en2 |= FLD_CLK2_DIFIO_EN; //enable dfifo clock.
	fhsBH = reg_fhs_sel & 0x01;   //register 0x70[0]
	fhsBL = reg_clk_sel & 0x80;   //register 0x66[7], 
	/***judge which source clock is the source of FHS. three selection: 1--PLL 2--RC32M 3--16Mhz crystal oscillator***/
	if((0==fhsBL)&&(0==fhsBH)){
		adc_mode = 192; //fhs is 192Mhz
		WriteAnalogReg(0x05,ReadAnalogReg(0x05) & 0x7f); //power up pll LDO
	}
	else if((0x80==fhsBL)&&(0==fhsBH)){
		adc_mode = 32; //FHS is RC_32Mhz
		WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0xfb); //Power up 32MHz RC oscillator
	}
	else{
		adc_mode = 16; //FHS is 16Mhz crystal oscillator
		WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0xf7); //power up 16MHz crystal oscillator
	}
	/**config the pin dmic_sda and dmic_scl**/
	gpio_set_func(GPIO_DMIC_DI,AS_DMIC);  //disable DI gpio function
	gpio_set_func(GPIO_DMIC_CK,AS_DMIC);  //disable CK gpio function
	gpio_set_input_en(GPIO_DMIC_DI,1);    //enable DI input
	gpio_set_input_en(GPIO_DMIC_CK,1);    //enable CK input
	reg_gpio_config_func0 |= FLD_DMIC_DI_PWM0; //enable PA0 as dmic pin
	/***configure DMIC clock ***/
	reg_dmic_step &= (~FLD_DMIC_STEP);
	reg_dmic_step |= (dmic_speed);
	reg_dmic_mod = adc_mode;
	reg_dmic_step |= FLD_DMIC_CLK_EN; // enable dmic clock
	
	reg_dfifo_ana_in = 0x00;
	reg_dfifo_ana_in = (FLD_DFIFO_AUD_INPUT_MONO&(~FLD_DFIFO_MIC_ADC_IN)); //enable dfifo and wptr i.e. write pointer. enable dmic

	/***************decimation/down sample[3:0]Decimation rate [6:4]decimation shift select(0~5)***/
	reg_dfifo_scale = 0x00;
	reg_dfifo_scale |= d_samp;
	reg_dfifo_scale |= (0x05<<4);

	/***************HPF setting[3:0]HPF shift [4]bypass HPF [5]bypass ALC [6]bypass LPF*************/
	reg_aud_hpf_alc &= (~BIT_RNG(0,3));
	reg_aud_hpf_alc |= 0x05; //different pcb may set different value.

	/***************ALC Volume[5:0]manual volume [6]0:manual 1:auto*********************************/
	reg_aud_alc_vol = 0x24;

	/*************enable HPF ,ALC and disable LPF***/
	reg_aud_hpf_alc &= (~BIT_RNG(4,6));
	reg_aud_hpf_alc |= FLD_AUD_IN_LPF_BYPASS;
}
/************************************************************************************
*
*	@brief	audio input set function, select analog audio input channel, start the filters
*
*	@param	adc_ch:	if audio input as signle end mode, should identify an analog audio signal input channel, enum variable of ADCINPUTCH
*
*	@return	none
*/
void audio_amic_input_set(enum audio_input_t adc_ch){
	unsigned char tem;
	if(adc_ch == PGA_CH){
		//PGA Setting
		pgaInit();
		preGainAdjust(DB20);//set pre pga gain to 0  ////DB20 by sihui
		postGainAdjust(DB9);//set post pga gain to 0 ////DB9 by sihui
	}
	else{
		//L channel's input as C[0]
		//adc_AnaChSet(LCHANNEL,adc_ch);
		reg_adc_chn_l_sel &= (~FLD_ADC_CHN_SEL);
		reg_adc_chn_l_sel |= adc_ch;
	}
	//open lpf, hpf, alc
	reg_aud_hpf_alc &= (~BIT_RNG(4,5));
	reg_aud_hpf_alc |= FLD_AUD_IN_LPF_BYPASS; //open hpf,alc and close lpf.
}
/**
*	@brief		reg0x30[1:0] 2 bits for fine tuning, divider for slow down sample rate
*	@param[in]	fine_tune - unsigned char fine_tune,range from 0 to 3
*	@return	    none
*/
void audio_finetune_sample_rate(unsigned char fine_tune)
{
    //if(fine_tune>3) return;
	reg_adc_period_chn0 &= 0xfffc;
	fine_tune &= 0x03;
    reg_adc_period_chn0 |= fine_tune;////reg0x30[1:0] 2 bits for fine tuning
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
	reg_dfifo_scale &= (~BIT_RNG(4,6));
	reg_dfifo_scale |= deci_shift;
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
	reg_aud_hpf_alc &= (~FLD_AUD_IN_HPF_SFT);
	reg_aud_hpf_alc |= hpf_shift;
	return 1;
 }
 /**
 *
 *	@brief	   sdm setting function, enable or disable the sdm output, configure SDM output paramaters
 *
 *	@param[in]	audio_out_en - audio output enable or disable set, '1' enable audio output; '0' disable output
 *	@param[in]	sdm_setp -	  SDM clk divider
 *	@param[in]	sdm_clk -	  SDM clk, default to be 8Mhz
 *
 *	@return	none
 */
void audio_sdm_output_set(unsigned char audio_out_en,unsigned short sdm_step,unsigned char sdm_clk)
{
	unsigned char fhsBL,fhsBH,adc_mode;
	if(audio_out_en){
		 /***configure sdm clock; ***/
		 reg_i2s_step &= (~FLD_I2S_STEP);
		 reg_i2s_step = (sdm_clk|FLD_I2S_CLK_EN);  
		/***judge which source clock is the source of FHS. three selection: 1--PLL 2--RC32M 3--16Mhz crystal oscillator***/
		fhsBL = (reg_fhs_sel & 0x01);  //0x70[0]
		fhsBH = (reg_clk_sel & 0x80);  //0x66[7]
		if((0==fhsBL)&&(0==fhsBH)){
			adc_mode = 192; //fhs is 192Mhz
			WriteAnalogReg(0x05,ReadAnalogReg(0x05) & 0x7f); //power up pll LDO
		}
		else if((0x80==fhsBL)&&(0==fhsBH)){
			adc_mode = 32; //FHS is RC_32Mhz
			WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0xfb); //Power up 32MHz RC oscillator
		}
		else{
			adc_mode = 16; //FHS is 16Mhz crystal oscillator
			WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0xf7); //power up 16MHz crystal oscillator
		}
		reg_i2s_mod = adc_mode;
		/*******/
		reg_ascl_step = sdm_step;
		/***2.Enable PN generator as dither control, clear bit 2, 3, 6***/
		reg_aud_ctrl &= (~(FLD_AUD_PN__SHAPPING_BYPASS|FLD_AUD_SHAPING_EN|FLD_AUD_CONST_VAL_INPUT_EN));
		reg_aud_ctrl |= (FLD_AUD_PN2_GENERATOR_EN|FLD_AUD_PN1_GENERATOR_EN);
		reg_aud_ctrl &= (~(FLD_AUD_ENABLE|FLD_AUD_SDM_PLAY_EN));
		
		reg_aud_pn1 = 0x08;  //PN generator 1 bits used
		reg_aud_pn2 = 0x08;  //PN generator 1 bits used
		/***enable SDM pins(sdm_n,sdm_p)***/
		gpio_set_func(GPIO_SDMP,AS_SDM);  //disable gpio function
		gpio_set_func(GPIO_SDMN,AS_SDM);  //disable gpio function
		gpio_set_input_en(GPIO_SDMP,1);   //in sdk, require to enable input.because the gpio_init() function in main() function.
		gpio_set_input_en(GPIO_SDMN,1);   //in sdk, require to enable input
		//enable dmic function of SDMP and SDMM
		reg_gpio_config_func4 &= (~(FLD_DMIC_CK_RX_CLK|FLD_I2S_DI_RX_DAT));
		//enable audio and sdm player.
		reg_aud_ctrl |= (FLD_AUD_ENABLE|FLD_AUD_SDM_PLAY_EN);
	}
	else{
		reg_aud_ctrl &= (~FLD_AUD_SDM_PLAY_EN);
	}
}

/**
*	@brief	    set audio volume level
*	@param[in]	input_out_sel - select the tune channel, '1' tune ALC volume; '0' tune SDM output volume
*	@param[in]	volume_level - volume level
*	@return	    none
*/
void audio_volume_tune(unsigned char input_out_sel, unsigned char volume_level)
{
	if(input_out_sel){
		reg_aud_alc_vol |= (volume_level&0x3f); //the low six bits is volume level bits
	}
	else{
		reg_aud_vol_ctrl = volume_level;
	}
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
	reg_aud_tick_interval &= 0xc000;
	reg_aud_tick_interval |= (gradual_interval&0x3fff);
}
#endif
