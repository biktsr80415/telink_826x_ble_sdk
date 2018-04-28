#include "../../proj/tl_common.h"
#include "../../proj/drivers/adc.h"


#if (DRIVER_TEST_MODE == TEST_ADC)


#define  TEST_ADC_GPIO_SINGLE_IN			1
#define  TEST_ADC_GPIO_DIFF_IN				2
#define  TEST_ADC_VBAT						3




#define  TEST_ADC_SELECT					TEST_ADC_GPIO_SINGLE_IN





//and do not change setting in this function
void ADC_Init(void )
{

	/******enable signal of 24M clock to sar adc********/
	SET_ADC_CLK_EN();

//	SET_ADC_BANDGAP_ON();

	/******set adc sample clk as 4MHz******/
	adc_set_sample_clk(5); //adc sample clk= 24M/(1+5)=4M

	/******set adc L R channel Gain Stage bias current trimming******/
	adc_set_left_gain_bias(GAIN_STAGE_BIAS_PER100);
	adc_set_right_gain_bias(GAIN_STAGE_BIAS_PER100);

	adc_power_on(1); //powen on adc

}




void adc_gpio_ain_init(void)
{

//	adc_set_mode(NORMAL_MODE);  	//set normal mode

	//set misc channel en,  and max state cnt 2
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);  	//set total length for sampling state machine and channel


	//set "capture state" length for misc channel  0xf0, 240 * 1/24M
	//set "set state" length 0x0a, 10 * 1/24M
	adc_set_state_length(0x0f0, 0, 0x0a);  	//set R_max_mc,R_max_c,R_max_s

////set misc channel use differential_mode,
#if (TEST_ADC_SELECT == TEST_ADC_GPIO_SINGLE_IN)
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


	//set misc channel en,  and max state cnt 2
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);  	//set total length for sampling state machine and channel


	//set "capture state" length for misc channel  0xf0, 240 * 1/24M
	//set "set state" length 0x0a, 10 * 1/24M
	adc_set_state_length(0x0f0, 0, 0x0a);  	//set R_max_mc,R_max_c,R_max_s


////set misc channel use differential_mode,
	//内部VBAT channel有问题， 选一个不用的GPIO，输出高电平，来测电源电压
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

#if 1 //(TEST_ADC_SELECT == TEST_ADC_GPIO_SINGLE_IN)
	//step 1. get raw data from adc sample
	DBG_CHN1_HIGH;
	Adc_cur_rawData =  ADC_SampleValueGet();
	DBG_CHN1_LOW;

	Adc_raw_data[Adc_raw_datIndex ++]  = Adc_cur_rawData;
	new_data_flg = 1;


	//step 2. change raw data to real data in differential_mode
	if(Adc_cur_rawData & BIT(13)) //14 bit resolution in differential_mode,  if 12 bit rers, here & BIT(11)
	{
		Adc_cur_rawData = 0;
	}
	else
	{
		Adc_cur_rawData &= 0x1FFF;   //if 12 bit res, here &= 0x7FF
	}





	if(new_data_flg){
//		Adc_cur_calData = Adc_cur_rawData * 1200/0x1FFF;  //14 bit resolution, vref = 1.2 V, pre_scaler = 1
//		Adc_cur_calData = Adc_cur_rawData * 2400/0x1FFF;  //14 bit resolution, vref = 1.2 V, pre_scaler = 2
		Adc_cur_calData = Adc_cur_rawData * 4800/0x1FFF;  //14 bit resolution, vref = 1.2 V, pre_scaler = 4

		Adc_cal_data[Adc_cal_datIndex ++] = Adc_cur_calData;



		Adc_cur_vol_oct =   (Adc_cur_calData/1000)<<12 | ((Adc_cur_calData/100)%10)<<8 \
				                      | ((Adc_cur_calData%100)/10)<<4  | (Adc_cur_calData%10);
		Adc_cal_vol_oct[Adc_cal_vol_octIndex ++] = Adc_cur_vol_oct;
	}

#else


#endif

}




#endif   //end of DRIVER_TEST_MODE == TEST_ADC
