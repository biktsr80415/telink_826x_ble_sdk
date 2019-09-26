#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/hci/hci.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/ble/att.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/phy/phy_test.h"
#include "../../proj/drivers/uart.h"
#include "../common/blt_soft_timer.h"
#include "../../proj/drivers/uart.h"
#include "../../proj_lib/ble/ble_common.h"


#if (FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION)


	#define RX_FIFO_SIZE 	288 //rx-28   max:251+28 = 279  16 align-> 288
	#define RX_FIFO_NUM 	8

	#define TX_FIFO_SIZE 	264 //tx-12   max:251+12 = 263  4 align-> 264
	#define TX_FIFO_NUM 	8

	#define MTU_SIZE_SETTING 			247
	#define DLE_TX_SUPPORTED_DATA_LEN 	MAX_OCTETS_DATA_LEN_EXTENSION //264-12 = 252 > Tx max:251


	u8 blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
	my_fifo_t blt_rxfifo = {
								RX_FIFO_SIZE,
								RX_FIFO_NUM,
								0,
								0,
								blt_rxfifo_b,
						   };


	u8 blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
	my_fifo_t blt_txfifo = {
								TX_FIFO_SIZE,
								TX_FIFO_NUM,
								0,
								0,
								blt_txfifo_b,
						   };



	u32 connect_event_occurTick = 0;
	u32 mtuExchange_check_tick = 0;

	int dle_started_flg = 0;

	int mtuExchange_started_flg = 0;

	u16 final_MTU_size = 23;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
int module_onReceiveData(rf_packet_att_write_t *p)
{
	u8 len = p->l2capLen - 3;
	if (len > 0)
	{
		printf("RF_RX len: %d\nc2s:write data: %d\n", p->rf_len, len);
		//printfArray(&p->value, len);
		printf("\n");

		printf("s2c:notify data: %d\n", len);
//		array_printf(&p->value, len);

		if (len + 3 <= final_MTU_size)
		{ //opcode: 1 byte; attHandle: 2 bytes
			bls_att_pushNotifyData(0x11, &p->value, len);
		}
		else
		{
			//can not send this packet, cause MTU size exceed
		}
	}

	return 0;
}


void	task_connect (u8 e, u8 *p, int n)
{
	connect_event_occurTick = clock_time()|1;
	bls_l2cap_requestConnParamUpdate(8, 8, 19, 200);

	bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(1000);

	//MTU size reset to default 23 bytes every new connection, it can be only updated by MTU size exchange procedure
	final_MTU_size = 23;
}


void	task_terminate (u8 e, u8 *p, int n)
{
    printf("----- terminate rsn: 0x%x -----\n", *p);
    connect_event_occurTick = 0;
    mtuExchange_check_tick = 0;

    //MTU size exchange and data length exchange procedure must be executed on every new connection,
    //so when connection terminate, relative flags must be cleared
    dle_started_flg = 0;
    mtuExchange_started_flg = 0;

    //MTU size reset to default 23 bytes when connection terminated
    final_MTU_size = 23;
}

void	task_dle_exchange (u8 e, u8 *p, int n)
{
	ll_data_extension_t* dle_param = (ll_data_extension_t*)p;
	printf("----- DLE exchange: -----\n");
	printf("connEffectiveMaxRxOctets: %d\n", dle_param->connEffectiveMaxRxOctets);
	printf("connEffectiveMaxTxOctets: %d\n", dle_param->connEffectiveMaxTxOctets);
	printf("connMaxRxOctets: %d\n", dle_param->connMaxRxOctets);
	printf("connMaxTxOctets: %d\n", dle_param->connMaxTxOctets);
	printf("connRemoteMaxRxOctets: %d\n", dle_param->connRemoteMaxRxOctets);
	printf("connRemoteMaxTxOctets: %d\n", dle_param->connRemoteMaxTxOctets);

	dle_started_flg = 1;
}



void mtu_size_exchange_func(u16 connHandle, u16 remoteMtuSize)
{
    final_MTU_size = min(remoteMtuSize, MTU_SIZE_SETTING);
    mtuExchange_started_flg = 1;

    printf("------ MTU Size exchange ------\n");
    printf("remote MTU size: %d\n", remoteMtuSize);
    printf("local MTU size: %d\n",  MTU_SIZE_SETTING);
    printf("effect MTU size: %d\n", final_MTU_size);
}


void feature_sdle_test_mainloop(void)
{
    if(connect_event_occurTick && clock_time_exceed(connect_event_occurTick, 1500000))
    {   //1.5s after connection established
        connect_event_occurTick = 0;

        if(!mtuExchange_started_flg)
        {   //master do not send MTU exchange request in time
            blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, MTU_SIZE_SETTING);
            printf("After conn 1.5s, S send  MTU size req to the Master.\n");
        }

        mtuExchange_check_tick = clock_time() | 1;
    }

    if (mtuExchange_check_tick && clock_time_exceed(mtuExchange_check_tick, 500000))
    {
    	//2s after connection established
        mtuExchange_check_tick = 0;

        if(!dle_started_flg)
        {   //master do not send data length request in time
            printf("Master hasn't initiated the DLE yet, S send DLE req to the Master.\n");
            blc_ll_exchangeDataLength(LL_LENGTH_REQ, DLE_TX_SUPPORTED_DATA_LEN);//DLE_TX_SUPPORTED_DATA_LEN
        }
    }
}



void feature_sdle_test_init(void){


////////////////// BLE stack initialization ////////////////////////////////////
	rf_set_power_level_index (RF_POWER_8dBm);

	u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
		memcpy (tbl_mac, pmac, 6);
	}
	else{
		tbl_mac[0] = (u8)rand();
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
	}

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization
	//HID_service_on_android7p0_init();  //hid device on android 7.0

    ///////////////////// USER application initialization ///////////////////
    u8 tbl_advData[] = {
        0x08,0x09,'t','e','s','t','D','L','E',
    };
    u8 tbl_scanRsp[] = {
        0x08,0x09,'t','e','s','t','D','L','E',
    };
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}

	bls_ll_setAdvEnable(1);  //adv enable

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	bls_app_registerEventCallback(BLT_EV_FLAG_DATA_LENGTH_EXCHANGE, &task_dle_exchange);

	blc_att_setRxMtuSize(MTU_SIZE_SETTING);
	blc_att_registerMtuSizeExchangeCb((att_mtuSizeExchange_callback_t *)&mtu_size_exchange_func);
}






#endif ///end of (FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION)
