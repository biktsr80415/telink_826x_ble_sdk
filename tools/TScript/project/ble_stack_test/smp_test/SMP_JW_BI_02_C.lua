require "smp_const"	

handle = tl_usb_init(0xffff);
print("handle : ",string.format("0x%04x",handle)) 
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print("handle bulk : ",string.format("0x%04x",handle_bulk)) 

tl_usb_bulk_monitor_start(handle_bulk)

print("\n Just Works BI-02-C , IUT Responder -Failure")
print("Test started");

-- 1, oob set to 0
print ("\n 1, set oob disable");
enable = 1
oob_code = array.new(16)
for i=1, 16 do
	oob_code [i] = 0
end
smp_set_OOB (handle, enable, oob_code)
print ("\n 1, set oob disable end");

tl_usb_bulk_monitor_end()