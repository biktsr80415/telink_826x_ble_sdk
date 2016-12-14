#ifndef I2C_INTERFACE_H

#define I2C_INTERFACE_H

#include "../../proj/tl_common.h"
//////////////////////////////////////////////
// I2C Pin.
//for 8261, only PB6 & PB7.
//However, on 8267 EVK , PB6 & PB7 are not used. so just select PC0 & PC1 here.

#define TL_I2C_SDA 		GPIO_PC0
#define TL_I2C_SCL		GPIO_PC1

//#define TL_I2C_SDA	GPIO_PB6
//#define TL_I2C_SCL	GPIO_PB7


/////////////////////////////////////////////
#define I2C_SLAVE_DEVICE_ID			0xA0

//below register addr for read.
#define REG_SW_VERSION				0x80
#define REG_ADC_STATUS				0x81
#define REG_ADC_VALUE				0x82

//below register addr for write.
#define REG_LCD_DATA				0x83
#define REG_THRESHOLD				0x89
////////////////////////////////////////////
#define I2C_SLAVE_ADC_RDY			0x10

////////////////////////////////////////////
void 	tl_8261_i2c_init();
bool	tl_is_adc_ready();
u8		tl_get_sw_version();
int 	tl_get_adc_value(u8 *bBuff, u8 len);

int 	tl_8261_write_LCD(u8 *abLcdData, u8 len);
int 	tl_8261_write_threshold(u16 wThreshold);


void test_i2c();	//for debug.

#endif
