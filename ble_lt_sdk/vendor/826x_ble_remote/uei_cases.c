/*
 * uei_cases.c
 *
 *  Created on: 2017-5-23
 *      Author: Administrator
 */

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#include "app_config.h"
#include "../common/blt_led.h"
#include "../../proj/drivers/adc.h"
#include "../../proj/drivers/spi.h"
#include "../../proj/drivers/i2c.h"
#include "../../proj/drivers/uart.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj/mcu/gpio.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"

#define UEI_CASE_START_CODE                   (VK_ENTER)
#define UEI_CASE_MENU_STEP_0_START_CODE       (VK_5)
#define UEI_CASE_MENU_STEP_1_START_CODE       (VK_1)
#define UEI_CASE_MENU_STEP_2_START_CODE       (VK_1)
#define UEI_CASE_MENU_END_CODE                (VK_0)
#define UEI_CASE_MENU_STEP_BASE_CODE          (VK_Z)
#define UEI_CASE_SUB_MEMU_BASE_CODE           (VK_Z)
#define UEI_CASE_GPIO_PORT_IDX_MAX            (5)
#define ARRAY_ELEM_CNT(array)                 (sizeof(array) / sizeof(array[0]))
#define UEI_CASE_CNT                          (ARRAY_ELEM_CNT(uei_case))
#define UEI_CASE_GPIO_CNT                     (ARRAY_ELEM_CNT(gpio_array))
#define UEI_WD_FLG_NONE                       (0x00)
#define UEI_WD_FLG_RST                        (0x01)
#define UEI_WD_FLG_DEEPSLEEP                  (0x02)
#define UEI_CASE_IDX_INVALID                  (-1)
#define UEI_CASE_TIMER_IDX_MAX                (2)
#define UEI_CASE_TIMER_CAPTURE_DEFAULT        (1000)
#define UEI_CASE_TIMER_GPIO_TRIGGER_CAP       (100)
#define UEI_CASE_FORCE_SUSPEND_TIME_WAKEUP    (BIT(0))
#define UEI_CASE_FORCE_SUSPEND_GPIO_WAKEUP    (BIT(1))
#define UEI_CASE_FORCE_SUSPEND                (BITS2(0, 1))
#define UEI_CASE_FORCE_SUSPEND_INTERVAL       (120 * CLOCK_SYS_CLOCK_1S)
#define UEI_CASE_FORCE_DEEPSLEEP              (BIT(2))
#define UEI_CASE_I2C_SLAVE_ID                 (0x5C)
#define UEI_CASE_SPI_CS_PIN                   (GPIO_PA5)
#define UEI_CASE_GPIO_INDICATE                (GPIO_PA0)
#define UEI_CASE_DEBUG_OPEN                   (0)
#define UEI_CASE_SERIAL_SLAVE_ADDR_LEN        (2)
#define UEI_CASE_SERIAL_SLAVE_ADDR_HIGH       (0x80)
#define UEI_CASE_SERIAL_SLAVE_ADDR_LOW        (0x00)
#define UEI_CASE_SERIAL_SLAVE_ADDR            ((UEI_CASE_SERIAL_SLAVE_ADDR_HIGH << 8) | UEI_CASE_SERIAL_SLAVE_ADDR_LOW)
#define UEI_CASE_SWIRE_SLAVE_ADDR             (UEI_CASE_SERIAL_SLAVE_ADDR)
#define UEI_CASE_SWIRE_SLAVE_READY_ADDR       (UEI_CASE_SERIAL_SLAVE_ADDR + 0x08)
#define UEI_CASE_SWIRE_SLAVE_WR_READY_VAL     (0xAA)
#define UEI_CASE_SWIRE_SLAVE_RD_READY_VAL     (0x55)

typedef char (* uei_case_func_t)();
typedef struct uei_case_menu_range {
    u8 start;  // start or minimum index of menu index
    u8 end;    // end or maximum index of menu index
} uei_case_menu_range_t;
typedef uei_case_menu_range_t uei_gpio_range_t;

typedef struct uei_cmd_meta {
    u8 cmd_cnt;  // command count
    u8 *cmd;     // command buffer
} uei_cmd_meta_t;

extern const u8 DRIVE_PINS_CNT;
extern const u8 SCAN_PINS_CNT;
extern u32 drive_pins[];  // pins used for keyboard, work with input for forever
extern u32 scan_pins[];   // pins used for keyboard, work with input for forever

/************************************************************************************
 ***************** UEI TEST CASE LISTS AND CASE MENU ********************************
 ************************************************************************************/
char uei_case_gpio_input();          // case index 0 (OK+500)
char uei_case_gpio_output_level();   // case index 1 (OK+501)
char uei_case_watdog();              // case index 2 (OK+502)
char uei_case_timer();               // case index 3 (OK+503)
char uei_case_mode();                // case index 4 (OK+504)
char uei_case_flash();               // case index 5 (OK+505)
char uei_case_uart();                // case index 6 (OK+506)
char uei_case_i2c();                 // case index 7 (OK+507)
char uei_case_spi();                 // case index 8 (OK+508)
char uei_case_adc();                 // case index 9 (OK+509)
char uei_case_single_wire();         // case index A (OK+510)
char uei_case_quadrature_decoder();  // case index B (OK+511)
char uei_case_remote_firmware();     // case index C (OK+512)

static void uei_case_timer_set_fhs(u8 fhs_idx);
static void uei_case_timer_set_sclk(u8 sclk_idx);
static void uei_case_force_sleep(u8 type);

const static uei_case_func_t uei_case[] = {
    uei_case_gpio_input,       uei_case_gpio_output_level,      uei_case_watdog,
    uei_case_timer,            uei_case_mode,                   uei_case_flash,
    uei_case_uart,             uei_case_i2c,                    uei_case_spi,
    uei_case_adc,              uei_case_single_wire,            uei_case_quadrature_decoder,
    uei_case_remote_firmware,  /*add new case function entrance here*/
};

const static struct uei_case_menu_range uei_menu_range[] = {
    /*   MIN                                 MAX        */
    {UEI_CASE_START_CODE,             UEI_CASE_START_CODE},
    {UEI_CASE_MENU_STEP_0_START_CODE, UEI_CASE_MENU_STEP_0_START_CODE},
    {UEI_CASE_MENU_STEP_1_START_CODE, UEI_CASE_MENU_END_CODE},
    {UEI_CASE_MENU_STEP_2_START_CODE, UEI_CASE_MENU_END_CODE}
};
/************************************************************************************/

extern void gpio_setup_up_down_resistor(u32 gpio, u32 up_down);
extern void swire_write (unsigned short adr, unsigned char *ptr, int cnt);
extern void swire_speed(unsigned char data);
extern void reset_swm_for_keyboard();
extern int swire_sync (int usb);

char uei_case_enter();
char uei_case_keyboard_pin(u32 gpio_pin);
char uei_case_manual();
int uei_case_suspend_wakeup_handle();
void uei_case_pm();
void uei_case_irq_handler();

enum {
    UEI_W_ENTER = 0,
    UEI_W_MENU_STEP_0,
    UEI_W_MENU_STEP_1,
    UEI_W_MENU_STEP_2,
    UEI_W_SUB_MENU,
};

enum {
    LED_UEI_SUCCESS = 0,
    LED_UEI_FAIL,
    LED_UEI_SHORT_BLINK,
    LED_UEI_ON,
    LED_UEI_OFF,
    LED_UEI_WD_RST,
    LED_UEI_WARN,
    LED_UEI_TIMER_WAKEUP,
    LED_UEI_GPIO_WAKEUP,
};

enum {
    UEI_UART_IDLE = 0,
    UEI_UART_START = 1,
    UEI_UART_RX_DONE = 2,
};

const static led_cfg_t uei_led_cfg[] = {
  /*OnTime, OffTime, Repeat,   Priority*/
    {200,     200,    0x02,      0x10},    // blink for success
    {600,     600,    0x02,      0x10},    // long blink for failure
    {250,     250,    0x02,      0x10},    // short blink
    {800,     0,      0xFF,      0x10},    // LED ON
    {0,       800,    0xFF,      0x10},    // LED OFF
    {250,     250,    0x03,      0x10},    // WatchDog Reset
    {100,     100,    0x06,      0x10},    // Warning to indicate invalidate GPIO pins
    {250,     250,    0x03,      0x10},    // Reset from Timer wake up
    {250,     250,    0x04,      0x10},    // Reset from GPIO wake up
};

const static u32 gpio_array[] = {
    GPIO_PA0, GPIO_PA1, GPIO_PA2, GPIO_PA3, GPIO_PA4, GPIO_PA5, GPIO_PA6, GPIO_PA7,
    GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7,
    GPIO_PC0, GPIO_PC1, GPIO_PC2, GPIO_PC3, GPIO_PC4, GPIO_PC5, GPIO_PC6, GPIO_PC7,
    GPIO_PD0, GPIO_PD1, GPIO_PD2, GPIO_PD3, GPIO_PD4, GPIO_PD5, GPIO_PD6, GPIO_PD7,
    GPIO_PE0, GPIO_PE1, GPIO_PE2, GPIO_PE3, GPIO_PE4, GPIO_PE5, GPIO_PE6, GPIO_PE7,
    GPIO_PF0, GPIO_PF1
};

static u8 g_uei_case_enter;
static u8 g_uei_force_sleep;
static u8 g_uei_timer_wakeup;
static volatile u8 g_uei_i2c_event;
static volatile u8 g_uei_spi_event;
static volatile u8 g_uei_timer_event;
static volatile u8 g_uei_uart_rx_event;
static u32 g_sys_clock = CLOCK_SYS_CLOCK_HZ;
static s8 g_uei_timer_idx = UEI_CASE_IDX_INVALID;
static s8 g_uei_timer_gpio_idx = UEI_CASE_IDX_INVALID;
static s8 g_uei_timer_mode_idx = UEI_CASE_IDX_INVALID;

void debug_print(void *buf, size_t len)
{
#if UEI_CASE_DEBUG_OPEN
    u32 debug_buf[3] = {0x00000008, 0x00000000, 0x00000000};
    u32 max_len = debug_buf[0];
    if (len > max_len)
        len = max_len;
    memcpy(debug_buf + 1, buf, len);
    debug_buf[0] = len;
    uart_Send_kma((u8 *)debug_buf);
    wd_stop();
    sleep_us(len * 8 * 140);
    wd_start();
    debug_buf[0] = 0x08;
    memset(debug_buf + 1, 0, debug_buf[0]);
#endif
}

void print_hex(u8 val)
{
#if UEI_CASE_DEBUG_OPEN
    debug_print(&val, 1);
#endif
}

_attribute_ram_code_ void uei_case_irq_handler()
{
    do {
        if (g_uei_timer_idx < 0 ||
            g_uei_timer_idx > UEI_CASE_TIMER_IDX_MAX)
            break;
        if (g_uei_timer_mode_idx < 0 ||
            g_uei_timer_mode_idx > CLOCK_MODE_TICK)
            break;
        if (g_uei_timer_mode_idx == CLOCK_MODE_GPIO ||
            g_uei_timer_mode_idx == CLOCK_MODE_WIDTH_GPI) {
            if (g_uei_timer_gpio_idx < 0 ||
                g_uei_timer_gpio_idx > UEI_CASE_GPIO_CNT)
                break;
        }
        /*
         * check the destination timer
         */
        for (u8 timer = 0; timer <= UEI_CASE_TIMER_IDX_MAX; timer ++) {
            timer = timer % (UEI_CASE_TIMER_IDX_MAX + 1);
            if ((clock_get_tmr_status(timer)) == 0)
                continue;
            clock_clr_tmr_irq(timer);  // clear the status bit
            g_uei_timer_event = 1;
            //gpio_toggle(UEI_CASE_GPIO_INDICATE);
            break;
        }
    } while (0);

    do {
        u8 irqS = uart_IRQSourceGet();
        if (!(irqS & FLD_DMA_UART_RX))
            break;
        g_uei_uart_rx_event = UEI_UART_RX_DONE;
        //gpio_toggle(UEI_CASE_GPIO_INDICATE);
    } while (0);

    do {
        I2C_I2CIrqSrcTypeDef irqS = i2c_slave_irq_get();
        if (irqS == 0)
            break;
        i2c_slave_irq_clr(I2C_IRQ_HOST_READ_ONLY);
        //gpio_toggle(UEI_CASE_GPIO_INDICATE);
        g_uei_i2c_event = irqS;
        g_uei_spi_event = irqS;
    } while (0);
}

static void uart_print_init()
{
    u8 bwpc = 13;

    u8 div = g_sys_clock / 115200 / (bwpc + 1);
    uart_Reset();
    uart_io_init(UART_GPIO_8267_PB2_PB3);
    gpio_set_input_en(GPIO_PB2, 1);
    gpio_set_input_en(GPIO_PB3, 1);
    gpio_setup_up_down_resistor(GPIO_PB2, PM_PIN_PULLUP_1M);
    gpio_setup_up_down_resistor(GPIO_PB3, PM_PIN_PULLUP_1M);
    uart_IRQSourceGet();  // clear UART RX/TX status
    uart_Init(div, bwpc, PARITY_NONE, STOP_BIT_ONE);
    uart_DmaModeInit(1,1);
    sleep_us(1000);
}

char uei_case_keyboard_pin(u32 gpio_pin)
{
    u8 i;
    for (i = 0; i < DRIVE_PINS_CNT; i ++) {
        if (drive_pins[i] == gpio_pin)
            return true;
    }

    for (i = 0; i < SCAN_PINS_CNT; i ++) {
        if (scan_pins[i] == gpio_pin)
            return true;
    }

    return false;
}

int uei_case_suspend_wakeup_handle()
{
    if (!(g_uei_force_sleep & UEI_CASE_FORCE_SUSPEND))
        return 0;

    if (g_uei_force_sleep & UEI_CASE_FORCE_SUSPEND_TIME_WAKEUP) {
        device_led_setup(uei_led_cfg[LED_UEI_TIMER_WAKEUP]);
    } else if (g_uei_force_sleep & UEI_CASE_FORCE_SUSPEND_GPIO_WAKEUP) {
        device_led_setup(uei_led_cfg[LED_UEI_GPIO_WAKEUP]);
    }

    return 0;
}

static void uei_case_force_sleep(u8 type)
{
    g_uei_force_sleep = type;
    /*
     * wait GPIO is released
     * otherwise, the board would wakeup imeadiately
     */
    wd_stop();
    sleep_us(500000);
    wd_start();
}

void uei_case_pm()
{
    static u8 led = 0;
    u8 rst_flg, deep_flg, deep_mask, suspend;

    if (DEVICE_LED_BUSY)
        return;

    suspend = 0;
    deep_mask = DEEPSLEEP_ADV;
    rst_flg = analog_read(DEEP_ANA_REG2);
    deep_flg = analog_read(DEEP_ANA_REG0);


    /*
     * clear analog flag
     */
    analog_write(DEEP_ANA_REG2, 0);

    /*
     * user force board to enter DeepSleep mode
     */
    if (g_uei_force_sleep) {
        if (g_uei_force_sleep & UEI_CASE_FORCE_SUSPEND) {
            // for Suspend
            suspend = 1;
        } else {
            g_uei_force_sleep = 0;
        }
        goto FORCE_DEEP;
    }

    if (rst_flg == UEI_WD_FLG_RST) {
        /*
         * blink for WatchDog reset back, double blink
         */
        device_led_setup(uei_led_cfg[LED_UEI_WD_RST]);
        led = 1;
    } else if (led == 0 && UEI_WD_FLG_NONE == deep_flg) {
        /*
         * blink for POR, three blink
         */
        device_led_setup(uei_led_cfg[LED_UEI_SHORT_BLINK]);
        led = 1;
        return;
    } else {
        /*
         * If the board is return from DeepSleep, enter menu selection
         * otherwise, enter DeepSleep to wait for user's selection
         */
        if (deep_flg == CONN_DEEP_FLG || deep_flg == ADV_DEEP_FLG)
            return;
FORCE_DEEP:
        if (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) {
            deep_flg = CONN_DEEP_FLG;
            //deep_mask = suspend ? SUSPEND_CONN : DEEPSLEEP_CONN;
        } else {
            deep_flg = ADV_DEEP_FLG;
            //deep_mask = suspend ? SUSPEND_ADV : DEEPSLEEP_ADV;
        }

        analog_write(DEEP_ANA_REG0, deep_flg);

        u16 source = PM_WAKEUP_PAD;

        if (suspend)
            source = PM_WAKEUP_CORE;

        if (g_uei_timer_wakeup) {
            // setup timer for WakeUp
            source |= PM_WAKEUP_TIMER;
        }

        cpu_sleep_wakeup(suspend ? SUSPEND_MODE : DEEPSLEEP_MODE,
                source, clock_time() + UEI_CASE_FORCE_SUSPEND_INTERVAL);
    }
}

char uei_case_enter()
{
    return g_uei_case_enter > 0;
}

char uei_case_manual()
{
    static u8 led_setup = 0;
    static u8 uei_case_idx = 0;
    static u8 g_uei_fsm_state = UEI_W_ENTER;

    u8 key = kb_event.keycode[0];
    u8 cnt = kb_event.cnt;

    if (g_uei_fsm_state < UEI_W_SUB_MENU) {
        /*
         * we just process one key and skip release key
         */
        if (cnt == 0)
            return 0;
        if (cnt != 1)
            goto UEI_CASE_FAIL;

        /*
         * to select UEI test case
         * if the case index is invalid, reset the FSM state
         */
        if (uei_menu_range[g_uei_fsm_state].start > key)
            goto UEI_CASE_FAIL;
        if (uei_menu_range[g_uei_fsm_state].end < key)
            goto UEI_CASE_FAIL;
        // get case index
        if (g_uei_fsm_state > UEI_W_MENU_STEP_0) {
            uei_case_idx = uei_case_idx * 10 +
                    (key - UEI_CASE_MENU_STEP_BASE_CODE) % 10;
        }
        // move to next step
        g_uei_fsm_state ++;
        return 0;
    }

    /*
     * out of user manual
     */
    if (uei_case_idx >= UEI_CASE_CNT) {
        goto UEI_CASE_FAIL;
    }

    g_uei_case_enter = 1;

    /*
     * blink LED to indicate SUCCESS
     */
    if (!led_setup) {
        device_led_setup(uei_led_cfg[LED_UEI_SUCCESS]);
        led_setup = 1;
    }

    /*
     * wait for the LED Blink finishes
     */
    if (DEVICE_LED_BUSY)
        return 0;

    /*
     * general framework for case parser
     */
    if (uei_case[uei_case_idx] &&
        uei_case[uei_case_idx]() != 0)
        return 0;

    g_uei_case_enter = 0;

    return 0;

UEI_CASE_FAIL:
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    uei_case_idx = 0;
    g_uei_fsm_state = UEI_W_ENTER;
    g_uei_case_enter = 0;
    return -1;
}

char uei_case_gpio_input()
{
    static u8 step = 0;
    static u8 gpio_idx = 0;
    u32 input, output = GPIO_PA1;  // default output
    uei_gpio_range_t range[] = {
        {VK_1, VK_5},  // to index PA, PB, PC, PD, PE
        {VK_1, VK_7},  // to index PX[...]
    };

    u8 warning = 0;
    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    print_hex(0x05);
    print_hex(0x00);

    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto GPIO_INPUT_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[step].start)
        goto GPIO_INPUT_FAIL;
    if (key != VK_0 &&
        key > range[step].end)
        goto GPIO_INPUT_FAIL;

    /*
     * 00~07 for PA0~PA7,   10~17 for PB0~PB7
     * 20~27 for PC0~PC7,   30~37 for PD0~PD7
     * 40~47 for PE0~PE7
     */
    gpio_idx = (gpio_idx << 3) + (key - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;

    step ++;

    if (step < 2)
        return 0;

    /*
     * if the default output is set to input,
     * we select GPIO_PA0 to work as output
     */
    input = gpio_array[gpio_idx];
    if (input == output)
        output = GPIO_PA0;
#if 0
    if (uei_case_keyboard_pin(input) ||
        uei_case_keyboard_pin(output)) {
        warning = 1;
        goto GPIO_INPUT_FAIL;
    }
#endif
    gpio_set_input_en(input, 1);
    gpio_setup_up_down_resistor(input, PM_PIN_UP_DOWN_FLOAT);
    gpio_set_func(input, AS_GPIO);
    gpio_set_func(output, AS_GPIO);
    gpio_set_output_en(output, 1);

    wd_stop();

    while (1) {
        /*
         * move the signal from input pin to output pin
         */
        u32 res = gpio_read(input);
        gpio_write(output, res);
    }

    return 0;

GPIO_INPUT_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(warning ? uei_led_cfg[LED_UEI_WARN] : uei_led_cfg[LED_UEI_FAIL]);
    gpio_idx = 0;
    step = 0;
    return 0;
}

char uei_case_gpio_output_level()
{
    static u8 cmd[2];
    static u8 step = 0;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    uei_gpio_range_t range[] = {
        {VK_1, VK_4},  // to index command
        {VK_1, VK_1},  // sub command index
    };

    u8 i, rst = 0;

    /*
     * command VK_2 needs to process some pins firstly,
     * then wait for later sub command
     * so, VK_2 works as one key command and two key command
     */
    u8 one_key_cmd[] = {VK_1, VK_2, VK_3, VK_4};
    u8 two_key_cmd[] = {VK_0, VK_2, VK_3};

    uei_cmd_meta_t cmd_meta[] = {
        {ARRAY_ELEM_CNT(one_key_cmd), one_key_cmd},
        {ARRAY_ELEM_CNT(two_key_cmd), two_key_cmd}
    };

    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto GPIO_OUTPUT_FAIL;

    print_hex(0x05);
    print_hex(0x01);

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[step].start)
        goto GPIO_OUTPUT_FAIL;
    if (key != VK_0 &&
        key > range[step].end)
        goto GPIO_OUTPUT_FAIL;

    /*
     * if keys of current command are ready, to process command,
     * otherwise, wait for command is ready
     */
    cmd[step] = key;
    for (i = 0; i < cmd_meta[step].cmd_cnt; i ++) {
        if (cmd[0] == cmd_meta[step].cmd[i])  // search command in command-list
            break;
    }
    if (i >= cmd_meta[step ++].cmd_cnt)  // current command is not ready
        return 0;

    switch (cmd[0]) {
    case VK_0:
        if (cmd[1] != VK_0 && cmd[1] != VK_1)
            goto GPIO_OUTPUT_FAIL;
        for (i = 0; i < ARRAY_ELEM_CNT(gpio_array); i ++) {
            // don't modify the pins used for keyboard
            if (uei_case_keyboard_pin(gpio_array[i]))
                continue;
            // cmd[1] == VK_0, Enable 1M Pull Up for all ports
            // cmd[1] == VK_1, Enable 10K Pull Up for all ports
            if (i < 36)
                gpio_set_func(gpio_array[i], AS_GPIO);
            gpio_setup_up_down_resistor(gpio_array[i],
                    cmd[1] == VK_0 ? PM_PIN_PULLUP_1M : PM_PIN_PULLUP_10K);
            wd_clear();
        }
        rst = 1;
        break;
    case VK_1:
        // Enable Pull Down for all ports
        for (i = 0; i < ARRAY_ELEM_CNT(gpio_array); i ++) {
            // don't modify the pin used for keyboard
            if (uei_case_keyboard_pin(gpio_array[i]))
                continue;
            if (i < 36)
                gpio_set_func(gpio_array[i], AS_GPIO);
            gpio_setup_up_down_resistor(gpio_array[i], PM_PIN_PULLDOWN_100K);
            wd_clear();
        }
        rst = 1;
        break;
    case VK_2:  // Strong Push Pull Output High
    case VK_3:  // Strong Push Pull Output Low
        for (i = 0; i < ARRAY_ELEM_CNT(gpio_array); i ++) {
            // don't modify the pin used for keyboard
            if (uei_case_keyboard_pin(gpio_array[i]))
                continue;
            if (i < 36)
                gpio_set_func(gpio_array[i], AS_GPIO);
            gpio_set_output_en(gpio_array[i], 1);
            gpio_write(gpio_array[i], cmd[0] == VK_2 ? 1 : 0);
            wd_clear();
        }

        // wait for later sub command
        if (step < 2)
            break;

        /*
         * if the sub command is invalidate,
         * we just wait for the next sub command,
         * don't reset the command context
         */
        if (cmd[1] != VK_0 && cmd[1] != VK_1) {
            step = 1;
            cmd[1] = 0;
            break;
        }

        for (i = 0; i < ARRAY_ELEM_CNT(gpio_array); i ++) {
            // don't modify the pins used for keyboard
            if (uei_case_keyboard_pin(gpio_array[i]))
                continue;
            // cmd[1] == VK_0, Set Drive Strength Low
            // cmd[1] == VK_1, Set Drive Strength High
            gpio_set_data_strength(gpio_array[i], cmd[1] == VK_0 ? 0 : 1);
        }

        wd_stop();
        while(1);
    case VK_4:
        // configure all ports input
        for (i = 0; i < ARRAY_ELEM_CNT(gpio_array); i ++) {
            // don't modify the pins used for keyboard
            if (uei_case_keyboard_pin(gpio_array[i]))
                continue;
            if (i < 36)
                gpio_set_func(gpio_array[i], AS_GPIO);
            gpio_set_input_en(gpio_array[i], 1);
            wd_clear();
        }
        wd_stop();
        while(1);
    default:
        goto GPIO_OUTPUT_FAIL;
    }

    if (rst) {
        memset(cmd, 0, sizeof(cmd));
        step = 0;
    }

    return 0;

GPIO_OUTPUT_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;
}

char uei_case_watdog()
{
    char repeat = 100;
    /*
     * turn LED ON
     */
    print_hex(0x05);
    print_hex(0x02);
    gpio_set_output_en(GPIO_LED, 1);
    gpio_write(GPIO_LED, 1);

    /*
     * LED ON for 1s
     */
    while (repeat-- > 0) {
        wd_clear();
        sleep_us(10000);  // 10ms
    }

    analog_write(DEEP_ANA_REG2, UEI_WD_FLG_RST);
    analog_write(DEEP_ANA_REG0, 0);

    gpio_write(GPIO_LED, 0);

    /*
     * start WatchDog and wait for WatchDog reset
     */
    wd_start();
    while(1);

    return 0;
}

static void uei_start_timer_mode(s8 timer, s8 mode, s8 gpio_idx, u32 capture)
{
    static s8 last_timer = UEI_CASE_IDX_INVALID;

    if (timer < 0 || timer > 2)
        return;

    if (last_timer != timer &&
        last_timer != UEI_CASE_IDX_INVALID) {
        clock_enable_clock(last_timer, 0);
        clock_clr_tmr_irq(last_timer);
    }

    last_timer = timer;

    clock_set_tmr_tick(timer, 0);
    clock_set_tmr_interval(timer, capture);
    if (mode == CLOCK_MODE_GPIO || mode == CLOCK_MODE_WIDTH_GPI) {
        clock_set_tmr_gpio_mode(timer, gpio_array[gpio_idx], 0);
    }
    clock_set_tmr_mode(timer, mode);
    clock_enable_tmr_irq(timer, 1);
    clock_enable_clock(timer, 1);
}

static void uei_case_enable_pad()
{
    analog_write(0x80, 0x61);
    sleep_us(100);
}

static void uei_case_timer_set_sclk(u8 sclk_idx)
{
    /* system clock select 0x66[6:5]
     * 00: RC_32M as system clock
     * 01: PLL as system clock
     * 10: PAD_12M/PAD_16M as system clock
     * 11: PAD_32.768K as system clock
     */
    enum {
        SCLK_SEL_RC_32M = 0x00,
        SCLK_SEL_PLL = 0x01,
        SCLK_SEL_PAD_12M_16M,
        SCLK_SEL_PAD_32K
    };

    u8 sclk_list[] = {SCLK_SEL_RC_32M, SCLK_SEL_PLL, SCLK_SEL_PAD_12M_16M, SCLK_SEL_PAD_32K};
    u32 clk_freq[] = {32000000, CLOCK_SYS_CLOCK_1S, 12000000, 32000};

    if (sclk_idx >= ARRAY_ELEM_CNT(sclk_list))
        return;

    if (sclk_list[sclk_idx] == SCLK_SEL_PAD_12M_16M ||
        sclk_list[sclk_idx] == SCLK_SEL_PAD_32K)
        uei_case_enable_pad();

    /* fhs_sel 0x70[0]~0x66[7-5]
     * 0001: PLL as FHS and FHS Divide, 192M/96M/64M/48M
     * 0101: RC_32M as FHS and FHS Divide
     * 1001: PAD_12M as FHS and FHS Divide
     * 1101: PAD_16M as FHS and FHS Divide
     * PLL needs special process
     */
    if (sclk_list[sclk_idx] != SCLK_SEL_PLL) {
        u8 r = irq_disable();
        reg_clk_sel = MASK_VAL(FLD_CLK_SEL_SRC, sclk_list[sclk_idx]);
        g_sys_clock = clk_freq[sclk_idx];
        set_tick_per_us(g_sys_clock / 1000000);
        irq_restore(r);
        return;
    }

    u8 pll = reg_clk_sel;
    u8 pll_sel = MASK_VAL(BIT_RNG(5,6), SCLK_SEL_PLL);

    // don't need to modify
    if ((pll & BIT_RNG(5,6)) == pll_sel)
        return;

    u8 r = irq_disable();
    pll &= ~BIT_RNG(5,6);
    pll |= pll_sel;
    reg_clk_sel = pll;
    irq_restore(r);
}

static void uei_case_timer_set_fhs(u8 fhs_idx)
{
#define PLL_CLK_SEL_REG    (0x88)
    /* fhs_sel 0x70[0]~0x66[7-5]
     * 0001: PLL as FHS and FHS Divide, 192M/96M/64M/48M
     * 0101: RC_32M as FHS and FHS Divide
     * 1001: PAD_12M as FHS and FHS Divide
     * 1101: PAD_16M as FHS and FHS Divide
     */
    enum {
        FHS_SEL_PLL_192M = 0x01,
        FHS_SEL_PLL_48M = 0x01,
        FHS_SEL_RC_32M = 0x05,
        FHS_SEL_PAD_12M = 0x09,
        FHS_SEL_PAD_16M = 0x0D,
    };

    u8 fhs_list[] = {FHS_SEL_PLL_192M, FHS_SEL_PLL_48M,
            FHS_SEL_RC_32M, FHS_SEL_PAD_12M, FHS_SEL_PAD_16M};
    u8 divide;
    /*************PLL 192M   48M   64M  96M***/
    u8 pll_list[] = {0x03, 0x00, 0x01, 0x02};
    u32 clk_freq[] = {192000000, 48000000, 32000000, 12000000, 16000000};

    if (fhs_idx >= ARRAY_ELEM_CNT(fhs_list))
        return;
    /*
     * revise the system clock
     */
    divide = clk_freq[fhs_idx] / CLOCK_SYS_CLOCK_1S;
    if (divide < 2)
        divide = 2;
    g_sys_clock = clk_freq[fhs_idx] / divide;

    /* pll_clk_sel register: 0x88[1:0]:
     * 00: 48M
     * 01: 64M
     * 10: 96M
     * 11: 192M
     */
    u8 r = irq_disable();
    if (fhs_list[fhs_idx] == FHS_SEL_PLL_192M ||
        fhs_list[fhs_idx] == FHS_SEL_PLL_48M) {
        u8 pll = analog_read(PLL_CLK_SEL_REG);
        pll &= 0xFC;
        pll |= pll_list[fhs_idx];
        analog_write(PLL_CLK_SEL_REG, pll);
    } else if (fhs_list[fhs_idx] == FHS_SEL_PAD_12M ||
        fhs_list[fhs_idx] == FHS_SEL_PAD_16M) {
        uei_case_enable_pad();
    }
    reg_fhs_sel = ((fhs_list[fhs_idx] & 0x08) ? 1 : 0);
    reg_clk_sel = MASK_VAL(FLD_CLK_SEL_DIV, divide, FLD_CLK_SEL_SRC, fhs_list[fhs_idx] & 0x07);
    set_tick_per_us(g_sys_clock / 1000000);
    irq_restore(r);
}

void uei_case_timer_wait_tick(u8 timer, u32 tick)
{
#define DIFF(start, end)  ((u32)((end) - (start)))
    u32 ref = clock_tmr_get_tick(timer);
    u32 now = clock_tmr_get_tick(timer);
    while (DIFF(ref, now) < tick) {
        now = clock_tmr_get_tick(timer);
    }
}

char uei_case_timer()
{
#define UEI_CASE_TIMER_CMD_LEN_MAX  (3)
    static u8 step = 0;
    static u8 cmd[UEI_CASE_TIMER_CMD_LEN_MAX];
    static s8 fhs_idx = UEI_CASE_IDX_INVALID;
    static s8 clk_src_idx = UEI_CASE_IDX_INVALID;

    uei_gpio_range_t range[] = {
        {VK_1, VK_8},  // to index command
        {VK_1, VK_3},  // to index sub-command
        {VK_1, VK_7},  // to index PX0~PX7
        {VK_1, VK_5},  // to index PA, PB, PC, PD, PE
        {VK_1, VK_2},  // to index timer0/timer1/timer2
    };

    u8 one_key_cmd[] = {VK_2, VK_3, VK_4, VK_5, VK_8};
    u8 two_key_cmd[] = {VK_0, VK_6, VK_7};
    u8 tri_key_cmd[] = {VK_1};
    u8 i, range_idx, warning = 0;
    u32 capture = UEI_CASE_TIMER_CAPTURE_DEFAULT;

    uei_cmd_meta_t cmd_meta[] = {
        {ARRAY_ELEM_CNT(one_key_cmd), one_key_cmd},
        {ARRAY_ELEM_CNT(two_key_cmd), two_key_cmd},
        {ARRAY_ELEM_CNT(tri_key_cmd), tri_key_cmd}
    };

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    print_hex(0x05);
    print_hex(0x03);

    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto TIMER_FAIL;
    /*
     * specific process for GPIO selection and Timer selection
     */
    range_idx = step;
    if (step == 1) {
        if (cmd[0] == VK_0)  // index timer0/timer1/timer2
            range_idx = UEI_CASE_TIMER_CMD_LEN_MAX + 1;
        else if (cmd[0] == VK_1)  // index PA, PB, PC, PD, PE
            range_idx = UEI_CASE_TIMER_CMD_LEN_MAX;
    }

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[range_idx].start)
        goto TIMER_FAIL;
    if (key != VK_0 && key > range[range_idx].end)
        goto TIMER_FAIL;
    /*
     * if keys of current command are ready, to process command,
     * otherwise, wait for command is ready
     */
    cmd[step] = key;
    for (i = 0; i < cmd_meta[step].cmd_cnt; i ++) {
        if (cmd[0] == cmd_meta[step].cmd[i])
            break;
    }
    if (i >= cmd_meta[step ++].cmd_cnt)  // current command is not ready
        return 0;

    switch (cmd[0]) {
    case VK_0:  // timer selection
        g_uei_timer_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (g_uei_timer_idx > UEI_CASE_TIMER_IDX_MAX) {
            g_uei_timer_idx = UEI_CASE_IDX_INVALID;
            goto TIMER_FAIL;
        }
        /*
         * if GPIO or timer is not ready, we can't set timer with GPIO mode
         */
        if (g_uei_timer_mode_idx == UEI_CASE_IDX_INVALID)
            break;
        goto SET_TIMER_MODE;
    case VK_1:  // GPIO selection
        /*
         * 00~07 for PA0~PA7, 10~17 for PB0~PB7
         * 20~27 for PC0~PC7, 30~37 for PD0~PD7
         * 40~47 for PE0~PE7, 50~51 for PF0~PF1
         */
        g_uei_timer_gpio_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        g_uei_timer_gpio_idx = (g_uei_timer_gpio_idx << 3) + (cmd[2] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (g_uei_timer_gpio_idx >= UEI_CASE_GPIO_CNT ||
            g_uei_timer_gpio_idx < 0) {
            g_uei_timer_gpio_idx = UEI_CASE_IDX_INVALID;
            goto TIMER_FAIL;
        }

        // the pins used by keyboard can't modify
        if (uei_case_keyboard_pin(gpio_array[g_uei_timer_gpio_idx])) {
            warning = 1;
            goto TIMER_FAIL;
        }

        gpio_set_output_en(gpio_array[g_uei_timer_gpio_idx], 0);
        gpio_set_input_en(gpio_array[g_uei_timer_gpio_idx], 1);

        /*
         * if GPIO or timer is not ready, we can't set timer with GPIO mode
         */
        if (g_uei_timer_mode_idx == UEI_CASE_IDX_INVALID ||
            g_uei_timer_idx == UEI_CASE_IDX_INVALID)
            break;
        goto SET_TIMER_MODE;
    case VK_3:  // CLOCK_MODE_GPIO, GPIO Trigger Mode
    case VK_4:  // CLOCK_MODE_WIDTH_GPI, GPIO Pulse Width Mode
        capture = UEI_CASE_TIMER_GPIO_TRIGGER_CAP;
    case VK_2:  // CLOCK_MODE_SCLK, System Timer Mode
    case VK_5:  // CLOCK_MODE_TICK, Tick Mode
        g_uei_timer_mode_idx = cmd[0] - VK_2;
        if (g_uei_timer_mode_idx > CLOCK_MODE_TICK ||
            g_uei_timer_mode_idx < 0) {
            g_uei_timer_mode_idx = UEI_CASE_IDX_INVALID;
            goto TIMER_FAIL;
        }
SET_TIMER_MODE:
        /*
         * if GPIO or timer is not ready, we can't set timer with GPIO mode
         */
        if (g_uei_timer_idx == UEI_CASE_IDX_INVALID)
            break;
        if ((g_uei_timer_mode_idx == CLOCK_MODE_GPIO || g_uei_timer_mode_idx == CLOCK_MODE_WIDTH_GPI) &&
            g_uei_timer_gpio_idx == UEI_CASE_IDX_INVALID)
            break;

        //gpio_set_output_en(UEI_CASE_GPIO_INDICATE, 1);

        if (UEI_CASE_TIMER_GPIO_TRIGGER_CAP == capture) {
            gpio_set_output_en(gpio_array[g_uei_timer_gpio_idx], 0);
            gpio_set_input_en(gpio_array[g_uei_timer_gpio_idx], 1);
        } else {
            gpio_set_output_en(gpio_array[g_uei_timer_gpio_idx], 1);
            gpio_set_input_en(gpio_array[g_uei_timer_gpio_idx], 0);
        }

        /*
         * turn on LED
         */
        gpio_set_output_en(GPIO_LED, 1);

        g_uei_timer_event = 1;
        wd_stop();

        while(1) {
            /*
             * CLOCK_MODE_TICK, no interrupt,
             * application has to loop to compare tick
             */
            if (g_uei_timer_mode_idx == CLOCK_MODE_TICK) {
                uei_case_timer_wait_tick(g_uei_timer_idx, capture);
                g_uei_timer_event = 1;
            }
            if (g_uei_timer_event == 0)
                continue;
            g_uei_timer_event = 0;
            gpio_toggle(GPIO_LED);

            uei_start_timer_mode(g_uei_timer_idx, g_uei_timer_mode_idx,
                    g_uei_timer_gpio_idx, capture);
            if (g_uei_timer_mode_idx == CLOCK_MODE_GPIO ||
                g_uei_timer_mode_idx == CLOCK_MODE_WIDTH_GPI) {
                g_uei_timer_idx ++;
                if (g_uei_timer_idx > UEI_CASE_TIMER_IDX_MAX)
                    g_uei_timer_idx = 0;
            } else if (capture == UEI_CASE_TIMER_CAPTURE_DEFAULT) {
                // current status of LED is OFF
                // CLOCK_MODE_SCLK, CLOCK_MODE_TICK
                // 100 tick for LED OFF
                capture = 2 * UEI_CASE_TIMER_CAPTURE_DEFAULT;
            } else {  // current status of LED is OFF
                // CLOCK_MODE_SCLK, CLOCK_MODE_TICK
                // 10 tick for LED ON
                capture = UEI_CASE_TIMER_CAPTURE_DEFAULT;
            }
        }
        break;
    case VK_6:
        // cmd[1], VK_0 for PLL 192M
        // cmd[1], VK_1 for PLL 48M
        // cmd[1], VK_2 for 32M RC
        // cmd[1], VK_3 for 12M/16M crystal
        fhs_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (fhs_idx > 3 || fhs_idx < 0) {
            fhs_idx = UEI_CASE_IDX_INVALID;
            goto TIMER_FAIL;
        }
        break;
    case VK_7:
        // cmd[1], VK_0 for 32M RC
        // cmd[1], VK_1 for FHS
        // cmd[1], VK_2 for 12M/16M crystal
        // cmd[1], VK_3 for 32K crystal
        clk_src_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (clk_src_idx > 3 || clk_src_idx < 0) {
            clk_src_idx = UEI_CASE_IDX_INVALID;
            goto TIMER_FAIL;
        }
        break;
    case VK_8:  // send selected clock out to destination GPIO
        /*
         * if GPIO or timer is not ready, we can't set timer with GPIO mode
         */
        if (g_uei_timer_gpio_idx == UEI_CASE_IDX_INVALID ||
            g_uei_timer_idx == UEI_CASE_IDX_INVALID) {
            break;
        }

        wd_stop();
        if (fhs_idx != UEI_CASE_IDX_INVALID)
            uei_case_timer_set_fhs(fhs_idx);
        if (clk_src_idx != UEI_CASE_IDX_INVALID)
            uei_case_timer_set_sclk(clk_src_idx);

        capture = UEI_CASE_TIMER_CAPTURE_DEFAULT;
        gpio_set_input_en(gpio_array[g_uei_timer_gpio_idx], 0);
        gpio_set_output_en(gpio_array[g_uei_timer_gpio_idx], 1);
        //gpio_set_output_en(UEI_CASE_GPIO_INDICATE, 1);

        g_uei_timer_event = 1;

        g_uei_timer_mode_idx = CLOCK_MODE_SCLK;

        while(1) {
            if (g_uei_timer_event == 0)
                continue;
            g_uei_timer_event = 0;
            gpio_toggle(gpio_array[g_uei_timer_gpio_idx]);
            uei_start_timer_mode(g_uei_timer_idx, g_uei_timer_mode_idx,
                    g_uei_timer_gpio_idx, capture);
        }
        break;
    default:
        goto TIMER_FAIL;
    }

    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;

TIMER_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(warning ? uei_led_cfg[LED_UEI_WARN] : uei_led_cfg[LED_UEI_FAIL]);
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;
}

char uei_case_mode()
{
    static u8 cmd[3];
    static u8 step = 0;
    static u32 gpio_idx = 0;
    static u8 ready = 0;

    g_uei_timer_wakeup = 0;

    uei_gpio_range_t range[] = {
        {VK_1, VK_4},  // to index command
        {VK_1, VK_5},  // to index PA, PB, PC, PD, PE
        {VK_1, VK_7},  // to index PX0~PX7
    };

    u8 i, warning = 0;
    u8 one_key_cmd[] = {VK_0, VK_1, VK_3, VK_4};
    u8 tri_key_cmd[] = {VK_2};

    uei_cmd_meta_t cmd_meta[] = {
        {ARRAY_ELEM_CNT(one_key_cmd), one_key_cmd},
        {0,                           NULL},
        {ARRAY_ELEM_CNT(tri_key_cmd), tri_key_cmd}
    };

    print_hex(0x05);
    print_hex(0x04);

    if (ready)
        goto PARSE_MODE_CMD;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];
    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto MODE_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[step].start)
        goto MODE_FAIL;
    if (key != VK_0 &&
        key > range[step].end)
        goto MODE_FAIL;
    /*
     * if keys of current command are ready, to process command,
     * otherwise, wait for command is ready
     */
    cmd[step] = key;
    if (cmd_meta[step].cmd_cnt == 0) {
        step ++;
        return 0;
    }

    for (i = 0; i < cmd_meta[step].cmd_cnt; i ++) {
        if (cmd[0] == cmd_meta[step].cmd[i])
            break;
    }
    if (i >= cmd_meta[step ++].cmd_cnt)  // current command is not ready
        return 0;
PARSE_MODE_CMD:
    switch (cmd[0]) {
    case VK_0:
        uei_case_force_sleep(UEI_CASE_FORCE_DEEPSLEEP);
        break;
    case VK_1:
        ready = 1;
        uei_case_force_sleep(UEI_CASE_FORCE_SUSPEND_TIME_WAKEUP);
        g_uei_timer_wakeup = 1;
        break;
    case VK_2:
        gpio_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        gpio_idx = (gpio_idx << 3) + (cmd[2] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (gpio_idx >= UEI_CASE_GPIO_CNT) {
            gpio_idx = 0;
            goto MODE_FAIL;
        }

        // the pins used by keyboard cann't modify
        if (uei_case_keyboard_pin(gpio_array[gpio_idx])) {
            gpio_idx = 0;
            warning = 1;
            goto MODE_FAIL;
        }
        ready = 1;
        gpio_set_output_en(gpio_array[gpio_idx], 0);
        gpio_set_input_en(gpio_array[gpio_idx], 1);
        gpio_set_wakeup(gpio_array[gpio_idx], 1, 1);  // drive pin core(GPIO) high wake up suspend
        cpu_set_gpio_wakeup(gpio_array[gpio_idx], 1, 1);  // drive pin pad high wake up deep sleep
        uei_case_force_sleep(UEI_CASE_FORCE_SUSPEND_GPIO_WAKEUP);
        break;
    case VK_3:
        // continuous RF receive mode
        rf_set_rxmode();
        wd_stop();
        while(1);
        //uei_case_force_sleep(UEI_CASE_FORCE_DEEPSLEEP);
    case VK_4:
        // continuous RF transmit mode
        rf_set_txmode();
        wd_stop();
        while(1);
        //uei_case_force_sleep(UEI_CASE_FORCE_DEEPSLEEP);
    default:
        goto MODE_FAIL;
    }

    if (ready)
        return 1;
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;

MODE_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(warning ? uei_led_cfg[LED_UEI_WARN] : uei_led_cfg[LED_UEI_FAIL]);
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;
}

char uei_case_flash()
{
    u8 wr_buf[256];
    u8 rd_buf[256];
    u32 usr_data_start = 0x78000;
    u32 usr_data_end = 0x80000;
    const u16 flash_page_sz = 0x100;
    const u16 flash_sect_sz = 0x1000;

    uei_gpio_range_t range[] = {
        {VK_1, VK_7},  // to index command
    };

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    u16 i = 0;

    print_hex(0x05);
    print_hex(0x05);

    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto FLASH_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[0].start)
        goto FLASH_FAIL;
    if (key != VK_0 &&
        key > range[0].end)
        goto FLASH_FAIL;

    gpio_set_output_en(GPIO_LED, 1);

    wd_stop();

    switch (key) {
    case VK_0:  // Erase Page
        gpio_write(GPIO_LED, 1);  // Turn BlackLight ON
        //flash_erase_page(usr_data_start);
        flash_erase_sector(usr_data_start);
        gpio_write(GPIO_LED, 0);  // Turn BlackLight OFF
        break;
    case VK_1:  // Write Page
        for (i = 0; i < sizeof(wr_buf); i ++) {
            wr_buf[i] = (i & 0x01) ? 0x55: 0xAA;
        }
        gpio_write(GPIO_LED, 1);  // Turn BlackLight ON
        flash_write_page(usr_data_start, flash_page_sz, wr_buf);
        flash_write_page(usr_data_start + flash_page_sz, flash_page_sz, wr_buf);
        gpio_write(GPIO_LED, 0);  // Turn BlackLight OFF
        break;
    case VK_2:  // Read Page
        gpio_write(GPIO_LED, 1);  // Turn BlackLight ON
        flash_read_page(usr_data_start, flash_page_sz, rd_buf);
        flash_read_page(usr_data_start + flash_page_sz, flash_page_sz, rd_buf);
        gpio_write(GPIO_LED, 0);  // Turn BlackLight OFF
        break;
    case VK_3:  // Erase Sector
        gpio_write(GPIO_LED, 1);  // Turn BlackLight ON
        flash_erase_sector(usr_data_start);
        gpio_write(GPIO_LED, 0);  // Turn BlackLight OFF
        break;
    case VK_4:  // Erase User Data
        gpio_write(GPIO_LED, 1);  // Turn BlackLight ON
        usr_data_start = 0x78000;
        usr_data_end = 0x80000;
        for (; usr_data_start < usr_data_end; usr_data_start += flash_sect_sz)
            flash_erase_sector(usr_data_start);
        usr_data_start = 0x40000;
        usr_data_end = 0x74000;
        for (; usr_data_start < usr_data_end; usr_data_start += flash_sect_sz)
            flash_erase_sector(usr_data_start);
        gpio_write(GPIO_LED, 0);  // Turn BlackLight OFF
        break;
    case VK_5:  // Erase two pages
        flash_erase_sector(usr_data_start);
        while(1);
    case VK_6:  // Write two pages
        for (i = 0; i < sizeof(wr_buf); i ++) {
            wr_buf[i] = (i & 0x01) ? 0x55: 0xAA;
        }
        flash_write_page(usr_data_start, flash_page_sz, wr_buf);
        flash_write_page(usr_data_start + flash_page_sz, flash_page_sz, wr_buf);
        while(1);
    case VK_7:  // Read two pages
        flash_read_page(usr_data_start, flash_page_sz, rd_buf);
        flash_read_page(usr_data_start + flash_page_sz, flash_page_sz, rd_buf);
        while(1);
    default:
        wd_start();
        goto FLASH_FAIL;
    }

    wd_start();
    uei_case_force_sleep(UEI_CASE_FORCE_DEEPSLEEP);
    return 0;

FLASH_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    return 0;

}

char uei_case_uart()
{
    static u8 ready = 0;
    static u8 cmd = 0;
    uei_gpio_range_t range[] = {
        {VK_1, VK_7},  // to index command
    };

    u32 sys_clk = CLOCK_SYS_CLOCK_1S;
    u32 high_rate = 115200;
    u32 low_rate = 9600;
    u32 rate = low_rate;

    u8 div, bwpc = 15;
    u8 match = 0;

    u32 tx_buf[3] = {0x00000008, 0xADACABAA, 0xB1B0AFAE};
    u32 rx_buf[4] = {0x00000000, 0x00000000, 0x00000000, 0x0000000000};

    print_hex(0x05);
    print_hex(0x06);

    if (ready)
        goto PARSE_UART_CMD;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto UART_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[0].start)
        goto UART_FAIL;
    if (key != VK_0 &&
        key > range[0].end)
        goto UART_FAIL;

    cmd = key;

    uart_Reset();
    uart_io_init(UART_GPIO_8267_PB2_PB3);
    gpio_set_input_en(GPIO_PB2, 1);
    gpio_set_input_en(GPIO_PB3, 1);
    gpio_setup_up_down_resistor(GPIO_PB2, PM_PIN_PULLUP_1M);
    gpio_setup_up_down_resistor(GPIO_PB3, PM_PIN_PULLUP_1M);
    uart_IRQSourceGet();  // clear UART RX/TX status
PARSE_UART_CMD:
    switch (cmd) {
    case VK_0:  // Highest Baud Rate, transmit mode
        bwpc = 13;
        rate = high_rate;
    case VK_1:  // Lowest Baud Rate, transmit mode
        // TX AA, AB, AC, AD, AE, AF, B0, B1 once
        div = sys_clk / rate / (bwpc + 1);
        uart_Init(div, bwpc, PARITY_NONE, STOP_BIT_ONE);
        uart_DmaModeInit(1, 0);
        sleep_us(1000);
        uart_Send_kma((u8 *)tx_buf);
        wd_stop();
        sleep_us(sizeof(tx_buf) * 8 * sys_clk / rate);
        wd_start();
        break;
    case VK_2:  // Highest Baud Rate, receive mode
        bwpc = 13;
        rate = high_rate;
    case VK_3:  // Lowest Baud Rate, receive mode
        // double blink for success
        // or double long blink for failure
        ready = 1;
        div = sys_clk / rate / (bwpc + 1);
        uart_Init(div, bwpc, PARITY_NONE, STOP_BIT_ONE);
        uart_DmaModeInit(1, 1);
        uart_RecBuffInit((u8 *)rx_buf, sizeof(rx_buf));

        g_uei_uart_rx_event = UEI_UART_START;

        while(1) {
            wd_clear();
            if (g_uei_uart_rx_event == UEI_UART_RX_DONE)
                break;
        }
#if 0
        uart_Send_kma((u8 *)rx_buf);
        wd_stop();
        sleep_us(sizeof(rx_buf) * 8 * sys_clk / rate);
        wd_start();
#endif
        do {
            if (g_uei_uart_rx_event != UEI_UART_RX_DONE)
                break;
            if (memcmp(tx_buf, rx_buf, sizeof(tx_buf)))
                break;
            match = 1;
        } while (0);
        /*
         * double blink for success
         * double long blink for failure
         */
        device_led_setup(uei_led_cfg[match ? LED_UEI_SUCCESS : LED_UEI_FAIL]);
        memset(rx_buf, 0, sizeof(rx_buf));
        g_uei_uart_rx_event = UEI_UART_IDLE;
        break;
    default:
        goto UART_FAIL;
    }

    if (ready)
        return 1;

    ready = 0;
    cmd = 0;

    return 0;

UART_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    ready = 0;
    cmd = 0;
    return 0;
}

static void uei_case_serail_slave_read(unsigned short addr, unsigned char* pbuf, int len)
{
    int i = 0;
    if (!pbuf)
        return;
    for (i = 0; i < len; i ++) {
        pbuf[i] = read_reg8(addr + i);
    }
}

static void uei_case_serail_slave_write(unsigned short addr, unsigned char* pbuf, int len)
{
    int i = 0;
    if (!pbuf)
        return;
    for (i = 0; i < len; i ++) {
        write_reg8(addr + i, pbuf[i]);
    }
}

char uei_case_i2c()
{
    static u8 cmd[2];
    static u8 step = 0;
    static s8 speed_idx = -1;
    static u8 ready = 0;

    enum {
        I2C_SPEED_100K = 100,
        I2C_SPEED_400K = 400,
        I2C_SPEED_1000K = 1000,
    };
    u32 speed[] = {I2C_SPEED_100K, I2C_SPEED_400K, I2C_SPEED_1000K};

    const u8 i2c_tx_buf[8] =  {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1};
    const u8 i2c_rsp_buf[8] = {0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0xF0, 0xF1};
    u8 i2c_rx_buf[8] = {0x00};

    uei_gpio_range_t range[] = {
        {VK_1, VK_2},  // to index command
        {VK_1, VK_2},  // to index speed
    };

    u8 i, match = 0;
    u8 one_key_cmd[] = {VK_1, VK_2};
    u8 two_key_cmd[] = {VK_0};

    uei_cmd_meta_t cmd_meta[] = {
        {ARRAY_ELEM_CNT(one_key_cmd), one_key_cmd},
        {ARRAY_ELEM_CNT(two_key_cmd), two_key_cmd}
    };

    print_hex(0x05);
    print_hex(0x07);

    if (ready)
        goto PARSE_I2C_CMD;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];
    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto I2C_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[step].start)
        goto I2C_FAIL;
    if (key != VK_0 &&
        key > range[step].end)
        goto I2C_FAIL;

    /*
     * if keys of current command are ready, to process command,
     * otherwise, wait for command is ready
     */
    cmd[step] = key;
    for (i = 0; i < cmd_meta[step].cmd_cnt; i ++) {
        if (cmd[0] == cmd_meta[step].cmd[i])
            break;
    }
    if (i >= cmd_meta[step ++].cmd_cnt)  // current command is not ready
        return 0;

    i2c_pin_initial(GPIO_PC0, GPIO_PC1);
PARSE_I2C_CMD:
    switch (cmd[0]) {
    case VK_0:
        speed_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (speed_idx < 0 || speed_idx >= ARRAY_ELEM_CNT(speed))
            goto I2C_FAIL;
        break;
    case VK_1:
        // set I2C Master with speed[speed_idx];
        if (speed_idx < 0 || speed_idx >= ARRAY_ELEM_CNT(speed))
            break;
        wd_stop();
        i2c_master_init1(UEI_CASE_I2C_SLAVE_ID, speed[speed_idx]);
        sleep_us(5000);
        /*
         * Master send data to register of slave with address of UEI_CASE_SERIAL_SLAVE_ADDR + REG_BASE_ADDR
         * the data flow sent by master is 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1,
         * 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1 is saved respectively in
         * UEI_CASE_SERIAL_SLAVE_ADDR + 0, UEI_CASE_SERIAL_SLAVE_ADDR + 1, ..., UEI_CASE_SERIAL_SLAVE_ADDR + 7
         */

        i2c_write_dma(UEI_CASE_SERIAL_SLAVE_ADDR, UEI_CASE_SERIAL_SLAVE_ADDR_LEN,
                (u8 *)i2c_tx_buf, sizeof(i2c_tx_buf));
        device_led_setup(uei_led_cfg[LED_UEI_SUCCESS]);
        wd_start();
        break;
    case VK_2:
        // set I2C Slave with speed[speed_idx];
        if (speed_idx < 0 || speed_idx >= ARRAY_ELEM_CNT(speed))
            break;
        if (ready == 0) {
            i2c_slave_init(UEI_CASE_I2C_SLAVE_ID, I2C_SLAVE_DMA, NULL);
            I2C_IRQ_EN;
        }

        // wait for data arrived
        while (!(g_uei_i2c_event & I2C_IRQ_HOST_WRITE_ONLY))
            wd_clear();

        if (ready == 0) {  // skip the first pseudo interrupt
            ready = 1;
            g_uei_i2c_event = 0;
            break;
        }

        /*
         * Master has sent data to register of slave with address of UEI_CASE_SERIAL_SLAVE_ADDR + REG_BASE_ADDR
         * so slave get data from register of UEI_CASE_SERIAL_SLAVE_ADDR + 0, UEI_CASE_SERIAL_SLAVE_ADDR + 1,
         * UEI_CASE_SERIAL_SLAVE_ADDR + 2, ..., UEI_CASE_SERIAL_SLAVE_ADDR + 7 respectively.
         */
        uei_case_serail_slave_read(UEI_CASE_SERIAL_SLAVE_ADDR, i2c_rx_buf, sizeof(i2c_rx_buf));
        /*
         * Master will read data from register of slave with address of UEI_CASE_SERIAL_SLAVE_ADDR + REG_BASE_ADDR
         * so we provide the data in register of UEI_CASE_SERIAL_SLAVE_ADDR + 0, UEI_CASE_SERIAL_SLAVE_ADDR + 1,
         * UEI_CASE_SERIAL_SLAVE_ADDR + 1, ..., UEI_CASE_SERIAL_SLAVE_ADDR + 7
         */
        uei_case_serail_slave_write(UEI_CASE_SERIAL_SLAVE_ADDR, (u8 *)i2c_rsp_buf, sizeof(i2c_rsp_buf));
        if (memcmp(i2c_rx_buf, i2c_tx_buf, sizeof(i2c_tx_buf)) == 0)
            match = 1;
        memset(i2c_rx_buf, 0, sizeof(i2c_rx_buf));
        device_led_setup(uei_led_cfg[match ? LED_UEI_SUCCESS : LED_UEI_FAIL]);
        g_uei_i2c_event = 0;
        break;
    default:
        goto I2C_FAIL;
    }

    if (ready)
        return 1;

    memset(cmd, 0, sizeof(cmd));
    step = 0;

    return 0;

I2C_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;
}

char uei_case_spi()
{
    static u8 ready = 0;
    static u8 cmd[2];
    static u8 step = 0;

    int high_baut = 0x0f;
    int low_baut = 0x7F;

    const u8 mode[4] = {SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3};
    static s8 mode_idx = -1;

    uei_gpio_range_t range[] = {
        {VK_1, VK_4},  // to index command
        {VK_1, VK_3},  // to index mode
    };

    u8 i, match = 0;
    u8 one_key_cmd[] = {VK_1, VK_2, VK_3, VK_4};
    u8 two_key_cmd[] = {VK_0};

    uei_cmd_meta_t cmd_meta[] = {
        {ARRAY_ELEM_CNT(one_key_cmd), one_key_cmd},
        {ARRAY_ELEM_CNT(two_key_cmd), two_key_cmd}
    };

    //const u8 spi_rd_cmd[] = {UEI_CASE_SERIAL_SLAVE_ADDR_HIGH, UEI_CASE_SERIAL_SLAVE_ADDR_LOW, SPI_CMD_RD};
    const u8 spi_wr_cmd[] = {UEI_CASE_SERIAL_SLAVE_ADDR_HIGH, UEI_CASE_SERIAL_SLAVE_ADDR_LOW, SPI_CMD_WR};
    const u8 spi_tx_buf[] = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1};
    const u8 spi_rsp_buf[] = {0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0xF0, 0xF1};
    u8 spi_rx_buf[8] = {0x00};

    print_hex(0x05);
    print_hex(0x08);

    if (ready)
        goto PARSE_SPI_CMD;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];
    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto SPI_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[step].start)
        goto SPI_FAIL;
    if (key != VK_0 &&
        key > range[step].end)
        goto SPI_FAIL;

    /*
     * if keys of current command are ready, to process command,
     * otherwise, wait for command is ready
     */
    cmd[step] = key;
    for (i = 0; i < cmd_meta[step].cmd_cnt; i ++) {
        if (cmd[0] == cmd_meta[step].cmd[i])
            break;
    }
    if (i >= cmd_meta[step ++].cmd_cnt)  // current command is not ready
        return 0;
PARSE_SPI_CMD:
    switch (cmd[0]) {
    case VK_0:
        mode_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (mode_idx < 0 || mode_idx >= ARRAY_ELEM_CNT(mode))
            goto SPI_FAIL;
        break;
    case VK_1:  // SPI Master with Highest Baud Rate;
    case VK_2:  // SPI Master with Lowest Baud Rate;
        if (mode_idx < 0 || mode_idx >= ARRAY_ELEM_CNT(mode))
            break;

        wd_stop();
        spi_master_init(cmd[0] == VK_1 ? high_baut : low_baut, mode[mode_idx]);
        spi_master_pin_init(SPI_PIN_GROUPA, UEI_CASE_SPI_CS_PIN);
        sleep_us(5000);
        /*
         * Master send data to register of slave with address of UEI_CASE_SERIAL_SLAVE_ADDR + REG_BASE_ADDR
         * the data flow sent by master is 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1,
         * 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1 is saved respectively in
         * UEI_CASE_SERIAL_SLAVE_ADDR + 0, UEI_CASE_SERIAL_SLAVE_ADDR + 1, ..., UEI_CASE_SERIAL_SLAVE_ADDR + 7
         */
        spi_write((u8 *)spi_wr_cmd, sizeof(spi_wr_cmd),
                (u8 *)spi_tx_buf, sizeof(spi_tx_buf), UEI_CASE_SPI_CS_PIN);
        // two blink to indicate success
        device_led_setup(uei_led_cfg[LED_UEI_SUCCESS]);
        wd_start();
        break;
    case VK_3:  // SPI Slave with Highest Baud Rate;
    case VK_4:  // SPI Slave with Lowest Baud Rate;
        if (mode_idx < 0 || mode_idx >= ARRAY_ELEM_CNT(mode))
            break;
        // Baud Rate of slave depends on master;
        if (ready == 0) {
            spi_slave_init(SPI_PIN_GROUPA, mode[mode_idx]);
            SPI_IRQ_EN;
        }
        // wait for data arrived
        while (!(g_uei_spi_event & SPI_IRQ_HOST_WRITE_ONLY))
            wd_clear();
        ready = 1;

        /*
         * Master has sent data to register of slave with address of UEI_CASE_SERIAL_SLAVE_ADDR + REG_BASE_ADDR
         * so slave get data from register of UEI_CASE_SERIAL_SLAVE_ADDR + 0, UEI_CASE_SERIAL_SLAVE_ADDR + 1,
         * UEI_CASE_SERIAL_SLAVE_ADDR + 2, ..., UEI_CASE_SERIAL_SLAVE_ADDR + 7 respectively.
         */
        uei_case_serail_slave_read(UEI_CASE_SERIAL_SLAVE_ADDR, spi_rx_buf, sizeof(spi_rx_buf));
        /*
         * Master will read data from register of slave with address of UEI_CASE_SERIAL_SLAVE_ADDR + REG_BASE_ADDR
         * so we provide the data in register of UEI_CASE_SERIAL_SLAVE_ADDR + 0, UEI_CASE_SERIAL_SLAVE_ADDR + 1,
         * UEI_CASE_SERIAL_SLAVE_ADDR + 1, ..., UEI_CASE_SERIAL_SLAVE_ADDR + 7
         */
        uei_case_serail_slave_write(UEI_CASE_SERIAL_SLAVE_ADDR, (u8 *)spi_rsp_buf, sizeof(spi_rsp_buf));
        if (memcmp(spi_rx_buf, spi_tx_buf, sizeof(spi_rx_buf)) == 0)
            match = 1;
        memset(spi_rx_buf, 0, sizeof(spi_rx_buf));
        device_led_setup(uei_led_cfg[match ? LED_UEI_SUCCESS : LED_UEI_FAIL]);
        g_uei_spi_event = 0;
        break;
    default:
        uei_case_force_sleep(UEI_CASE_FORCE_DEEPSLEEP);
        break;
    }

    if (ready)
        return 1;

    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;

SPI_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;
}

char uei_case_adc()
{
#define UEI_CASE_ADC_CMD_LEN_MAX     (3)
#define UEI_CASE_ADC_SAMPLE_MIN      (ADC_CLK_4M)
#define UEI_CASE_ADC_SAMPLE_MAX      (ADC_CLK_4M)
#define UEI_CASE_ADC_RESOLUTION_MIN  (RES7)
#define UEI_CASE_ADC_RESOLUTION_MAX  (RES14)
#define UEI_CASE_ADC_SAMPLE_CNT_BIT  (4)
#define UEI_CASE_ADC_SAMPLE_CNT      (1 << (UEI_CASE_ADC_SAMPLE_CNT_BIT))
    static u8 step = 0;
    static s8 sample = -1;
    static s8 resolution = -1;
    static u8 channel = NOINPUT;
    static u8 cmd[UEI_CASE_ADC_CMD_LEN_MAX];
    static u8 uart_init = 0;
    static u32 gpio_chan = 0;

    uei_gpio_range_t range[] = {
        {VK_1, VK_3},  // to index command
        {VK_1, VK_1},  // to index sub-command
        {VK_1, VK_9},  // to index ADC channel in chan_array
    };

    u8 chan_array[] = {C0, C1, C6, C7, B0, B1, B2, B3, B4, B5, B6, B7,
            PGAVOM, PGAVOP, TEMSENSORN, TEMSENSORP, AVSS, OTVDD};
    u32 chan_list[] = {GPIO_PC0, GPIO_PC1, GPIO_PC6, GPIO_PC7,
            GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PB4,
            GPIO_PB5, GPIO_PB6, GPIO_PB7};
    s8 chan_idx = -1;

    const u8 str[] = "0123456789";

    u8 serail_output_buf[] = {0x08, 0x00, 0x00, 0x00,
            'V', '0', '.', '0', '0', '0', 0x0D, 0xA};

    u8 one_key_cmd[] = {VK_0, VK_1, VK_3};
    u8 two_key_cmd[] = {VK_0, VK_1};
    u8 tri_key_cmd[] = {VK_2};
    u8 i, rst = 1;

    uei_cmd_meta_t cmd_meta[] = {
        {ARRAY_ELEM_CNT(one_key_cmd), one_key_cmd},
        {ARRAY_ELEM_CNT(two_key_cmd), two_key_cmd},
        {ARRAY_ELEM_CNT(tri_key_cmd), tri_key_cmd},
    };

    u32 adc_data;
    u16 voltage;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    print_hex(0x05);
    print_hex(0x09);

    /*
     * we just process one key and skip release key
     */
    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto ADC_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    do {
        if (step == 1)
            break;
        if (key < range[step].start)
            goto ADC_FAIL;
        if (key != VK_0 && key > range[step].end)
            goto ADC_FAIL;
    } while (0);

    /*
     * if keys of current command are ready, to process command,
     * otherwise, wait for command is ready
     */
    cmd[step] = key;
    for (i = 0; i < cmd_meta[step].cmd_cnt; i ++) {
        if (cmd[0] == cmd_meta[step].cmd[i])
            break;
    }
    if (i >= cmd_meta[step ++].cmd_cnt)  // current command is not ready
        return 0;

    if (!uart_init) {
        uart_init = 1;
        uart_print_init();
    }

    switch (cmd[0]) {
    case VK_0:  // sample rate selection
        rst = 0;
        if (step < 2)
            break;
        if (cmd[1] != VK_0 && cmd[1] != VK_1) {
            // wait for new sub command
            step --;
            break;
        }
        rst = 1;
        sample = (cmd[1] == VK_0 ? UEI_CASE_ADC_SAMPLE_MIN : UEI_CASE_ADC_SAMPLE_MAX);
        break;
    case VK_1:  // resolution selection
        rst = 0;
        if (step < 2)
            break;
        if (cmd[1] != VK_0 && cmd[1] != VK_1) {
            // wait for new sub command
            step --;
            break;
        }
        rst = 1;
        resolution = (cmd[1] == VK_0 ? UEI_CASE_ADC_RESOLUTION_MIN : UEI_CASE_ADC_RESOLUTION_MAX);
        break;
    case VK_2:  // channel selection
        chan_idx = (cmd[1] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        chan_idx *= 10;
        chan_idx += (cmd[2] - UEI_CASE_SUB_MEMU_BASE_CODE) % 10;
        if (chan_idx < 0 || chan_idx >= ARRAY_ELEM_CNT(chan_array))
            goto ADC_FAIL;

        channel = chan_array[chan_idx];

        if (chan_idx < ARRAY_ELEM_CNT(chan_list))
            gpio_chan = chan_list[chan_idx];
        break;
    case VK_3: // display
        if (sample < 0 || resolution < 0)
            break;
        wd_stop();
        // read channel
        // send voltage to serial terminal using protocol <Vx.xx><CR><NL>
        if (gpio_chan > 0) {
            gpio_set_input_en(gpio_chan, 1);
            gpio_set_output_en(gpio_chan, 0);
        }
        adc_Init(sample, channel, SINGLEEND, RV_AVDD, resolution, S_3);
        adc_data = 0;
        while (1) {
            /*
             * prevent the illegal sample data
             */
            u32 mean = 0;
            u16 adc = 0;
            for (i = 0; i < UEI_CASE_ADC_SAMPLE_CNT; i ++)
                mean += adc_SampleValueGet();
            mean = mean >> UEI_CASE_ADC_SAMPLE_CNT_BIT;
            for (i = 0; i < UEI_CASE_ADC_SAMPLE_CNT; i ++) {
                adc = adc_SampleValueGet();
                // regulate the ADC sample
                if (adc > (mean << 1) || adc < (mean >> 1))
                    adc = mean;
                adc_data += adc;
            }
            adc_data = adc_data >> UEI_CASE_ADC_SAMPLE_CNT_BIT;
            if (adc_data < 128)
                adc_data = 128;

            i = 5;
            voltage = 3300 * (adc_data - 128) / (16383 - 256);  // 2^14 - 1 = 16383;
            adc_data = 0;

            memset(serail_output_buf + i, '0', 5);  // clear the last voltage value
            while (voltage != 0 || i < 7) {
                if (i == 6)
                    serail_output_buf[i ++] = '.';
                serail_output_buf[i ++] = str[voltage / 1000];
                voltage = voltage % 1000;
                voltage = voltage * 10;
            }
            uart_Send_kma(serail_output_buf);

            sleep_us(serail_output_buf[0] * 8 * 140);
            sleep_us(1000000);
        }
        break;
    default:
        goto ADC_FAIL;
    }

    if (rst) {
        memset(cmd, 0, sizeof(cmd));
        step = 0;
    }
    return 0;

ADC_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    memset(cmd, 0, sizeof(cmd));
    step = 0;
    return 0;
}

char uei_case_single_wire()
{
    static u8 ready = 0;
    static u8 cmd = 0;
    uei_gpio_range_t range[] = {
        {VK_1, VK_3},  // to index command
    };

    u8 sw_high = 0x10;
    u8 sw_low = 0xA0;
    u8 rate = sw_low;

    const u8 sw_tx_buf[8] = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1};
    u8 sw_rx_buf[8] = {0x00};

    u8 match = 0;

    print_hex(0x05);
    print_hex(0x10);

    if (ready)
        goto PARSE_SINGLE_WIRE_CMD;

    u8 cnt = kb_event.cnt;
    u8 key = kb_event.keycode[0];

    if (cnt == 0)
        return 0;
    if (cnt != 1)
        goto SWI_FAIL;

    /*
     * if the input is invalid,
     * reset the context of current test case
     */
    if (key < range[0].start)
        goto SWI_FAIL;
    if (key != VK_0 &&
        key > range[0].end)
        goto SWI_FAIL;
    cmd = key;
PARSE_SINGLE_WIRE_CMD:
    switch (cmd) {
    case VK_0:  // Set SWI peripheral, highest baud rate as transmitter
        rate = sw_high;
    case VK_1:  // Set SWI peripheral, lowest baud rate as transmitter
        swire_speed(rate);
        wd_stop();
        swire_sync(0);
        swire_write(UEI_CASE_SWIRE_SLAVE_ADDR, (u8 *)sw_tx_buf, sizeof(sw_tx_buf));

        // indicate slave the data is ready
        ready = UEI_CASE_SWIRE_SLAVE_WR_READY_VAL;
        swire_write(UEI_CASE_SWIRE_SLAVE_READY_ADDR, &ready, sizeof(ready));

        device_led_setup(uei_led_cfg[LED_UEI_SUCCESS]);
        /*
         * GPIO_PA7 is used for SWM and ROW of keyboard
         * after SWM is done, reset GPIO_PA7 to ROW configure
         * and wait for the next input from user.
         */
        reset_swm_for_keyboard();

        ready = 0;
        wd_start();
        break;
    case VK_2:  // Set SWI peripheral, highest baud rate as receiver
        rate = sw_high;
    case VK_3:  // Set SWI peripheral, lowest baud rate as receiver
        //swire_speed(rate);
        do {
            /*
             * wait for data from master is ready.
             */
            uei_case_serail_slave_read(UEI_CASE_SWIRE_SLAVE_READY_ADDR, &ready, sizeof(ready));
            if (ready == UEI_CASE_SWIRE_SLAVE_WR_READY_VAL)
                break;
            wd_clear();
        } while (1);

        /*
         * Master has sent data to register of slave with address of UEI_CASE_SWIRE_SLAVE_ADDR + REG_BASE_ADDR
         * so slave get data from register of UEI_CASE_SWIRE_SLAVE_ADDR + 0, UEI_CASE_SWIRE_SLAVE_ADDR + 1,
         * UEI_CASE_SWIRE_SLAVE_ADDR + 2, ..., UEI_CASE_SWIRE_SLAVE_ADDR + 7 respectively.
         */
        uei_case_serail_slave_read(UEI_CASE_SWIRE_SLAVE_ADDR, sw_rx_buf, sizeof(sw_rx_buf));
        if (memcmp(sw_rx_buf, sw_tx_buf, sizeof(sw_rx_buf)) == 0)
            match = 1;
        memset(sw_rx_buf, 0, sizeof(sw_rx_buf));
        device_led_setup(uei_led_cfg[match ? LED_UEI_SUCCESS : LED_UEI_FAIL]);

        // clear ready status
        ready = UEI_CASE_SWIRE_SLAVE_RD_READY_VAL;
        uei_case_serail_slave_write(UEI_CASE_SWIRE_SLAVE_READY_ADDR, &ready, sizeof(ready));

        ready = 1;
        break;
    default:
        goto SWI_FAIL;
    }

    if (ready)
        return 1;

    ready = 0;
    cmd = 0;

    return 0;

SWI_FAIL:
    /*
     * reset the context of current test case,
     * and indicate user failure of operation
     */
    device_led_setup(uei_led_cfg[LED_UEI_FAIL]);
    return 0;
}

char uei_case_quadrature_decoder()
{
    print_hex(0x05);
    print_hex(0x11);
    return 0;
}

char uei_case_remote_firmware()
{
    print_hex(0x05);
    print_hex(0x12);
    return 0;
}

#if defined(__cplusplus)
}
#endif
