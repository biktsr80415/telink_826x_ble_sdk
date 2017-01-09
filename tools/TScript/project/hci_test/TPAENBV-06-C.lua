require "hci_le_read_local_p256_pkey"
require "hci_le_set_event_mask"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_set_event_mask(BLE_SUCCESS, 0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00)

hci_le_read_local_p256_pkey(BLE_SUCCESS)

wait_for_event(10000, BLE_SUCCESS) --LE Read Local P256 Public Key Complete

hci_le_read_local_p256_pkey(BLE_SUCCESS)

wait_for_event(10000, BLE_SUCCESS) --LE Read Local P256 Public Key Complete

local answer
io.write("continue with this operation (y/n)? ")
repeat
   answer=tl_input_get()
until answer=='y'


print("end!")
tl_usb_bulk_monitor_end()


