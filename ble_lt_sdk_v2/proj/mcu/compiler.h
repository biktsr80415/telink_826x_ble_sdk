
#pragma once


#define _attribute_packed_		__attribute__((packed))
#define _attribute_aligned_(s)	__attribute__((aligned(s)))
#define _attribute_session_(s)	__attribute__((section(s)))
#define _attribute_ram_code_  	_attribute_session_(".ram_code")
#define _attribute_custom_code_  	_attribute_session_(".custom") volatile
#define _attribute_no_inline_   __attribute__((noinline)) 

#define _inline_ 				inline				//   C99 meaning


#define _attribute_data_retention_   __attribute__((section(".retention_data")))
#define _attribute_bss_retention_    __attribute__((section(".retention_bss")))
