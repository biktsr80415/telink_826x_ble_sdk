/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision$
 */

/** @file 
 *
 * @defgroup drivers_mouse_sensor
 * @{
 * @ingroup nrfready_drivers
 * @brief Mouse sensor driver.
 *
 * @details This header defines prototypes for mouse sensor driver functions.
 */
#ifndef __DRV_MOUSE_H__
#define __DRV_MOUSE_H__

#include "../../proj/tl_common.h"
#include "../../proj/common/types.h"
#include "mouse_spi.h"

typedef struct
{
	u8 motion;
	u16 deltaX;
	u16 deltaY;
	u8 squal;
	u8 shutterhi;
	u8 shutterlo;
	u8 pixMax;
	u8 pixAccum;
} burst_data;

//u8 sensor_cpi[] = {0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xe0}; /*{800, 1200(default), 1600, 2000, 2400, 2800, 3200}*/

/**@brief Mouse driver initialization.
 *
 * @return
 * @retval NRF_SUCCESS
 * @retval NRF_ERROR_x
 */

u8 sensor_spi_page(void);
void drv_mouse_sensor_mod_cpi(void);
void smart_enable(bool m_smart_control);
void sensor_write_operation_control(bool m_sensor_control);

//void sensor_config(void);


u32 drv_mouse_motion_report(s8 *pBuf, u8 no_flg );
u32 drv_mouse_sensor_init(void);



//uint32_t drv_mouse_sensor_shutter_read(int8_t* shutter_lower, int8_t* shutter_higher);
//u32 drv_mouse_sensor_shutter_read(u8* shutter_lower, u8* shutter_higher, int8_t* squal);
/**@brief Read movement deltas.
 *
 * @param[out] delta_x
 * @param[out] delta_y
 * @return
 * @retval NRF_SUCCESS
 */
//u32 drv_mouse_sensor_read(u16* delta_x, u16* delta_y);

/**@brief Burst read movement deltas.
 *
 * @param[out] bustData
 */
//void drv_motion_burst_read(burst_data *burstData);

/**@brief Get interrupt line status
 *
 * @return true if sensor has set an interrupt line
 */
bool drv_mouse_sensor_int_get(void);

/**@brief Enable pin sense for mouse sensor interrupt pin
 *
 * @param[in] enable if true, enable pin sense
 */
void drv_mouse_sensor_sense_enable(bool enable);

/**@brief Preform sensor readout, but discard data.
 */
void drv_mouse_sensor_dummy_read(void);

/**@brief Prepare for sleep and subsequent wakeup.
 */
bool drv_mouse_sensor_wakeup_prepare(void);



#endif /* __DRV_MOUSE_H__ */

/** @} */
