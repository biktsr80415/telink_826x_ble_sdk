
#include "adc.h"

#include "analog.h"
#include "clock.h"


void adc_set_ref_voltage(ADC_ChTypeDef ch_n, ADC_RefVolTypeDef v_ref)
{
	if(ch_n & ADC_LEFT_CHN)
	{
		adc_set_vref_chn_left(v_ref);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_vref_chn_right(v_ref);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_vref_chn_misc(v_ref);
	}


	if(v_ref == ADC_VREF_1P2V)
	{
		//Vref buffer bias current trimming: 		150%
		//Vref buffer bias current trimming: 		150%
		//Comparator preamp bias current trimming:  100%
		analog_write(anareg_ain_scale, (analog_read(anareg_ain_scale)&(0xC0)) | 0x3d );
	}
	else
	{
		//Vref buffer bias current trimming: 		100%
		//Vref buffer bias current trimming: 		100%
		//Comparator preamp bias current trimming:  100%
		analog_write(anareg_ain_scale, (analog_read(anareg_ain_scale)&(0xC0)) | 0x15 );
	}

}


void adc_set_resolution(ADC_ChTypeDef ch_n, ADC_ResTypeDef v_res)
{
	if(ch_n & ADC_LEFT_CHN)
	{
		adc_set_resolution_chn_left(v_res);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_resolution_chn_right(v_res);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_resolution_chn_misc(v_res);
	}
}



void adc_set_tsample_cycle(ADC_ChTypeDef ch_n, ADC_SampCycTypeDef adcST)
{
	if(ch_n & ADC_LEFT_CHN)
	{
		adc_set_tsample_cycle_chn_left(adcST);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_tsample_cycle_chn_right(adcST);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_tsample_cycle_chn_misc(adcST);
	}
}


void adc_set_input_mode(ADC_ChTypeDef ch_n,  ADC_InputModeTypeDef m_input)
{
	if(ch_n & ADC_LEFT_CHN)
	{
		adc_set_input_mode_chn_left(m_input);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_input_mode_chn_right(m_input);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_input_mode_chn_misc(m_input);
	}
}

/**
 * @brief      This function sets ADC input channel for SINGLE_ENDED_MODE
 * @param[in]  adcInCha - enum variable of adc input channel.
 * @return     none
 */
void adc_set_ain_channel_single_ended_input_mode(ADC_ChTypeDef ch_n, ADC_InputPchTypeDef InPCH)
{

	if(ch_n & ADC_LEFT_CHN)
	{
		adc_set_ain_negative_chn_left(GND);
		adc_set_ain_positive_chn_left(InPCH);
		adc_set_input_mode_chn_left(SINGLE_ENDED_MODE);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_ain_negative_chn_right(GND);
		adc_set_ain_positive_chn_right(InPCH);
		adc_set_input_mode_chn_right(SINGLE_ENDED_MODE);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_ain_negative_chn_misc(GND);
		adc_set_ain_positive_chn_misc(InPCH);
		adc_set_input_mode_chn_misc(SINGLE_ENDED_MODE);
	}
}

/**
 * @brief      This function sets ADC input channel for DIFFERENTIAL_MODE
 * @param[in]  adcInCha - enum variable of adc input channel.
 * @return     none
 */
void adc_set_ain_channel_differential_mode(ADC_ChTypeDef ch_n, ADC_InputPchTypeDef InPCH,ADC_InputNchTypeDef InNCH)
{

	if(ch_n & ADC_LEFT_CHN)
	{
		adc_set_ain_negative_chn_left(InNCH);
		adc_set_ain_positive_chn_left(InPCH);
		adc_set_input_mode_chn_left(DIFFERENTIAL_MODE);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_ain_negative_chn_right(InNCH);
		adc_set_ain_positive_chn_right(InPCH);
		adc_set_input_mode_chn_right(DIFFERENTIAL_MODE);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_ain_negative_chn_misc(InNCH);
		adc_set_ain_positive_chn_misc(InPCH);
		adc_set_input_mode_chn_misc(DIFFERENTIAL_MODE);
	}
}

//state length indicates number of 24M clock cycles occupied by the state
//R_max_mc[9:0] serves to set length of "capture" state for RNS and Misc channel
//R_max_c[9:0]  serves to set length of "capture" state for left and right channel
//R_max_s[9:0]  serves to set length of "set" state for left, right and Misc channel
void adc_set_state_length(unsigned short R_max_mc, unsigned short R_max_c,unsigned char R_max_s)
{
	unsigned char data[3]={0};
	if(R_max_mc&0x3ff)    //r_max_mc[9:0]serves to set length of state for RNS and Misc channel.
	{
		data[0] = (unsigned char)R_max_mc;
		data[2] = (unsigned char)(R_max_mc>>2)&0xc0;
	}
	if(R_max_c&0x3ff)     //r_max_c*9:0+ serves to set length of  state for left and right channel.
	{
		data[1] = (unsigned char)R_max_c;
		data[2] |= (unsigned char)(R_max_c>>4)&0x30;
	}
	if(R_max_s)     //r_max_s serves to set length of  state for left, right and Misc channel.
	{
		data[2] |= (unsigned char)(R_max_s&0x0f);
	}

	WriteAnalogReg(anareg_r_max_mc, data[0]);			//
	WriteAnalogReg(anareg_r_max_c, 	data[1]);			//
	WriteAnalogReg(anareg_r_max_s,  data[2]);			//
}





void adc_set_ain_pre_scaler(ADC_PreScalingTypeDef v_scl)
{


	analog_write(anareg_ain_scale, (analog_read(anareg_ain_scale)&(~FLD_SEL_AIN_SCALE)) | (v_scl<<6) );

	//setting adc_sel_atb ,if stat is 0,clear adc_sel_atb,else set adc_sel_atb[0]if(stat)
	unsigned char tmp;
	if(v_scl)
	{
		//ana_F9<4> must be 1
		tmp = ReadAnalogReg(0xF9);
		tmp = tmp|0x10;                    //open tmp = tmp|0x10;
		WriteAnalogReg (0xF9, tmp);
	}
	else
	{
		//ana_F9 <4> <5> must be 0
		tmp = ReadAnalogReg(0xF9);
		tmp = tmp&0xcf;
		WriteAnalogReg (0xF9, tmp);
	}


}



/**
 * Name     :RNG_Set
 * Function :Set the source and mode of the random number generator
 * Input    :RNG_SrcTypeDef stat
 *          :RNG_UpdataTypeDef stat1
 * return   :void
 */
void RNG_Set(RNG_SrcTypeDef stat,RNG_UpdataTypeDef stat1){

	unsigned datast = stat|stat1;

	WriteAnalogReg(0x80+126,datast);			//Set

}

/**
 * Name     :RNG_read
 * Function :Read the value of the random number generator
 * Input    :None
 * return   :unsigned short RngValue
 *          :random number
 */
unsigned short RNG_Read(void){

	unsigned short tmp1,tmp2,RngValue;

	tmp1 = ReadAnalogReg(0x80+118);  //read
	tmp2 = ReadAnalogReg(0x80+117);
	RngValue = (tmp1<<8) + tmp2;

	return RngValue;
}
