#include "battery_check.h"
#include "tl_common.h"
#include "drivers.h"

#if 0

/**
 * @Brief:  Check battery voltage.
 * @Param:  ADC_Module ->
 * @RetVal: None.
 */
void TL_BatteryCheckInit(eADC_ChannelTypeDef ADC_Channel)
{
	//ADC clock settings.
	ADC_ClockSet(3000000);//3M

	//Clear ADC Enable register.
	analog_write(ADC_ENABLE_ADDR,0x00);//Must

	//Set ADC stable time.
	ADC_StableTime(ADC_Module_M, ADC_Stable_Time_6Cyc);

	//Reference voltage settings
	ADC_VrefSet(ADC_Vref_1p2);

	//Resolution settings
	ADC_ResolutionSet(ADC_Module_M, ADC_Resolution_14b);

	//Battery divider voltage factor settings
	ADC_VbatDivFactorSet(ADC_Vbat_Div_Factor_Off);

	//ADC mode settings:singlend or differential
	ADC_ModeSet(ADC_Module_M, ADC_Mode_Diff);

	//ADC channel settings
	gpio_set_output_en(ADC_BATTERY_CHECK_PIN,1);//must
	gpio_set_input_en(ADC_BATTERY_CHECK_PIN,0);//must
	gpio_write(ADC_BATTERY_CHECK_PIN,1);//must
	ADC_DifferChannelSet(ADC_Module_M, ADC_Channel, ADC_Channel_GND);

	//ADC input voltage divider factor settings
	ADC_InputVolDivFactor(ADC_Vref_1p2, ADC_Input_Vol_Div_Factor_8);

	//Sample Rate settings.
	ADC_SampleRateSet(ADC_Module_M,0x0a,0x0f0);

	//ADC enable
	ADC_Cmd(ADC_Module_M, ENABLE);
}
/**
 * @Brief:  Bubble sort.
 * @Param:  pData -> pointer point to data
 * @Param:  len -> lenght of data
 * @Return: None.
 */
void BubbleSort(unsigned short *pData, unsigned int len)
{
	for(int i = 0; i< len-1; i++)
	{
		for(int j = 0; j<len-1 - i; j++)
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
#define SAMPLE_NUM   8
u16 batteryVol[SAMPLE_NUM];
u8 batteryVolCnt = 0;
void TL_BattteryCheckProc(void)
{
	/* Battery check frequency is 1 per 100ms. */
	static unsigned int batteryCheckStartTick = 0;
	if(clock_time_exceed(batteryCheckStartTick, 50*1000)){
		batteryCheckStartTick = clock_time();

		//Power on ADC
		ADC_PowerOn();
	}else{
		return;
	}

	/* Get ADC value. */
	unsigned short adcValue[SAMPLE_NUM] = {0};
	for(int i = 0; i < SAMPLE_NUM; i++)
	{
		adcValue[i] = ADC_GetConvertValue();
		if((adcValue[i] & BIT(14)) != 0)
		{
			adcValue[i] += 1;
			adcValue[i] = ~adcValue[i];
		}
	}

	BubbleSort(adcValue, SAMPLE_NUM);

	/* Process ADC value. */
	unsigned int sum = 0;
	for(int i = 1; i< SAMPLE_NUM - 1; i++)
	{
		sum += adcValue[i];
	}
	unsigned short adcAverageValue = sum / (SAMPLE_NUM-2);

	/* Battery voltage calculation. */
	unsigned short vol = ((adcAverageValue * 1200)>>13) * 8;

	/* Power off ADC for saving power */
	ADC_PowerOff();

	/* Low voltage processing. Enter deep sleep. */
#if 0
	if(vol < 2000){
		analog_write(DEEP_ANA_REG2, BATTERY_VOL_LOW);
		cpu_sleep_wakeup(PM_SLeepMode_Deep, PM_WAKEUP_PAD, 0);
	}else{
		static u8 isVolRecovery = 0;
		if(isVolRecovery == 0){
			analog_write(DEEP_ANA_REG2, BATTERY_VOL_OK);
			isVolRecovery = 1;
		}
	}
#else
	batteryVol[batteryVolCnt++] = vol;
	u8 lowVolCnt = 0;
	u8 highVolCnt = 0;
	if(batteryVolCnt < 5){
		return;
	}else{
		for(int i = 0; i<batteryVolCnt; i++)
		{
			if(batteryVol[i] > 2000){
				highVolCnt++;
			}else{
				lowVolCnt++;
			}
		}
	}

	if(highVolCnt > lowVolCnt){
		batteryVolCnt = 0;

		//Run a time after power on.
		static u8 isVolRecovery = 0;
		if(isVolRecovery == 0){
			analog_write(DEEP_ANA_REG2, BATTERY_VOL_OK);
			isVolRecovery = 1;
		}
	}else{
		batteryVolCnt = 0;
		analog_write(DEEP_ANA_REG2, BATTERY_VOL_LOW);
		cpu_sleep_wakeup(PM_SLeepMode_Deep, PM_WAKEUP_PAD, 0);
	}
#endif
}

#else
void TL_BatteryCheckInit(void)
{
	/****** sar adc Reset ********/
	//reset whole digital adc module
	adc_reset_adc_module();

	/******power on sar adc********/
	adc_power_on_sar_adc(1);

	/******enable signal of 24M clock to sar adc********/
	adc_enable_clk_24m_to_sar_adc(1);

	/******set adc clk as 3MHz******/
	adc_set_sample_clk(7);

	/**** 优化了ADC低温抖动的问题 ******************/
	adc_set_atb(ADC_SEL_ATB_1);

	pga_left_chn_power_on(1);
	pga_right_chn_power_on(1);
	adc_set_left_gain_bias(GAIN_STAGE_BIAS_PER100);
	adc_set_right_gain_bias(GAIN_STAGE_BIAS_PER100);

	//set R_max_mc,R_max_c,R_max_s
	adc_set_length_capture_state_for_chn_misc_rns(0xf0);//max_mc
	adc_set_length_set_state(0x0a);	//max_s

	//set channel Vref
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);

	//set Vbat divider select,
	adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_OFF);

	//must
	gpio_set_func(BATTERY_CHECK_PIN,AS_GPIO);
	gpio_set_input_en(BATTERY_CHECK_PIN,0);
	gpio_set_output_en(BATTERY_CHECK_PIN,1);
	gpio_write(BATTERY_CHECK_PIN,1);

	//set channel mode and channel
	adc_set_input_mode(ADC_MISC_CHN, DIFFERENTIAL_MODE);
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, B3P, GND);

	//set resolution for MISC
	adc_set_resolution(ADC_MISC_CHN, RES14);

	//Number of ADC clock cycles in sampling phase
	adc_set_tsample_cycle(ADC_MISC_CHN, SAMPLING_CYCLES_6);

	//set Analog input pre-scaling and
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);

	//set RNG mode
	adc_set_mode(NORMAL_MODE);

	//set total length for sampling state machine and channel
	adc_set_max_state_cnt(0x02);
	adc_set_chn_enable(ADC_MISC_CHN);
}

/**
 * @Brief:  Bubble sort.
 * @Param:  pData -> pointer point to data
 * @Param:  len -> lenght of data
 * @Return: None.
 */
void BubbleSort(unsigned short *pData, unsigned int len)
{
	for(int i = 0; i< len-1; i++)
	{
		for(int j = 0; j<len-1 - i; j++)
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
#define SAMPLE_NUM   8
//u16 batteryVol[SAMPLE_NUM];
//u8 batteryVolCnt = 0;
void TL_BattteryCheckProc(void)
{
	/* Battery check frequency is 1 per 100ms. */
	static unsigned int batteryCheckStartTick = 0;
	if(clock_time_exceed(batteryCheckStartTick, 50*1000)){
		batteryCheckStartTick = clock_time();

		//Power on ADC
		//ADC_PowerOn();
		adc_power_on_sar_adc(1);
	}else{
		return;
	}

	/* Get ADC value. */
	unsigned short adcValue[SAMPLE_NUM] = {0};
	for(int i = 0; i < SAMPLE_NUM; i++)
	{
		adcValue[i] = ADC_SampleValueGet();
		if((adcValue[i] & BIT(14)) != 0)
		{
			adcValue[i] += 1;
			adcValue[i] = ~adcValue[i];
		}
	}

	BubbleSort(adcValue, SAMPLE_NUM);

	/* Process ADC value. */
	unsigned int sum = 0;
	for(int i = 1; i< SAMPLE_NUM - 1; i++)
	{
		sum += adcValue[i];
	}
	unsigned short adcAverageValue = sum / (SAMPLE_NUM-2);

	/* Battery voltage calculation. */
	unsigned short vol = ((adcAverageValue * 1200)>>13) * 8;

	/* Power off ADC for saving power */
	//ADC_PowerOff();
	adc_power_on_sar_adc(0);

	/* Low voltage processing. Enter deep sleep. */
#if 1
	if(vol < 2000){
		cpu_sleep_wakeup(PM_SLeepMode_Deep, PM_WAKEUP_PAD, 0);
	}
#else
	batteryVol[batteryVolCnt++] = vol;
	u8 lowVolCnt = 0;
	u8 highVolCnt = 0;
	if(batteryVolCnt < 5){
		return;
	}else{
		for(int i = 0; i<batteryVolCnt; i++)
		{
			if(batteryVol[i] > 2000){
				highVolCnt++;
			}else{
				lowVolCnt++;
			}
		}
	}

	if(highVolCnt > lowVolCnt){
		batteryVolCnt = 0;

		//Run a time after power on.
		static u8 isVolRecovery = 0;
		if(isVolRecovery == 0){
			analog_write(DEEP_ANA_REG2, BATTERY_VOL_OK);
			isVolRecovery = 1;
		}
	}else{
		batteryVolCnt = 0;
		analog_write(DEEP_ANA_REG2, BATTERY_VOL_LOW);
		cpu_sleep_wakeup(PM_SLeepMode_Deep, PM_WAKEUP_PAD, 0);
	}
#endif
}

#endif
