#pragma once

#if (UART_PRINT_DEBUG_ENABLE || USB_PRINT_DEBUG_ENABLE)//print info by a gpio or usb printer
	int  u_printf(const char *fmt, ...);
	int  u_sprintf(char* s, const char *fmt, ...);
	void u_array_printf(unsigned char*data, unsigned int len);

	#define printf	 		u_printf
	#define sprintf	 		u_sprintf
    #define array_printf	u_array_printf
#else
	#define printf
	#define sprintf
	#define array_printf
#endif

