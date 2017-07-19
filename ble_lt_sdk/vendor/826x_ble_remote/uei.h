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

#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#endif
#define ARRAY_SIZE(a)    (sizeof((a)) / sizeof((a[0])))

#define UEI_IR_STUCK_MAX_TIME_US   (30000000)

extern void uei_ir_pm(void);
extern u8 ir_learning(void);
extern u8 uei_ftm_entered(void);
extern u8 uei_stuck_key_check(void);
extern void uei_ftm(const kb_data_t *kb_data);
extern void ir_learn(const kb_data_t *kb_data);
extern void uei_blink_out(const kb_data_t *kb_data);

#endif

#endif /* UEI_H_ */
