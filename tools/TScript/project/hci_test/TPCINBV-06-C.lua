require "hci_le_read_white_list_size"
require "hci_le_clear_white_list"
require "hci_le_add_device_2_white_list"
require "hci_le_remove_device_to_whitelist"

handle = tl_usb_init(0xffff);
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
--- print("handle : ",string.format("0x%04x",handle_bulk)) 
tl_usb_bulk_monitor_start(handle_bulk)

hci_le_clear_white_list(BLE_SUCCESS)

s,WLs=hci_le_read_white_list_size(BLE_SUCCESS)

i = 0
flg = 0;
mac = array.new(8)
if WLs >= 1 and WLs <= 8 and s == BLE_SUCCESS then
	mac[1] =0x11
	mac[2] =0x22
	mac[3] =0x33
	mac[4] =0x44
	mac[5] =0x55
	mac[6] =0x66
	mac[7] =0x77
	mac[8] =0x88
	
	print(string.format("Repeat %x times to send HCI CMD: hci_le_add_dev_2_white_list", WLs))
	repeat
	   i = i + 1
	   ss,sr=hci_le_add_dev_2_white_list(BLE_SUCCESS, mac[i],0x34,0x12,0xEF,0xCD,0xAB)
	   tl_sleep_ms(50)
	   if(sr ~= BLE_SUCCESS) then
	       flg = 1
	       print(string.format("Add Whitlist error. %x times!", i))
	   end
	until i == WLs or flg == 1
	
	if flg == 0 then
		hci_le_add_dev_2_white_list(0x07,0xaa,0x34,0x12,0xEF,0xCD,0xAB)--ERROR CODE:0x07
		hci_le_remove_device_to_whitelist(BLE_SUCCESS,0x00,0x11,0x34,0x12,0xEF,0xCD,0xAB)--remove public mac 
		hci_le_add_dev_2_white_list(BLE_SUCCESS,0xaa,0x34,0x12,0xEF,0xCD,0xAB)
	end	
else
	print("White list size ERROR")
	
end


print("end!")
tl_usb_bulk_monitor_end()


