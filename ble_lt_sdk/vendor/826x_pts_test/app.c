#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/hci/hci.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/uart.h"


#if ( __826x_PTS_TEST__ )

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);


///////////////////////////// PTS test addr ///////////////////////////////////////////////////
u8  tbl_mac []     = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xb7};  // 826x PTS_test addr(IUT), add_type: Public
u8  pts_mac[6]     = {0xe5, 0x30, 0x07, 0xdc, 0x1b, 0x00};	// PTS dongle addr, add_type: Public



#if (CONFIG_BLE_ROLE == BLE_ROLE_AS_MASTER)//Master role
	extern 	void blc_l2cap_register_handler (void *p);
	int 	app_event_callback (u32 h, u8 *p, int n);
	int 	app_l2cap_handler (u16 conn_handle, u8 *raw_pkt);
	int 	app_host_smp_finish (void);
	void 	main_loop (void);
	u16		current_connHandle = BLE_INVALID_CONNECTION_HANDLE;	 //	handle of  connection
	u8 		current_conn_adr_type;
	u8 		current_conn_address[6];
	u32 	master_connecting_tick_flag;  //for smp trigger proc
	int 	user_manual_paring;
	u8		app_host_smp_sdp_pending = 0; 		//security & service discovery
	int		dongle_pairing_enable = 0;
	int 	dongle_unpair_enable = 0;
#else//(CONFIG_BLE_ROLE == BLE_ROLE_AS_SLAVE)
	void	task_connect (u8 e, u8 *p, int n)
	{
	#if (PTS_TEST_MODE == CONN_CPUP_BV01 || PTS_TEST_MODE == CONN_CPUP_BV02 || PTS_TEST_MODE == CONN_CPUP_BV03)
		bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  //interval=10ms latency=99 timeout=4s
	#elif(PTS_TEST_MODE == CONN_TERM_BV01 )
		bls_ll_terminateConnection(0x16);
	#endif
	}

	void	task_terminate (u8 e, u8 *p, int n)
	{

	}

	void	task_paring_begin (u8 e, u8 *p, int n)
	{

	}



	u8 paring_result = 0xff;
	void	task_paring_end (u8 e, u8 *p, int n)
	{
			paring_result = *p;

			if(paring_result == BLE_SUCCESS){

			}
			else{
				// paring_result is fail reason
			}

	}

	void	task_encryption_done (u8 e, u8 *p, int n)
	{
			if(*p == SMP_STANDARD_PAIR){  //first paring

			}
			else if(*p == SMP_FAST_CONNECT){  //auto connect

			}
	}




	int AA_dbg_suspend;
	void  func_suspend_enter (u8 e, u8 *p, int n)
	{
		AA_dbg_suspend ++;
	}

	void  func_suspend_exit (u8 e, u8 *p, int n)
	{

	}

	#if (PTS_TEST_MODE == L2CAP_LE_CPU_BI02)
		/*------------------------------------------------------------------- l2cap data pkt(SIG) ---------------------------------------------------*
		 | stamp_time(4B) |llid nesn sn md |  pdu-len   | l2cap_len(2B)| chanId(2B)| Code(1B)|Id(1B)|Data-Len(2B) |           Result(2B)             |
		 |                |   type(1B)     | rf_len(1B) |       L2CAP header       |          SIG pkt Header      |  SIG_Connection_param_Update_Rsp |
		 |                |                |            |     0x0006   |    0x05   |   0x13  | 0x01 |  0x0002     |             0x0000               |
		 |                |          data_headr         |                                                       payload                              |
		 *-------------------------------------------------------------------------------------------------------------------------------------------*/
		void  blc_l2cap_SendConnParamUpdateReject(u16 connHandle)
		{
			u8 conn_update_rsp[16];  //12 + 4(mic)

			rf_packet_l2cap_connParaUpRsp_t *pRsp = (rf_packet_l2cap_connParaUpRsp_t *)conn_update_rsp;
			pRsp->llid = L2CAP_FIRST_PKT_C2H;
			pRsp->rf_len = 10;
			pRsp->l2capLen = 6;
			pRsp->chanId = L2CAP_CID_SIG_CHANNEL;
			pRsp->opcode = 0x01;
			pRsp->id = 0x01;
			pRsp->data_len = 2;
			pRsp->result = 0;

			bls_ll_pushTxFifo (connHandle | HANDLE_STK_FLAG, conn_update_rsp);
		}

		int att_sig_proc_handler (u16 connHandle, u8 * p)
		{
			blc_l2cap_SendConnParamUpdateReject (BLS_CONN_HANDLE);
		}
	#endif
#endif



void user_init()
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	#if (USB_ADV_REPORT_TO_PC_ENABLE)
		REG_ADDR8(0x74) = 0x53;
		REG_ADDR16(0x7e) = 0x08d1;
		REG_ADDR8(0x74) = 0x00;
		usb_log_init ();
		usb_dp_pullup_en (1);  //open USB enum
	#endif

	//////////////////////gpio configutation//////////////////////
	#if(PTS_TEST_MODE == GATT_SR_GAI_NV01 || PTS_TEST_MODE == GATT_SR_GAS_BV01)
		gpio_set_func(GPIO_PD2, AS_GPIO);
		gpio_set_input_en(GPIO_PD2, 1);
		gpio_set_output_en(GPIO_PD2, 0);
		gpio_write(GPIO_PD2, 1);
	#endif


////////////////// BLE stack initialization ////////////////////////////////////
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
	#if (CONFIG_BLE_ROLE == BLE_ROLE_AS_SLAVE)
		blc_ll_initAdvertising_module(tbl_mac);
		blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

		////// Host Initialization  //////////
		extern void my_att_init ();
		my_att_init (); //gatt initialization
		blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
		#if (PTS_TEST_MODE == L2CAP_LE_CPU_BI02)
			blc_l2cap_reg_att_sig_hander(att_sig_proc_handler);         //register sig process handler
		#endif

		u8 tbl_advData[] = {
			 0x05, 0x09, 't', 'e', 's', 't',
			 0x02, 0x01, 0x05,
			 0x07, 0x03, 0x18, 0x00, 0x18, 0x01, 0x18, 0x0f,
			 0x03, 0xff, 0x02, 0x11,
			 0x02, 0x19, 0x00, 0x00,
			};
		u8	tbl_scanRsp [] = {
				 0x08, 0x09, 'T', 'E', 'S', 'T', 'A', 'D', 'V',	//scan name
			};
		bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
		bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

		//ble event call back
		bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);

	#else//#if (CONFIG_BLE_ROLE == BLE_ROLE_AS_MASTER)
		blc_ll_initScanning_module(tbl_mac); 	//scan module: 		 mandatory for BLE master,
		blc_ll_initInitiating_module();			//initiate module: 	 mandatory for BLE master,
		blc_ll_initMasterRoleSingleConn_module();			//master module: 	 mandatory for BLE master,

		//// controller hci event mask config ////
		//bluetooth event
		blc_hci_setEventMask_cmd (HCI_EVT_MASK_DISCONNECTION_COMPLETE | HCI_EVT_MASK_ENCRYPTION_CHANGE);
		//bluetooth low energy(LE) event
		blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_CONNECTION_COMPLETE  \
									|	HCI_LE_EVT_MASK_ADVERTISING_REPORT \
									|   HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE \
									|   HCI_LE_EVT_MASK_CONNECTION_ESTABLISH);  //connection establish: telink private event

		////// Host Initialization  //////////
		blc_l2cap_register_handler (app_l2cap_handler);  //controller data to host(l2cap data) all processed in this func
		blc_hci_registerControllerEventHandler(app_event_callback); //controller hci event to host all processed in this func

	#endif

	#if (PTS_TEST_MODE == CONN_DCON_BV01)
		u8 peer_addr[6] = {0xe5,0x30,0x07,0xdc,0x1b,0x00}; //change to the PTS donele addr
		rf_packet_adv_t	pkt_adv;
		memcpy(pkt_adv.advA,tbl_mac,6);
		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_40MS, \
											ADV_TYPE_CONNECTABLE_DIRECTED_HIGH_DUTY, OWN_ADDRESS_PUBLIC, \
												BLE_ADDR_PUBLIC, peer_addr, BLT_ENABLE_ADV_ALL,	ADV_FP_NONE);
		if(status != BLE_SUCCESS) { write_reg8(0x8000, 0x11); 	while(1); }  //debug: adv setting err
	#elif (PTS_TEST_MODE == CONN_UCON_BV01 || PTS_TEST_MODE == GAP_DISC_NONN_BV02)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x04,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_40MS, \
										ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == CONN_UCON_BV02 || PTS_TEST_MODE == GAP_DISC_GENM_BV04)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x06,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_40MS, \
										ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_DISC_GENM_BV03 || PTS_TEST_MODE == GAP_CONN_NCON_BV01 || PTS_TEST_MODE == GAP_CONN_NCON_BV02)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x06,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_BROB_BCST_BV01)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x04,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_BROB_BCST_BV02 || PTS_TEST_MODE == GAP_DISC_NONN_BV01)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x04,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC, \
										0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_CONN_NCON_BV03)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x05,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_DISC_LIMM_BV03)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x05,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC, \
										0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_DISC_LIMM_BV04)
		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x05,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC, \
										0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#elif (PTS_TEST_MODE == GAP_BROB_BCST_BV03)
		u8 irk[] = {};
		smp_quickResolvPrivateAddr(irk,tbl_mac);

		u8 tbl_advudData[] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
			 0x02, 0x01, 0x04,
			};
		bls_ll_setAdvData( (u8 *)tbl_advudData, sizeof(tbl_advudData) );

		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_105MS, ADV_INTERVAL_200MS, \
										ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}

	#elif (    PTS_TEST_MODE == SM_MAS_PROT_BV01C \
			|| PTS_TEST_MODE == SM_MAS_KDU_BV06C \
			|| PTS_TEST_MODE == SM_MAS_SIP_BV02C \
			|| PTS_TEST_MODE == GAP_BOND_NBON_BV01C \
			|| PTS_TEST_MODE == GAP_BOND_NBON_BV02C)

		blm_host_smp_init(FLASH_ADR_PARING);
		blm_smp_registerSmpFinishCb(app_host_smp_finish);
		blm_host_smp_setSecurityTrigger(MASTER_TRIGGER_SMP_FIRST_PAIRING | MASTER_TRIGGER_SMP_AUTO_CONNECT);
		u32 index = tbl_bond_slave_search(BLE_ADDR_PUBLIC, pts_mac);

		#if (PTS_TEST_MODE == SM_MAS_KDU_BV06C)
			blc_smp_expectDistributeKey(1, 0, 0);
        #endif

        #if (PTS_TEST_MODE == SM_MAS_SIP_BV02C)
			blc_smp_setIoCapability(IO_CAPABILITY_KEYBOARD_DISPLAY);
        #endif

		#if (PTS_TEST_MODE == GAP_BOND_NBON_BV01C || \
		     PTS_TEST_MODE == GAP_BOND_NBON_BV02C)
			blc_smp_enableBonding(0);//non-bondable mode.
			if(index){
				printf("keep IUT not bonded\n");
				tbl_bond_slave_delete_by_index(index-1);
			}
		#endif

		ll_whiteList_reset();  //clear whitelist
		ll_whiteList_add(0, pts_mac);

		//set scan paramter and scan enable
		blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS,
								  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
		blc_ll_setScanEnable (BLC_SCAN_ENABLE, 0);


		#if 0
			/*
			 * 这几个测试用例需要先运行PTS run按钮,然后IUT(826x pts test)上电发起Conn_req,执行后续认证流程
			 * 否则使用按键触发(使能UI_BUTTON_ENABLE)：sw1:send conn_req pkt, sw2:disconnect link ...
			 */
			blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS, INITIATE_FP_ADV_WL,  BLE_ADDR_PUBLIC, \
								 pts_mac, BLE_ADDR_PUBLIC, CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 0, CONN_TIMEOUT_4S, 0, 0);
		#endif

		printf("***init ok***\n");

	#elif (    PTS_TEST_MODE == SM_SLA_PROT_BV02C \
			|| PTS_TEST_MODE == SM_SLA_KDU_BV01C \
			|| PTS_TEST_MODE == GAP_BOND_NBON_BV03C \
			|| PTS_TEST_MODE == GAP_BOND_BON_BV01C \
			|| PTS_TEST_MODE == GAP_BOND_BON_BV03C)
		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_20MS, ADV_INTERVAL_30MS, \
				                        ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}

		#if (PTS_TEST_MODE == GAP_BOND_BON_BV03C)
		    //Master trigger SMP
			bls_smp_enableParing (SMP_PARING_PEER_TRRIGER);
		#else
			//Slave sent 'Secure_req' to trigger SMP
			bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );
		#endif

		#if (PTS_TEST_MODE == GAP_BOND_NBON_BV03C)
			blc_smp_enableBonding(0);//non-bondable mode.
			//blc_smp_expectDistributeKey(0, 0, 0);//according to master's Pairing_req pkt's keyDistribute bit settings, there no need.
			//blc_smp_setDistributeKey(0, 0, 0);   //according to master's Pairing_req pkt's keyDistribute bit settings, there no need.
		#endif

		#if (PTS_TEST_MODE == GAP_BOND_BON_BV01C || PTS_TEST_MODE == GAP_BOND_BON_BV03C)
			//blc_smp_enableBonding(1);//default: bondable mode.
			extern u32	blc_smp_isCurrentAddrBonded(u8 addr_type, u8* addr );
			extern u8	blc_smp_param_getIndexByFLashAddr(u32 flash_addr);
			extern int	blc_smp_param_deleteByIndex(u8 index);

			blc_smp_enableAuthMITM(1, 123456);
			blc_smp_setIoCapability(IO_CAPABILITY_DISPLAY_ONLY);

			u32 flash_addr = blc_smp_isCurrentAddrBonded(BLE_ADDR_PUBLIC , pts_mac);
			if(flash_addr){
				printf("should delete the older smp banding info\n");
				u8 index = blc_smp_param_getIndexByFLashAddr(flash_addr);
				blc_smp_param_deleteByIndex(index);
			}
		#endif

	#else
		u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_40MS, \
										ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
										 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

		if(status != BLE_SUCCESS){  //adv setting err
			write_reg8(0x8000, 0x11);  //debug
			while(1);
		}
	#endif

	#if (CONFIG_BLE_ROLE == BLE_ROLE_AS_SLAVE)
		bls_ll_setAdvEnable(1);  //adv enable
    #endif

	rf_set_power_level_index (RF_POWER_8dBm);

}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_wakeup;
void main_loop (void)
{
	static u32 tick_loop;

	tick_loop ++;

	blt_sdk_main_loop();
#if(PTS_TEST_MODE == GATT_SR_GAI_NV01 || PTS_TEST_MODE == GATT_SR_GAS_BV01)
	if(!gpio_read(GPIO_PD2))
	{
		u8 ind[2] = {0x2a, 0x00, 0x2d, 0x00};
		bls_att_pushIndicateData(GenericAttribute_ServiceChanged_DP_H, ind, 2);
	}
#endif

#if (UI_BUTTON_ENABLE)
	extern void proc_button (void);
	proc_button();  //button proc

	//------------- sw2 pressed ------------
	static int master_disconnect_flag;
	if(dongle_unpair_enable)
	{
		if(!master_disconnect_flag && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN)
		{
			if( blm_ll_disconnect(BLM_CONN_HANDLE, HCI_ERR_REMOTE_USER_TERM_CONN) == BLE_SUCCESS)
			{
				master_disconnect_flag = 1;
				dongle_unpair_enable = 0;
				printf("terminate connection\n");
			}
		}
	}
	if(master_disconnect_flag && blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){
		master_disconnect_flag = 0;
	}
#endif

#if (CONFIG_BLE_ROLE == BLE_ROLE_AS_MASTER)
	if( blm_host_smp_getSecurityTrigger() )
	{
		//Master trigger Pairing... after connected 1s.
		if(	master_connecting_tick_flag && clock_time_exceed(master_connecting_tick_flag, 1000000) )
		{
			master_connecting_tick_flag = 0;
			printf("IUT trigger SMP\n");
			blm_host_smp_procSecurityTrigger(BLM_CONN_HANDLE);
		}
	}
#endif

#if (SMP_BLE_CERT_TEST)
	blc_smp_certTimeoutLoopEvt (CONFIG_BLE_ROLE - 1);// for ble cert use
#endif
}

#if (CONFIG_BLE_ROLE == BLE_ROLE_AS_MASTER)
int app_host_smp_finish (void)  //smp finish callback
{
	app_host_smp_sdp_pending = 0;  //no sdp

	return 0;
}
int app_l2cap_handler (u16 conn_handle, u8 *raw_pkt)
{
	//l2cap data packeted, make sure that user see complete l2cap data
	rf_packet_l2cap_t *ptrL2cap = blm_l2cap_packet_pack (conn_handle, raw_pkt);
	if (!ptrL2cap)
	{
		return 0;
	}

	//l2cap data channel id, 4 for att, 5 for signal, 6 for smp
	if(ptrL2cap->chanId == L2CAP_CID_SMP) //smp
	{
		if(app_host_smp_sdp_pending == SMP_PENDING)
		{
			blm_host_smp_handler(conn_handle, (u8 *)ptrL2cap);
		}
	}

	return 0;
}


int app_event_callback (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;

	if (h &HCI_FLAG_EVENT_BT_STD)		//ble controller hci event
	{
		u8 evtCode = h & 0xff;

		//------------ disconnect -------------------------------------
		if(evtCode == HCI_CMD_DISCONNECTION_COMPLETE)  //connection terminate
		{
			current_connHandle = BLE_INVALID_CONNECTION_HANDLE;  //when disconnect, clear conn handle

			master_connecting_tick_flag = 0;

			//if previous connection smp&sdp not finished, clear this flag
			if(app_host_smp_sdp_pending){
				app_host_smp_sdp_pending = 0;
			}

			printf("connection terminate\n");

			//should set scan mode again to scan slave adv packet
			blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS,
									  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
			blc_ll_setScanEnable (BLC_SCAN_ENABLE, 0);


		}
		else if(evtCode == HCI_EVT_ENCRYPTION_CHANGE)
		{
			event_enc_change_t *pe = (event_enc_change_t *)p;
			blm_smp_encChangeEvt(pe->status, pe->handle, pe->enc_enable);
		}
		else if(evtCode == BLM_CONN_ENC_REFRESH)
		{
			event_enc_refresh_t *pe = (event_enc_refresh_t *)p;
			blm_smp_encChangeEvt(pe->status, pe->handle, 1);
		}
		else if(evtCode == HCI_EVT_LE_META)
		{
			u8 subEvt_code = p[0];

			//------------le connection complete event-------------------------------------
			if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
			{
				printf("connection complete\n");
			}
			else if(subEvt_code == HCI_SUB_EVT_LE_CONNECTION_ESTABLISH)  //connection establish(telink private event)
			{
				event_connection_complete_t *pc = (event_connection_complete_t *)p;
				if (pc->status == BLE_SUCCESS)	// status OK
				{
					printf("connection establish\n");

					current_connHandle = pc->handle;   //mark conn handle, in fact this equals to BLM_CONN_HANDLE
					master_connecting_tick_flag = clock_time() | 1;  //none zero value

					//save current connect address type and address
					current_conn_adr_type = pc->peer_adr_type;
					memcpy(current_conn_address, pc->mac, 6);

					app_host_smp_sdp_pending = SMP_PENDING; //pair & security first
				}
			}
			//------------ le ADV report event ------------------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{
				//after controller is set to scan state, it will report all the adv packet it received by this event
				event_adv_report_t *pa = (event_adv_report_t *)p;
				s8 rssi = pa->data[pa->len];

				 //if previous connection smp&sdp not finish, can not create a new connection
				if(app_host_smp_sdp_pending){
					return 1;
				}

				#if (UI_BUTTON_ENABLE)
				    //------------- sw1 pressed ------------
					if(dongle_pairing_enable)// && !memcmp(pts_mac, pa->mac, 6))//  && (rssi > -56))
					{
						printf("master adr_type: %s	BD_ADR:%x:%x:%x:%x:%x:%x\n", pa->adr_type ? "random":"public", \
								pa->mac[5], pa->mac[4], pa->mac[3], pa->mac[2], pa->mac[1], pa->mac[0]);

						tbl_bond_slave_search(BLE_ADDR_PUBLIC, pts_mac);

						printf("start Conn_Req pkt\n");
						blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS, INITIATE_FP_ADV_WL,  BLE_ADDR_PUBLIC, \
												  pts_mac, BLE_ADDR_PUBLIC, CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 0, CONN_TIMEOUT_4S, 0, 0);
					}
				#endif
			}
		}
	}
}
#endif

#endif  // end of __826x_PTS_TEST__
