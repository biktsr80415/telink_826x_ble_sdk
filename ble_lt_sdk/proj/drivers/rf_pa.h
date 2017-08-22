/*
 * blt_pa.h
 *
 *  Created on: 2017-8-14
 *      Author: Administrator
 */

#ifndef BLT_PA_H_
#define BLT_PA_H_

#include "../tl_common.h"


#ifndef PA_ENABLE
#define PA_ENABLE                           0
#endif



#ifndef PA_TXEN_PIN
#define PA_TXEN_PIN                         GPIO_PD4
#endif

#ifndef PA_RXEN_PIN
#define PA_RXEN_PIN                         GPIO_PD5
#endif



#if 1
static inline void rf_pa_tx_on(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 1);
    gpio_write(PA_TXEN_PIN, 1);
#endif
}


static inline void rf_pa_tx_off(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);
#endif
}



static inline void rf_pa_rx_on(void)
{
#if(PA_ENABLE)
    gpio_write(PA_RXEN_PIN, 1);
    gpio_set_output_en(PA_RXEN_PIN, 1);
#endif
}

static inline void rf_pa_rx_off(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);
#endif
}


static inline void rf_pa_rxOff_txON(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);
    gpio_set_output_en(PA_TXEN_PIN, 1);
    gpio_write(PA_TXEN_PIN, 1);
#endif
}

static inline void rf_pa_txOff_rxON(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);
    gpio_set_output_en(PA_RXEN_PIN, 1);
    gpio_write(PA_RXEN_PIN, 1);
#endif
}

static inline void rf_pa_txOff_rxOff(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);
#endif
}

#else
void rf_pa_tx_on(void);
void rf_pa_tx_off(void);
void rf_pa_rx_on(void);
void rf_pa_rx_off(void);
void rf_pa_rxOff_txON(void);
void rf_pa_txOff_rxON(void);
void rf_pa_txOff_rxOff(void);
#endif

void rf_pa_init(u8 tx_pin_level, u8 rx_pin_level);


#endif /* BLT_PA_H_ */
