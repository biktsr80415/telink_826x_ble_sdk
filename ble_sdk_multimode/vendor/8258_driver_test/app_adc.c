#include "tl_common.h"
#include "drivers.h"


#if (DRIVER_TEST_MODE == TEST_ADC)


#define  TEST_ADC_GPIO_SINGLE_IN			1
#define  TEST_ADC_GPIO_DIFF_IN				2
#define  TEST_ADC_VBAT						3




#define  TEST_ADC_SELECT					TEST_ADC_GPIO_SINGLE_IN




signed int misc_dat_buf[4];  //must 16 byte


//and do not change setting in this function
void ADC_Init(void )
{
	//reset whole digital adc module
	adc_reset_adc_module();

	/******power on sar adc********/
	adc_power_on_sar_adc(1);

	/******enable signal of 24M clock to sar adc********/
	adc_enable_clk_24m_to_sar_adc(1);


	/******set adc sample clk as 4MHz******/
	adc_set_sample_clk(5); //adc sample clk= 24M/(1+5)=4M

	/******set adc L R channel Gain Stage bias current trimming******/
	adc_set_left_gain_bias(GAIN_STAGE_BIAS_PER100);
	adc_set_right_gain_bias(GAIN_STAGE_BIAS_PER100);



}




void adc_gpio_ain_init(void)
{
	//set misc channel en,  and adc state machine state cnt 2( "set" stage and "capture" state for misc channel)
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);  	//set total length for sampling state machine and channel

	//set "capture state" length for misc channel: 240
	//set "set state" length for misc channel: 10
	//adc state machine  period  = 24M/250 = 96K
	adc_set_state_length(240, 0, 10);  	//set R_max_mc,R_max_c,R_max_s

////set misc channel use differential_mode,
#if (TEST_ADC_SELECT == TEST_ADC_GPIO_SINGLE_IN)
	//application use single in, but hardware must use differential_mode, negative channel set as GND
	//PB4 positive channel, GND negative channel
	gpio_set_func(GPIO_PB4, AS_GPIO);
	gpio_set_input_en(GPIO_PB4, 0);
	gpio_set_output_en(GPIO_PB4, 0);
	gpio_write(GPIO_PB4, 0);
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, B4P, GND);
#elif (TEST_ADC_SELECT == TEST_ADC_GPIO_DIFF_IN)
	//PB4 positive channel, PB5 negative channel
	gpio_set_func(GPIO_PB4, AS_GPIO);
	gpio_set_input_en(GPIO_PB4, 0);
	gpio_set_output_en(GPIO_PB4, 0);
	gpio_write(GPIO_PB4, 0);
	gpio_set_func(GPIO_PB5, AS_GPIO);
	gpio_set_input_en(GPIO_PB5, 0);
	gpio_set_output_en(GPIO_PB5, 0);
	gpio_write(GPIO_PB5, 0);
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, B4P, B5N);
#endif

	//set misc channel vref 1.2V
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);  					//set channel Vref

	//set misc channel resolution 14 bit
	//notice that: in differential_mode MSB is sign bit, rest are data,  here BIT(13) is sign bit
	adc_set_resolution(ADC_MISC_CHN, RES14);						//set resolution

	//set misc t_sample 6 cycle of adc clock:  6 * 1/4M
	adc_set_tsample_cycle(ADC_MISC_CHN, SAMPLING_CYCLES_6);  	//Number of ADC clock cycles in sampling phase

	//set Analog input pre-scaling 1/4
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F4);


}




void adc_vbat_detect_init(void)
{


	//set misc channel en,  and adc state machine state cnt 2( "set" stage and "capture" state for misc channel)
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);  	//set total length for sampling state machine and channel

	//set "capture state" length for misc channel: 240
	//set "set state" length for misc channel: 10
	//adc state machine  period  = 24M/250 = 96K
	adc_set_state_length(240, 0, 10);  	//set R_max_mc,R_max_c,R_max_s


////set misc channel use differential_mode,
	//内部VBAT channel有问题， 选一个不用的GPIO，输出高电平（此电平值等于电源电压），来测电源电压
	gpio_set_func(GPIO_PB0, AS_GPIO);
	gpio_set_input_en(GPIO_PB0, 0);
	gpio_set_output_en(GPIO_PB0, 1);
	gpio_write(GPIO_PB0, 1);
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, B0P, GND);

	//set misc channel vref 1.2V
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);  					//set channel Vref

	//set misc channel resolution 14 bit
	//notice that: in differential_mode MSB is sign bit, rest are data,  here BIT(13) is sign bit
	adc_set_resolution(ADC_MISC_CHN, RES14);						//set resolution

	//set misc t_sample 6 cycle of adc clock:  6 * 1/4M
	adc_set_tsample_cycle(ADC_MISC_CHN, SAMPLING_CYCLES_6);  	//Number of ADC clock cycles in sampling phase

	//set Analog input pre-scaling 1/4
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F4);


}




void app_adc_test_init(void)
{
	ADC_Init();  //this is must


#if (TEST_ADC_SELECT == TEST_ADC_GPIO_SINGLE_IN || TEST_ADC_SELECT == TEST_ADC_GPIO_DIFF_IN)
	adc_gpio_ain_init();

#elif (TEST_ADC_SELECT == TEST_ADC_VBAT)

	adc_vbat_detect_init();

#endif


	//use dfifo mode to get adc sample data
	adc_config_misc_channel_buf((s16 *)misc_dat_buf, sizeof(misc_dat_buf));

}





u16 Adc_cur_rawData;
u16 Adc_raw_data[256];
u8  Adc_raw_datIndex = 0;



u32 Adc_cur_calData;  //unit: m V
u16 Adc_cal_data[256];
u8  Adc_cal_datIndex = 0;



//just for display, fake data
u32 Adc_cur_vol_oct; //debug
u16 Adc_cal_vol_oct[256];
u8  Adc_cal_vol_octIndex= 0;

int new_data_flg;


void app_adc_test_start(void)
{
	new_data_flg = 0 ;


	//step 1. get raw data from adc sample
//	DBG_CHN2_TOGGLE;

//	DBG_CHN1_HIGH;  //debug
	Adc_cur_rawData =  (u16)misc_dat_buf[0];
//	DBG_CHN1_LOW;  //debug

	Adc_raw_data[Adc_raw_datIndex ++]  = Adc_cur_rawData;
	new_data_flg = 1;


	//step 2. change raw data to real data in differential_mode
#if (TEST_ADC_SELECT == TEST_ADC_GPIO_SINGLE_IN || TEST_ADC_SELECT == TEST_ADC_VBAT)
	//application use single in, but hardware must use differential_mode, negative channel set as GND

	if(Adc_cur_rawData & BIT(13))   //14 bit resolution in differential_mode,  BIT<13> is symbol bit
//	if(Adc_cur_rawData & BIT(12)) 	//13 bit resolution in differential_mode,  BIT<12> is symbol bit
//	if(Adc_cur_rawData & BIT(11)) 	//12 bit resolution in differential_mode,  BIT<11> is symbol bit
	{
		Adc_cur_rawData = 0;
	}
	else
	{
		Adc_cur_rawData &= 0x1FFF; //14 bit resolution, BIT<12:0> is valid data
//	    Adc_cur_rawData &= 0x0FFF; //14 bit resolution, BIT<11:0> is valid data
//		Adc_cur_rawData &= 0x07FF; //14 bit resolution, BIT<10:0> is valid data
	}
#elif (TEST_ADC_SELECT == TEST_ADC_GPIO_DIFF_IN)
	//application use diff in

	// to be add
#endif


	if(new_data_flg){
//		Adc_cur_calData = Adc_cur_rawData * 1200/0x1FFF;  //14 bit resolution, vref = 1200 mV, pre_scaler = 1
//		Adc_cur_calData = Adc_cur_rawData * 2400/0x1FFF;  //14 bit resolution, vref = 1200 mV, pre_scaler = 2
		Adc_cur_calData = Adc_cur_rawData * 4800/0x1FFF;  //14 bit resolution, vref = 1200 mV, pre_scaler = 4
//		Adc_cur_calData = Adc_cur_rawData * 2400/0x0FFF;  //13 bit resolution, vref = 1200 mV, pre_scaler = 2
//		Adc_cur_calData = Adc_cur_rawData * 4800/0x0FFF;  //13 bit resolution, vref = 1200 mV, pre_scaler = 4
//		Adc_cur_calData = Adc_cur_rawData * 2400/0x07FF;  //12 bit resolution, vref = 1200 mV, pre_scaler = 2
//		Adc_cur_calData = Adc_cur_rawData * 4800/0x07FF;  //12 bit resolution, vref = 1200 mV, pre_scaler = 4

		Adc_cal_data[Adc_cal_datIndex ++] = Adc_cur_calData;



		Adc_cur_vol_oct =   (Adc_cur_calData/1000)<<12 | ((Adc_cur_calData/100)%10)<<8 \
				                      | ((Adc_cur_calData%100)/10)<<4  | (Adc_cur_calData%10);
		Adc_cal_vol_oct[Adc_cal_vol_octIndex ++] = Adc_cur_vol_oct;
	}


}




#endif   //end of DRIVER_TEST_MODE == TEST_ADC
