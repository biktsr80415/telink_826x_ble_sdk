require "hci_read_local_supported_features"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
tl_usb_bulk_monitor_start(handle_bulk)


hci_read_local_supported_features(BLE_SUCCESS)


tl_usb_bulk_monitor_end()


