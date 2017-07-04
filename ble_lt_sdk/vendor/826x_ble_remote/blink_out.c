/*
 * Blink Outs Test Case
 *
 *  Created on: 2017-6-30
 *      Author: Telink
 */

#include "uei.h"

#if UEI_CASE_OPEN

#include "../../proj/tl_common.h"
#include "../../proj/common/string.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/blt_led.h"

#define UEI_STA_CODE      "uei_sta_code(0.00)2345"
#define UEI_TV_CODE       "uei_tv_code(1.20)4325"
#define DIGITAL(c)        ((c) >= '0' && (c) <= '9')

enum {
    LED_BLINK_SUCCESS = 0,
    LED_BLINK_OUTS = 0,
    LED_BLINK_FAILURE,
};

const led_cfg_t blink_led[] = {
        {250,     250,    2,      0x04,  },    // Success
        {1000,    1000,   1,      0x04,  },    // For Failure of tests
};

led_cfg_t mode_led[] = {
        {500,     500,    0,      0x04,  },    // For Blink outs
};

static u8 * uei_get_mode(const void *code_str, size_t len)
{
#define ENTER_BRACKET(c)  ((c) == '(' || (c) == '[' || (c) == '{')
#define EXIT_BRACKET(c)   ((c) == ')' || (c) == ']' || (c) == '}')
    size_t i, bracket = 0;
    u8 *code = (u8 *)code_str;

    if (!code_str)
        return NULL;

    /*
     * the mode match ignore the bracket
     * treat bracket
     */
    for (i = 0; i < len; i ++) {
        if (ENTER_BRACKET(code[i])) {
            bracket ++;
        } else if (EXIT_BRACKET(code[i])) {
            if (bracket)
                bracket --;
        } else if (DIGITAL(code[i])) {
            if (bracket == 0)
                return code + i;
        }
    }

    return NULL;
}

static u8 uei_get_mode_val(const void *mode_str, size_t idx)
{
    u8 *mode = (u8 *)mode_str;

    if (!mode_str)
        return 0;

    if (!DIGITAL(mode[idx]))
        return 0;

    return mode[idx] - '0';
}

void uei_blink_out(const kb_data_t *kb_data)
{
#define BLINK_INTERVAL_TIMEOUT    ((u32)(10000000))
    static u8 step = 0;
    static u32 last_time = 0;
    static u8 *code_str = NULL;

    const u8 cmd_list[10] = {VK_W_MUTE, VK_ENTER, VK_9, VK_9, VK_0, VK_0,
            VK_1, VK_2, VK_3, VK_4};
    const u8 code_idx = 5;
    const u8 code_list[] = {VK_0, VK_1, VK_3};
    const u8 blink_idx[] = {2, 5, 6, 7, 8, 9, 10};

    u8 blink_repeat = 0, blink = 0;
    u8 i;

    /*
     * timeout for blink out
     */
    if (step > 0 &&
        clock_time_exceed(last_time, BLINK_INTERVAL_TIMEOUT))
        goto BLINK_OUT_FAIL;

    if (!kb_data)
        return;

    /*
     * skip release key and multiple keys
     */
    if (kb_data->cnt != 1)
        return;

    u8 key = kb_data->keycode[0];

    last_time = clock_time();

    /*
     * it's just for blink out case
     */
    if (step == 0 && key != cmd_list[step])
        return;

    if (step == code_idx) {
        /*
         * it's ready to process mode string
         */
        for (i = 0; i < ARRAY_SIZE(code_list); i ++) {
            if (code_list[i] != key)
                continue;
            code_str = (u8 *)(key == VK_0 ? UEI_STA_CODE : UEI_TV_CODE);
            break;
        }
        if (!code_str)
            goto BLINK_OUT_FAIL;
    } else if (key != cmd_list[step]) {
        goto BLINK_OUT_FAIL;
    }

    if (step > code_idx) {
        /*
         * Get mode string from code string
         */
        u8 *mode_str = uei_get_mode(code_str, strlen((const char *)code_str));
        blink_repeat = uei_get_mode_val(mode_str, step - code_idx - 1);
    }

    step ++;

    /*
     * Process the step to indicate user with blinking LED
     */
    for (i = 0; i < ARRAY_SIZE(blink_idx); i ++) {
        if (step < blink_idx[i])
            break;
        if (step == blink_idx[i]) {
            blink = 1;
            break;
        }
    }

    if (!blink)  // No need to blink
        return;

    if (blink_repeat) {
        /*
         * use the blink count from mode string
         */
        led_cfg_t *led = (led_cfg_t *)(mode_led + LED_BLINK_OUTS);
        led->repeatCount = blink_repeat;
    }
    device_led_setup(blink_repeat ? mode_led[LED_BLINK_OUTS] : blink_led[LED_BLINK_SUCCESS]);

    /*
     * reset context of blink out when it is finish
     */
    if (step >= ARRAY_SIZE(cmd_list)) {
        step = 0;
        code_str = NULL;
    }

    return;

BLINK_OUT_FAIL:
    device_led_setup(blink_led[LED_BLINK_FAILURE]);
    code_str = 0;
    step = 0;
    return;
}

#endif
