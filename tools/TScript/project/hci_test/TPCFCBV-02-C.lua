require "hci_le_read_buf_size"
require "hci_read_buffer_size"


handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
tl_usb_bulk_monitor_start(handle_bulk)


hci_le_read_buf_size(BLE_SUCCESS)

hci_read_buf_size(BLE_SUCCESS)


tl_usb_bulk_monitor_end()


