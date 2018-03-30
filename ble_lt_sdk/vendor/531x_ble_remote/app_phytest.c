/*
 * app_phytest.c
 *
 *  Created on: 2017-11-16
 *      Author: Administrator
 */


#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj/drivers/battery.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/ble_phy.h"
#include "../../proj/drivers/uart.h"


#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE )


MYFIFO_INIT(hci_rx_fifo, 20, 2);
MYFIFO_INIT(hci_tx_fifo, 20, 2);






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


		if (p && !uart_tx_is_busy ())
		{
			memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
			T_txdata_buf.len = p[0]+p[1]*256 ;

			while(T_txdata_buf.len--){
				uart_notDmaModeSendByte(T_txdata_buf.data[i]);
				i++;
			}
			my_fifo_pop (&hci_tx_fifo);
			uart_tx_tick = clock_time();
		}
		return 0;
	}
#endif




#if (BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART )
	extern my_fifo_t hci_rx_fifo;
	unsigned char uart_no_dma_rec_data[6] = {0x02,0, 0,0,0,0};
#elif(BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
	extern my_fifo_t hci_rx_fifo;
	unsigned char uart_no_dma_rec_data[20] = {0};
#endif



_attribute_ram_code_ void irq_phyTest_handler(void)
{
#if (BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART )

	if( GET_UART_NOT_DMA_IRQ() )
	{
		uart_no_dma_rec_data[4] = uart_notDmaModeRevData();
		uart_no_dma_rec_data[5] = uart_notDmaModeRevData();
		u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
		memcpy (w, uart_no_dma_rec_data, 6);
		my_fifo_next(&hci_rx_fifo);
	}

#elif(BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
	static unsigned char idx = 3;
	static unsigned char cmd_data_len = 0;

	if( GET_UART_NOT_DMA_IRQ() )
	{
		idx++;
		uart_no_dma_rec_data[idx] = uart_notDmaModeRevData();
		if( uart_no_dma_rec_data[4] == HCI_TYPE_CMD ){
			if(idx==7){  //param len
				cmd_data_len = uart_no_dma_rec_data[idx];
			}

			if(idx == cmd_data_len + 7){  //data end
				uart_no_dma_rec_data[0] = 4 + cmd_data_len;
				u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
				memcpy (w, uart_no_dma_rec_data, 8 + cmd_data_len);
				my_fifo_next(&hci_rx_fifo);
				cmd_data_len = 0;
				idx = 3;  //clear idx
				uart_no_dma_rec_data[0] = 0; //clear data
			}
		}
	}

#endif
}



extern const led_cfg_t led_cfg[];

void app_trigger_phytest_mode(void)
{
	static u8 phyTestFlag = 0;
	if(!phyTestFlag && blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){  //can not enter phytest in connection state
		phyTestFlag = 1;
		device_led_setup(led_cfg[4]);  //LED_SHINE_FAST: 4


		//adjust some rf parameters here if needed
		write_reg8(0x402, 0x2b);   //must: adjust rf packet preamble for BQB


		blc_phy_setPhyTestEnable( BLC_PHYTEST_ENABLE );
	}

}






void app_phytest_init(void)
{
	blc_phy_initPhyTest_module();

	#if (MCU_CORE_TYPE == MCU_CORE_8266)
		//8266 only PC6/PC7 uart function
		gpio_set_input_en(GPIO_PC6, 1);
		gpio_set_input_en(GPIO_PC7, 1);
		gpio_setup_up_down_resistor(GPIO_PC6, PM_PIN_PULLUP_1M);
		gpio_setup_up_down_resistor(GPIO_PC7, PM_PIN_PULLUP_1M);
		gpio_set_func(GPIO_PC6, AS_UART);
		gpio_set_func(GPIO_PC7, AS_UART);
	#else
		//8261/8267/8269 demo code use PB2/PB3, user can change them to PC2/PC3(c2/c3 must use 10K pullup) or PA6/PA7
		gpio_set_input_en(GPIO_PB2, 1);
		gpio_set_input_en(GPIO_PB3, 1);
		gpio_setup_up_down_resistor(GPIO_PB2, PM_PIN_PULLUP_1M);
		gpio_setup_up_down_resistor(GPIO_PB3, PM_PIN_PULLUP_1M);
		gpio_set_func(GPIO_PB2, AS_UART);
		gpio_set_func(GPIO_PB3, AS_UART);
	#endif


	#if (CLOCK_SYS_CLOCK_HZ == 32000000)
		uart_Init(30,8,PARITY_NONE,STOP_BIT_ONE);  //baud rate: 115200
	#elif(CLOCK_SYS_CLOCK_HZ == 24000000)
		uart_Init(12, 15, PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
	#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
		uart_Init(9, 13, PARITY_NONE, STOP_BIT_ONE); //baud rate: 115200
	#else
		need config uart clock here
	#endif


	#if	(BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART)
		uart_notDmaModeInit(2, 0, 1, 0);  //only 2 byte cmd, rx_level set 2
		blc_register_hci_handler (phy_test_2_wire_rx_from_uart, phy_test_2_wire_tx_to_uart);
	#elif(BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
		uart_notDmaModeInit(1, 0, 1, 0);
		blc_register_hci_handler (rx_from_uart_cb, tx_to_uart_cb);		//default handler
	#endif


	reg_uart_ctrl0 &= ~(FLD_UART_RX_DMA_EN|FLD_UART_TX_DMA_EN);


}








#endif  //end of  BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE
