/*
 * ll_init.h
 *
 *  Created on: 2017-3-9
 *      Author: Administrator
 */

#ifndef LL_INIT_H_
#define LL_INIT_H_



typedef struct {
	u8		init_en;
	u8		conn_policy;
	u8		conn_advType;

	u8		conn_mac[6];

}st_ll_init_t;

//st_ll_init_t  blti;



extern rf_packet_ll_init_t	pkt_init;




typedef int  (*ll_module_init_callback_t)(u8 *, u32);



/******************************* User Interface  ************************************/
void	 	blc_ll_initInitiating_module(void);


ble_sts_t 	blc_ll_createConnection (u16 scan_interval, u16 scan_window, u8 policy,
							  u8 adr_type, u8 *mac, u8 own_adr_type,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

ble_sts_t 	blc_ll_createConnectionCancel ();





/************************* Stack Interface, user can not use!!! ***************************/
void 	blc_init_sendConnectionCancleEvent(u16 handle);



#endif /* LL_INIT_H_ */
