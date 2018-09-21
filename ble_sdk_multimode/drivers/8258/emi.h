/*
 * emi.h
 *
 *  Created on: 2018-5-31
 *      Author: Administrator
 */

#ifndef EMI_H_
#define EMI_H_

#include "rf_drv.h"

//singletone mode
void rf_emi_single_tone(RF_PowerTypeDef power_level,signed char rf_chn);

//rx function
void rf_emi_rx(RF_ModeTypeDef mode,signed char rf_chn);
void rf_emi_rx_loop(void);
unsigned int rf_emi_get_rxpkt_cnt(void);
char rf_emi_get_rssi_avg(void);
unsigned char *rf_emi_get_rxpkt(void);

//tx cd mode
void rf_emi_tx_continue_setup(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type);
void rf_continue_mode_loop(void);

//tx brust mode
void rf_emi_tx_brust_setup(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type);
void rf_emi_tx_brust_loop(RF_ModeTypeDef rf_mode,unsigned char pkt_type);

//stop tx rx
void rf_emi_stop(void);

#endif /* EMI_H_ */
