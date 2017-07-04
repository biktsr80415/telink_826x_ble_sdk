/*
 * Factory Test Mode
 *
 *  Created on: 2017-6-30
 *      Author: Telink
 */
#include "uei.h"

#if UEI_CASE_OPEN

#include "../../proj/tl_common.h"
#include "../../proj/common/string.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../common/blt_led.h"

enum {
    LED_FTM_BATTERY_ON = 0,
    LED_FTM_ENTER,
};

const led_cfg_t ftm_led[] = {
        {250,     250,    2,      0x04,  },    // Success
        {250,     250,    4,      0x05,  },    // For Factory Test Mode
};

static u8 uei_ftm_enter = 0;

static void uei_ftm_reset_factory()
{
#define UEI_FLASH_SECTOR_SIZE    (0x1000)
    u32 start = 0x40000, end = 0x76000;

    wd_stop();
    /*
     * 0x40000 ~ 0x73FFF: User Data Area
     * 0x74000 ~ 0x75FFF: Pair & Sec Info
     * 0x76000 ~ 0x76FFF: MAC Address
     * 0x77000 ~ 0x77FFF: Customed value
     * 0x78000 ~ 0x7FFFF: User Data Area
     */
    for (; start < end; start += UEI_FLASH_SECTOR_SIZE)
        flash_erase_sector(start);
    start = 0x77000;
    end = 0x80000;
    for (; start < end; start += UEI_FLASH_SECTOR_SIZE)
        flash_erase_sector(start);
    wd_start();

    if (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN)
        bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
}

u8 uei_ftm_entered()
{
	return uei_ftm_enter != 0;
}

void uei_ftm(const kb_data_t *kb_data)
{
#define FTM_ENTER_TIMEOUT        ((u32)(6000000))
#define FTM_INTERVAL_TIMEOUT     ((u32)(30000000))

    static u32 last_time = 0;

    if (!last_time) {
        /*
         * indicate power or battery on
         */
        last_time = clock_time();
        device_led_setup(ftm_led[LED_FTM_BATTERY_ON]);
    }

    /*
     * the maximue interval between two key is 30s
     * timeout, reset context of FTM
     */
    if (uei_ftm_enter > 0 &&
        clock_time_exceed(last_time, FTM_INTERVAL_TIMEOUT))
        goto FTM_FAIL;

    if (!kb_data)
        return;

    u8 cnt = kb_data->cnt;

    do {
        if (uei_ftm_enter)
            break;
        // Press two keys simultaneously
        if (cnt != 2)
            return;
        if (kb_data->keycode[0] != VK_1 ||
            kb_data->keycode[1] != VK_3)
            return;
        /*
         * if timeout occurs, there is no possiblity
         * to enter FTM without re-power on
         */
        if (clock_time_exceed(last_time, FTM_ENTER_TIMEOUT))
            return;
        uei_ftm_enter = 1;
        device_led_setup(ftm_led[LED_FTM_ENTER]);
        last_time = clock_time();
    } while (0);

    /*
     * clear all the data, and reset to factory setting
     */
    uei_ftm_reset_factory();

    // send IR data with software version number
    gpio_write(GPIO_LED, 1);
    // ir_send(...);
    return;

FTM_FAIL:
    uei_ftm_enter = 0;
    last_time = 0;
    return;
}

#endif
