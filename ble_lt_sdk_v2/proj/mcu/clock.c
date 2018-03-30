
#include "../tl_common.h"

#if MODULE_AUDIO_ENABLE
#define CLK_SBC_ENABLE		1
#define CLK_AUD_ENABLE		1
#define CLK_DFIFO_ENABLE	1
#define CLK_USB_ENABLE		1
#endif

#if (MODULE_USB_ENABLE)
#define CLK_AUD_ENABLE		1
#endif

#ifndef CLK_SBC_ENABLE
#define CLK_SBC_ENABLE		1
#endif
#ifndef CLK_AUD_ENABLE
#define CLK_AUD_ENABLE		1
#endif
#ifndef CLK_DFIFO_ENABLE
#define CLK_DFIFO_ENABLE	1
#endif
#ifndef CLK_USB_ENABLE
#define CLK_USB_ENABLE		(APPLICATION_DONGLE)
#endif


void clock_init(){


#if(CLOCK_SYS_TYPE == CLOCK_TYPE_PLL)
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		REG_ADDR8(0x66) = 0x43;			//change to PLL clock: 48/3 = 16M
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		REG_ADDR8(0x66) = 0x42;
	#else
//		#error clock not set properly
	#endif
#elif(CLOCK_SYS_TYPE == CLOCK_TYPE_PAD)


	
#elif(CLOCK_SYS_TYPE == CLOCK_TYPE_OSC)

#else
	#error clock not set properly
#endif

//	reg_tmr_ctrl = MASK_VAL(FLD_TMR0_EN, 1
//		, FLD_TMR_WD_CAPT, (MODULE_WATCHDOG_ENABLE ? (WATCHDOG_INIT_TIMEOUT * CLOCK_SYS_CLOCK_1MS >> WATCHDOG_TIMEOUT_COEFF):0)
//		, FLD_TMR_WD_EN, (MODULE_WATCHDOG_ENABLE?1:0));
}


unsigned long pm_systick_offset = 0;

_attribute_ram_code_ u32 clock_time(void)
{
	u8 r = irq_disable();

	u32 ret = ( reg_system_tick + pm_systick_offset);

	irq_restore(r);

	return ret;
}

_attribute_ram_code_ void sleep_us (u32 us)
{
	u32 t = clock_time();
	while(!clock_time_exceed(t, us)){
	}
}



