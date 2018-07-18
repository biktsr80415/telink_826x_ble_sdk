#pragma once


#include "compiler.h"

unsigned char analog_read(unsigned char addr);
void analog_write(unsigned char addr, unsigned char v);

#define WriteAnalogReg	analog_write
#define ReadAnalogReg	analog_read
