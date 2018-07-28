
#include "register.h"
#include "clock.h"
#include "irq.h"
#include "analog.h"

#if 0
void clock_init(SYS_CLK_TYPEDEF SYS_CLK)
{
    switch(SYS_CLK)
    {
    	case SYS_CLK_12M_Crystal:
    	    WRITE_REG8(0x66,0x44);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    	case SYS_CLK_16M_Crystal:
    	    WRITE_REG8(0x66,0x43);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    	case SYS_CLK_24M_Crystal:
    	    WRITE_REG8(0x66,0x42);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    	case SYS_CLK_32M_Crystal:
    	    WRITE_REG8(0x66,0x60);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    	case SYS_CLK_48M_Crystal:
    	    WRITE_REG8(0x66,0x20);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    	case SYS_CLK_24M_RC:
    	    WRITE_REG8(0x66,0x00);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    	default:
    	    WRITE_REG8(0x66,0x00);
    	    WRITE_REG8(0x70,READ_REG8(0x70)&0xfe);
    		break;
    }
}
#endif


_attribute_ram_code_ void sleep_us (unsigned long us)
{
	unsigned long t = clock_time();
	while(!clock_time_exceed(t, us)){
	}
}


