
#include "usbhw.h"
#include "irq.h"
#include "bsp.h"
#include "usbhw_i.h"
#include "common/assert.h"

// Endpont8 is the alias of endpoint0
void usbhw_disable_manual_interrupt(int m) {
	BM_SET(reg_ctrl_ep_irq_mode, m);
}

void usbhw_enable_manual_interrupt(int m) {
	BM_CLR(reg_ctrl_ep_irq_mode, m);
}

void usbhw_write_ep(unsigned int ep, unsigned char * data, int len) {
	assert(ep < 8);
	reg_usb_ep_ptr(ep) = 0;

	//foreach(i,len){
	for(int i = 0; i < (len); ++i){
		reg_usb_ep_dat(ep) = data[i];
	}
	reg_usb_ep_ctrl(ep) = FLD_EP_DAT_ACK;		// ACK
}

// handy help function
void usbhw_write_ctrl_ep_u16(unsigned short v){
	usbhw_write_ctrl_ep_data(v & 0xff);
	usbhw_write_ctrl_ep_data(v >> 8);
}

unsigned short usbhw_read_ctrl_ep_u16(void){
	unsigned short v = usbhw_read_ctrl_ep_data();
	return (usbhw_read_ctrl_ep_data() << 8) | v;
} 

