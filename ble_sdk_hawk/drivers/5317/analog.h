
#pragma once

#include "driver_config.h"
#include "compiler.h"

_attribute_ram_code_ unsigned char 	analog_read(unsigned char addr);
_attribute_ram_code_ void analog_write(unsigned char addr, unsigned char v);

#define WriteAnalogReg			analog_write
#define ReadAnalogReg			analog_read
