require "hci_read_loopback_mode"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)


hci_read_loopback_mode_cmd(BLE_SUCCESS)


print("end!")
tl_usb_bulk_monitor_end()


