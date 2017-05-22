#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"
#include "mouse_info.h"
#include "mouse.h"

#define			PM_REG_START		0x19
#define			PM_REG_END			0x1f

#ifndef PM_POWERON_DETECTION_ENABLE
#define PM_POWERON_DETECTION_ENABLE 0
#endif


extern int SysMode;
static device_info_t device_info;

/*
 *  Base on from power on or deep sleep back
 *  Load customizable information from the 3.3V Analog register
 *  or Load from OTP
 *
 */
void device_info_load(mouse_status_t *mouse_status)
{
#if DEVICE_INFO_STORE
    u8 * pd = (u8 *) &device_info;

#if 0
    for (int i=DEEP_ANA_REG1; i<=DEEP_ANA_REG4; i++) {
        *pd ++ = analog_read (i);
    }
#else
    for (int i=DEEP_ANA_REG1; i<=DEEP_ANA_REG4; i++) {
        *pd ++ = analog_read (i);
    }
    for (int i=DEEP_ANA_REG5; i<=DEEP_ANA_REG8; i++) {
        *pd ++ = analog_read (i);
    }
#endif

    mouse_status->mouse_mode = device_info.mode & 0x0f;

//   Need get poweron, cpi, etc back first
    if ( mouse_status->mouse_mode == STATE_NORMAL ){
    	mouse_status->cpi = device_info.sensor & INFO_SENSOR_CPI_CTRL;
        mouse_status->mouse_sensor = device_info.sensor & INFO_SENSOR_STATUS_CTRL;
#if 0
    	mouse_status->dongle_id = rf_access_code_16to32(device_info.dongle_id);
        rf_set_access_code1 (mouse_status->dongle_id);
#else
    	mouse_status->dongle_id = device_info.dongle_id;
        rf_set_access_code1 (mouse_status->dongle_id);
#endif
    }
#else
    mouse_status->mouse_mode = device_info.mode ? STATE_NORMAL : STATE_POWERON;
#endif
}

#if DEVICE_INFO_STORE
/*
 * Save the information need from the deep sleep back
 *
 */
void device_info_save(mouse_status_t *mouse_status, u32 sleep_save)
{

    u8 * pd = (u8 *) &device_info;


    device_info.mode = ( (SysMode<<4) | mouse_status->mouse_mode);

    if(SysMode == RF_2M_2P4G_MODE){

    	device_info.sensor = (mouse_status->mouse_sensor & 0xf0) | (mouse_status->cpi & 0x0f) ;

#if 0
    	device_info.dongle_id = rf_access_code_32to16(mouse_status->dongle_id);
    	for (u8 i=DEEP_ANA_REG1; i<=DEEP_ANA_REG4; i++) {
    		analog_write (i, *pd ++);
    	}
#else
    	device_info.dongle_id = mouse_status->dongle_id;
    	for (u8 i=DEEP_ANA_REG1; i<=DEEP_ANA_REG4; i++) {
    		analog_write (i, *pd ++);
    	}

    	for(u8 i=DEEP_ANA_REG5; i<=DEEP_ANA_REG8; i++){
    		analog_write(i, *pd ++);
    	}



#endif

    }
}
#endif



