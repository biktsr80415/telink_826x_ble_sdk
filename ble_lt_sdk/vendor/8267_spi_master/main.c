
#include "../../proj/tl_common.h"

#if(__PROJECT_8267_SPI_MASTER__)

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "mspi_test.h"
#include "../../proj/drivers/flash.h"

extern _attribute_ram_code_ void irq_handler(void);
_attribute_ram_code_ void irq_handler(void)
{

}

///////////////////////////test Add by tuyf 9-02//////////////////////////////////////////
#define NEW_FW_ADR   (64<<12) //8269 firmware bin(Slave SPI) is stored in 0x40000 (256k) in 8267EVK(Master SPI) FLASH
u8  spi_read_buff[64];// to store data from sspi 8269 //_attribute_aligned_(1)
u8 loadramcode_success_flg;
u8 load8269_att_parameter_flg;
///////////////////////////////////////////////////////////////////////////////////////////

extern void user_init();


// .\tcdb.exe wf 40000 -eb -i "xxx.bin�ļ�·��\xxx.bin"
int main (void) {
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();
	clock_init();
	gpio_init();
	rf_drv_init(0);
	user_init ();
	irq_enable();//Note:when loading 8269 ram code,the processing should not be disturbed!

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif

		if(1){//loadramcode_success_flg && load8269_att_parameter_flg){
			proc_host();

#if 0
			extern void proc_button (void);
			proc_button();
#endif

		}
	}
	return 0;
}

#endif
