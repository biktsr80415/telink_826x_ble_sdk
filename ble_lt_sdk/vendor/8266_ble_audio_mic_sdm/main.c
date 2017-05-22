
#include "../../proj/tl_common.h"

#if(__PROJECT_8266_AUDIO_MIC_SDM__)

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"

#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"


int main (void) {

	cpu_wakeup_init();

	set_tick_per_us (CLOCK_SYS_CLOCK_HZ/1000000);

	clock_init();

	dma_init();

	gpio_init();

	rf_drv_init(CRYSTAL_TYPE);

    user_init ();

    irq_enable();

	while (1) {
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		main_loop ();
	}
}

#endif


