require "hci_le_create_conn"
require "hci_reset"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_create_conn(BLE_SUCCESS,0x30,0x00,0x20,0x00,0x00,0x00,0xf3,0xe1,0xe2,0xe3,0xe4,0xc7,0x00,0x20,0x00,0x30,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00)

-- hci_reset(BLE_SUCCESS)

local answer
io.write("continue with this operation (y/n)? ")
repeat
   answer=tl_input_get()
until answer=='y'


print("end!")
tl_usb_bulk_monitor_end()


