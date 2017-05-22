#ifndef _MOUSE_INFO_H_
#define _MOUSE_INFO_H_


#ifndef DEVICE_INFO_STORE
#define DEVICE_INFO_STORE   1
#endif

typedef struct{
	u8 	mode;
	u8	sensor;

#if 1
	u16 rsvd;
	u32 dongle_id;

#else
	u16 dongle_id;
#endif

} device_info_t;

#define INFO_SENSOR_CPI_CTRL    0x0f
#define INFO_SENSOR_STATUS_CTRL 0xf0

//void device_info_load(mouse_status_t *mouse_status);
#if DEVICE_INFO_STORE
//void device_info_save(mouse_status_t *mouse_status, u32 sleep_save);
#else
static inline void device_info_save(mouse_status_t *mouse_status, u32 sleep_save) {}
#endif

#endif
