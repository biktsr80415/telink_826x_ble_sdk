/*
 * app_pair.h
 *
 *  Created on: 2017-6-1
 *      Author: Administrator
 */

#ifndef BLM_PAIR_H_
#define BLM_PAIR_H_




#if (!BLE_HOST_SMP_ENABLE)

	typedef struct{
		u8 manual_pair;
		u8 mac_type;  //addrsss type
		u8 mac[6];
		u32 pair_tick;
	}man_pair_t;

	extern man_pair_t blm_manPair;


	void user_master_host_pairing_management_init(void);
	void user_tbl_salve_mac_unpair_proc(void);
	int user_tbl_slave_mac_search(u8 adr_type, u8 * adr);
	int user_tbl_slave_mac_add(u8 adr_type, u8 *adr);
	int user_tbl_slave_mac_delete_by_adr(u8 adr_type, u8 *adr);

#endif


void app_setCurrentReadReq_attHandle(u16 handle);

void host_pair_unpair_proc(void);



extern int	dongle_pairing_enable;
extern int dongle_unpair_enable;



#endif /* APP_PAIR_H_ */
