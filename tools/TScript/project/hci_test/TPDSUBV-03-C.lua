require "hci_reset"
require "hci_le_set_advertise_enable"
require "hci_le_set_advertising_param"
require "basic_ble_const"
require "func_wait_for_event"
require "hci_le_set_event_mask"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_set_event_mask(BLE_SUCCESS, 0xff,0x04,0x00,0x00,0x00,0x00,0x00,0x00) 

bls_ll_setAdvParam(ADV_INTERVAL_20MS,0, ADV_INTERVAL_20MS, 0 ,ADV_TYPE_CONNECTABLE_UNDIRECTED,
				   OWN_ADDRESS_PUBLIC,BLE_ADDR_PUBLIC,  0xef, 0xcd, 0xab,
				   0x89, 0x67, 0x45,  BLT_ENABLE_ADV_37,ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_ANY ,
				   0);
				   			   
bls_ll_setAdvEnable(1,BLE_SUCCESS)

wait_for_event(10000);  --le connection complete event

--tl_sleep_ms(3000)

hci_reset(BLE_SUCCESS)



print("end!")
tl_usb_bulk_monitor_end()


