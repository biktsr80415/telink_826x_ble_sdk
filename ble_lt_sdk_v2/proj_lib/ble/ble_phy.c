/*
 * ble_phy.c
 *
 *  Created on: 2017-8-17
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"

#include "ble_common.h"
#include "ll/ll.h"
#include "ble_phy.h"
#include "hci/hci.h"

#include "service/ble_ll_ota.h"

#include "../../proj/drivers/uart.h"


phy_data_t  bltPhy;


#define TX_PKT_SHARE_SAVE_RAM		1

#if TX_PKT_SHARE_SAVE_RAM //save ramcode
	u8 *pkt_phytest;
#else
	u8		pkt_phytest [64] = {
			39, 0, 0, 0,
			0, 37,
			0, 1, 2, 3, 4, 5, 6, 7
	};
#endif



extern my_fifo_t hci_rx_fifo;
extern my_fifo_t hci_tx_fifo;


extern blc_main_loop_phyTest_callback_t	blc_main_loop_phyTest_cb;


void blc_phy_initPhyTest_module(void)
{
	blc_main_loop_phyTest_cb = blc_phy_test_main_loop;
}






u8	phyTest_Channel (u8 chn)
{
	if (chn == 0)
	{
		return 37;
	}
	else if (chn < 12)
	{
		return chn - 1;
	}
	else if (chn == 12)
	{
		return 38;
	}
	else if (chn < 39)
	{
		return chn - 2;
	}
	else
	{
		return 39;
	}
}



void phyTest_PRBS9 (u8 *p, int n)
{
	//PRBS9: (x >> 1) | (((x<<4) ^ (x<<8)) & 0x100)
	u16 x = 0x1ff;
	for (int i=0; i<n; i++)
	{
		u8 d = 0;
		for (int j=0; j<8; j++)
		{
			if (x & 1)
			{
				d |= BIT(j);
			}
			x = (x >> 1) | (((x<<4) ^ (x<<8)) & 0x100);
		}
		*p++ = d;
	}
}



ble_sts_t blc_phy_setReceiverTest (u8 rx_chn)
{

	if(!bltParam.phy_en){  //must set phy mode
		blc_phy_setPhyTestEnable(BLC_PHYTEST_ENABLE);
	}

	bltPhy.pkts = 0;
	rf_set_ble_channel ( phyTest_Channel(rx_chn) );
	rf_set_rxmode ();

	return BLE_SUCCESS;
}


ble_sts_t blc_phy_setTransmitterTest (u8 tx_chn, u8 length, u8 pkt_type)
{

	if(!bltParam.phy_en){  //must set phy mode
		blc_phy_setPhyTestEnable(BLC_PHYTEST_ENABLE);
	}


	if (pkt_type == PKT_TYPE_PRBS9)
	{
		phyTest_PRBS9 (pkt_phytest + 6, length);
	}
	else if (pkt_type == PKT_TYPE_0X0F)
	{
		memset (pkt_phytest + 6, 0x0f, length);
	}
	else
	{
		memset (pkt_phytest + 6, 0x55, length);
	}
	pkt_phytest[4] = pkt_type;



	rf_set_ble_channel ( phyTest_Channel(tx_chn) );


	bltPhy.pkts = 0;
	bltPhy.tx_start = 1;


	return BLE_SUCCESS;
}


ble_sts_t blc_phy_setPhyTestEnd (u8 *pkt_num)
{
	if(!bltParam.phy_en){  //must set phy mode
		blc_phy_setPhyTestEnable(BLC_PHYTEST_ENABLE);
	}


	pkt_num[0] = U16_LO(bltPhy.pkts);
	pkt_num[1] = U16_HI(bltPhy.pkts);

	rf_set_tx_rx_off ();

	bltPhy.pkts = 0;  //clear

	return BLE_SUCCESS;
}

ble_sts_t blc_phy_reset(void)
{

	STOP_RF_STATE_MACHINE;
	rf_set_tx_rx_off ();
	reg_rf_irq_status = 0xffff;

	bltPhy.pkts = 0;

	return BLE_SUCCESS;
}


ble_sts_t blc_phy_setPhyTestEnable (u8 en)
{
	u8 r = irq_disable();


	if(en && !bltParam.phy_en)
	{
		reg_irq_mask &= ~FLD_IRQ_SYSTEM_TIMER;
		reg_irq_src = FLD_IRQ_SYSTEM_TIMER;
		reg_rf_irq_mask = 0;
		CLEAR_ALL_RFIRQ_STATUS;

		blt_state = BLS_LINK_STATE_IDLE;

		reg_dma_rf_rx_addr = (u16)(u32) (blt_rxfifo_b);  //reset RX buffer

		#if TX_PKT_SHARE_SAVE_RAM
				pkt_phytest = blt_rxfifo_b + blt_rxfifo.size; //blt_rxfifo.size at least 64 byte
				*(u32 *)pkt_phytest = 39;
				pkt_phytest[4] = 0;
				pkt_phytest[5] = 37;
		#endif


		write_reg8(0x401, 0);					//disable PN
		write_reg32 (0x800408, 0x29417671);	//accesscode: 1001-0100 1000-0010 0110-1110 1000-1110   29 41 76 71
	}
	else if(!en && bltParam.phy_en)
	{
		start_reboot();  //clear all status
	}



	bltParam.phy_en = en;


	irq_restore(r);

	return BLE_SUCCESS;
}

bool 	  blc_phy_isPhyTestEnable(void)
{
	return bltParam.phy_en;
}



int blc_phytest_cmd_handler (u8 *p, int n)
{
	//Commands and Events are sent most significant byte (MSB) first, followed
	//by the least significant byte (LSB).

	u8 pkt_type = p[1] & 3;
	u8 len =  (p[1] >> 2) & 0x3f;
	u8 chn = p[0] & 0x3f;
	bltPhy.cmd = p[0] >> 6;


	u16 phy_event = 0;


	if (bltPhy.cmd == PHY_CMD_RESET)		//reset
	{
		blc_phy_reset();
		phy_event = PHY_EVENT_STATUS | PHY_STATUS_SUCCESS;
	}
	else if (bltPhy.cmd == PHY_CMD_RX)	//rx
	{
		blc_phy_setReceiverTest(chn);
		phy_event = PHY_EVENT_STATUS | PHY_STATUS_SUCCESS;
	}
	else if (bltPhy.cmd == PHY_CMD_TX)	//tx
	{
		blc_phy_setTransmitterTest(chn, len, pkt_type);
		phy_event = PHY_EVENT_STATUS | PHY_STATUS_SUCCESS;

	}
	else  if(bltPhy.cmd == PHY_CMD_END)				//end
	{
		u16 pkt_num;
		phy_event = PHY_EVENT_PKT_REPORT | bltPhy.pkts;
		blc_phy_setPhyTestEnd((u8 *)&pkt_num);
	}

	u8 returnPara[2] = {phy_event>>8, phy_event};
	blc_hci_send_data(HCI_FLAG_EVENT_PHYTEST_2_WIRE_UART, returnPara, 2);


	return 0;
}





int blc_phy_test_main_loop(void)
{
	//phytest depend on blc_hci_rx_handler/blc_hci_tx_handler, so it must before phytest
	//------------------   HCI -------------------------------
	extern blc_hci_rx_handler_t		blc_hci_rx_handler;
	extern blc_hci_tx_handler_t		blc_hci_tx_handler;
	///////// RX //////////////
	if (blc_hci_rx_handler)
	{
		blc_hci_rx_handler ();
	}
	///////// TX //////////////
	if (blc_hci_tx_handler)
	{
		blc_hci_tx_handler ();
	}


	if (bltPhy.cmd == PHY_CMD_TX)
	{
		if (reg_rf_irq_status & FLD_RF_IRQ_TX || bltPhy.tx_start)
		{
			if(bltPhy.tx_start){
				bltPhy.tick_tx = clock_time();
				bltPhy.tx_start = 0;
			}
			else{
				bltPhy.pkts ++;
			}

			rf_start_stx (pkt_phytest, bltPhy.tick_tx + 100 * CLOCK_SYS_CLOCK_1US);
			bltPhy.tick_tx += 625 * CLOCK_SYS_CLOCK_1US;
			reg_rf_irq_status = FLD_RF_IRQ_TX;
		}
	}
	else if (bltPhy.cmd == PHY_CMD_RX)
	{
		if (reg_rf_irq_status & FLD_RF_IRQ_RX)
		{
			reg_rf_irq_status = FLD_RF_IRQ_RX;

			u8 * raw_pkt = (u8 *) blt_rxfifo_b;
			if	( RF_PACKET_LENGTH_OK(raw_pkt) && RF_PACKET_CRC_OK(raw_pkt) )
			{
				bltPhy.pkts++;
			}
		}
	}



	return 0;
}






int phy_test_2_wire_rx_from_uart (void)
{
	if(my_fifo_get(&hci_rx_fifo) == 0)
	{
		return 0;
	}

	u8* p = my_fifo_get(&hci_rx_fifo);
	u32 rx_len = p[0];

	if (rx_len)
	{
		blc_phytest_cmd_handler(&p[4], rx_len);
		my_fifo_pop(&hci_rx_fifo);
	}

	return 0;

}


typedef struct{
    unsigned int len;        // data max 252
    unsigned char data[16];
}uart_phy_t;

uart_phy_t uart_txdata_buf;

int phy_test_2_wire_tx_to_uart(void)
{
	static u32 uart_tx_tick = 0;

	u8 *p = my_fifo_get (&hci_tx_fifo);


#if (ADD_DELAY_FOR_UART_DATA)
	if (p && !uart_tx_is_busy () && clock_time_exceed(uart_tx_tick, 30000))
#else
	if (p && !uart_tx_is_busy ())
#endif
	{
		memcpy(&uart_txdata_buf.data, p + 2, p[0]+p[1]*256);
		uart_txdata_buf.len = p[0]+p[1]*256 ;

		if (uart_Send((u8 *)(&uart_txdata_buf)))
		{
			uart_tx_tick = clock_time();

			my_fifo_pop (&hci_tx_fifo);
		}
	}
	return 0;
}
