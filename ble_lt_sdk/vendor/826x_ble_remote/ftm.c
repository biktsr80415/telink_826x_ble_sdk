/*
 * Factory Test Mode
 *
 *  Created on: 2017-6-30
 *      Author: Telink
 */
#include "uei.h"

#if UEI_CASE_OPEN

#include "../../proj_lib/pm.h"
#include "../../proj/tl_common.h"
#include "../../proj/common/string.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../common/blt_led.h"

#define UEI_IR_IDLE_MAX_TIME_US    (60000000)
#define UEI_IR_STUCK_MAX_TIME_US   (30000000)

enum {
    LED_FTM_BATTERY_ON = 0,
    LED_FTM_ENTER,
};

enum {
    FTM_ACTIVE = 0,
    FTM_SUSPEND,
    FTM_DEEPSLEEP,
};

const led_cfg_t ftm_led[] = {
        {250,     250,    2,      0x04,  },    // Success
        {250,     250,    4,      0x05,  },    // For Factory Test Mode
};

static u8 g_tx_fm_ver = 0;
static u8 g_firmware_ver[2];
static u8 g_key_released = 0;
static u8 g_uei_ftm_enter = 0;
static u8 g_ftm_sleep_type = 0;
static u32 g_uei_stuck_tick = 0;
static u32 g_uei_last_ir_tick = 0;

extern u8 user_key_mode;
extern u8 ota_is_working;
extern u8 ir_not_released;
extern u8 lowBatt_alarmFlag;
extern u8 ir_is_repeat_timer_enable;
extern u8 sendTerminate_before_enterDeep;
extern const u8 kb_map_ir[49];

extern void ir_dispatch(u8 type, u8 syscode ,u8 ircode);

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
#define TX_VER_WAIT_TIME           (500000)
#define TX_VER_DATA_INTERVAL       (100000)
#define TX_VER_RELEASE_INTERVAL    (120000)
    static u8 ver_idx = 0;
    static u8 ver_data = 0;
    static u32 tx_ver_time = 0;

    if (!g_tx_fm_ver)
        return;

    if (!tx_ver_time)
        tx_ver_time = clock_time();

    if (DEVICE_LED_BUSY)
        return;

    /*
     * wait for IR is ready
     */
    if (!ver_idx &&
        !clock_time_exceed(tx_ver_time, TX_VER_WAIT_TIME))
        return;

    /*
     * IR is busy, wait for the next loop
     */
    extern int ir_is_sending();
    if (ir_is_sending())
        return;

    if (ver_data) {
        /*
         * send release
         */
        if (!clock_time_exceed(tx_ver_time, TX_VER_RELEASE_INTERVAL))
            return;
        if (!ir_is_repeat_timer_enable)
            return;
        ver_data = 0;
        ir_dispatch(TYPE_IR_RELEASE, 0x00, 0x00);
        tx_ver_time = clock_time();
    } else {
        /*
         * send version data
         */
        if (!clock_time_exceed(tx_ver_time, TX_VER_DATA_INTERVAL))
            return;
        ver_data = 1;
        ir_dispatch(TYPE_IR_SEND, 0x00, g_firmware_ver[ver_idx++]);
        tx_ver_time = clock_time();
        return;
    }

    if (ver_idx < ARRAY_SIZE(g_firmware_ver))
        return;

    if (ver_data != 0)
        return;

    ver_data = 0;
    ver_idx = 0;
    g_tx_fm_ver = 0;
    ir_dispatch(TYPE_IR_RELEASE, 0x00, 0x00);
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
    return g_uei_ftm_enter != 0;
}

static void uei_ftm_pm()
{
    if (g_ftm_sleep_type == FTM_ACTIVE)
        return;

    int src = PM_WAKEUP_PAD;
    int sleep_type = g_ftm_sleep_type - 1;
    g_ftm_sleep_type = FTM_ACTIVE;

    analog_write(DEEP_ANA_REG1, user_key_mode);

    if (sleep_type == SUSPEND_MODE)
        src = PM_WAKEUP_CORE | PM_WAKEUP_TIMER;

    cpu_sleep_wakeup(sleep_type, src, clock_time() + CLOCK_SYS_CLOCK_1S);
}

void uei_ir_pm()
{
    u8 status;
    if (user_key_mode != KEY_MODE_IR)
        return;

    if (!g_tx_fm_ver && !DEVICE_LED_BUSY && !ir_not_released &&
        clock_time_exceed(g_uei_last_ir_tick, 100000))
        g_ftm_sleep_type = FTM_SUSPEND;
    /*
     * the maximue timeout of stuck key is 30s
     * if it occurs, push system to deepsleep
     */
    if (!g_key_released && clock_time_exceed(g_uei_stuck_tick, UEI_IR_STUCK_MAX_TIME_US)) {
        g_ftm_sleep_type = FTM_DEEPSLEEP;
        u32 pin[] = KB_DRIVE_PINS;
        for (u8 i = 0; i < ARRAY_SIZE(pin); i ++) {
            extern u8 stuckKeyPress[];
            if(!stuckKeyPress[i])
                continue;
            cpu_set_gpio_wakeup (pin[i], 0, 1);  // reverse stuck key pad wakeup level
            gpio_set_wakeup(pin[i], 0, 1);       // reverse stuck key pad wakeup level
        }
    }

    if (clock_time_exceed(g_uei_last_ir_tick, UEI_IR_IDLE_MAX_TIME_US))
        g_ftm_sleep_type = FTM_DEEPSLEEP;
    status = g_ftm_sleep_type;
    uei_ftm_pm();
    //if (status != FTM_ACTIVE)
    //    sleep_us(5000);
}

void uei_ftm(const kb_data_t *kb_data)
{
#define FTM_ENTER_TIMEOUT        ((u32)6000000)
#define FTM_INTERVAL_TIMEOUT     ((u32)30000000)
#define FTM_BAT_VOL_LOW          ((u16)2000)

    static u32 last_time = 0;
    static u32 power_time = 0;

    if (!power_time) {
        /*
         * indicate power or battery on
         */
        power_time = clock_time();
        g_uei_last_ir_tick = power_time;
        g_key_released = 1;
        device_led_setup(ftm_led[LED_FTM_BATTERY_ON]);
    }

    if (kb_data)
        g_uei_last_ir_tick = clock_time();

    if (kb_data && kb_data->cnt > 0) {
        g_uei_stuck_tick = clock_time();
        g_key_released = 0;
    } else if (kb_data && kb_data->cnt == 0) {
        g_key_released = 1;
    }

    /*
     * send version to master
     */
    uei_ftm_send_version();

    /*
     * the maximue interval between two key is 30s
     * timeout, reset context of FTM
     */
    if (g_uei_ftm_enter > 0 &&
        clock_time_exceed(last_time, FTM_INTERVAL_TIMEOUT)) {
        g_uei_ftm_enter = 0;
        goto FTM_FAIL;
    }

    if (!kb_data)
        return;

    u8 cnt = kb_data->cnt;

    do {
        if (g_uei_ftm_enter)
            break;
        // Press two keys simultaneously
        if (cnt != 2)
            break;
        if (lowBatt_alarmFlag)
            break;
        u8 key0 = kb_data->keycode[0];
        u8 key1 = kb_data->keycode[1];
        key0 = kb_map_ir[key0];
        key1 = kb_map_ir[key1];
        if (key0 != IR_VK_1 || key1 != IR_VK_3)
            break;
        /*
         * if timeout occurs, there is no possiblity
         * to enter FTM without re-power on
         */
        if (clock_time_exceed(power_time, FTM_ENTER_TIMEOUT))
            goto FTM_FAIL;

        g_uei_ftm_enter = 1;
        device_led_setup(ftm_led[LED_FTM_ENTER]);

        g_tx_fm_ver = 1;
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
        last_time = clock_time();
        /*
         * clear all the data, and reset to factory setting
         */
        uei_ftm_reset_factory();

        // send IR data with software version number
        uei_ftm_send_version();
        return;
    } while (0);

    last_time = clock_time();

    if (!g_uei_ftm_enter)
        return;

    gpio_write(GPIO_LED, 1);
    u8 key = 0;  // release key
    if (cnt > 0) {
        key = kb_data->keycode[0];
        key = kb_map_ir[key];
        ir_not_released = 1;
        ir_dispatch(TYPE_IR_SEND, 0x00, key);
    } else {
        ir_not_released = 0;
        ir_dispatch(TYPE_IR_RELEASE, 0x00, key);
    }
    gpio_write(GPIO_LED, 0);

    return;

FTM_FAIL:
    last_time = 0;
    gpio_write(GPIO_LED, 0);
    return;
}

#endif
