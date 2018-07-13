#ifndef	_PGA_H
#define	_PGA_H

#include "driver_config.h"
#include "bsp.h"
#include "analog.h"

/* PGA Input channel type define */
typedef enum{
	PGA_Input_Channel_PA7PB0 = 0x00,
	PGA_Input_Channel_PB4PB5 = 0x01,
}ePGA_InputChannelTypeDef;

/* PGA preGain deefine */
typedef enum{
	PGA_PreGain_26dB = 0x00,
	PGA_PreGain_46dB = 0x20,
}ePGA_PreGain_TypeDef;

/* PGA postGain deefine */
typedef enum{
	PGA_PostGain_N10dB = 0x00,//-10dB
	PGA_PostGain_N9dB  = 0x01,
	PGA_PostGain_N8dB  = 0x02,
	PGA_PostGain_N7dB  = 0x03,
	PGA_PostGain_N6dB  = 0x04,
	PGA_PostGain_N5dB  = 0x05,
	PGA_PostGain_N4dB  = 0x06,
	PGA_PostGain_N3dB  = 0x07,
	PGA_PostGain_N2dB  = 0x08,
	PGA_PostGain_N1dB  = 0x09,

	PGA_PostGain_0dB   = 0x0a,//0dB
	PGA_PostGain_1dB   = 0x0b,
	PGA_PostGain_2dB   = 0x0c,
	PGA_PostGain_3dB   = 0x0d,
	PGA_PostGain_4dB   = 0x0e,
	PGA_PostGain_5dB   = 0x0f,
	PGA_PostGain_6dB   = 0x10,
	PGA_PostGain_7dB   = 0x11,
	PGA_PostGain_8dB   = 0x12,
	PGA_PostGain_9dB   = 0x13,
	PGA_PostGain_10dB  = 0x14,
	PGA_PostGain_11dB  = 0x15,
	PGA_PostGain_12dB  = 0x16,
	PGA_PostGain_13dB  = 0x17,
	PGA_PostGain_14dB  = 0x18,//14dB
}ePGA_PostGain_TypeDef;

/**
 * @Brief: Power on PGA.
 * @Param: None.
 * @Param: None.
 */
void PGA_PowerOn(void)
{
	analog_write(0xfc,analog_read(0xfc)&(~BIT(6)));
}

/**
 * @Brief: Power off PGA.
 * @Param: None.
 * @Param: None.
 */
void PGA_PowerOff(void)
{
	analog_write(0xfc,analog_read(0xfc)|BIT(6));
}

/**
 * @Brief: Set PGA input channel.
 * @Param: PGA_InputChannel -> .
 * @Param: None.
 */
void PGA_SetInputChannel(ePGA_InputChannelTypeDef PGA_InputChannel)
{
	if(PGA_InputChannel == PGA_Input_Channel_PA7PB0)
	{
		analog_write(0xfd, analog_read(0xfd)&(~BIT(0)));//Select PA7 PB0 as PGA input
	}
	else if(PGA_InputChannel == PGA_Input_Channel_PB4PB5)
	{
		analog_write(0xfd, analog_read(0xfd)|BIT(0));//Select PB4 PB5 as PGA input
	}
}

/**
 * @Brief: Set PGA pre Gain and post Gain.
 * @Param: PGA_PreGain  -> pre Gain.
 * @Param: PGA_PostGain -> post Gain
 * @Param: None.
 */
void PGA_SetGain(ePGA_PreGain_TypeDef PGA_PreGain, ePGA_PostGain_TypeDef PGA_PostGain)
{
	unsigned char temp = ((PGA_PreGain | PGA_PostGain) & 0x3f);
	temp |= BIT(7);//Enable gain function.
	reg_pga_gain = temp;

	//Enable PAG output
	reg_pga_mute_output &= ~BIT(0);

	/* The configuration of 0xb55 can take effect. */
	reg_pga_trigger = 0x01;//|= BIT(0);//0x01;
	reg_pga_trigger = 0x00;//&= ~BIT(0);//0x00;
}

/**
 * @Brief: Set PGA pre Gain and post Gain.
 * @Param: gain  -> include pre Gain and post Gain.
 * @Param: None.
 */
void PGA_SetGainEx(unsigned char gain)
{
	unsigned char temp = (gain & 0x3f);
	temp |= BIT(7);//Enable gain function.
	reg_pga_gain = temp;

	//Enable PAG output
	reg_pga_mute_output &= ~BIT(0);

	/* The configuration of 0xb55 can take effect. */
	reg_pga_trigger = 0x01;//|= BIT(0);//0x01;
	reg_pga_trigger = 0x00;//&= ~BIT(0);//0x00
}

/**
 * @Brief: Get PGA output state.
 * @Param: None.
 * @Param: 1: PGA ouput ON; 0: PGA ouput off.
 */
unsigned char PGA_GetOutputState(void)
{
	return (reg_pga_mute_state & 0x01) ? 1:0;//1:Enable; 0: Disable;
}


#if 0


enum PREAMPValue{
	DB26,
	DB46,
};

enum POSTAMPValue{
	DB_N10= 0x00,
	DB_N5 = 0x0a,
	DB_0  = 0x14,
	DB_5  = 0x1d,
	DB_10 = 0x28,
	DB_14 = 0x30,
};

typedef enum{
	PGA_LEFT_CHN  = BIT(6),
	PGA_RIGHT_CHN = BIT(7)
}PGA_CHN_Typdef;

#define		SET_PGA_CHN_ON(v)				do{\
												unsigned char val=ReadAnalogReg(0x80+124);\
												val &= (~(v));\
												WriteAnalogReg(0x80+124,val);\
											}while(0)
enum{
	PGA_AIN_C0,
	PGA_AIN_C1,
	PGA_AIN_C2,
	PGA_AIN_C3,
};

#define		SET_PGA_LEFT_P_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_LEFT_N_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_RIGHT_P_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0xcf;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_RIGHT_N_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0x3f;\
												val |= (((unsigned char)(v)<<6)&0xc0);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_GAIN_FIX_VALUE(v)		do{\
												unsigned char val=0;\
												val |= (((unsigned char)(v))&0x7f);\
												write_reg8(0xb63,val|0x80);\
											}while(0)

enum{
	PGA_POST_GAIN		= BIT_RNG(0,5),
	PGA_PRE_GAIN_26DB	= BIT(6)
};
#define		REG_PGA_GAIN				READ_REG8(0xb63)


/**************************************************************
*
*	@brief	pga initiate function, call this function to enable the PGA module
*			the input channel deafult set to ANA_C<3> and ANA_C<2>
*
*	@para	chn - select PGA_LEFT_CHN or PGA_RIGHT_CHN to initiate the PGA
*
*	@return	None
*
*/
extern void pgaInit(unsigned char chn);

/**************************************************************
*
*	@brief	adjust pre-amplifier gain value
*
*	@para	preGV - enum var of PREAMPValue, 0DB or 20DB
*
*	@return	'1' adjusted; '0' adjust error
*
*/
unsigned char preGainAdjust(enum PREAMPValue preGV);

/**************************************************************
*
*	@brief	adjust post-amplifier gain value
*
*	@para	posGV - enum var of POSTAMPValue, 0,3,6 or 9dB
*
*	@return	'1' adjusted; '0' adjust error
*
*/
unsigned char postGainAdjust(enum POSTAMPValue posGV);
#endif

#endif/* End of _PGA_H */
