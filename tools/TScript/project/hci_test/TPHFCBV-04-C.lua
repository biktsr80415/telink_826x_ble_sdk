require "hci_le_set_event_mask"
require "hci_le_set_scan_enable"
require "hci_le_set_scan_parameter"
require "hci_le_create_conn"
require "func_wait_for_event"
require "hci_le_clear_white_list"
require "hci_le_add_device_2_white_list"
require "hci_le_set_advertising_data"
require "hci_le_scan_response_data"
require "basic_ble_const"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

--hci_le_clear_white_list(BLE_SUCCESS)
--hci_le_add_dev_2_white_list(BLE_SUCCESS,0x1c,0xfb,0x62,0x38,0xc1,0xa4)

hci_le_set_event_mask(BLE_SUCCESS,0x89,0x18,0x00,0x00,0x00,0x80,0x00,0x20)

hci_le_set_event_mask(BLE_SUCCESS,0x1d,0x00,0x00,0x00,0x00,0x00,0x00,0x00) 

hci_le_set_scan_parameter(BLE_SUCCESS, SCAN_TYPE_ACTIVE, SCAN_INTERVAL_330MS, SCAN_INTERVAL_330MS,
										   OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY)

--bls_ll_setAdvData(BLE_SUCCESS, 31, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
--bls_ll_setScanRspData(BLE_SUCCESS, 31, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

hci_le_set_scan_enable(1,BLE_SUCCESS)

hci_le_set_scan_enable(0,BLE_SUCCESS)


---LE_Scan_Interval 0x00A0 \LE_Scan_Window 0x00A0 \Initiator_Filter_Policy 0x00 \Peer_Address_Type 0x00\ 
---Peer_Address 0xa4c13862fb1c\ Own_Address_Type 0x00\ Conn_Interval_Min 0x0020\ Conn_Interval_Max 0x0030\ 
---Conn_Latency 0x0000\ Supervision_Timeout 0x0080\ Minimum_CE_Length 0x0000\Maximum_CE_Length 0x0000
hci_le_create_conn(BLE_SUCCESS,0xA0,0x00,0xA0,0x00,0x00,0x00,0x1c,0xfb,0x62,0x38,0xc1,0xa4,0x00,0x18,0x00,0x18,0x00,0x00,0x00,0x90,0x01,0x00,0x00,0x00,0x00)
--hci_le_create_conn(160, 160, 0, 0, 0xa4c1, 0x3862, 0xfb1c, 0, 24, 24,  0,  400,    0, 0,BLE_SUCCESS)
wait_for_event(10000);  --le connection complete event
--wait_for_event(20000, BLE_SUCCESS)   --le disconnection event
print("end!")
tl_usb_bulk_monitor_end()


