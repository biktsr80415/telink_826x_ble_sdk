#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"



#include "blm_att.h"
#include "blm_pair.h"
#include "blm_host.h"
#include "blm_ota.h"

MYFIFO_INIT(blt_rxfifo, 64, 8);











int main_idle_loop (void);



























///////////////////////////////////////////
void user_init()
{
	random_generator_init();  //this is must

	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	//set USB ID
	usb_log_init ();
	REG_ADDR8(0x74) = 0x62;
	REG_ADDR16(0x7e) = 0x08d0;
	REG_ADDR8(0x74) = 0x00;

	//////////////// config USB ISO IN/OUT interrupt /////////////////
	reg_usb_mask = BIT(7);			//audio in interrupt enable
	reg_irq_mask |= FLD_IRQ_IRQ4_EN;
	reg_usb_ep6_buf_addr = 0x80;
	reg_usb_ep7_buf_addr = 0x60;
	reg_usb_ep_max_size = (256 >> 3);

	usb_dp_pullup_en (1);  //open USB enum



	///////////////// SDM /////////////////////////////////
#if (AUDIO_SDM_ENBALE)
	u16 sdm_step = config_sdm  (buffer_sdm, TL_SDM_BUFFER_SIZE, 16000, 4);
#endif


///////////// BLE stack Initialization ////////////////
	u8  tbl_mac [6];
	if (*(u32 *) CFG_ADR_MAC == 0xffffffff){
		generateRandomNum(6, tbl_mac);
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);  //store master address
	}
	else{
		memcpy (tbl_mac, (u8 *) CFG_ADR_MAC, 6);  //copy from flash
	}



	////// Controller Initialization  //////////
	blc_ll_initBasicMCU();
	blc_ll_initStandby_module(tbl_mac);				//mandatory
	blc_ll_initScanning_module(tbl_mac); 	//scan module: 		 mandatory for BLE master,
	blc_ll_initInitiating_module();			//initiate module: 	 mandatory for BLE master,
	blc_ll_initMasterRoleSingleConn_module();			//master module: 	 mandatory for BLE master,


	rf_set_power_level_index (RF_POWER_P3p01dBm);


	////// Host Initialization  //////////
	blc_l2cap_register_handler (app_l2cap_handler);    			//l2cap initialization
	blc_hci_registerControllerEventHandler(app_event_callback); //controller hci event to host all processed in this func

	//bluetooth event
	blc_hci_setEventMask_cmd (HCI_EVT_MASK_DISCONNECTION_COMPLETE | HCI_EVT_MASK_ENCRYPTION_CHANGE);

	//bluetooth low energy(LE) event
	blc_hci_le_setEventMask_cmd(		HCI_LE_EVT_MASK_CONNECTION_COMPLETE  \
									|	HCI_LE_EVT_MASK_ADVERTISING_REPORT \
									|   HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE \
									|   HCI_LE_EVT_MASK_CONNECTION_ESTABLISH);  //connection establish: telink private event



	#if (BLE_HOST_SMP_ENABLE)
		blm_host_smp_init(FLASH_ADR_PARING);
		blm_smp_registerSmpFinishCb(app_host_smp_finish);

		//default smp trigger by slave
		//blm_host_smp_setSecurityTrigger(MASTER_TRIGGER_SMP_FIRST_PAIRING | MASTER_TRIGGER_SMP_AUTO_CONNECT);
	#else  //telink referenced paring&bonding
		user_master_host_pairing_management_init();
	#endif



	extern int host_att_register_idle_func (void *p);
	host_att_register_idle_func (main_idle_loop);





	//set scan parameter and scan enable

	blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS,	\
								  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, 0);



	#if (UI_UPPER_COMPUTER_ENABLE)
		extern void app_upper_com_init(void);
		app_upper_com_init();
	#endif
}








extern void usb_handle_irq(void);
extern void proc_button (void);
extern void proc_audio (void);
/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
_attribute_ram_code_ int main_idle_loop (void)
{
	static u32 tick_loop;
	tick_loop ++;


	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();


	///////////////////////////////////// proc usb cmd from host /////////////////////
	usb_handle_irq();



	/////////////////////////////////////// HCI ///////////////////////////////////////
	blc_hci_proc ();



	////////////////////////////////////// UI entry /////////////////////////////////
#if (UI_BUTTON_ENABLE)
	static u8 button_detect_en = 0;
	if(!button_detect_en && clock_time_exceed(0, 1000000)){// proc button 1 second later after power on
		button_detect_en = 1;
	}
	static u32 button_detect_tick = 0;
	if(button_detect_en && clock_time_exceed(button_detect_tick, 5000))
	{
		button_detect_tick = clock_time();
		proc_button();  //button triggers pair & unpair  and OTA
	}
#endif


#if (UI_UPPER_COMPUTER_ENABLE)
	extern void app_upper_com_proc(void);
	app_upper_com_proc();
#endif


	////////////////////////////////////// proc audio ////////////////////////////////
#if (UI_AUDIO_ENABLE)
	proc_audio();

	static u32 tick_bo;
	if (REG_ADDR8(0x125) & BIT(0))
	{
		tick_bo = clock_time ();
	}
	else if (clock_time_exceed (tick_bo, 200000))
	{
		REG_ADDR8(0x125) = BIT(0);
	}
#endif


	host_pair_unpair_proc();


#if(BLE_MASTER_OTA_ENABLE)
	proc_ota();
#endif

#if 1
	//proc master update
	if(host_update_conn_param_req){
		host_update_conn_proc();
	}
#endif




	return 0;
}




_attribute_ram_code_ void main_loop (void)
{

	main_idle_loop ();

	if (main_service)
	{
		main_service ();
		main_service = 0;
	}
}





