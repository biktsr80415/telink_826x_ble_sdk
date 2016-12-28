require "hci_le_set_event_mask"
require "hci_le_set_scan_enable"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)


hci_le_set_event_mask(BLE_SUCCESS,0x20,0x00,0x80,0x00,0x00,0x00,0x18,0x89)

hci_le_set_event_mask(BLE_SUCCESS,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d)
local answer
io.write("continue with this operation (y/n)? ")
repeat
   answer=tl_input_get()
until answer=='y'

hci_le_set_scan_enable(0,BLE_SUCCESS)


print("end!")
tl_usb_bulk_monitor_end()


