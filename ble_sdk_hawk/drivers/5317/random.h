
#pragma once

#include "driver_config.h"
#include "register.h"

extern _attribute_ram_code_  unsigned int clock_time(void);

static inline unsigned short rand(void){
	return (unsigned short)((clock_time() & 0xffff) ^ reg_rnd_number);
}

