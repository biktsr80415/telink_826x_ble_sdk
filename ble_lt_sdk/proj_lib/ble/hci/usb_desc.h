#pragma once

enum {
	BTUSB_USB_STRING_LANGUAGE = 0,
	BTUSB_USB_STRING_VENDOR,
	BTUSB_USB_STRING_PRODUCT,
	BTUSB_USB_STRING_SERIAL,
};

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_Association_t intfa;

	USB_Descriptor_Interface_t intf0;
	USB_Descriptor_Endpoint_t irq_in;
	USB_Descriptor_Endpoint_t bulk_in;
	USB_Descriptor_Endpoint_t bulk_out;

	USB_Descriptor_Interface_t intf1_0;
	USB_Descriptor_Endpoint_t iso_in;
	USB_Descriptor_Endpoint_t iso_out;

	USB_Descriptor_Interface_t intf1_1;
	USB_Descriptor_Endpoint_t iso_in1;
	USB_Descriptor_Endpoint_t iso_out1;

	USB_Descriptor_Interface_t intf_prn;
	USB_Descriptor_Endpoint_t endp0;
	USB_Descriptor_Endpoint_t endp1;
} BTUSB_Descriptor_Configuration_with_Printer_t;

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_Association_t intfa;

	USB_Descriptor_Interface_t intf0;
	USB_Descriptor_Endpoint_t irq_in;
	USB_Descriptor_Endpoint_t bulk_in;
	USB_Descriptor_Endpoint_t bulk_out;

	USB_Descriptor_Interface_t intf1_0;
	USB_Descriptor_Endpoint_t iso_in;
	USB_Descriptor_Endpoint_t iso_out;

	USB_Descriptor_Interface_t intf1_1;
	USB_Descriptor_Endpoint_t iso_in1;
	USB_Descriptor_Endpoint_t iso_out1;
} BTUSB_Descriptor_Configuration_t;

u8* sniffer_usbdesc_get_language(void);
u8* sniffer_usbdesc_get_vendor(void);
u8* sniffer_usbdesc_get_product(void);
u8* sniffer_usbdesc_get_serial(void);
u8* sniffer_usbdesc_get_device(void);
u8* sniffer_usbdesc_get_configuration(void);
int sniffer_usbdesc_get_configuration_size(void);


///////////////////////////////////////////////////////////////////////////////
//typedef void (*usb_bulk_out_callback_t)(u8 *p, int n, int offset);

void myusb_bulk_drv_init (void *p);	//initialization
int myusb_bulk_in (u8 *p, int n);		//send status
int myusb_bulk_proc ();				//called in main loop
void myusb_bulk_out_command (u8 *p, int n, int offset);

#define CMD_GET_VERSION                 0x00
#define CMD_SELECT_DPIMPL               0x10
#define CMD_SET_TCK_FREQUENCY           0x11
#define CMD_GET_TCK_FREQUENCY           0x12
#define CMD_MEASURE_MAX_TCK_FREQ        0x15
#define CMD_MEASURE_RTCK_RESPONSE       0x16
#define CMD_TAP_SHIFT                   0x17
#define CMD_SET_TAPHW_STATE             0x20
#define CMD_GET_TAPHW_STATE             0x21
#define CMD_TGPWR_SETUP                 0x22
