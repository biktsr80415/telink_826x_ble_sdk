require "print_table"
print("LUA Version: ".._VERSION)
print("lua begin:")
handle = 1;
print(handle);
handle = tl_usb_init(0xffff);
print(string.format("usb id = 0x%x",handle));
handle_bulk = tl_usb_bulk_monitor_init(0xffff);
print(string.format("usb id = 0x%x",handle_bulk));


tl_usb_bulk_monitor_start(handle_bulk)

alen = 5;
a = array.new(alen)
a[1] = 0x0a;
a[2] = 0xff;
a[3] = 0x01;
a[4] = 0x00;
a[5] = 0x01;
len = tl_usb_bulk_out(handle,a,5 )
print(string.format("out len = %d",len));
repeat
	r,r_len = tl_usb_bulk_read()
until(r_len>0)
print(r_len)
PrintTable(r)