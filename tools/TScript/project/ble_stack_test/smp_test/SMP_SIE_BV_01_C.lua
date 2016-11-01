require "smp_const"	

handle = tl_usb_init(0xffff);
print("handle : ",string.format("0x%04x",handle)) 
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print("handle bulk : ",string.format("0x%04x",handle_bulk)) 

tl_usb_bulk_monitor_start(handle_bulk)

print("\n Slave Initiated Encryption SIE BV-01-C , Slave initiates Encryption")
print("Test started");

-- 1. set MITM
MITM_en = 0
tk_value = array.new(4)
tk_value[1] = 0x00
tk_value[2] = 0x00
tk_value[3] = 0x00
tk_value[4] = 0x00
smp_set_MITM (handle, MITM_en, tk_value)

-- 2. set IO capability
smp_set_IO_capability (handle, IO_CAPABILITY_NO_INPUT_NO_OUTPUT)

tl_usb_bulk_monitor_end()