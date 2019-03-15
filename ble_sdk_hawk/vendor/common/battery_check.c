#include "battery_check.h"
#include "tl_common.h"
#include "drivers.h"
#include <stack/ble/ble.h>

#if BATT_CHECK_ENABLE

int lowBattDet_enable = 1;
int adc_hw_initialized = 0;


void battery_set_detect_enable (int en)
{
	lowBattDet_enable = en;

	if(!en){
		adc_hw_initialized = 0;   //need initialized again
	}

}

int battery_get_detect_enable (void)
{
	return lowBattDet_enable;
}

void TL_BatteryCheckInit(void)
{
	/****** sar adc Reset ********/
	//reset whole digital adc module
	adc_reset_adc_module();

	/******power on sar adc********/
	adc_power_on_sar_adc(0);

	/******enable signal of 24M clock to sar adc********/
	adc_enable_clk_24m_to_sar_adc(1);

	/******set adc clk as 4MHz******/
	adc_set_sample_clk(5);

	/**** 优化了ADC低温抖动的问题 ******************/
	adc_set_atb(ADC_SEL_ATB_1);
	adc_set_left_gain_bias(GAIN_STAGE_BIAS_PER100);
	adc_set_right_gain_bias(GAIN_STAGE_BIAS_PER100);

	//set R_max_mc,R_max_c,R_max_s
	adc_set_length_capture_state_for_chn_misc_rns(0xf0);//max_mc  //sample rate: 96K
	adc_set_length_set_state(0x0a);	//max_s

	//set channel Vref
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);
	adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_OFF);

	//set Vbat divider select,
	adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_OFF);

	//must
	gpio_set_output_en(BATTERY_CHECK_PIN,1);
	gpio_write(BATTERY_CHECK_PIN,1);

	//set channel mode and channel
	adc_set_input_mode(ADC_MISC_CHN, DIFFERENTIAL_MODE);
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, BATTERY_CHECK_ADC_CHN, GND);

	//set resolution for MISC
	adc_set_resolution(ADC_MISC_CHN, RES14);

	//Number of ADC clock cycles in sampling phase
	adc_set_tsample_cycle(ADC_MISC_CHN, SAMPLING_CYCLES_6);

	//set Analog input pre-scaling and
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);

	//set RNG mode
	adc_set_mode(NORMAL_MODE);//MUST

	//set total length for sampling state machine and channel
	adc_set_max_state_cnt(0x02);
	adc_set_chn_enable(ADC_MISC_CHN);

	//adc_config_misc_channel_buf((s16*)adcValue, sizeof(adcValue));
	reg_dfifo_mode &= ~DFIFO_Mode_FIFO2_Input;

//	adc_power_on_sar_adc(1);
}

/**
 * @Brief:  Bubble sort.
 * @Param:  pData -> pointer point to data
 * @Param:  len -> lenght of data
 * @Return: None.
 */
void BubbleSort(unsigned short *pData, unsigned int len)
{
	for(volatile int i = 0; i< len-1; i++)
	{
		for(volatile int j = 0; j<len-1 - i; j++)
		{
			if(pData[j] > pData[j+1])
			{
				unsigned short temp = pData[j];
				pData[j] = pData[j+1];
				pData[j+1] = temp;
			}
		}
	}
}

/**
 * @Brief:  Battery check.
 * @Param:  None.
 * @Return: None.
 */
volatile short int adcValue[ADC_SAMPLE_NUM];
u16 A_Vol = 0;
void TL_BattteryCheckProc(int minVol_mV)
{

	if(!adc_hw_initialized){
		adc_hw_initialized = 1;
		TL_BatteryCheckInit();
	}


	adc_reset_adc_module();
	adc_power_on_sar_adc(1);

	//clear adcValue buffer
	for(volatile int i=0; i<ADC_SAMPLE_NUM; i++){
		adcValue[i] = 0;
	}



	sleep_us(25);//must wait 2 adc cycle;

	adc_config_misc_channel_buf((s16*)adcValue, sizeof(adcValue));
	reg_dfifo_mode |= DFIFO_Mode_FIFO2_Input;

	/* Get ADC value. */
	u16 adcTempBuf[ADC_SAMPLE_NUM] = {0};

	for(volatile int i = 0; i<ADC_SAMPLE_NUM; i++)
	{
		while(!adcValue[i]);

		if(adcValue[i] & BIT(13)){//negative voltage
			adcTempBuf[i] = 0;
		}else{
			adcTempBuf[i] = adcValue[i] & 0x1fff;
		}
	}

	//Power off ADC and DFIFO2 for saving power
	adc_power_on_sar_adc(0);
	reg_dfifo_mode &= ~DFIFO_Mode_FIFO2_Input;

	BubbleSort(adcTempBuf, ADC_SAMPLE_NUM);

	u16 adcValueEx = (adcTempBuf[2] + adcTempBuf[3] + adcTempBuf[4] + adcTempBuf[5]) >> 2;

	u16 vol = (adcValueEx * 1180 * 8)>>13;//Unit:mV; 由于参考电压不准，实际的参考电压为1.18V(Vref = 1.2V)

	//Debug
	A_Vol = vol;
//	REG_ADDR16(0x8000) = A_Vol;

	/* Low voltage processing. Enter deep sleep. */
	if(vol < minVol_mV){

		#if (1 && BLT_APP_LED_ENABLE)  //led indicate
			gpio_set_output_en(GPIO_LED, 1);  //output enable
			for(int k=0;k<3;k++){
				gpio_write(GPIO_LED, 1);
				sleep_us(200000);
				gpio_write(GPIO_LED, 0);
				sleep_us(200000);
			}
			gpio_set_output_en(GPIO_LED, 0);
		#endif

		analog_write(DEEP_ANA_REG2, BATTERY_VOL_LOW);
		cpu_sleep_wakeup(PM_SLeepMode_Deep, PM_WAKEUP_PAD, 0);
	}


#if 0  //debug
	u8	tbl_advData[20 ] = {0};

	u32 avg_convert_oct =   (vol/1000)<<12 | ((vol/100)%10)<<8 | ((vol%100)/10)<<4  | (vol%10);
	tbl_advData[0] = avg_convert_oct>>8;
	tbl_advData[1] = avg_convert_oct&0xff;


	if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV){
		bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	}
	else if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
		bls_att_pushNotifyData (BATT_LEVEL_INPUT_DP_H, tbl_advData, 20);
	}
#endif
}

#endif

