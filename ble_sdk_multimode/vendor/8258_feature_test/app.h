#ifndef _APP_H
#define _APP_H

#include "tl_common.h"
#include "drivers.h"

void user_init_normal(void);
void user_init_deepRetn(void);
void main_loop (void);


void feature_linklayer_state_test_init_normal(void);
void feature_linklayer_state_test_init_deepRetn(void);


void feature_adv_power_test_init_normal(void);
void feature_adv_power_test_init_deepRetn(void);


void feature_security_test_init_normal(void);
void feature_security_test_init_deepRetn(void);


void feature_soft_timer_test_init_normal(void);
void feature_soft_timer_test_init_deepRetn(void);


void feature_whitelist_test_init_normal(void);
void feature_whitelist_test_init_deepRetn(void);


void feature_phytest_init_normal(void);
void feature_phytest_irq_proc(void);













#endif /* APP_H_ */
