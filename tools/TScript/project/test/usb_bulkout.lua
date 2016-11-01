print("LUA Version: ".._VERSION)
print("lua begin:")
handle = 1;
print(handle);
handle = tl_usb_init(0xffff);
print(string.format("usb id = 0x%x",handle));
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print(string.format("usb id = 0x%x",handle_bulk));


tl_usb_bulk_monitor_start(handle_bulk)
--tl_usb_bulk_monitor_end()


-- usb write
alen = 100;
a = array.new(alen)
a[1] = 0xf0
len = tl_usb_bulk_out(handle,a,1)
print(string.format("out len = %d",len));
tl_sleep_ms(50);
--len = tl_usb_bulk_out(handle,a,1)
tl_sleep_ms(50);
result_tbl ={};
repeat
   
   result_tbl,result_len = tl_usb_bulk_read() 
   if(result_len ~=0)
   then
     print(string.format("%c %c",result_tbl[1],result_tbl[2]))
	 break
   end	
   
-- tl_sleep_ms(50);
until (result_len>0)
tl_usb_bulk_monitor_end()


r={};
r,r2 = tl_usb_read(handle,0x8000,16)
print ("len:",r2)
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end







