
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


#include "config.h"

//////////// product  Infomation  //////////////////////////////
#ifndef ID_VENDOR
#define ID_VENDOR			0x248a			// for report 
#endif
#ifndef ID_PRODUCT_BASE
#define ID_PRODUCT_BASE		0x8800
#endif
#ifndef STRING_VENDOR
#define STRING_VENDOR		L"Telink"
#endif
#ifndef STRING_PRODUCT
#define STRING_PRODUCT		L"2.4G Wireless Audio"
#endif
#ifndef STRING_SERIAL
#define STRING_SERIAL		L"TLSR8869"
#endif



//#ifndef MODULE_WATCHDOG_ENABLE
//#define MODULE_WATCHDOG_ENABLE	0
//#endif
//
//
//
//#ifndef WATCHDOG_INIT_TIMEOUT
//#define WATCHDOG_INIT_TIMEOUT		200		//  in ms
//#endif




///////////////////  USB   /////////////////////////////////
#ifndef IRQ_USB_PWDN_ENABLE
#define	IRQ_USB_PWDN_ENABLE  	0
#endif


#ifndef USB_PRINTER_ENABLE
#define	USB_PRINTER_ENABLE 		0
#endif
#ifndef USB_SPEAKER_ENABLE
#define	USB_SPEAKER_ENABLE 		0
#endif
#ifndef USB_MIC_ENABLE
#define	USB_MIC_ENABLE 			0
#endif
#ifndef USB_MOUSE_ENABLE
#define	USB_MOUSE_ENABLE 			0
#endif
#ifndef USB_KEYBOARD_ENABLE
#define	USB_KEYBOARD_ENABLE 		0
#endif
#ifndef USB_SOMATIC_ENABLE
#define	USB_SOMATIC_ENABLE 		0
#endif
#ifndef USB_CUSTOM_HID_REPORT
#define	USB_CUSTOM_HID_REPORT 		0
#endif
#ifndef USB_AUDIO_441K_ENABLE
#define USB_AUDIO_441K_ENABLE  	0
#endif
#ifndef USB_MASS_STORAGE_ENABLE
#define USB_MASS_STORAGE_ENABLE  	0
#endif
#ifndef MIC_CHANNLE_COUNT
#define MIC_CHANNLE_COUNT  			2
#endif

#ifndef USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE
#define USB_DESCRIPTER_CONFIGURATION_FOR_KM_DONGLE  			0
#endif

#ifndef USB_ID_AND_STRING_CUSTOM
#define USB_ID_AND_STRING_CUSTOM  								0
#endif

#define KEYBOARD_RESENT_MAX_CNT			3
#define KEYBOARD_REPEAT_CHECK_TIME		300000	// in us	
#define KEYBOARD_REPEAT_INTERVAL		100000	// in us	
#define KEYBOARD_SCAN_INTERVAL			16000	// in us
#define MOUSE_SCAN_INTERVAL				8000	// in us	
#define SOMATIC_SCAN_INTERVAL     		8000

#define USB_KEYBOARD_POLL_INTERVAL		10		// in ms	USB_KEYBOARD_POLL_INTERVAL < KEYBOARD_SCAN_INTERVAL to ensure PC no missing key
#define USB_MOUSE_POLL_INTERVAL			4		// in ms
#define USB_SOMATIC_POLL_INTERVAL     	8		// in ms

#define USB_KEYBOARD_RELEASE_TIMEOUT    (450000) // in us
#define USB_MOUSE_RELEASE_TIMEOUT       (200000) // in us
#define USB_SOMATIC_RELEASE_TIMEOUT     (200000) // in us


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

