require "hci_le_create_conn"
require "hci_reset"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

---LE_Scan_Interval 0x00A0 \LE_Scan_Window 0x00A0 \Initiator_Filter_Policy 0x00 \Peer_Address_Type 0x00\ 
---Peer_Address 0xa4c13862fb1c\ Own_Address_Type 0x00\ Conn_Interval_Min 0x0020\ Conn_Interval_Max 0x0030\ 
---Conn_Latency 0x0000\ Supervision_Timeout 0x0080\ Minimum_CE_Length 0x0000\Maximum_CE_Length 0x0000
hci_le_create_conn(BLE_SUCCESS,0xA0,0x00,0xA0,0x00,0x00,0x00,0x1c,0xfb,0x62,0x38,0xc1,0xa4,0x00,0x18,0x00,0x18,0x00,0x00,0x00,0x90,0x01,0x00,0x00,0x00,0x00)
--hci_le_create_conn(BLE_SUCCESS, 160, 128, 0, 0, 0xa4c1, 0x3862, 0xfb1c, 0, 32, 32,  0,  400,    0, 0)
--wait_for_event(10000);  --le connection complete event


hci_reset(BLE_SUCCESS)


print("end!")
tl_usb_bulk_monitor_end()


