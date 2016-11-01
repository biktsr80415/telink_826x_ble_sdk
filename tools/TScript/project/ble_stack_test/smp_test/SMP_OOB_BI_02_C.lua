require "smp_const"	

handle = tl_usb_init(0xffff);
print("handle : ",string.format("0x%04x",handle)) 
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print("handle bulk : ",string.format("0x%04x",handle_bulk)) 

tl_usb_bulk_monitor_start(handle_bulk)

print("\n OOB BI-02-C , IUT Responder - Both sides have OOB data - Failure")
print("Test started");

-- 1, set MITM enable
print("set MITM enable")
MITM_en = 1
tk_value = array.new(4)
smp_set_MITM (handle, MITM_en, tk_value);
print("set MITM end")

-- 2, set oob enable, and ob data[16]
print("set oob enable")
oob_enable = 1
oob_code = array.new(16)
for i = 1,16 do
	oob_code[i] = 0
end
smp_set_OOB (handle, oob_enable, oob_code)
print("set oob end")

tl_usb_bulk_monitor_end()