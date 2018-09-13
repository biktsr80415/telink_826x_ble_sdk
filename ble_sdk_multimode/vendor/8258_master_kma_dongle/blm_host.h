/*
 * app_host.h
 *
 *  Created on: 2018-8-29
 *      Author: Administrator
 */

#ifndef APP_HOST_H_
#define APP_HOST_H_



#define CHAR_HANDLE_MAX					10

// connection character device information
typedef struct
{
	u16 conn_handle;
	u8 conn_state;
	u8 mac_adrType;
	u8 mac_addr[6];
	u8 char_handle[CHAR_HANDLE_MAX];
}dev_char_info_t;

extern dev_char_info_t cur_conn_device;


typedef void (*main_service_t) (void);

extern main_service_t		main_service;


int app_event_callback (u32 h, u8 *p, int n);
int app_l2cap_handler (u16 conn_handle, u8 *raw_pkt);
int app_host_smp_finish (void);

void host_update_conn_proc(void);

extern u32 host_update_conn_param_req;
extern int	app_host_smp_sdp_pending;


#endif /* APP_HOST_H_ */
