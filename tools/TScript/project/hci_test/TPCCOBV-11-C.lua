require "hci_le_read_suggested_default_data_length_cmd"
require "hci_le_write_suggested_default_data_length_cmd"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_write_suggested_default_data_length_cmd(BLE_SUCCESS, TxOctets, TxTime)

hci_le_read_suggested_default_data_length_cmd(BLE_SUCCESS)
										 
										 										 

tl_usb_bulk_monitor_end()