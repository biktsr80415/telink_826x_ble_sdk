-- require "hci_read_local_version_info"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
tl_usb_bulk_monitor_start(handle_bulk)

hci_read_local_ver_info(BLE_SUCCESS)

tl_usb_bulk_monitor_end()


