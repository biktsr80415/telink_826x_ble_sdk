require "hci_le_create_conn"
require "hci_reset"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

---LE_Scan_Interval 0x0030 \LE_Scan_Window 0x0020 \Initiator_Filter_Policy 0x00 \Peer_Address_Type 0x00\ 
---Peer_Address 0xa4c13862fb1c\ Own_Address_Type 0x00\ Conn_Interval_Min 0x0020\ Conn_Interval_Max 0x0030\ 
---Conn_Latency 0x0000\ Supervision_Timeout 0x0080\ Minimum_CE_Length 0x0000\Maximum_CE_Length 0x0000
hci_le_create_conn(BLE_SUCCESS,0x30,0x00,0x20,0x00,0x00,0x00,0x1c,0xfb,0x62,0x38,0xc1,0xa4,0x00,0x20,0x00,0x30,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00)

hci_reset(BLE_SUCCESS)

local answer
io.write("continue with this operation (y/n)? ")
repeat
   answer=tl_input_get()
until answer=='y'


print("end!")
tl_usb_bulk_monitor_end()


