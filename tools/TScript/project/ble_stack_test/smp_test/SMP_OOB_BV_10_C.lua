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
for i = 1,16 do
	oob_code[i] = 0
end

smp_set_OOB (handle, oob_enable, oob_code)

print("set oob end")

tl_usb_bulk_monitor_end()