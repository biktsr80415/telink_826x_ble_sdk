require "smp_const"	

handle = tl_usb_init(0xffff);
print("handle : ",string.format("0x%04x",handle)) 
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print("handle bulk : ",string.format("0x%04x",handle_bulk)) 

tl_usb_bulk_monitor_start(handle_bulk)

print("\n OOB BV-02-C , IUT Responder - Both sides have OOB data")
print("Test started");

-- 1, set oob enable, and ob data[16]
oob_enable = 1
oob_code = array.new(16)
--PTS OOB Code 00 00 00 00 00 00 00 00 FE 12 03 6E 5A 88 9F 4D
oob_code[1] = 0x4d 
oob_code[2] = 0x9f
oob_code[3] = 0x88
oob_code[4] = 0x5a
oob_code[5] = 0x6e
oob_code[6] = 0x03
oob_code[7] = 0x12 
oob_code[8] = 0xfe
for i = 9,16 do
	oob_code[i] = 0
end

smp_set_OOB (handle, oob_enable, oob_code)

print("set oob end")

tl_usb_bulk_monitor_end()