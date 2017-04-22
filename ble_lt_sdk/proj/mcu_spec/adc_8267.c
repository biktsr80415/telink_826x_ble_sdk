/*
 * adc_8267.c
 *
 *  Created on: 2015-12-10
 *      Author: Telink
 */

#include "../tl_common.h"

#include "adc_8267.h"

#if(__TL_LIB_8267__ || (MCU_CORE_TYPE == MCU_CORE_8267) || \
	__TL_LIB_8261__ || (MCU_CORE_TYPE == MCU_CORE_8261) || \
	__TL_LIB_8269__ || (MCU_CORE_TYPE == MCU_CORE_8269))

/**************************************************************************************************
  Filename:       adc.c
  Author:		  qiuwei.chen@telink-semi.com
  Created Date:	  2017/04/19

  Description:    This file contains the adc driver functions for the Telink 8267.
                  It provided some sample applications like battery check and temperature check

**************************************************************************************************/
//#include "adc.h"

#define     ADC_DONE_SIGNAL_FALLING   (BIT(7))
//enable ADC clock
#define     EN_ADCCLK           (reg_adc_clk_en |= FLD_ADC_MOD_H_CLK)

//Select ADC auto mode
#define     EN_ADC_AUTO         do{\
	                            	reg_adc_ctrl &= 0x00;\
	                            	reg_adc_ctrl |= (FLD_ADC_CHNM_AUTO_EN|ADC_DONE_SIGNAL_FALLING);\
                                }while(0)


//Read sampling data
#define		READOUTPUTDATA		read_reg16(0x38)
//#define		READOUTPUTDATA		reg_adc_dat_byp_outp

/********************************************************
*
*	@brief		set ADC reference voltage for the Misc
*
*	@param		adcCha - enum variable adc channel.
*				adcRF - enum variable of adc reference voltage.
*
*	@return		None
*/
void adc_RefVoltageSet(enum ADCRFV adcRF){
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
void adc_ResSet(enum ADCRESOLUTION adcRes){
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

void adc_SampleTimeSet( enum ADCST adcST){

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
void adc_AnaChSet(enum ADCINPUTCH adcInCha){
	unsigned char cnI;

	cnI = (unsigned char)adcInCha;

	*(volatile unsigned char  *)(0x80002c) &= 0xE0;
	*(volatile unsigned char  *)(0x80002c) |= cnI;
}




/***************************************************************************
*
*	@brief	set IO power supply for the 1/3 voltage division detection, there are two input sources of the
*			IO input battery voltage, one through the VDDH and the other through the  ANA_B<7> pin
*
*	@param	IOp - input power source '1' is the VDDH; '2' is the ANA_B<7>.
*
*	@return	'1' setting success; '0' set error
*/
static unsigned char adc_IOPowerSupplySet(unsigned char IOp){
	unsigned char vv1;
	if(IOp>2||IOp<1){

		return 0;
	}
	else{
		vv1 = ReadAnalogReg(0x02);
		vv1 = vv1 & 0xcf;
		vv1 = vv1 | (IOp<<4);
		WriteAnalogReg(0x02,vv1);
		return 1;
	}
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
void adc_AnaModeSet( enum ADCINPUTMODE inM){
	unsigned char cnM;

	cnM = (unsigned char)inM;
	*(volatile unsigned char  *)(0x80002c) &= 0x1F;
	*(volatile unsigned char  *)(0x80002c) |= (cnM<<5);
}


/**********************************************************************
*	@brief	ADC initiate function, set the ADC clock details (4MHz) and start the ADC clock.
*			ADC clock relays on PLL, if the FHS isn't selected to 192M PLL (probably modified
*			by other parts codes), adc initiation function will returns error.
*
*	@param	None
*
*	@return	setResult - '1' set success; '0' set error
*/
unsigned char adc_Init(enum ADCCLOCK adc_clk){

	unsigned char fhsBL,fhsBH;

	fhsBH = reg_fhs_sel & 0x01;
	fhsBL = reg_clk_sel & 0x80;
	if((fhsBH!=0x00)&&(fhsBL!=0x00)){  //FHS select 192M, or return error.
		return 0;                      //the FHS is not 192Mhz
	}

	/******set adc clk as 4MHz; adc_clock = FHS*step/mode******/
	reg_adc_step_l = adc_clk; //adc clock step is 4
	reg_adc_mod_l  = 192;  //adc clock mode is 192.

//	WriteAnalogReg(0x88,0x0f);// select 192M clk output
	WriteAnalogReg(0x05, ReadAnalogReg(0x05) & 0x7f); //power on pll
	WriteAnalogReg(0x06, ReadAnalogReg(0x06) & 0xfe); //power on sar
	reg_adc_period_chn0 = (0xE2<<2);//set M channel period with 0xE2, the adc convert frequency is: system_clock/(4*0xE2);
	EN_ADCCLK;//Enable adc CLK
	EN_ADC_AUTO;
	return 1;
}

/**
 * @brief     set input channel,set reference voltage, set resolution bits, set sample cycle
 * @param[in] chl          - enum variable ADCINPUTCH ,acd channel
 * @param[in] ref_vol      - enum variable ADCRFV
 * @param[in] resolution   - enum variable ADCRESOLUTION
 * @param[in] sample_cycle - enum variable ADCST
 * @return    none
 */
void ADC_ParamSetting(enum ADCINPUTCH chn,enum ADCINPUTMODE mode,enum ADCRFV ref_vol,enum ADCRESOLUTION resolution,enum ADCST sample_cycle){
	/***1.set the analog input pin***/
	adc_AnaChSet(chn);

	/***2.set ADC mode,signle-end or differential mode***/
	adc_AnaModeSet(mode);///default is single-end

	/***3.set reference voltage***/
	adc_RefVoltageSet(ref_vol);

	/***4.set resolution***/
	adc_ResSet(resolution);

	/***5.set sample cycle**/
	adc_SampleTimeSet(sample_cycle);
}
/********************************************************
*
*	@brief		Initiate function for the battery check function
*
*	@param		checkM - Battery check mode, '0' for battery dircetly connected to chip,
*				'1' for battery connected to chip via boost DCDC
*
*	@return		None
*/
void adc_BatteryCheckInit(enum BATT_INPUTCHN checkM){
	/***1.set adc input***/
#if BATT_ONETHIRD_DIV_INTERNAL    ///if 1,internal 1/3 voltage division open.
	adc_AnaChSet(OTVDD);   //select "1/3 voltage division detection" as ADC input.
	/***1.1 select B7 or AVDD as input***/
	if(!checkM)
		adc_IOPowerSupplySet(1);
	else
		adc_IOPowerSupplySet(2);
#else
	adc_AnaChSet(B7);
#endif
	/***2.conft adc mode***/
	adc_AnaModeSet(SINGLEEND);

	/***3.set adc reference voltage***/
	adc_RefVoltageSet(RV_1P428);     //Set reference voltage (V_REF)as  1.428V

	/***4.set adc resultion***/
	adc_ResSet(RES14);               //Set adc resolution to 14 bits, bit[14] to bit bit[1]

	/***5.set adc sample time***/
	adc_SampleTimeSet(S_3);          //set sample time
}
/********************************************************
*
*	@brief		get the battery value
*
*	@param		None
*
*	@return		unsigned long - return the sampling value
*/
unsigned short adc_BatteryValueGet(void){

	unsigned short sampledValue;

	while(!CHECKADCSTATUS);

	while(CHECKADCSTATUS);

	sampledValue = READOUTPUTDATA & 0x3FFF;

	return sampledValue;
}

/********************************************************
*
*	@brief	Initiate function for the temperature sensor.
*	        temperature adc value = TEMSENSORP_adc_value - TEMSENSORN_adc_value
*           step 1: adc_TemSensorInit(TEMSENSORP);
*           step 2: TEMSENSORP_adc_value = adc_SampleValueGet();
*           step 3: adc_TemSensorInit(TEMSENSORN);
*           step 4: TEMSENSORN_adc_value = adc_SampleValueGet();
*           step 5: temperature adc value = TEMSENSORP_adc_value - TEMSENSORN_adc_value
*	@param	chn -- just select TEMSENSORN or TEMSENSORP
*
*	@return		None
*/

void adc_TemSensorInit(enum ADCINPUTCH chn){
	/***1.set adc mode and input***/
	adc_AnaChSet(chn);

	/***2.conft adc mode***/
	adc_AnaModeSet(SINGLEEND);

	/***3. set adc reference voltage***/
	adc_RefVoltageSet(RV_AVDD);

	/***4.set adc resultion***/
	adc_ResSet(RES14);

	/***5.set adc sample time***/
	adc_SampleTimeSet(S_3);

	/***6.enable manual mode***/
	EN_ADC_AUTO;
}

/*************************************************************************
*
*	@brief	get adc sampled value
*
*	@param	none
*
*	@return	sampled_value:	raw data
*/
unsigned short adc_SampleValueGet(void){
	unsigned short sampledValue;

	while(!CHECKADCSTATUS);

	while(CHECKADCSTATUS);

	sampledValue = READOUTPUTDATA & 0x3FFF;

	return sampledValue;
}

#endif
