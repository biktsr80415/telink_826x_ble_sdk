require "hci_le_set_scan_enable"
require "hci_le_set_scan_parameter"
require "hci_const"
require "wait_for_le_report_event"
require "basic_ble_const"
require "hci_le_set_event_mask"
require "hci_le_set_advertising_data"
require "hci_le_scan_response_data"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_set_event_mask(BLE_SUCCESS, 0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00) 
hci_le_set_scan_parameter(BLE_SUCCESS, SCAN_TYPE_PASSIVE, SCAN_INTERVAL_330MS, SCAN_INTERVAL_330MS,
										   OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY)

--bls_ll_setAdvData(BLE_SUCCESS, 31, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
--bls_ll_setScanRspData(BLE_SUCCESS, 31, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

hci_le_set_scan_enable(1,BLE_SUCCESS)
										   
wait_for_le_report_event(10000, DATA_CHECK_ENABLE, LL_TYPE_ADV_IND, BLE_ADDR_PUBLIC, 0x456789ab, 0xcdef)

hci_le_set_scan_enable(0,BLE_SUCCESS)

wait_for_le_report_event(3000, CHECK_NO_MORE_EVENT, 0, 0, 0, 0)	  --check no other adv event come


print("end!")
tl_usb_bulk_monitor_end()


