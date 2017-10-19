
#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"

MYFIFO_INIT(blt_rxfifo, 64, 8);
#define RSSI_OFFSET 110
u32 beacon_count = 0;

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1};

#if UART_ENABLE
#include "../../proj/drivers/uart.h"

#define UART_DATA_LEN    44      // data max 252
typedef struct{
    u32 len;        // data max 252
    u8 data[UART_DATA_LEN];
}uart_data_t;

uart_data_t T_txdata_user;
uart_data_t T_txdata_buf;      // not for user

uart_data_t T_rxdata_user;
uart_data_t T_rxdata_buf;   // data max 252, user must copy rxdata to other Ram,but not use directly
unsigned char uart_rx_true;
#endif

void entry_ota_mode(void)
{
	//bls_pm_setSuspendMask(SUSPEND_DISABLE);
}
void show_ota_result(int result)
{
}

/*agc gain mode only used for detector.
 *max gain mode user for ble connect process
 *parameter: mode = 0, AGC mode
 *parameter: mode = 1, Max Gain mode
 *default mode =  Max gain mode*/
static inline void blt_gain_chg(u8 Gain_mod){
	if(Gain_mod){ // max gain mode
		write_reg8(0x43a, 0x77);  // gain mode
		write_reg8(0x43d, 0x71); // max cnt period
	}else{ // agc mode. auto tune
	    write_reg8(0x43a, 0x22);   // agc mode
	    write_reg8(0x43d, 0xb1); // max cnt period
	}
}

#if 0//common.c
typedef void (*fp_irq_timer1)(u8 *p);
fp_irq_timer1 p_vendor_irq_timer1 = 0;

void hw_timer1_start(u32 t_us,void *cb)	//timer start,max ms is 2^32/16(for 16M sys clock)us
{
	reg_irq_mask |= FLD_IRQ_TMR1_EN;	//enable timer1 interrupt
	reg_tmr1_tick = 0;
	reg_tmr1_capt = CLOCK_SYS_CLOCK_1US * t_us;
	reg_tmr_ctrl |= FLD_TMR1_EN;
	p_vendor_irq_timer1 = (fp_irq_timer1)cb;
}

void hw_timer1_stop(void)
{
	reg_tmr1_tick = 0;
	reg_tmr1_capt = 0;
	reg_tmr_ctrl &= ~FLD_TMR1_EN;
	reg_irq_mask &= ~FLD_IRQ_TMR1_EN;
}

u8 timer_expired(void)
{
	if(reg_tmr_ctrl&FLD_TMR1_EN && (reg_irq_mask&FLD_IRQ_TMR1_EN)==0)	//only one shot mode has expire
		return 1;
	else
		return 0;	//continual mode and not expired
}

void irq_timer1(void){
	static u8 a_irq_timer1;
	a_irq_timer1++;
	if(p_vendor_irq_timer1){
		p_vendor_irq_timer1(0);
		//return;
	}

	/*one shot mode,if continual mode comment this line*/
	 reg_irq_mask &= ~FLD_IRQ_TMR1_EN;
}

void irq_timer2(void){

}


// rssi_filter.c

#ifndef FILTER_CALC_NUM
#define FILTER_CALC_NUM		5   // The number of data one device use.
#endif

#ifndef FILTER_MAX_CNT
#define FILTER_MAX_CNT		20  //The max number of device filter_system could filter.
#endif

#ifndef FILTER_TIMEOUT_US
#define FILTER_TIMEOUT_US		5*1000*1000  // unit of us
#endif

///////////////////////////////define structure of filter rssi /////////////////
//1. size = 14 + FILTER_CALC_NUM
typedef struct{
	u32  last_recv_tick; //record last packet recvice time
	u8 rssi_buf_valid;  //=1, represent using;
	u8 last_filter_data; // receive packet count in valid time
	u8 mac_addr[6];
	u8 rssi_buf_full;  //valid cnt in rssi_buf
	u8 rssi_buf_index;
	u8 rssi_buf[FILTER_CALC_NUM];
	u8 rsvd;
}rssi_param_t;

typedef struct{

	int idle_buf_cnt;  //valid buf cnt;
	rssi_param_t rssi_param[FILTER_MAX_CNT];

}filter_param_t;

static filter_param_t  detetctor_filter_system;

// is new device. 0xff new device. or  exist device in filter system  and return array number.
int is_newDevice(u8* addr){
	int i;
	if(detetctor_filter_system.idle_buf_cnt == FILTER_MAX_CNT){
		return 0xff;
	}
	for(i = 0; i < FILTER_MAX_CNT; ++i){

		if(detetctor_filter_system.rssi_param[i].rssi_buf_valid){

			u8 is_exist = !memcmp(detetctor_filter_system.rssi_param[i].mac_addr, addr, 6);// equal ,return 0.
			if(is_exist){
				return i;
			}

		}

	}
	return 0xff;
}

// return idle buf cnt
int clear_filtSysBuf(){

	int i ;
	for(i = 0; i < FILTER_MAX_CNT; ++i){
		if(clock_time_exceed(detetctor_filter_system.rssi_param[i].last_recv_tick, FILTER_TIMEOUT_US)){
			detetctor_filter_system.rssi_param[i].rssi_buf_valid = 0;
			detetctor_filter_system.idle_buf_cnt ++;
		}
	}

	return detetctor_filter_system.idle_buf_cnt;
}
// return !0 represent the filter system buffer full, can't save. or not full
int is_filtSysFull(){

	if(detetctor_filter_system.idle_buf_cnt == 0){
		clear_filtSysBuf();
	}

	return !detetctor_filter_system.idle_buf_cnt;

}

// Get rssi idle or vaild
// or return buf num + 1.
int rssi_getBufPtr(u8 *mac_addr){
	int i;
	u8 device_buf_num = is_newDevice(mac_addr);
	u8 new_device = (device_buf_num == 0xff);
	u8 buf_full = is_filtSysFull();

//	printf("2::: %d, %d, %d \n", new_device, buf_full, detetctor_filter_system.idle_buf_cnt);

	if(new_device && buf_full){
		return 0;
	}

	//started to save
	u8 save_num = device_buf_num;
	if(new_device){
		for(i = 0; i < FILTER_MAX_CNT; ++i){

			if( detetctor_filter_system.rssi_param[i].rssi_buf_valid == 0){

				u8 *mac_addr_target = detetctor_filter_system.rssi_param[i].mac_addr;
				memcpy (mac_addr_target, mac_addr, 6);

				detetctor_filter_system.rssi_param[i].rssi_buf_full = 0;
				detetctor_filter_system.rssi_param[i].rssi_buf_index = 0;
				detetctor_filter_system.rssi_param[i].rssi_buf_valid = 1;
				detetctor_filter_system.idle_buf_cnt --;
				save_num = i;
				break;
			}

		}

	}

	return save_num + 1;
	//started to calculate
//	return rssi_filter(rssi, &detetctor_filter_system.rssi_param[save_num]);



//	u8 save_cnt = detetctor_filter_system.rssi_param[save_num].rssi_buf_full;
//	detetctor_filter_system.rssi_param[save_num].rssi_buf[save_cnt] = rssi;
//	detetctor_filter_system.rssi_param[save_num].rssi_buf_index ++;

}

u8 rssi_filter(u8 tmp_rssi, rssi_param_t *rssi_param){

	u8 rssi_index = rssi_param->rssi_buf_index;
	u8 store_full_flag = rssi_param->rssi_buf_full;
	u8 mid_cal_data = 0;
	u8 *store_rssi_value = rssi_param->rssi_buf;
	u8 sort_rssi_value[FILTER_CALC_NUM] = {0};

	if((rssi_index < FILTER_CALC_NUM-1)&& (!store_full_flag)){
		store_rssi_value[rssi_index] = tmp_rssi;
		mid_cal_data = tmp_rssi;
	}
	else{
		store_full_flag = 1;////receive 5 packets' flag
		store_rssi_value[rssi_index] = tmp_rssi;
		u8 copy_index = 0;

		for(copy_index = 0;copy_index<FILTER_CALC_NUM;copy_index++){
			sort_rssi_value[copy_index] = store_rssi_value[copy_index];
		}
		u8 i = 0,j = 0;
		u8 tmp_sort = 0;
		for(i=0;i<FILTER_CALC_NUM-1;i++){
			for(j=0;j<FILTER_CALC_NUM-1-i;j++){
				if(sort_rssi_value[j] > sort_rssi_value[j+1]){
					tmp_sort = sort_rssi_value[j];
					sort_rssi_value[j] = sort_rssi_value[j+1];
					sort_rssi_value[j+1] = tmp_sort;
				}
			}
		}
		mid_cal_data = sort_rssi_value[FILTER_CALC_NUM>>1];
	}

	u8 rssi_filter_data = rssi_param->last_filter_data;

	if(!store_full_flag && !rssi_index){/////filter_first_flag == 0
		rssi_filter_data = mid_cal_data;
	}
	else{////filter_first_flag ==  1
		rssi_filter_data = (3*rssi_filter_data + 1*mid_cal_data + 2)>>2;////+2 to rounding  +2/4
	}

	rssi_index++;
	rssi_index %= FILTER_CALC_NUM;////from the header restore data

	rssi_param->rssi_buf_index = rssi_index;
	rssi_param->rssi_buf_full = store_full_flag;
	rssi_param->last_recv_tick = clock_time();
	rssi_param->last_filter_data = rssi_filter_data;


	return rssi_filter_data;
}

//*set adv type  to filter 1 adv ind. 2 directed adv. 4.nonconn
u8 rssi_filte_adv_flag = 0;
void rssi_setAdvFiltType(u8 adv_flag){
	rssi_filte_adv_flag = adv_flag;
}

//return is packet type in filter system
u8 rssi_IsAdvFiltType(u8 adv_type){

	u8 filter_en = rssi_filte_adv_flag & 0x01 && adv_type == 0x00;  // adv ind
	filter_en |= rssi_filte_adv_flag & 0x02 && adv_type == 0x01; // adv directed
	filter_en |= rssi_filte_adv_flag & 0x04 && adv_type == 0x02;  // adv nonconn

	return filter_en;
}

/*
 *  Function: filter process;
 *  Interface: int rssi_handleProc(u8* mac_addr, u8 pkt_rssi);
 *  Parameter: 	* mac_addr -  The advertising packet mac address.
				* pkt_rssi -  The advertising packet.
   Return:	*  packet rssi filter from filter system.
            *  0x80  --  filter buffer full
*/
int rssi_handleProc(u8* mac_addr, u8 pkt_rssi){
	// get rssi filter buffer + 1
	u8 rssi_buf_ptr = rssi_getBufPtr(mac_addr);

	if(rssi_buf_ptr){  // get buffer num success
		return rssi_filter(pkt_rssi , &(detetctor_filter_system.rssi_param[rssi_buf_ptr-1]));
	}else{
		return 0x80;
	}

}
/*
 * Function: filter system initial
 * Interface: rssi_filtSysInit();
 * Brief: Called before filter. must.
 * */
void rssi_filtSysInit(){

	detetctor_filter_system.idle_buf_cnt = FILTER_MAX_CNT;

}

//rssi
unsigned char detector_rssi_filter(unsigned char tmp_rssi){
    
	#define RSSI_NUM 5
	
	//unsigned char tmp_rssi = pRfRxBuff[4];////get the RSSI value of this packet
	
	static unsigned char store_full_flag = 0,filter_first_flag = 0;
	static unsigned char store_rssi_value[RSSI_NUM] = {0};
	static unsigned char sort_rssi_value[RSSI_NUM] = {0};
	
	static unsigned int rssi_filter = 0,mid_cal_data = 0;
	static unsigned char rssi_index = 0;
	
	if((rssi_index < RSSI_NUM-1)&& (!store_full_flag)){
		store_rssi_value[rssi_index] = tmp_rssi;
		mid_cal_data = tmp_rssi;
	}
	else{
		store_full_flag = 1;////receive 5 packets' flag
		store_rssi_value[rssi_index] = tmp_rssi;
		unsigned char copy_index = 0;
		
		for(copy_index = 0;copy_index<RSSI_NUM;copy_index++){
			sort_rssi_value[copy_index] = store_rssi_value[copy_index];
		}
		unsigned char i = 0,j = 0;
		unsigned char tmp_sort = 0;
		for(i=0;i<RSSI_NUM-1;i++){
			for(j=0;j<RSSI_NUM-1-i;j++){
				if(sort_rssi_value[j] > sort_rssi_value[j+1]){
					tmp_sort = sort_rssi_value[j];
					sort_rssi_value[j] = sort_rssi_value[j+1];
					sort_rssi_value[j+1] = tmp_sort;
				}
			}
		}
		mid_cal_data = sort_rssi_value[RSSI_NUM>>1];
	}

	if(!filter_first_flag){/////filter_first_flag == 0
		filter_first_flag = 1;
		rssi_filter = mid_cal_data;
	}
	else{////filter_first_flag ==  1
		rssi_filter = (3*rssi_filter + 1*mid_cal_data + 2)>>2;////+2 to rounding  +2/4
	}
	
	rssi_index++;
	rssi_index %= RSSI_NUM;////from the header restore data
	return rssi_filter;
}

u8 passivescan_cb(int mask,u8*data, u8 chip_rssi){
  
    // mac 0xC51211E2E1EF for test  
    if(data[2]== 0xEF && data[3]== 0xE1){
        printf("%d, ", beacon_count++);

        //TODO : filter here
        u8 rssi = RSSI_OFFSET-chip_rssi;    
        printf("chip rssi %d, ", rssi);
        printf("filtered %d, ", detector_rssi_filter(rssi));
        

        printf("mac: ");
        for (int i=2; i<8; i++)    printf("%x ", data[i]);
        printf("\n");
    }
    
    return PASSIVE_SCAN_CONTINUE;
}

u8 passivescan_filter_cb(int mask,u8*data, u8 chip_rssi){
    
	return 0;
	u8 filter_rssi = rssi_handleProc( (u8*)(data+2) , chip_rssi);

	// mac 0xC51211E2E1EF for test
    if(data[2] == 0xEF)
	{
		printf("rssi %d, ", 110 - chip_rssi);
		printf("filtered %d, ", 110 - filter_rssi);

		printf("mac: ");
		for (int i=2; i<8; i++)    printf("%x ", data[i]);
		printf("chn: %d", mask);
		printf("\n");
	}
    return PASSIVE_SCAN_CONTINUE;
}
#endif


//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////
int app_event_callback (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;

	if (h &HCI_FLAG_EVENT_BT_STD)		//ble controller hci event
	{
		u8 evtCode = h & 0xff;
		if(evtCode == HCI_EVT_LE_META)
		{
			u8 subEvt_code = p[0];
			if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{
				//after controller is set to scan state, it will report all the adv packet it received by this event
				event_adv_report_t *pa = (event_adv_report_t *)p;
				//u8 rssi = RSSI_OFFSET-pa->data[pa->len];	
				//printf("chip rssi %d, ", rssi);
				//printf("filtered %d, ", detector_rssi_filter(rssi));
				if(pa->mac[0] == 0x11 && pa->mac[1] == 0x11){
					beacon_count++;
					static s8 beacon_rssi = 0;
					beacon_rssi = RSSI_OFFSET - pa->data[pa->len];
				}
			}
		}
	}

	return 0;
}

void user_init()
{
	sleep_us(1000*1000);
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value
	////////////////// BLE slave initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        tbl_mac[0] = (u8)rand();
        flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
    }

	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initScanning_module(tbl_mac); 	//scan module: 		 mandatory for BLE master,
	blc_ll_initInitiating_module();			//initiate module: 	 mandatory for BLE master,
	//blc_ll_initMasterRoleSingleConn_module();			//master module: 	 mandatory for BLE master,
	//// controller hci event mask config ////
	//bluetooth event
	//blc_hci_setEventMask_cmd (HCI_EVT_MASK_DISCONNECTION_COMPLETE | HCI_EVT_MASK_ENCRYPTION_CHANGE);
	//bluetooth low energy(LE) event
	blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT);

	rf_set_power_level_index (RF_POWER_8dBm);

	bls_pm_setSuspendMask(SUSPEND_DISABLE);


	/////////////////////////////////////////////////////////////////
	extern void my_att_init ();
	my_att_init ();

	#if __PROJECT_BEACON_DETECTOR_8267__
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);
	#endif

	///////////////////////////State Init//////////////////////////////////
	//blc_l2cap_register_handler (app_l2cap_handler);  //controller data to host(l2cap data) all processed in this func
	blc_hci_registerControllerEventHandler(app_event_callback); //controller hci event to host all processed in this func

	/*To use timer1, register timer1 callback function here*/
	//hw_timer1_start(2000, timerCb);


    ///////////// 4. rf driver agc mode setting, must, in detector ////////////////////
	blt_gain_chg (0); // 0 for agc mode

	////////////  5. if use filter ,rssi_filtSysInit, ////////////////////////////////
	//rssi_filtSysInit();

	blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS,
							  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, 0);


#if UART_ENABLE
    //Initial IO
    //UART_GPIO_CFG_PA6_PA7();
    //UART_GPIO_CFG_PC2_PC3();
    UART_GPIO_CFG_PB2_PB3();
    
    CLK32M_UART115200;
    uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
#endif
    
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 A_loop = 0;
	A_loop++;
	//POWER_ON_64MHZ_CLK;
	blt_sdk_main_loop();
}

