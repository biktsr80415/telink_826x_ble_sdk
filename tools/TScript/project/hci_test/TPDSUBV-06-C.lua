require "hci_le_create_conn"
require "hci_reset"
require "func_wait_for_event"
require "hci_le_set_event_mask"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_set_event_mask(BLE_SUCCESS,0xFF,0x04,0x00,0x00,0x00,0x00,0x00,0x00)

---LE_Scan_Interval 0x00A0 \LE_Scan_Window 0x00A0 \Initiator_Filter_Policy 0x00 \Peer_Address_Type 0x00\ 
---Peer_Address 0xa4c13862fb1c\ Own_Address_Type 0x00\ Conn_Interval_Min 0x0020\ Conn_Interval_Max 0x0030\ 
---Conn_Latency 0x0000\ Supervision_Timeout 0x0080\ Minimum_CE_Length 0x0000\Maximum_CE_Length 0x0000
hci_le_create_conn(BLE_SUCCESS, 160, 160, 0, 0, 0x4567, 0x89ab, 0xcdef, 0, 32, 32,  0,  400,    0, 0)

wait_for_event(10000);  --le connection complete event


hci_reset(BLE_SUCCESS)


print("end!")
tl_usb_bulk_monitor_end()

