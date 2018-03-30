/*
 * adc_8267.c
 *
 *  Created on: 2015-12-10
 *      Author: Telink
 */
#include "../tl_common.h"


#include "adc.h"

const unsigned char  VBAT_tab[4] = {0,4,3,2};
const unsigned char  Vref_tab[4] = {2,3,4,1};

#define DEBUG_VREF    0

/**
 * @brief      This function sets ADC reference voltage for the Misc  L channel and  R channel
 * @param[in]  adcRF - enum variable of adc reference voltage.
 * @param[in]  ch_n  - enum variable of adc channel
 * @return     none
 */
//void ADC_RefVoltageSet(ADC_RefVolTypeDef adcRF,ADC_ChTypeDef ch_n){
//
//	unsigned char st_ref = 0;
//
//	//set REF voltage
//	ADC_Vref = (unsigned char)adcRF;
//
//	//set L R M channels vref
//	if(ch_n & ADC_LEFT_CHN)
//	{
//		st_ref |= ((unsigned char)adcRF)&0x03;
//	}
//	if(ch_n & ADC_RIGHT_CHN)
//	{
//		st_ref |= ((((unsigned char)adcRF)&0x03)<<2);
//	}
//	if(ch_n & ADC_MISC_CHN)
//	{
//		st_ref |= ((((unsigned char)adcRF)&0x03)<<4);
//	}
//	WriteAnalogReg(0x80+103,st_ref); //Select Vref
//}

/**
 * @brief      This function select Vbat voltage divider
 * @param[in]  vbat_div - enum variable of Vbat Vbat voltage divider.
 * @return     none
 */
//void ADC_VbatDivSet(ADC_VbatDivTypeDef vbat_div){
//
//	unsigned char tmp;
//
//	//set Vbat divider select
//	ADC_VBAT_Scale = VBAT_tab[(unsigned char)vbat_div];
//
//	tmp = ReadAnalogReg(0x80+121);
//	tmp = tmp&(0xf3);
//	tmp = tmp|(vbat_div<<2);
//	WriteAnalogReg (0x80+121, tmp);
//
//}

/**
 * @brief      This function sets ADC resolution for channel Misc
 * @param[in]  adcRes - enum variable adc resolution.
 * @param[in]  ch_n  - enum variable of adc channel
 * @return     none
 */
void ADC_ResSet(ADC_ResTypeDef adcRes,ADC_ChTypeDef ch_n){

	if(ch_n & ADC_LEFT_CHN)
	{
		SET_ADC_LEFT_RES(adcRes);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		SET_ADC_RIGHT_RES(adcRes);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		SET_ADC_MISC_RES(adcRes);
	}
}

/**
 * @brief      This function sets ADC sample time(the number of adc clocks for each sample)
 * @param[in]  adcST - enum variable of adc sample time.
 * @param[in]  ch_n  - enum variable of adc channel
 * @return     none
 */
void ADC_SampleTimeSet( ADC_SampCycTypeDef adcST, ADC_ChTypeDef ch_n){

	if(ch_n & ADC_LEFT_CHN)
	{
		SET_ADC_LEFT_TSAMP_CYCLE(adcST);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		SET_ADC_RIGHT_TSAMP_CYCLE(adcST);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		SET_ADC_MISC_TSAMP_CYCLE(adcST);
	}
}

/**
 * @brief      This function sets ADC input channel
 * @param[in]  adcInCha - enum variable of adc input channel.
 * @return     none
 */
void ADC_AnaSingleEndChSet(ADC_InputPchTypeDef InPCH, ADC_ChTypeDef ch_n){

	if(ch_n & ADC_LEFT_CHN)
	{
		SET_ADC_LEFT_N_CHN_AIN(GND);
		SET_ADC_LEFT_P_CHN_AIN(InPCH);
		SET_ADC_CHN_SINGLE_EN(ADC_LEFT_CHN_MODE);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		SET_ADC_RIGHT_N_CHN_AIN(GND);
		SET_ADC_RIGHT_P_CHN_AIN(InPCH);
		SET_ADC_CHN_SINGLE_EN(ADC_RIGTH_CHN_MODE);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		SET_ADC_MISC_N_CHN_AIN(GND);
		SET_ADC_MISC_P_CHN_AIN(InPCH);
		SET_ADC_CHN_SINGLE_EN(ADC_MISC_CHN_MODE);
	}
}

/**
 * @brief      This function sets ADC input channel
 * @param[in]  adcInCha - enum variable of adc input channel.
 * @return     none
 */
void ADC_AnaDiffChSet(ADC_InputPchTypeDef InPCH,ADC_InputNchTypeDef InNCH, ADC_ChTypeDef ch_n){

	if(ch_n & ADC_LEFT_CHN)
	{
		SET_ADC_LEFT_N_CHN_AIN(InNCH);
		SET_ADC_LEFT_P_CHN_AIN(InPCH);
		SET_ADC_CHN_DIFF_EN(ADC_LEFT_CHN_MODE);
	}
	if(ch_n & ADC_RIGHT_CHN)
	{
		SET_ADC_RIGHT_N_CHN_AIN(InNCH);
		SET_ADC_RIGHT_P_CHN_AIN(InPCH);
		SET_ADC_CHN_DIFF_EN(ADC_RIGTH_CHN_MODE);
	}
	if(ch_n & ADC_MISC_CHN)
	{
		SET_ADC_MISC_N_CHN_AIN(InNCH);
		SET_ADC_MISC_P_CHN_AIN(InPCH);
		SET_ADC_CHN_DIFF_EN(ADC_MISC_CHN_MODE);
	}
}


void ADC_State_Length_Set(short R_max_mc,short R_max_c,unsigned char R_max_s)
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

	WriteAnalogReg(0x80+111,data[0]);			//
	WriteAnalogReg(0x80+112,data[1]);			//
	WriteAnalogReg(0x80+113,data[2]);			//
}

void ADC_State_SCNT_EN(ADC_ChTypeDef ad_ch,unsigned char cnt)
{
	unsigned char ch_en =0;
	if(ad_ch & ADC_LEFT_CHN)				// enable Left channel
	{
		ch_en = 0x01;
		if(ad_ch & ADC_RIGHT_CHN)        	// enable Right channel
		{
			ch_en |= 0x02;
		}
	}

	if(ad_ch & ADC_MISC_CHN)				// enable Misc channel
	{
		ch_en |= 0x04;
	}
	if(ad_ch & ADC_RNS_CHN)					// enable RNS channel
	{
		ch_en |= 0x08;
	}

	ch_en |= ((cnt&0x07)<<4);				// set total length for sampling state machine

	WriteAnalogReg(0x80+114,ch_en);			//write total length and channel enable

}

/**
 * Name     :ADC_ScaleSet
 * Function :Set the source and mode of the random number generator
 * Input    :unsigned char stat
 *          :RNG_SrcTypeDef|RNG_UpdataTypeDef
 * return   :None
 */
void ADC_modeSet(ADCModeTypeDef stat){
	unsigned char tmp=0;

	tmp = ReadAnalogReg(0x80+124);
	tmp = tmp&0xef;
	tmp = tmp|stat;
	WriteAnalogReg (0x80+124, tmp);

}

//void ADC_ScaleSet(ADC_PreScalingTypeDef stat){
//
//	unsigned char tmp;
//
//	ADC_Pre_Scale = 1<<(unsigned char)stat;
//
//	if(ADC_Vref == 2)
//	{
//		SET_ADC_PRESCALER(stat);
//		tmp = ReadAnalogReg(0x80+122);
//		WriteAnalogReg(0x80+122,tmp|0x3d);		//Settmp|stat
//	}
//	else
//	{
//		SET_ADC_PRESCALER(stat);
//		tmp = ReadAnalogReg(0x80+122);
//		WriteAnalogReg(0x80+122,tmp|0x14);		//Settmp|stat
//
//
//	}
//	//setting adc_sel_atb ,if stat is 0,clear adc_sel_atb,else set adc_sel_atb[0]if(stat)
//	if(stat)
//	{
//		tmp = ReadAnalogReg(0x80+121);
//		tmp = tmp|0x10;                    //open tmp = tmp|0x10;
//		WriteAnalogReg (0x80+121, tmp);
//	}
//	else
//	{
//		tmp = ReadAnalogReg(0x80+121);
//		tmp = tmp&0xcf;
//		WriteAnalogReg (0x80+121, tmp);
//	}
//
//#if DEBUG_VREF
//	tmp = ReadAnalogReg(0x80+121);
//	tmp = tmp|0x20;                    //open tmp = tmp|0x10;
//	WriteAnalogReg (0x80+121, tmp);
//#endif
//
//}



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


/**
 * @brief  get adc sampled value
 * @param  none
 * @return sampled_value, raw data
 */
//unsigned short ADC_SampleValueGet(void){
//
//	unsigned short sampledValue,tmp1,tmp2;
//
//	while(CHECK_ADC_MISC_STATUS);
//	while(!CHECK_ADC_MISC_STATUS);
//	WaitUs (2);
//	tmp1 = ReadAnalogReg(0x80+120);  //read
//	tmp2 = ReadAnalogReg(0x80+119);
//	sampledValue = (tmp1<<8) + tmp2;
//
//	if(ADC_SingleEnd_mode)
//	{
//		sampledValue = sampledValue & 0x3FFF;
//	}
//	else
//	{
//		sampledValue = sampledValue & 0x7FFF;
//	}
//
//	return sampledValue;
//}
