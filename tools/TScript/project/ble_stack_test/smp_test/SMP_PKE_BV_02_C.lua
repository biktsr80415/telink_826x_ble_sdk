require "smp_const"	

handle = tl_usb_init(0xffff);
print("handle : ",string.format("0x%04x",handle)) 
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print("handle bulk : ",string.format("0x%04x",handle_bulk)) 

tl_usb_bulk_monitor_start(handle_bulk)

print("\n PassKey Entry BV-02-C , IUT Responder with Keyboard - Success")
print("Test started");

-- 1. set io display only
smp_set_IO_capability (handle, IO_CAPABILITY_DISPLAY_ONLY)

-- 2. set oob flag 0.
oob_enable = 0
oob_value = array.new(16)
for i = 1,16 do
	oob_value[i] = 0
end
smp_set_OOB(handle, oob_enable, oob_value)

-- 3. set MITM
MITM_en = 1
tk_value = array.new(4)
tk_value [1] = 0x07 
tk_value [2] = 0xb2
tk_value [3] = 0x01
tk_value [4] = 0x00
smp_set_MITM (handle, MITM_en, tk_value)

tl_usb_bulk_monitor_end()