
#ifndef WIN32

#include "register.h"
#include "putchar.h"
#include "usbhw.h"

#define USB_PRINT_TIMEOUT	 10		//  about 10us at 30MHz
#define USB_SWIRE_BUFF_SIZE  248	// 256 - 8

#define USB_EP_IN  		(USB_EDP_PRINTER_IN  & 0X07)	//  from the point of HOST 's view,  IN is the printf out
#define USB_EP_OUT  	(USB_EDP_PRINTER_OUT & 0X07)

int usb_putc(int c) {
	int i = 0;
	while(i ++ < USB_PRINT_TIMEOUT){
		if(!(reg_usb_ep8_fifo_mode & FLD_USB_ENP8_FULL_FLAG)){
			reg_usb_ep_dat(USB_EP_IN) = (unsigned char)c;
			return c;
		}
	}
	return -1;
}

static inline void swire_set_clock(unsigned char div){
	reg_swire_clk_div = div;
}

static int swire_is_init = 0;
void swire_init(){
#if(USB_SOMATIC_ENABLE)
    //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#else
//	r_usb.ep_adr[USB_EP_IN] = r_usb.ep_adr[USB_EP_OUT] = 0;
	reg_usb_ep_ptr(USB_EP_IN) = reg_usb_ep_ptr(USB_EP_OUT) = 0;
	reg_usb_ep8_send_max = 64;				// 32 * 8 == 256

	//swire_set_clock(2);

#endif
}

int swire_putc(int c) {
#if(USB_SOMATIC_ENABLE)
    //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#else
	if(!swire_is_init){
		swire_init();
		swire_is_init = 1;
	}
	int i = 0;
	while(i ++ < USB_PRINT_TIMEOUT){
		if(reg_usb_ep_ptr(USB_EP_IN) - reg_usb_ep_ptr(USB_EP_OUT) <= USB_SWIRE_BUFF_SIZE){	//  not full
			reg_usb_ep_dat(USB_EP_IN) = (unsigned char)c;
			return c;
		}
	}
#endif
	return -1;
}

int putchar(int c){
	if(reg_usb_host_conn){
		swire_is_init = 0;		// should re-init swire if connect swire again
		return usb_putc((char)c);
	}else{
		return swire_putc((char)c);
	}
}

#endif

