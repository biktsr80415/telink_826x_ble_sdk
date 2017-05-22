#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../vendor/common/mouse_type.h"
#include "../../vendor/common/tl_audio.h"

// ISO out timer out
void timer_irq1_handler (void) {

}

_attribute_ram_code_ void irq_handler(void)
{
	u32 src = reg_irq_src;

	if(src & FLD_IRQ_TMR1_EN){
		timer_irq1_handler();
		reg_tmr_sta = FLD_TMR_STA_TMR1;
	}

}


#define			LEN_ADV_PKT				32
u8 	rf_pkt_adv[LEN_ADV_PKT] = {
		LEN_ADV_PKT - 4,		// dma_len
		0, 0, 0,
		0,						// type
		LEN_ADV_PKT - 6,		// rf_len
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,	// advA

		0x0e, 0x09, 'T', 'e', 'l', 'i', 'n', 'k', ' ', 't', 'L', 'i', 'g' ,'h', 't',
};

int		proc_rf_enable = 1;

void proc_rf ()
{
	static u32 tick_rf;
	static int tms = 0;
	static int	tx_sn;

	if (clock_time_exceed (tick_rf, 1000))
	{
		tick_rf = clock_time ();

		tms ++;
		if (tms >= 30)
		{
			tms = 0;
		}
		if (tms < 3)
		{
			rf_set_ble_access_code_adv ();
			rf_set_ble_crc_adv ();
			rf_set_ble_channel (37 + tms);
			rf_start_stx2rx ((void *)rf_pkt_adv, clock_time() + 500);
			tx_sn ++;
		}
		else
		{
			rf_set_tx_rx_off ();
			sleep_us (100);
		}
	}

}

void proc_ui ()
{
	if (!gpio_read (BUTTON_RECORD)) {
		proc_rf_enable = 1;
	}
	if (!gpio_read (BUTTON_PLAY)) {
		proc_rf_enable = 0;
	}
}


s16		buff_adpcm[64];//128bytesÊý¾Ý
u16 	sdm_step;
void main_loop(void)
{
	//proc_debug ();

	proc_ui ();

	if (proc_rf_enable)
	{
		proc_rf ();
	}

	{	//////////////// mic --> sdm //////////////
#if 1
		proc_mic_encoder ();
		if (mic_encoder_data_ready (buff_adpcm)) {
			//decoding dat
			sdm_decode_rate (sdm_step, 2);
			adpcm_to_sdm (buff_adpcm, 128);
		}
#else
		proc_mic_encoder ();
		int *p = mic_encoder_data_buffer ();
		if (p)					//around 3.2 ms @16MHz clock
		{
            #if 1
				sdm_decode_rate (sdm_step, 2);//decoding dat
				adpcm_to_sdm(p,128);
			#else//same as above
				sdm_decode_rate (sdm_step, 2);//decoding dat
				adpcm_to_pcm((s16 *)(p),buff_adpcm, 128);
				pcm_to_sdm (buff_adpcm, 128);
			#endif
		}
#endif
	}
}


u32	buff_rx[64];//RF buffer

void  user_init(void)
{
	config_mic_buffer (buffer_mic, TL_MIC_BUFFER_SIZE);
#if 0//AMIC
	// FLD_ADC_PGA_L_C1, FLD_ADC_PGA_L_C3, FLD_ADC_PGA_R_C2, FLD_ADC_PGA_R_C0
	config_adc (FLD_ADC_PGA_C01, FLD_ADC_CHN_D0, SYS_32M_AMIC_8K);
	//config_adc (FLD_ADC_PGA_C01, FLD_ADC_CHN_D0, SYS_32M_AMIC_12K);
	//config_adc (FLD_ADC_PGA_R_C0, FLD_ADC_CHN_D0, CLOCK_SYS_CLOCK_HZ == 32000000);
	//config_adc (FLD_ADC_PGA_L_C1, FLD_ADC_CHN_D0, CLOCK_SYS_CLOCK_HZ == 32000000);

	gpio_set_output_en (GPIO_PB7, 1);		//AMIC Bias output
	gpio_write (GPIO_PB7, 1);
#else//DMIC
	config_dmic (16000);//16k
#endif

	sdm_step = config_sdm  (buffer_sdm, TL_SDM_BUFFER_SIZE, 16000, 4);//16k

	usb_dp_pullup_en (1);
	reg_wakeup_en = FLD_WAKEUP_SRC_USB;

	reg_dma_rf_rx_addr = (u16)(u32) (buff_rx);
	reg_dma2_ctrl = FLD_DMA_WR_MEM | (64>>4);
	reg_dma_chn_irq_msk = 0;
	reg_irq_mask = 0;

	rf_set_power_level_index (RF_POWER_8dBm);
}


