
#include "adc.h"

#include "analog.h"
#include "clock.h"


void adc_set_ref_voltage(ADC_ChTypeDef ch_n, ADC_RefVolTypeDef v_ref)
{
#if 0
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
#else
	adc_set_vref(v_ref, v_ref, v_ref);
#endif


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
		adc_set_ain_chn_left(InPCH, InNCH);
		adc_set_input_mode_chn_left(DIFFERENTIAL_MODE);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		adc_set_ain_chn_right(InPCH, InNCH);
		adc_set_input_mode_chn_right(DIFFERENTIAL_MODE);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		adc_set_ain_chn_misc(InPCH, InNCH);
		adc_set_input_mode_chn_misc(DIFFERENTIAL_MODE);
	}
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
