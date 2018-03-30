
#pragma once
#include "register.h"


#ifdef __GNUC__
#define _ASM_NOP_				asm("tnop")
#else
#define _ASM_NOP_
#endif

