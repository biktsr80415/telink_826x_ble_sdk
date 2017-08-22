/*
 * blt_pa.c
 *
 *  Created on: 2017-8-14
 *      Author: Administrator
 */

#include "../tl_common.h"
#include "rf_pa.h"



void rf_pa_init(u8 tx_pin_level, u8 rx_pin_level)
{
#if(PA_ENABLE)
    //rf_set_power_level_index (RF_POWER_0dBm);
    gpio_set_func(PA_TXEN_PIN, AS_GPIO);
    gpio_set_input_en(PA_TXEN_PIN, 0);
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, tx_pin_level);

    gpio_set_func(PA_RXEN_PIN, AS_GPIO);
    gpio_set_input_en(PA_RXEN_PIN, 0);
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, tx_pin_level);
#endif
}


#if 0
void rf_pa_tx_on(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 1);
    gpio_write(PA_TXEN_PIN, 1);
#endif
}


void rf_pa_tx_off(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);
#endif
}



void rf_pa_rx_on(void)
{
#if(PA_ENABLE)
    gpio_write(PA_RXEN_PIN, 1);
    gpio_set_output_en(PA_RXEN_PIN, 1);
#endif
}

void rf_pa_rx_off(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);
#endif
}


void rf_pa_rxOff_txON(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);
    gpio_set_output_en(PA_TXEN_PIN, 1);
    gpio_write(PA_TXEN_PIN, 1);
#endif
}

void rf_pa_txOff_rxON(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);
    gpio_set_output_en(PA_RXEN_PIN, 1);
    gpio_write(PA_RXEN_PIN, 1);
#endif
}

void rf_pa_txOff_rxOff(void)
{
#if(PA_ENABLE)
    gpio_set_output_en(PA_TXEN_PIN, 0);
    gpio_write(PA_TXEN_PIN, 0);
    gpio_set_output_en(PA_RXEN_PIN, 0);
    gpio_write(PA_RXEN_PIN, 0);
#endif
}

#endif
