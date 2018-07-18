#ifndef _DFIFO_H
#define _DFIFO_H

#include "driver_config.h"
#include "register.h"

typedef enum{
	DFIFO_Mode_FIFO0_Input    = BIT(0),
	DFIFO_Mode_FIFO1_Input    = BIT(1),
	DFIFO_Mode_FIFO2_Input    = BIT(2),

	DFIFO_Mode_FIFO0_Output   = BIT(3),

	DFIFO_Mode_FIFO0_Low_Irq_EN  = BIT(4),
	DFIFO_Mode_FIFO0_High_Irq_EN = BIT(5),

	DFIFO_Mode_FIFO1_High_Irq_EN = BIT(6),
	DFIFO_Mode_FIFO2_High_Irq_EN = BIT(7),
}eDFIFO_ModeTypeDef;

static inline void dfifo_config_dfifo0(signed short* pbuff,unsigned int byte_size)
{
	reg_dfifo0_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo0_size = (byte_size>>4)-1;
}

static inline void dfifo_config_dfifo1(signed short* pbuff,unsigned int byte_size)
{
	reg_dfifo1_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo1_size = (byte_size>>4)-1;
}

static inline void dfifo_config_dfifo2(signed short* pbuff,unsigned int byte_size)
{
	reg_dfifo2_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo2_size = (byte_size>>4)-1;
}

static inline void dfifo_set_mode(eDFIFO_ModeTypeDef DFIFO_Mode)
{
	reg_dfifo_mode = (unsigned char)DFIFO_Mode;

	//Set DFIFO as auto mode
	reg_dfifo_manual &= ~FLD_DFIFO_MANUAL_EN;
}

static inline void adc_config_misc_channel_buf(signed short* pbuff,unsigned int byte_size)
{
	reg_dfifo_misc_chn_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo_misc_chn_size = (byte_size>>4)-1;

	reg_dfifo_mode |= FLD_AUD_DFIFO2_IN; // misc chn can only use dfifo2
}

static inline void audio_config_mic_buf(signed short* pbuff,unsigned int byte_size)
{
	reg_dfifo_audio_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo_audio_size = (byte_size>>4)-1;
}
#endif
