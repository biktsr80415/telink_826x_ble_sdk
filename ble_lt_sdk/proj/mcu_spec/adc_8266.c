/*
 * adc_8266.c
 *
 *  Created on: 2015-12-10
 *      Author: Telink
 */


#include "../tl_common.h"

#if(__TL_LIB_8266__ || (MCU_CORE_TYPE == MCU_CORE_8266))

#if ( MODULE_ADC_ENABLE)
#include "../../proj_lib/rf_drv.h"

u8 adc_clk_step_l  = 0;
u8 adc_chn_m_input = 0;

#define		EN_MANUALM			(reg_adc_ctrl = 0x20)

static inline void adc_SetClkFreq(u8 mhz){
	reg_adc_step_l = mhz*4;
	reg_adc_mod = MASK_VAL(FLD_ADC_MOD, 192*4, FLD_ADC_CLK_EN, 1);
}

static inline void adc_SetPeriod(void){
	reg_adc_period_chn0 = 77;
	reg_adc_period_chn12 = 8;
}

void adc_ClkEn(int en){
	if (en) {
		reg_adc_clk_en |= BIT(7);                    // Eanble the clock
		analog_write(0x06,(analog_read(0x06)&0xfe)); // Enable ADC LDO
	} else {
	    reg_adc_clk_en &= ~ BIT(7);                  // Disable ADC clock
		analog_write(0x06,(analog_read(0x06)|0x01)); // Disable ADC LDO
	}
	adc_clk_poweron ();
}

/********************************************************
*
*	@brief		set ADC reference voltage for the Misc and L channel
*
*	@param		adcCha - enum variable adc channel.
*				adcRF - enum variable of adc reference voltage.
*
*	@return		None
*/
void adc_RefVoltageSet(ADC_REFVOL_t adcRF){
	unsigned char st;

	st = (unsigned char)adcRF;
	*(volatile unsigned char  *)0x80002b &= 0xFC;

	*(volatile unsigned char  *)0x80002b |= st;
}

/********************************************************
*
*	@brief		set ADC resolution for channel Misc
*
*	@param		adcRes - enum variable adc resolution.
*
*	@return		None
*/
void adc_ResSet(ADC_RESOLUTION_t adcRes){
	unsigned char resN;
	resN = (unsigned char )adcRes;
	*(volatile unsigned char  *)0x80003c &= 0xC7;
	*(volatile unsigned char  *)0x80003c |= (resN<<3);
}

/********************************************************
*
*	@brief		set ADC sample time(the number of adc clocks for each sample)
*
*	@param		adcCha - enum variable adc channel.
*				adcST - enum variable of adc sample time.
*
*	@return		None
*/

void adc_SampleTimeSet(ADC_SAMPCYC_t adcST){

	unsigned char st;
	st = (unsigned char)adcST;

	*(volatile unsigned char  *)(0x80003c) &= 0xF8;

	*(volatile unsigned char  *)(0x80003c) |= st;
}

/********************************************************
*
*	@brief		set ADC analog input channel
*
*	@param		adcCha - enum variable adc channel.
*				adcInCha - enum variable of adc input channel.
*
*	@return		None
*/
void adc_AnaChSet(ADC_INPUTCHN_t adcInCha){
	unsigned char cnI;

	cnI = (unsigned char)adcInCha;

	*(volatile unsigned char  *)(0x80002c) &= 0xE0;
	*(volatile unsigned char  *)(0x80002c) |= cnI;
}

/********************************************************
*
*	@brief		set ADC input channel mode - signle-end or differential mode
*
*	@param		adcCha - enum variable adc channel.
*				inM - enum variable of ADCINPUTMODE.
*
*	@return		None
*/
void adc_AnaModeSet(ADC_INPUTMODE_t inM){
	unsigned char cnM;

	cnM = (unsigned char)inM;
	*(volatile unsigned char  *)(0x80002c) &= 0x1F;
	*(volatile unsigned char  *)(0x80002c) |= (cnM<<5);
}

/*****
 * @brief init adc module. such as adc clock, input channel, resolution, reference voltage and so on.
 *        notice: adc clock: when the reference voltage is AVDD, the adc clock must be lower than 5Mhz.
 *        when the reference voltage is 1.4, the adc clock must be lower than 4Mhz.
 * @param[in] adc_clock    - enum ADC_CLK_t, set adc clock.
 * @param[in] chn          - enum ADC_INPUTCHN_t ,acd channel
 * @param[in] mode         - enum ADC_INPUTMODE_t
 * @param[in] ref_vol      - enum ADC_REFVOL_t, adc reference voltage.
 * @param[in] resolution   - enum ADC_RESOLUTION_t
 * @param[in] sample_cycle - enum ADC_SAMPCYC_t
 * @return    none
 */
void adc_init(ADC_CLK_t adc_clock, ADC_INPUTCHN_t chn, ADC_INPUTMODE_t mode, ADC_REFVOL_t ref_vol, ADC_RESOLUTION_t resolution, ADC_SAMPCYC_t sample_cycle)
{
	/**set adc clock**/
	adc_SetClkFreq(adc_clock);

	/**select the input channel**/
	adc_AnaChSet(chn);
	/**set the adc's mode**/
	adc_AnaModeSet(mode);
	/**set the reference voltage**/
	adc_RefVoltageSet(ref_vol);
	/**set resolution**/
	adc_ResSet(resolution);
	/**set sample cycle**/
	adc_SampleTimeSet(sample_cycle);

	adc_ClkEn(1);    // enable ADC clock
	EN_MANUALM;      // enable manual mode
}

u16 adc_get(void)
{
	reg_adc_chn1_outp = FLD_ADC_CHN_MANU_START;  // Set a run signal,start to run adc

	sleep_us(5);   // wait for data

	return (reg_adc_dat_byp_outp & 0x3FFF);// read data
}

/* between start and end must > 200us
 * ½öÊÊÓÃÓÚ8266 A2Ð¾Æ¬
 * power down step£º
 * 	1, change input to GND
 * 	2, lower adc clk
 * 	3, start adc
 * 	4, wait > 200us(we can do something else)
 * 	5, start adc again
 * 	6, wait adc latch state,the interval calculation base on system clk and adc clk
 * 	7, power down adc in analog register
 * 	9, recover adc setting
 * */
void adc_power_down(void){
	u8 adc_m_input = reg_adc_chn_m_sel;
	u8 adc_step_low = reg_adc_step_l;
	/*step 0*/
	write_reg8(0x2c,0x12);  //chnm: GND
	write_reg8(0x69,0x01);  //set adc clk = 192M/(192*4) = 0.25K
	write_reg8(0x35,0x80);  //start

	sleep_us(200);
	/*step 1*/
	write_reg8(0x35,0x80);  //start again
	sleep_us(26);             //14 us
	analog_write(0x06,analog_read(0x06) | 0x01);
	/*step 2*/
	reg_adc_chn_m_sel = adc_m_input;
	reg_adc_step_l = adc_step_low;//set adc clk= 192M * 16/(192 *4) = 4M
}

void adc_power_down_start(void){

	write_reg8(0x2c,0x12);  //chn: GND
	write_reg8(0x69,0x01);  //set adc clk = 192M/(192*4) = 0.25K
	write_reg8(0x35,0x80);  //start
}
void adc_power_down_end(void){
	write_reg8(0x35,0x80);  //start again
	sleep_us(26);             //14 us
	analog_write(0x06,analog_read(0x06) | 0x01);
}
void adc_setting_recover(void){
#if 1
	reg_adc_step_l = adc_clk_step_l;
	reg_adc_chn_m_sel = adc_chn_m_input;
#else
	reg_adc_chn_m_sel = FLD_ADC_CHN_C7;
	reg_adc_step_l = 16;//set adc clk= 192M * 16/(192 *4) = 4M

#endif
}

/*
void adc_example(void)
{
    adc_init(GPIO_PC2, ADC_REF_1_3V);
    ......
    u16 result = adc_get();
    printf("voltage = %dmv\r\n", ((result*1300)>>14)); // Here >>14 means ( result * 1300 / 0x3fff)
}
*/

#endif
#endif

