/*
 * uei.h
 *
 *  Created on: 2017-7-3
 *      Author: Telink
 */

#ifndef UEI_H_
#define UEI_H_

#include "app_config.h"

#if UEI_CASE_OPEN

#include "../../proj/tl_common.h"
#include "../../proj/drivers/keyboard.h"

extern u8 uei_ftm_entered();
extern void uei_ftm(const kb_data_t *kb_data);
extern void uei_blink_out(const kb_data_t *kb_data);

#endif

#endif /* UEI_H_ */
