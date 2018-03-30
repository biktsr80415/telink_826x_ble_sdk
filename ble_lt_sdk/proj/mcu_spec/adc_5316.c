/**
  ******************************************************************************
  * @File: 		 adc_5316.c
  * @CreateDate: 2017-12-22
  * @Author:     qiu.gao@telink-semi.com
  * @Copyright:  (C)2017-2020 telink-semi Ltd.co
  * @Brief:
  ******************************************************************************
  */
#include "../tl_common.h"
#include "adc_5316.h"

#if(MCU_CORE_TYPE == MCU_CORE_5316)
/**
 * @Brief: ADC power on.
 * @Param:
 * @RetVal: None.
 */
void ADC_PowerOn(void)
{
	WriteAnalogReg (0x80+124, analog_read(0x80+124) & (~BIT(5)));
}

/**
 * @Brief: ADC power off.
 * @Param:
 * @RetVal: None.
 */
void ADC_PowerOff(void)
{
	WriteAnalogReg (0x80+124,analog_read(0x80+124)|BIT(5));
}

/**
 * @Brief: Set ADC Clock Frequency(unit:Hz).
 * @Param:
 * @RetVal: None.
 */
void ADC_ClockSet(u32 ADC_Clk_Hz)
{
	u8 ADC_ClkDiv = 24000000/ADC_Clk_Hz - 1;
	if(ADC_ClkDiv > 0x07)
		return;

	//Enable 24M clock to ADC.
	u8 temp = analog_read(0x80);
	temp |= BIT(7);
	analog_write(0x80,temp);

	//Set ADC clock divider.
	temp = analog_read(ADC_CLK_DIV_ADDR);
	temp &= ~ADC_CLK_DIV_Pos;
	temp |= ADC_ClkDiv;
	analog_write(ADC_CLK_DIV_ADDR,temp);

	//ADC Module power on.Must
	WriteAnalogReg (0x80+124, 0x05);
}

/**
 * @Brief: Set ADC Clock Frequency(unit:Hz; max = 3MHz).
 * @Param:
 * @RetVal: None.
 */
void ADC_ClockSetEx(u8 ADC_ClkDiv)
{
	if(ADC_ClkDiv > 0x07)
			return;

	//Enable 24M clock to ADC.
	u8 temp = analog_read(0x80);
	temp |= BIT(7);
	analog_write(0x80,temp);

	//Set ADC clock divider.
    temp = analog_read(ADC_CLK_DIV_ADDR);
	temp &= ~ADC_CLK_DIV_Pos;
	temp |= ADC_ClkDiv;
	analog_write(ADC_CLK_DIV_ADDR,temp);

	//ADC Module power on.Must
	WriteAnalogReg (0x80+124, 0x05);
}

/**
 * @Brief: ADC initialization.
 * @Param:
 * @Param:
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_Init(eADC_ModuleTypeDef ADC_Module,eADC_ModeTypeDef ADC_Mode,
		      eADC_VrefTypeDef ADC_Vref,eADC_ResTypeDef ADC_Resolution)
{
	//Clear ADC Enable register.
	analog_write(ADC_ENABLE_ADDR,0x00);//Must

	//Set ADC stable time.
	ADC_StableTime(ADC_Module, ADC_Stable_Time_6Cyc);

	//Reference voltage settings
	ADC_VrefSet(ADC_Vref);

	//Resolution settings
	ADC_ResolutionSet(ADC_Module, ADC_Resolution);

	//Sample Rate settings.
	ADC_SampleRateSet(ADC_Module,0x0a,0x0f0);

	//ADC mode settings:singleend or differential
	ADC_ModeSet(ADC_Module,ADC_Mode);

//	ADC_Cmd(ADC_Module,ENABLE);
}

#if 0
/**
 * @Brief: Set ADC Vref.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_VrefSet(eADC_ModuleTypeDef ADC_Module, eADC_VrefTypeDef ADC_Vref)
{
	u8 temp = 0;
    u8 IsVbatAsVref = ((ADC_Vref & 0x0f) == 0x03) ? 1:0;
    u8 VbatDivFactor = (ADC_Vref >> 4) & 0x0f;

    /* Set ADC VBat Divider factor. */
    if(IsVbatAsVref == 1)
    {
    	temp = analog_read(ADC_VBAT_DIV_ADDR);
		temp &= ~ADC_VBAT_DIV_Pos;
		temp |= MASK_VAL(ADC_VBAT_DIV_Pos,(VbatDivFactor & 0x0f));
		analog_write(ADC_VBAT_DIV_ADDR, temp);
    }
    else
    {
    	temp = analog_read(ADC_VBAT_DIV_ADDR);
    	temp &= ~ADC_VBAT_DIV_Pos;
    	analog_write(ADC_VBAT_DIV_ADDR, temp);
    }

	/* Set Misc Module reference voltage. */
	if(ADC_Module & ADC_Module_M)
	{
		temp = analog_read(ADC_VREF_ADDR);
		temp &= ~ADC_VREF_M_Pos;
		temp |= MASK_VAL(ADC_VREF_M_Pos, ADC_Vref & 0x0f);
		analog_write(ADC_VREF_ADDR,temp);
	}

	/* Set Left Module reference voltage. */
	if(ADC_Module & ADC_Module_L)
	{
		temp = analog_read(ADC_VREF_ADDR);
		temp &= ~ADC_VREF_L_Pos;
		temp |= MASK_VAL(ADC_VREF_L_Pos, ADC_Vref & 0x0f);
		analog_write(ADC_VREF_ADDR,temp);
	}

	/* Set Right Module reference voltage. */
	if(ADC_Module & ADC_Module_R)
	{
		temp = analog_read(ADC_VREF_ADDR);
		temp &= ~ADC_VREF_R_Pos;
		temp |= MASK_VAL(ADC_VREF_R_Pos, ADC_Vref & 0x0f);
		analog_write(ADC_VREF_ADDR,temp);
	}
}
#else
/**
 * @Brief: Set ADC Vref.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_VrefSet(eADC_VrefTypeDef ADC_Vref)
{
	u8 temp;
	u8 IsVbatAsVref = ((ADC_Vref & 0x0f) == 0x03) ? 1:0;
	u8 VbatDivFactor = (ADC_Vref >> 4) & 0x0f;

	/* Set ADC VBat Divider factor. */
	if(IsVbatAsVref == 1)
	{
		temp = analog_read(ADC_VBAT_DIV_ADDR);
		temp &= ~ADC_VBAT_DIV_Pos;
		temp |= MASK_VAL(ADC_VBAT_DIV_Pos,(VbatDivFactor & 0x0f));
		analog_write(ADC_VBAT_DIV_ADDR, temp);
	}
//	else
//	{
//		temp = analog_read(ADC_VBAT_DIV_ADDR);
//		temp &= ~ADC_VBAT_DIV_Pos;
//		analog_write(ADC_VBAT_DIV_ADDR, temp);
//	}

	/* Set M/L/R reference voltage */
	temp = analog_read(ADC_VREF_ADDR);
	temp &= ~(ADC_VREF_M_Pos|ADC_VREF_R_Pos|ADC_VREF_L_Pos);
	temp |= MASK_VAL(ADC_VREF_M_Pos,(ADC_Vref & 0x0f),ADC_VREF_R_Pos,(ADC_Vref & 0x0f),ADC_VREF_L_Pos,(ADC_Vref & 0x0f));
	analog_write(ADC_VREF_ADDR, temp);
}
#endif

/**
 * @Brief: Set ADC VBat Divider factor when ADC Reference Voltage select VBat/N.
 * @Param:
 * @RetVal: None.
 */
void ADC_VbatDivFactorSet(eADC_VbatDivFactorTypeDef ADC_VbatDivFactor)
{
	u8 temp = 0;

	temp = analog_read(ADC_VBAT_DIV_ADDR);
	temp &= ~ADC_VBAT_DIV_Pos;
	temp |= MASK_VAL(ADC_VBAT_DIV_Pos,ADC_VbatDivFactor);
	analog_write(ADC_VBAT_DIV_ADDR,temp);
}

/**
 * @Brief: Set ADC Input Channel when ADC is in Single-End Mode.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_SingleEndChannelSet(eADC_ModuleTypeDef ADC_Module,eADC_ChannelTypeDef ADC_Channel)
{
	u8 temp = 0;

	/* Set Misc module input channel. */
	if(ADC_Module & ADC_Module_M)
	{
		temp = analog_read(ADC_INPUT_CHANNEL_M_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_P_Pos;
		temp |=MASK_VAL(ADC_INPUT_CHANNEL_P_Pos, ADC_Channel);
		analog_write(ADC_INPUT_CHANNEL_M_ADDR,temp);
	}

	/* Set Left module input channel. */
    if(ADC_Module & ADC_Module_L)
	{
		temp = analog_read(ADC_INPUT_CHANNEL_L_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_P_Pos;
		temp |=MASK_VAL(ADC_INPUT_CHANNEL_P_Pos, ADC_Channel);
		analog_write(ADC_INPUT_CHANNEL_L_ADDR,temp);
	}

    /* Set Right module input channel. */
	if(ADC_Module & ADC_Module_R)
	{
		temp = analog_read(ADC_INPUT_CHANNEL_R_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_P_Pos;
		temp |=MASK_VAL(ADC_INPUT_CHANNEL_P_Pos, ADC_Channel);
		analog_write(ADC_INPUT_CHANNEL_R_ADDR,temp);
	}

	// ????????????????
//	if(ADC_Module & ADC_Module_RNS)
//	{
//		temp = analog_read(ADC_INPUT_CHANNEL_M_ADDR);
//		temp &= 0x0f;
//		analog_write(ADC_INPUT_CHANNEL_M_ADDR,temp);
//
//		temp = analog_read(ADC_INPUT_CHANNEL_L_ADDR);
//		temp &= 0x0f;
//		analog_write(ADC_INPUT_CHANNEL_L_ADDR,temp);
//
//		temp = analog_read(ADC_INPUT_CHANNEL_R_ADDR);
//		temp &= 0x0f;
//		analog_write(ADC_INPUT_CHANNEL_R_ADDR,temp);
//	}
}

/**
 * @Brief: Set ADC Input Channel when ADC is in Differential Mode.
 * @Param:
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_DifferChannelSet(eADC_ModuleTypeDef  ADC_Module,
					      eADC_ChannelTypeDef ADC_Channel_P,
					      eADC_ChannelTypeDef ADC_Channel_N)
{
	u8 temp = 0;

	/* Set Misc module differential channel. */
	if(ADC_Module & ADC_Module_M)
	{
		//Set Positive input pin.
		temp = analog_read(ADC_INPUT_CHANNEL_M_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_P_Pos;
		temp |=MASK_VAL(ADC_INPUT_CHANNEL_P_Pos, ADC_Channel_P);
		analog_write(ADC_INPUT_CHANNEL_M_ADDR,temp);

		//Set Negative input pin.
		temp = analog_read(ADC_INPUT_CHANNEL_M_ADDR);
	    temp &= ~ADC_INPUT_CHANNEL_N_Pos;
	    temp |= MASK_VAL(ADC_INPUT_CHANNEL_N_Pos, ADC_Channel_N);
	    analog_write(ADC_INPUT_CHANNEL_M_ADDR, temp);
	}

	/* Set L module differential channel. */
	if(ADC_Module & ADC_Module_L)
	{
		//Set Positive input pin.
		temp = analog_read(ADC_INPUT_CHANNEL_L_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_P_Pos;
		temp |=MASK_VAL(ADC_INPUT_CHANNEL_P_Pos, ADC_Channel_P);
		analog_write(ADC_INPUT_CHANNEL_L_ADDR,temp);

		//Set Negative input pin.
		temp = analog_read(ADC_INPUT_CHANNEL_L_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_N_Pos;
		temp |= MASK_VAL(ADC_INPUT_CHANNEL_N_Pos, ADC_Channel_N);
		analog_write(ADC_INPUT_CHANNEL_L_ADDR, temp);
	}

	/* Set R module differential channel. */
	if(ADC_Module & ADC_Module_R)
	{
		//Set Positive input pin.
		temp = analog_read(ADC_INPUT_CHANNEL_R_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_P_Pos;
		temp |=MASK_VAL(ADC_INPUT_CHANNEL_P_Pos, ADC_Channel_P);
		analog_write(ADC_INPUT_CHANNEL_R_ADDR,temp);

		//Set Negative input pin.
		temp = analog_read(ADC_INPUT_CHANNEL_R_ADDR);
		temp &= ~ADC_INPUT_CHANNEL_N_Pos;
		temp |= MASK_VAL(ADC_INPUT_CHANNEL_N_Pos, ADC_Channel_N);
		analog_write(ADC_INPUT_CHANNEL_R_ADDR, temp);
	}
}

/**
 * @Brief: Set ADC Resolution.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_ResolutionSet(eADC_ModuleTypeDef ADC_Module, eADC_ResTypeDef ADC_Res)
{
	u8 temp = 0;

	/* Set Misc module resolution. */
	if(ADC_Module & ADC_Module_M)
	{
		temp = analog_read(ADC_RES_M_ADDR);
		temp &= ~ADC_RES_M_Pos;
		temp |= MASK_VAL(ADC_RES_M_Pos, ADC_Res);
		analog_write(ADC_RES_M_ADDR, temp);
	}

	/* Set L module resolution. */
    if(ADC_Module & ADC_Module_L)
	{
		temp = analog_read(ADC_RES_L_OR_R_ADDR);
		temp &= ~ADC_RES_L_Pos;
		temp |= MASK_VAL(ADC_RES_L_Pos, ADC_Res);
		analog_write(ADC_RES_L_OR_R_ADDR, temp);
	}

    /* Set R module resolution. */
    if(ADC_Module & ADC_Module_R)
	{
		temp = analog_read(ADC_RES_L_OR_R_ADDR);
		temp &= ~ADC_RES_R_Pos;
		temp |= MASK_VAL(ADC_RES_R_Pos, ADC_Res);
		analog_write(ADC_RES_L_OR_R_ADDR, temp);
	}
}

/**
 * @Brief: Set ADC Input Mode.(Single-end/Differential)
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_ModeSet(eADC_ModuleTypeDef ADC_Module, eADC_ModeTypeDef ADC_Mode)
{
	u8 temp = 0;

	if(ADC_Mode == ADC_Mode_Single)
	{
		if(ADC_Module & ADC_Module_M)
		{
			temp = analog_read(ADC_INPUT_MODE_ADDR);
			temp &= ~ADC_INPUT_MODE_M_Pos;
			analog_write(ADC_INPUT_MODE_ADDR,temp);
		}

		if(ADC_Module & ADC_Module_L)
		{
			temp = analog_read(ADC_INPUT_MODE_ADDR);
			temp &= ~ADC_INPUT_MODE_L_Pos;
			analog_write(ADC_INPUT_MODE_ADDR,temp);
		}

		if(ADC_Module & ADC_Module_R)
		{
			temp = analog_read(ADC_INPUT_MODE_ADDR);
			temp &= ~ADC_INPUT_MODE_R_Pos;
			analog_write(ADC_INPUT_MODE_ADDR,temp);
		}
	}
	else if(ADC_Mode == ADC_Mode_Diff)
	{
		if(ADC_Module & ADC_Module_M)
		{
			temp = analog_read(ADC_INPUT_MODE_ADDR);
			temp |= ADC_INPUT_MODE_M_Pos;
			analog_write(ADC_INPUT_MODE_ADDR,temp);
		}

		if(ADC_Module & ADC_Module_L)
		{
			temp = analog_read(ADC_INPUT_MODE_ADDR);
			temp |= ADC_INPUT_MODE_L_Pos;
			analog_write(ADC_INPUT_MODE_ADDR,temp);
		}

		if(ADC_Module & ADC_Module_R)
		{
			temp = analog_read(ADC_INPUT_MODE_ADDR);
			temp |= ADC_INPUT_MODE_R_Pos;
			analog_write(ADC_INPUT_MODE_ADDR,temp);
		}
	}
}

/**
 * @Brief: Set ADC Input stable time.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_StableTime(eADC_ModuleTypeDef ADC_Module, eADC_StableTimeTypeDef ADC_Stabletime)
{
	u8 temp = 0;

	if(ADC_Stabletime > 0x0f)
		return;

	if(ADC_Module & ADC_Module_M)
	{
		temp = analog_read(ADC_STABLE_TIME_M_ADDR);
		temp &= ~ADC_STABLE_TIME_M_Pos;
		temp |= MASK_VAL(ADC_STABLE_TIME_M_Pos, ADC_Stabletime & 0x0f);
		analog_write(ADC_STABLE_TIME_M_ADDR,temp);
	}

	if(ADC_Module & ADC_Module_L)
	{
		temp = analog_read(ADC_STABLE_TIME_L_OR_R_ADDR);
		temp &= ~ADC_STABLE_TIME_L_Pos;
		temp |= MASK_VAL(ADC_STABLE_TIME_L_Pos, ADC_Stabletime & 0x0f);
		analog_write(ADC_STABLE_TIME_L_OR_R_ADDR,temp);
	}

	if(ADC_Module & ADC_Module_R)
	{
		temp = analog_read(ADC_STABLE_TIME_L_OR_R_ADDR);
		temp &= ~ADC_STABLE_TIME_R_Pos;
		temp |= MASK_VAL(ADC_STABLE_TIME_R_Pos, ADC_Stabletime & 0x0f);
		analog_write(ADC_STABLE_TIME_L_OR_R_ADDR,temp);
	}
}

/**
 * @Brief:  Set ADC state number.
 * @Param:  ADC_StateNum -> Max=6; Min=0; ADC_StateNum % 2 == 0
 * @RetVal: None.
 */
void ADC_StateNumSet(u8 ADC_StateNum)
{
	u8 temp = 0;

	if(ADC_StateNum > 6 || (ADC_StateNum % 2 != 0))
		return;

	temp = analog_read(ADC_STATE_NUM_ADDR);
	temp &= ~ADC_STATE_NUM_Pos;
	temp |= MASK_VAL(ADC_STATE_NUM_Pos,ADC_StateNum);
	analog_write(ADC_STATE_NUM_ADDR,temp);
}

/**
 * @Brief:  Set ADC input voltage pre-division factor.
 * @Param:
 * @RetVal: None.
 */
void ADC_InputVolDivFactor(eADC_InputVolDivFactorTypeDef ADC_InputVolDivFactor)
{
	u8 temp = 0;

	if(ADC_InputVolDivFactor > 3)
		return;

	temp = analog_read(ADC_VREF_ADDR);
	if((temp & 0x03) == ADC_Vref_1p2)
	{
		temp = analog_read(ADC_INPUT_VOL_PRESCALE_ADDR);
		temp &= ~ADC_INPUT_VOL_Prescale_Pos;
		temp |= MASK_VAL(ADC_INPUT_VOL_Prescale_Pos, ADC_InputVolDivFactor);
		temp |= 0x3d;
		analog_write(ADC_INPUT_VOL_PRESCALE_ADDR,temp);

		temp = analog_read(0x80 + 121);
		temp |= 0x10;
		analog_write(0x80 + 121, temp);
	}
	else
	{
		temp = analog_read(ADC_INPUT_VOL_PRESCALE_ADDR);
		temp &= ~ADC_INPUT_VOL_Prescale_Pos;
		temp |= MASK_VAL(ADC_INPUT_VOL_Prescale_Pos, ADC_InputVolDivFactor);
		temp |= 0x14;
		analog_write(ADC_INPUT_VOL_PRESCALE_ADDR,temp);

		temp = analog_read(0x80 + 121);
		temp &= 0xcf;
		analog_write(0x80 + 121, temp);
	}
}

/**
 * @Brief: Set ADC "Set" state time. L/R/M Module shared.
 * @Param:
 * @RetVal: None.
 */
void ADC_SetStateTimeSet(u8 ADC_SetTime)
{
	u8 temp = 0;

	if(ADC_SetTime > 0x0f)
		return;

	//Set "Set" state time.
	temp = analog_read(ADC_SET_STATE_TIME_LEN_ADDR);
	temp &= ~ADC_SET_STATE_TIME_LEN_Pos;
	temp |= MASK_VAL(ADC_SET_STATE_TIME_LEN_Pos, ADC_SetTime);
	analog_write(ADC_SET_STATE_TIME_LEN_ADDR, temp);
}

/**
 * @Brief: Set ADC "Capture" state time. L/R shared, M/RNS shared.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_CaptureTimeSet(eADC_ModuleTypeDef ADC_Module,u16 ADC_CaptureTime)
{
	u8 temp = 0;
	u8 CaptureValueL =  ADC_CaptureTime & 0xff;
	u8 CaptureValueH = (ADC_CaptureTime>>8) & 0x03;

	if(ADC_CaptureTime > 0x03ff)
		return;

	//Set "Capture" state time of M/RNS.
	if((ADC_Module & ADC_Module_M) || (ADC_Module & ADC_Module_RNS))
	{
		analog_write(ADC_CAPTURE_TIME_LEN_M_OR_RNS_ADDRL,CaptureValueL);

		temp = analog_read(ADC_CAPTURE_TIME_LEN_M_OR_RNS_ADDRH);
		temp &= ~ADC_CAPTURE_TIME_LEN_M_OR_RNS_Pos;
		temp |= MASK_VAL(ADC_CAPTURE_TIME_LEN_M_OR_RNS_Pos,CaptureValueH);
		analog_write(ADC_CAPTURE_TIME_LEN_M_OR_RNS_ADDRH,temp);
	}

	//Set "Capture" state time of L/R.
	if((ADC_Module & ADC_Module_L) || (ADC_Module & ADC_Module_R))
	{
		analog_write(ADC_CAPTURE_TIME_LEN_L_OR_R_ADDRL, CaptureValueL);

		temp = analog_read(ADC_CAPTURE_TIME_LEN_L_OR_R_ADDRH);
		temp &= ~ADC_CAPTURE_TIME_LEN_L_OR_R_Pos;
		temp |= MASK_VAL(ADC_CAPTURE_TIME_LEN_L_OR_R_Pos,CaptureValueH);
		analog_write(ADC_CAPTURE_TIME_LEN_L_OR_R_ADDRH,temp);
	}
}

/**
 * @Brief: Set ADC sample rate.
 * @Param:
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_SampleRateSet(eADC_ModuleTypeDef ADC_Module,u8 ADC_SetTime,u16 ADC_CaptureTime)
{
	//Set "Set" state time.
	ADC_SetStateTimeSet(ADC_SetTime);

	//Set "Capture" state time.
	ADC_CaptureTimeSet(ADC_Module,ADC_CaptureTime);
}

/**
 * @Brief: Enable ADC L/R/M/RNS Module.
 * @Param:
 * @Param:
 * @RetVal: None.
 */
void ADC_Cmd(eADC_ModuleTypeDef ADC_Module, u8 ADC_Enable)
{
	u8 temp = 0;
	u8 i = 0;
	u8 adcModuleCnt = 0;
	u8 adcStateNum = 0;

	if(ADC_Enable == ENABLE)
	{
		//Enable ADC Module.
		temp = analog_read(ADC_ENABLE_ADDR);
		temp |= ADC_Module;

		//Set ADC state machine number.
		for(i = 0; i < 3; i++)
		{
			if((temp >> i) & 0x01)
			{
				adcModuleCnt++;
			}
		}
		adcStateNum = adcModuleCnt * 2;
		temp &= ~ADC_STATE_NUM_Pos;
		temp |= (adcStateNum << 4);
		if((ADC_Module & ADC_Module_R) == ADC_Module_R  && (ADC_Module & ADC_Module_L) == ADC_Module_L )
		{
			temp &= ~ADC_Module_R;
			analog_write(ADC_ENABLE_ADDR,temp);

			temp = analog_read(ADC_ENABLE_ADDR);
			temp |= ADC_Module_R;
			analog_write(ADC_ENABLE_ADDR,temp);
		}
		else
		{
			analog_write(ADC_ENABLE_ADDR,temp);
		}
	}
	else if(ADC_Enable == DISABLE)
	{
		//Disable ADC Module.
		temp = analog_read(ADC_ENABLE_ADDR);
		temp &= ~ADC_Module;

		//Set ADC state machine number.
		for(i = 0; i < 3; i++)
		{
			if((temp >> i) & 0x01)
			{
				adcModuleCnt++;
			}
		}

		adcStateNum = adcModuleCnt * 2;
		temp &= ~ADC_STATE_NUM_Pos;
		temp |= (adcStateNum << 4);
		analog_write(ADC_ENABLE_ADDR,temp);
	}

	//If RNS module is enable, RNS mode must be enable((0x80+124)[4] = 1).
	if(ADC_Module & ADC_Module_RNS)
	{
		temp = analog_read(0x80 + 124);
		temp |= (1<<4);//RNS mode enable
		analog_write(0x80 + 124,temp);
	}
	else
	{
		temp = analog_read(0x80 + 124);
		temp &= ~(1<<4);//Normal mode enable
		analog_write(0x80 + 124,temp);
	}
}

/**
 * @Brief:  Check battery voltage.
 * @Param:  ADC_Module ->
 * @RetVal: None.
 */
void ADC_BatteryCheckInit(eADC_ChannelTypeDef ADC_Channel)
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
	ADC_ModeSet(ADC_Module_M,ADC_Mode_Diff);

	//ADC channel settings
	gpio_set_output_en(ADC_BATTERY_CHECK_PIN,1);//must
	gpio_set_input_en(ADC_BATTERY_CHECK_PIN,0);//must
	gpio_write(ADC_BATTERY_CHECK_PIN,1);//must
	ADC_DifferChannelSet(ADC_Module_M,ADC_Channel,ADC_Channel_GND);

	//ADC input voltage divider factor settings
	ADC_InputVolDivFactor(ADC_Input_Vol_Div_Factor_8);

	//Sample Rate settings.
	ADC_SampleRateSet(ADC_Module_M,0x0a,0x0f0);

	//ADC enable
	ADC_Cmd(ADC_Module_M, ENABLE);
}

/**
 * @Brief:  Get ADC Busy flag.
 * @Param:  None.
 * @RetVal: 1:busy, 0:convert completely.
 */
u8 ADC_IsBusy(void)
{
	u8 temp = analog_read(ADC_DATA_H_ADDR);//bit7 == 1->convert completely
	return (temp & 0x80)? 0:1;
}

/**
 * @Brief:  Get ADC convert value.
 * @Param:  None.
 * @RetVal: ADC value.
 */
u16 ADC_GetConvertValue(void)
{
	u8 adcValueL = 0;
	u8 adcValueH = 0;
	u16 adcValue = 0;

//	while(!ADC_IsBusy());
	while(ADC_IsBusy());
	sleep_us(2);

	adcValueH = analog_read(ADC_DATA_H_ADDR);
	adcValueL = analog_read(ADC_DATA_L_ADDR);

	adcValue = ((adcValueH<<8) + adcValueL) & 0x7FFF;
	analog_write(ADC_DATA_H_ADDR,0x00);

	return adcValue;
}

/**
 * @Brief:  Get Random number value.
 * @Param:  None.
 * @RetVal: Random number value.
 */
u16 ADC_GetRandomNumber(void)
{
	u8 rndNumL = 0;
	u8 rndNumH = 0;
	u16 rndNum = 0;

	rndNumL = analog_read(ADC_RND_DATA_L_ADDR);
	rndNumH = analog_read(ADC_RND_DATA_H_ADDR);

	rndNum = (rndNumH<<8)|rndNumL;

	return rndNum;
}

#endif

/*----------------------------------File End----------------------------------*/


