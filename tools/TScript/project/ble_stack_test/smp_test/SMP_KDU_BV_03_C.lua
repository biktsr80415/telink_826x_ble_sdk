require "smp_const"	

handle = tl_usb_init(0xffff);
print("handle : ",string.format("0x%04x",handle)) 
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print("handle bulk : ",string.format("0x%04x",handle_bulk)) 

tl_usb_bulk_monitor_start(handle_bulk)

print("\n Key Distribution and Usage BV-03-C , IUT Responder - Tester sets Sign bit")
print("Test started");

-- 1, set key, ltk = 1, irk = 0 , csrk = 1
ltk_en  = 0
irk_en  = 0
csrk_en = 1
smp_set_keyDistribute (handle, ltk_en, irk_en, csrk_en)

tl_usb_bulk_monitor_end()