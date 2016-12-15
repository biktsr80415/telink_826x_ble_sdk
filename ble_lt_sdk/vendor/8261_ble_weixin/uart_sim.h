#ifndef UART_SIM_H

#define UART_SIM_H

#include "../../proj/tl_common.h"

#define ETU_US_9600					102

void uart_sim_init(u32 pin_tx);

void uart_sim_send_one_byte(u8 bData);
#endif
