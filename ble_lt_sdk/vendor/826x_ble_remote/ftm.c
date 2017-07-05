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
static u8 tx_firmware_ver = 0;

extern u8 user_key_mode;
extern u8 ota_is_working;
extern u8 sendTerminate_before_enterDeep;
extern const u8 kb_map_ir[49];

extern void ir_dispatch(u8 type, u8 syscode ,u8 ircode);

static u8 g_firmware_ver[2];

static void uei_get_firmware_ver()
{
    /*
     * Get firmware version from flash
     */
    u32 ver_addr = 0x02;
    flash_read_page(ver_addr, sizeof(g_firmware_ver), g_firmware_ver);
}

static void uei_ftm_send_version()
{
    static u8 ver_idx = 0;

    if (!tx_firmware_ver)
        return;

    /*
     * ir is busy, wait for the next loop
     */
    extern int ir_is_sending();
    if (ir_is_sending())
        return;

    gpio_write(GPIO_LED, 1);

    ir_dispatch(TYPE_IR_SEND, 0x00, g_firmware_ver[ver_idx++]);

    if (ver_idx < ARRAY_SIZE(g_firmware_ver))
        return;

    ver_idx = 0;
    tx_firmware_ver = 0;
    gpio_write(GPIO_LED, 0);
}

static void uei_ftm_reset_factory()
{
#define UEI_FLASH_SECTOR_SIZE    (0x1000)
    u32 start = 0x40000, end = 0x76000;

    wd_stop();
    /*
     * 0x40000 ~ 0x73FFF: User Data Area
     * 0x74000 ~ 0x75FFF: Pair & Sec Info
     * 0x76000 ~ 0x76FFF: MAC Address
     * 0x77000 ~ 0x77FFF: Customed value for calibrate, can't erase.
     * 0x78000 ~ 0x7FFFF: User Data Area
     */
    for (; start < end; start += UEI_FLASH_SECTOR_SIZE)
        flash_erase_sector(start);
    start = 0x78000;
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
#define FTM_STUCK_TIMEOUT        ((u32)(30000000))

    static u32 last_time = 0;
    static u32 stuck_time = 0;
    static u8 key_released = 1;

    if (!last_time) {
        /*
         * indicate power or battery on
         */
        last_time = clock_time();
        device_led_setup(ftm_led[LED_FTM_BATTERY_ON]);
    }

    /*
     * send version to master
     */
    uei_ftm_send_version();

    /*
     * the maximue interval between two key is 30s
     * timeout, reset context of FTM
     */
    if (uei_ftm_enter > 0 &&
        clock_time_exceed(last_time, FTM_INTERVAL_TIMEOUT))
        goto FTM_FAIL;

    if (!kb_data) {
        key_released = 1;
        return;
    }

    u8 cnt = kb_data->cnt;

    if (key_released)
        stuck_time = clock_time();
    key_released = 0;

    /*
     * the maximue timeout of stuck key is 30s
     * if it occurs, push system to deepsleep
     */
    if (clock_time_exceed(stuck_time, FTM_STUCK_TIMEOUT)) {
        sendTerminate_before_enterDeep = 1;
        ota_is_working = 0;
        bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
        return;
    }

    do {
        if (uei_ftm_enter)
            break;
        // Press two keys simultaneously
        if (cnt != 2)
            break;
        u8 key0 = kb_data->keycode[0];
        u8 key1 = kb_data->keycode[1];
        key0 = kb_map_ir[key0];
        key1 = kb_map_ir[key1];
        if (key0 != VK_1 || key1 != VK_3)
            break;
        /*
         * if timeout occurs, there is no possiblity
         * to enter FTM without re-power on
         */
        if (clock_time_exceed(last_time, FTM_ENTER_TIMEOUT))
            goto FTM_FAIL;
        uei_ftm_enter = 1;
        device_led_setup(ftm_led[LED_FTM_ENTER]);

        tx_firmware_ver = 1;
        uei_get_firmware_ver();

        /*
         * IR and BLE can work together
         * FTM needs to work with IR,
         * so we switch BLE to idle state
         */
        if (user_key_mode != KEY_MODE_IR) {
            user_key_mode = KEY_MODE_IR;
            if (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) {
                bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
            } else {
                bls_ll_setAdvEnable(0);  //switch to idle state
            }
            ota_is_working = 0;
        }
        /*
         * clear all the data, and reset to factory setting
         */
        uei_ftm_reset_factory();

        // send IR data with software version number
        uei_ftm_send_version();
    } while (0);

    last_time = clock_time();

    if (!uei_ftm_enter || cnt != 1)
        return;

    gpio_write(GPIO_LED, 1);
    u8 key = kb_data->keycode[0];
    key = kb_map_ir[key];

    gpio_write(GPIO_LED, 1);
    ir_dispatch(TYPE_IR_SEND, 0x00, key);
    gpio_write(GPIO_LED, 0);

    return;

FTM_FAIL:
    uei_ftm_enter = 0;
    last_time = 0;
    stuck_time = 0;
    key_released = 1;
    gpio_write(GPIO_LED, 0);
    return;
}

#endif
