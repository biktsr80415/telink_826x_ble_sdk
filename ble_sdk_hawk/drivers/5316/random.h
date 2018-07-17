
#pragma once

#include "register.h"

static inline unsigned short rand(void){
	return (unsigned short)((clock_time() & 0xffff) ^ reg_rnd_number);
}

