#include "i2c_interface.h"

void tl_8261_i2c_init()
{
	i2c_pin_initial(TL_I2C_SDA,TL_I2C_SCL);

	i2c_init();	//MACRO I2C_SPEED is defined in app_config.h.
}

void test_i2c()
{
	u8 ab_data_test[2] = {0xAA,0x55};

	i2c_burst_write(I2C_SLAVE_DEVICE_ID,REG_LCD_DATA,ab_data_test,2);

}


bool tl_is_adc_ready()
{
	u8 bTmp;

	i2c_burst_read(I2C_SLAVE_DEVICE_ID,REG_ADC_STATUS,&bTmp,1);
	if( bTmp & I2C_SLAVE_ADC_RDY )
		return true;
	else
		return false;
}

u8 tl_get_sw_version()
{
	u8 bTmp;
	i2c_burst_read(I2C_SLAVE_DEVICE_ID,REG_SW_VERSION,&bTmp,1);

	return bTmp;
}

int tl_get_adc_value(u8 *bBuff, u8 len)
{
	return i2c_burst_read(I2C_SLAVE_DEVICE_ID,REG_ADC_VALUE,bBuff,len);
}

int tl_8261_write_LCD(u8 *abLcdData, u8 len)
{
	return i2c_burst_write(I2C_SLAVE_DEVICE_ID,REG_LCD_DATA,abLcdData,len);
}

int	tl_8261_write_threshold(u16 wThreshold)
{
	u8 tmp[2];
	tmp[0] = (u8)(wThreshold >> 8);
	tmp[1] = (u8)(wThreshold);
	return i2c_burst_write(I2C_SLAVE_DEVICE_ID,REG_THRESHOLD,tmp,2);
}
