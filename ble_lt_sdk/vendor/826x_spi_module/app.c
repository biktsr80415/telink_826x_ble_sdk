#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/spi.h"
#include "spp.h"

//statement in file: spp.c line 118 & 119.
extern u8 *spi_rx_buff;  //fe00~fe48 72 rx buff
extern u8 *spi_tx_buff;	 //fe60~fea8 72 tx buff

u8 tx_done_status = 1;

MYFIFO_INIT(hci_rx_fifo, 72, 2);
MYFIFO_INIT(hci_tx_fifo, 72, 8);

MYFIFO_INIT(blt_rxfifo, 64, 8);

MYFIFO_INIT(blt_txfifo, 40, 16);
//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
};

u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
	};


u8 	ui_ota_is_working = 0;
u8  ui_task_flg;
u32	ui_advertise_begin_tick;

void entry_ota_mode(void)
{
	ui_ota_is_working = 1;

	bls_ota_setTimeout(30 * 1000000); //set OTA timeout  30 S

	//gpio_write(GPIO_LED, 1);  //LED on for indicate OTA mode
}

void show_ota_result(int result)
{
#if 0
	if(result == OTA_SUCCESS){
		for(int i=0; i< 8;i++){  //4Hz shine for  4 second
			gpio_write(GPIO_LED, 0);
			sleep_us(125000);
			gpio_write(GPIO_LED, 1);
			sleep_us(125000);
		}
	}
	else{
		for(int i=0; i< 8;i++){  //1Hz shine for  4 second
			gpio_write(GPIO_LED, 0);
			sleep_us(500000);
			gpio_write(GPIO_LED, 1);
			sleep_us(500000);
		}

		//write_reg8(0x8000,result); ;while(1);  //debug which err lead to OTA fail
	}


	gpio_write(GPIO_LED, 0);
#endif
}


void	task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (12, 32, 0, 400);
}


u32 tick_wakeup;
int	mcu_uart_working;
int	module_uart_working;
int module_task_busy;
int	module_uart_data_flg;
u32 module_wakeup_module_tick;
#define HCI_TX_BUSY			( (hci_tx_fifo.rptr != hci_tx_fifo.wptr) || *(u32*)(spi_tx_buff)!=0 )
#define HCI_RX_BUSY			(hci_rx_fifo.rptr != hci_rx_fifo.wptr || *(u32*)(spi_rx_buff)!=0)

int app_module_busy ()
{
	mcu_uart_working = (gpio_read(GPIO_UCTSC5)==0);
	module_uart_working = HCI_TX_BUSY || HCI_RX_BUSY;
	module_task_busy = mcu_uart_working || module_uart_working;
	return module_task_busy;
}

void app_suspend_exit ()
{
	GPIO_WAKEUP_MODULE_HIGH;  //module enter working state
	bls_pm_setSuspendMask(SUSPEND_DISABLE);
	tick_wakeup = clock_time () | 1;
}

int app_suspend_enter ()
{
	if (app_module_busy ())
	{
		app_suspend_exit ();
		return 0;
	}
	return 1;
}

void app_power_management ()
{

#if (BLE_MODULE_INDICATE_DATA_TO_MCU)
	module_uart_working = UART_TX_BUSY || UART_RX_BUSY;


	//闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨崇槐鎺斾沪閻愵剙澧ule闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨崇槐鎺戠暆閿熻棄顥攔t闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨堕弻锝夊Χ鎼达紕浠╁┑鐐村焾娴滎亜顬夐搹瑙勫磯闁靛鍎查悗楣冩⒑閸濆嫷鍎忔い顓犲厴閻涱喛绠涘☉娆忥拷闂佸吋浜介崕鎻掆枍閵忋倖鈷戦柛婵嗗閿熶粙鏌涙繝鍐⒈闁跨喎锟介妵鎰板箳閹捐泛寮抽梻浣虹帛濡礁鐣烽悽鐢电幓婵炴垯鍨洪悡鏇熸叏濮楀棗澧柛銈堜含缁辨帗娼忛妸褏鐤勯悗娈垮櫘閸撶喐淇婇崼鏇炲耿婵°倕锕ｇ槐顩孭IO_WAKEUP_MCU闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨堕弻锝夊箣閿濆憛鎾绘煛閸涱喗鍊愭慨濠冩そ瀵爼宕归钘夛拷缂傚倷绶氱涵鎼佸闯閿濆宓侀煫鍥ㄧ♁閸婂鏌ら幁鎺戝姕婵炲懌鍨藉娲传閸曨偓鎷烽梺绋匡攻閻楁粓鏁撴禒瀣櫢濞寸姴顑嗛悡鏇熺箾閸℃ê濮夊褏鏁婚弻鈩冩媴鐟欏嫬纾抽悗娈垮櫘閸撶喐淇婇崼鏇炲耿婵°倕锕ｇ槐锕傛⒒閸屾瑦绁版繛澶嬫礋瀹曚即骞囬弶鍨殤婵犵數濮甸懝鍓х不椤栨粎纾肩�锟芥嫹婵＄炒r闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨堕弻褍顬楅锟芥嫹闂佹眹鍊曠�鏉款瀴閾忚宕夐柕濞垮劜閻庨箖姊洪崫鍕靛剰妞ゎ厾鍏橀悰顔跨疀濞戞瑥锟介梺鍏间航閸庢彃鈻嶉姀銈嗏拺閻犳亽鍔屽▍鎰版煙閸戙倖瀚�if(module_uart_data_flg && !module_uart_working){
		module_uart_data_flg = 0;
		module_wakeup_module_tick = 0;
		GPIO_WAKEUP_MCU_LOW;
	}
#endif


	// pullup GPIO_WAKEUP_MODULE: exit from suspend
	// pulldown GPIO_WAKEUP_MODULE: enter suspend

#if (PM_ENABLE)

	if (!app_module_busy() && !tick_wakeup)
	{
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨堕弻锝夊Χ鎼达紕浼囧┑鐐存尫缁瑥顬夐搹瑙勫磯闁靛鍎查悗楣冩⒑閸濆嫷鍎忔い顓犲厴閻涱喛绠涘☉娆愭缂備浇浜稉顢疧_WAKEUP_MODULE 闂傚倸鍊峰ù鍥р枖閺囥垹绐楅柟鐗堟緲閸戠姴鈹戦悩瀹犲缂佺媭鍨堕弻锝夊箣閿濆憛鎾绘煛閸涱喗鍊愰柡宀嬬節瀹曟帒螣閻氬瓨瀚归梺璇插閻旑剟骞忛敓绲�

	if (tick_wakeup && clock_time_exceed (tick_wakeup, 500))
	{
		GPIO_WAKEUP_MODULE_LOW;
		tick_wakeup = 0;
	}

#endif
}



void led_init(void){
	gpio_set_func(RED_LED, AS_GPIO);//LED RED EVK 8269
	gpio_set_input_en(RED_LED,0);
	gpio_set_output_en(RED_LED,1);
	gpio_write(RED_LED,1);
//////////////////////////////////////
	gpio_set_func(GREEN_LED, AS_GPIO);
	gpio_set_input_en(GREEN_LED,0);
	gpio_set_output_en(GREEN_LED,1);
	gpio_write(GREEN_LED,1);
/////////////////////////////////////
	gpio_set_func(WHITE_LED, AS_GPIO);
	gpio_set_input_en(WHITE_LED,0);
	gpio_set_output_en(WHITE_LED,1);
	gpio_write(WHITE_LED,1);
/////////////////////////////////////
	sleep_us(200000);
	gpio_write(RED_LED,0);
	gpio_write(GREEN_LED,0);
	gpio_write(WHITE_LED,0);
}

void led_onoff(u32 pin,u8 onoff){
	gpio_write(pin, onoff&&0x01);
}

//SPI function
void sspi_init(int pin, int divider){
	//SPI B port: GPIO:B<4~7>
	write_reg8(0x58e,read_reg8(0x58e)&0x0f);///disable GPIO:B<4~7>
	write_reg8(0x5b1,read_reg8(0x5b1)|0xf0);///enable SPI function:B<4~7>
	write_reg8(0x5b0,read_reg8(0x5b0)&0xC3);///disable SPI function:A<2~5>
	gpio_set_input_en(GPIO_PB4,1);
	gpio_set_input_en(GPIO_PB5,1);
	gpio_set_input_en(GPIO_PB6,1);
	gpio_set_input_en(GPIO_PB7,1);

	reg_spi_sp |= FLD_SPI_ENABLE;			//force PADs act as spi
	//div_clock: bit[6:0]
	write_reg8(0x0a,read_reg8(0x0a)|divider);// spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)&0xfd);// disable master mode
	write_reg8(0x0b,read_reg8(0x0b)|SPI_MODE0);// select SPI mode,surpport four modes

	reg_irq_mask |= FLD_IRQ_HOST_CMD_EN;
//	reg_irq_src = FLD_IRQ_HOST_CMD_EN;			//enable spi irq
}

volatile u8 dbg_handle;
int blc_hci_rx (void)
{
	u8* p = my_fifo_get(&hci_rx_fifo);
	if(p)
	{
		spp_onModuleCmd (p+2,  p[0] + p[1]<<8);  //para1 has no use
		my_fifo_pop(&hci_rx_fifo);
	}

	return 0;
}

int blc_hci_tx ()
{
	u8 *p = my_fifo_get (&hci_tx_fifo);
	if(*(u32*)spi_tx_buff == 0 && p)
	{
		//                             len = length(stateL stateH data)
        //spi tx buffer format(Hex):ff len stateL stateH data(data length = len-2)
		memcpy(spi_tx_buff, p, p[1] + 2);
		my_fifo_pop (&hci_tx_fifo);
	}
	if(p!=0 ||*(u32*)spi_tx_buff != 0)
	{
		dbg_handle++;
		tx_done_status = 0;
		SPI_MODULE_DATA_READY;
		sleep_us(500);
	}
	else if(p == 0&&*(u32*)spi_tx_buff == 0)
	{
		tx_done_status = 1;
		SPI_MODULE_DATA_FINISH;
	}
	return 0;
}
volatile u8 dlen;
void spi_write_handler (void)
{
	//  spi rx buffer:
	//spp format(Hex):xx ff lenL lenH data(data len = lenL+lenH<<8)
	dlen = 4 + *(u16*)(spi_rx_buff+2);
	if(dlen < 64){
		my_fifo_push(&hci_rx_fifo, spi_rx_buff, dlen);		//todo:copy spi received data to spi buffer
	}
	*(u32*)(spi_rx_buff) = 0;
}

void spi_read_handler (void)
{
	*(u32*)spi_tx_buff = 0; //reset command buffer, if master read zero data, ignore the event
	SPI_MODULE_DATA_FINISH;
}

void user_init()
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

    led_init();

	////////////////// BLE stack initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        //TODO : should write mac to flash after pair OK
        tbl_mac[0] = (u8)rand();
        flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
    }

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional

	//gatt initialization
	extern void my_att_init ();
	my_att_init ();

	//l2cap initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);

	//smp initialization
	//bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );


	///////////////////// USER application initialization ///////////////////

	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));


	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}

	bls_ll_setAdvEnable(1);  //adv enable

	rf_set_power_level_index (RF_POWER_8dBm);

#if(PM_ENABLE)
	gpio_set_wakeup(SSPI_PM_WAKEUP_PIN,1,1);//drive pin core(gpio) high wakeup suspend

	GPIO_WAKEUP_MODULE_LOW;

	bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );

#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif

	////////////////// SPI initialization ///////////////////////////////////
	sspi_init(SSPI_TX_NOTIFY_PIN, 0x25);//SSPI_TX_NOTIFY_PIN : 8269 EVK G2
	blc_register_hci_handler(blc_hci_rx,blc_hci_tx);//customized spi spp handler

	extern void event_handler(u32 h, u8 *para, int n);
	blc_hci_registerControllerEventHandler(event_handler);		//register event callback
	bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h

	// OTA init
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	ui_advertise_begin_tick = clock_time();
}

void blt_pm_proc(void){
//Wakeup source setting
#if PM_ENABLE
	if((spp_task_finished_flg) || gpio_read(SSPI_PM_WAKEUP_PIN)){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	else{
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);
	}

#endif
}
/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;

	tick_loop ++;

	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop ();

	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task
	blt_pm_proc();

}
