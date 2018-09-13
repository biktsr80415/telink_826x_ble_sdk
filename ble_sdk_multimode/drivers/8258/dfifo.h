#ifndef 	DFIFO_H
#define 	DFIFO_H

#include "register.h"


static inline void dfifo_enable_dfifo2(void)
{
	reg_dfifo_mode |= FLD_AUD_DFIFO2_IN;
}

static inline void dfifo_disable_dfifo2(void)
{
	reg_dfifo_mode &= ~FLD_AUD_DFIFO2_IN;
}


static inline void dfifo_config_dfifo0(signed short* pbuff,unsigned int size_buff)
{
	reg_dfifo0_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo0_size = (size_buff>>4)-1;
}

static inline void dfifo_config_dfifo1(signed short* pbuff,unsigned int size_buff)
{
	reg_dfifo1_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo1_size = (size_buff>>4)-1;
}

static inline void dfifo_config_dfifo2(signed short* pbuff,unsigned int size_buff)
{
	reg_dfifo2_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo2_size = (size_buff>>4)-1;
}




static inline void adc_config_misc_channel_buf(signed short* pbuff,unsigned int size_buff)
{
	reg_dfifo_misc_chn_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo_misc_chn_size = (size_buff>>4)-1;

	reg_dfifo2_wptr = 0;  //clear dfifo2 write pointer
}



/**
 * @brief     configure the mic buffer's address and size
 * @param[in] pbuff - the first address of SRAM buffer to store MIC data.
 * @param[in] size_buff - the size of pbuff.
 * @return    none
 */
static inline void audio_config_mic_buf(signed short* pbuff,unsigned int size_buff)
{
	reg_dfifo_audio_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo_audio_size = (size_buff>>4)-1;
}


#endif
