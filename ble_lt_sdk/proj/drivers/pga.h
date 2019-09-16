#ifndef		pga_H
#define		pga_H


#define		PGA_POWER_ON	 (	analog_write(0x87, analog_read(0x87) & 0xF7) )
#define		PGA_POWER_OFF	 (	analog_write(0x87, analog_read(0x87) | 0x08) )


//set ANA_C<3> and ANA_C<2> as positive and minus input of the PGA
#define		setPGAchannel_ONE			(*(volatile unsigned char  *)0x800028 &= 0xFE)

//set ANA_C<5> and ANA_C<4> as positive and minus input of the PGA
#define		setPGAchannel_TWO			(*(volatile unsigned char  *)0x800028 |= 0x01)

enum PREAMPValue{
	PRE_DBP0,
	PRE_DB20,
};

enum POSTAMPValue{
	POST_DB0,
	POST_DB3,
	POST_DB6,
	POST_DB9,
};


void inline setChannel(unsigned char chM){
	//set ANA_C<5> and ANA_C<4> as positive and minus input of the PGA
	if(chM){
		*(volatile unsigned char  *)0x800028 |= 0x01;
		*(volatile unsigned char  *)0x800029 |= 0x01;
	}

	//set ANA_C<3> and ANA_C<2> as positive and minus input of the PGA
	else{
		*(volatile unsigned char  *)0x800028 &= 0xFE;
		*(volatile unsigned char  *)0x800029 &= 0xFE;
	}

}




/**************************************************************
*
*	@brief	pga output enable or disable function, call this function to enable or disable the PGA output
*
*	@para	enDis - '1' enable output/mute off audio; '0' disable output/mute  on audio
*
*	@return	None
*
*/
static void setPGAOutput(unsigned char enDis){

	unsigned char tem;
	tem = ReadAnalogReg(0x86);
	if(enDis)
		tem = tem & 0xFB;//output enable
	else
		tem = tem | 0x04;//output dis able
	WriteAnalogReg(0x86,tem);

}
/**************************************************************
*
*	@brief	adjust pre-amplifier gain value
*
*	@para	preGV - enum var of PREAMPValue, 0DB or 20DB
*
*	@return	'1' adjusted; '0' adjust error
*
*/

static inline unsigned char preGainAdjust(enum PREAMPValue preGV){
	unsigned char tem0,tem1;
	tem0 = (unsigned char)preGV;
	if(tem0>1)
		return 0;
	tem1 = ReadAnalogReg(0x87);
	tem1 = tem1 & 0xFE;
	tem1 = tem1 |tem0;
	WriteAnalogReg(0x87,tem1);

	return 0;
}
/**************************************************************
*
*	@brief	adjust post-amplifier gain value
*
*	@para	posGV - enum var of POSTAMPValue, 0,3,6 or 9dB
*
*	@return	'1' adjusted; '0' adjust error
*
*/
static inline unsigned char postGainAdjust(enum POSTAMPValue posGV){
	unsigned char tem0,tem1;
	tem0 = (unsigned char)posGV;
	if(tem0>3)
		return 0;
	tem1 = ReadAnalogReg(0x87);
	tem1 = tem1 & 0xF9;
	tem1 = tem1 |(tem0<<1);
	WriteAnalogReg(0x87,tem1);

	return 0;
}

/**************************************************************
*
*	@brief	pga initiate function, call this function to enable the PGA module
*			the input channel deafult set to ANA_C<3> and ANA_C<2>
*
*	@para	None
*
*	@return	None
*
*/
static void pgaInit(void){
	unsigned char tem;

	//Power on pga
	tem = ReadAnalogReg(0x87);
	tem = tem & 0xF7;
	WriteAnalogReg(0x87,tem);
	//set PGA input channel
	setChannel(1);
	setPGAOutput(1);
}

#endif
