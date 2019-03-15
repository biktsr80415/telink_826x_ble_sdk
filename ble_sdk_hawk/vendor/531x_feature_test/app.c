#include "app.h"
#include <stack/ble/ble.h>
#include "tl_common.h"
#include "drivers.h"
#include "app_config.h"
#include "vendor/common/blt_led.h"
#include "vendor/common/keyboard.h"
#include "vendor/common/tl_audio.h"

#if (FEATURE_TEST_MODE)
	MYFIFO_INIT(hci_rx_fifo, 72, 2);
	MYFIFO_INIT(hci_tx_fifo, 72, 8);
#endif

#if (BLE_LONG_PACKET_ENABLE)
	MYFIFO_INIT(blt_rxfifo, 96, 8);
	MYFIFO_INIT(blt_txfifo, 240, 16);
#else
	MYFIFO_INIT(blt_rxfifo, 64, 8);
	MYFIFO_INIT(blt_txfifo, 40, 16);
#endif

#if (BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
	int rx_from_uart_cb (void)
	{
		if(my_fifo_get(&hci_rx_fifo) == 0)
		{
			return 0;
		}

		u8* p = my_fifo_get(&hci_rx_fifo);
		u32 rx_len = p[0]; //usually <= 255 so 1 byte should be sufficient

		if (rx_len)
		{
			blc_hci_handler(&p[4], rx_len - 4);
			my_fifo_pop(&hci_rx_fifo);
		}

		return 0;
	}


	int tx_to_uart_cb (void)
	{
		uart_data_t T_txdata_buf;
		static u32 uart_tx_tick = 0;
		unsigned char i = 0;
		u8 *p = my_fifo_get (&hci_tx_fifo);


	#if (ADD_DELAY_FOR_UART_DATA)
		if (p && !uart_tx_is_busy () && clock_time_exceed(uart_tx_tick, 30000))
	#else
		if (p && !uart_tx_is_busy ())
	#endif
		{
			memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
			T_txdata_buf.len = p[0]+p[1]*256 ;
			uart_dma_send((unsigned char*)&T_txdata_buf);

			my_fifo_pop (&hci_tx_fifo);
			uart_tx_tick = clock_time();
		}
		return 0;
	}
#endif

void user_init()
{
	blc_app_loadCustomizedParameters();

	rf_set_power_level_index (RF_POWER_m5dBm);

	u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
		memcpy (tbl_mac, pmac, 6);
	}
	else{
		tbl_mac[0] = (u8)rand();
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
	}
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

#if(FEATURE_TEST_MODE == TEST_BLE_PHY)

	write_reg8(0x402, 0x2b);   //set rf packet preamble for BQB
	blc_phy_initPhyTest_module();
	blc_phy_setPhyTestEnable( BLC_PHYTEST_ENABLE );
	blc_phy_preamble_length_set(12);


	#if(BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART || BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)  //uart
		#if (MCU_CORE_TYPE == MCU_CORE_5317)
			uart_pin_set(UART_TX_PA1, UART_RX_PA2);
			uart_reset();

		#elif(MCU_CORE_TYPE == MCU_CORE_5316)
			gpio_set_input_en(GPIO_PA3, 1);
			gpio_set_input_en(GPIO_PA4, 1);
			gpio_setup_up_down_resistor(GPIO_PA3, PM_PIN_PULLUP_1M);
			gpio_setup_up_down_resistor(GPIO_PA4, PM_PIN_PULLUP_1M);
			gpio_set_func(GPIO_PA3, AS_UART);
			gpio_set_func(GPIO_PA4, AS_UART);
		#endif
	#endif


	uart_rx_buff_init(hci_rx_fifo_b, hci_rx_fifo.size);
	uart_init(115200,PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
	uart_dma_mode_init(0,1);

	#if	(BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART)
		blc_register_hci_handler (phy_test_2_wire_rx_from_uart, phy_test_2_wire_tx_to_uart);
	#elif(BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
		blc_register_hci_handler (rx_from_uart_cb, tx_to_uart_cb);		//default handler
	#endif
#endif
}

u32 tick_loop=0;
/*----------------------------------------------------------------------------*/
/*-------- Main Loop                                                ----------*/
/*----------------------------------------------------------------------------*/
void main_loop (void)
{
	tick_loop++;
	blt_sdk_main_loop();

}

/*----------------------------- End of File ----------------------------------*/


