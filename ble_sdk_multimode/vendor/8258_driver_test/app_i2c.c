#include "tl_common.h"
#include "drivers.h"


extern void i2c_master_test_init(void);
extern void i2c_slave_test_init(void);
extern void	i2c_master_mainloop(void);
extern void	i2c_slave_mainloop(void);


void app_i2c_test_init(void)
{
	WaitMs(2000);  //leave enough time for SWS_reset when power on


#if(I2C_DEMO_SELECT == I2C_DEMO_MASTER)

	i2c_master_test_init();

#elif(I2C_DEMO_SELECT == I2C_DEMO_SLAVE)

	i2c_slave_test_init();

#endif
}



void app_i2c_test_start(void)
{
#if (I2C_DEMO_SELECT == I2C_DEMO_MASTER)   //master demo mainloop


	i2c_master_mainloop();


#elif(I2C_DEMO_SELECT == I2C_DEMO_SLAVE)  //slave demo mainloop

	i2c_slave_mainloop();

#endif
}
