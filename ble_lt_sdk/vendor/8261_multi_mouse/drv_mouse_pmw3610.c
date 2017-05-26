/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision$
 */

#include "../../proj/common/string.h"
#include "drv_mouse_pmw3610.h"
#include "mouse_spi.h"

/*lint ++flb "Enter library region" */

#define PMW3610_PRODUCT_ID (0x3EU) /*!< PMW3610 product id */
#define PMW3610_RESET_NUMBER (0x5AU) /*!< PMW3610 reset code */

/* PMW3610 register addresses */
#define REG_PROD_ID (0x00U) /*!< Product ID. Default value : 0x3E */
#define REG_REV_ID (0x01U) /*!< Revision ID. Default value : 0x01 */
#define REG_MOTION_ST (0x02U) /*!< Motion Status. Default value : 0x09 */
#define REG_DELTA_X_L (0x03U) /*!< Lower byte of Delta_X. Default value : 0x00 */
#define REG_DELTA_Y_L (0x04U) /*!< Lower byte of Delta_Y. Default value : 0x00 */
#define REG_DELTA_XY_H (0x05U) /*!< Upper 4 bits of Delta X and Y displacement. Default value : 0x00 */
#define REG_SQUAL (0x06U) /*!< Squal Quality. Default value : 0x00 */
#define REG_SHUT_HI (0x07U) /*!< Shutter Open Time (Upper 8-bit). Default value : 0x00 */
#define REG_SHUT_LO (0x08U) /*!< Shutter Open Time (Lower 8-bit). Default value : 0x64 */
#define REG_PIX_MAX (0x09U) /*!< Maximum Pixel Value. Default value : 0xD0 */
#define REG_PIX_ACCUM (0x0AU) /*!< Average Pixel Value. Default value : 0x80 */
#define REG_PIX_MIN (0x0BU) /*!< Minimum Pixel Value. Default value : 0x00 */
#define REG_CRC0 (0x0CU) /*Default value : 0x00 */
#define REG_CRC1 (0x0DU)
#define REG_CRC2 (0x0EU)
#define REG_CRC3 (0x0FU)
#define REG_SELF_TEST (0x10) /* Default value : 0x00 */
#define REG_PERFORMANCE (0x11U) /*!< Performance. Default value : 0x01 */
#define REG_BURST_READ (0x12U) /* Default value : 0x0b */
#define REG_SET_GAIN_3X (0x13U) /*set gain value */
#define REG_RUN_DOWNSHIFT (0x1BU) /*!< Run to Rest1 Time. Default value : 0x02 */
#define REG_REST1_PERIOD (0x1CU) /*!< Rest1 Period. Default value : 0x04 */
#define REG_REST1_DOWNSHIFT (0x1DU) /*!< Rest1 to Rest2 Time. Default value : 0x1f */
#define REG_REST2_PERIOD (0x1EU) /*!< Rest2 Period. Default value : 0x0a */
#define REG_REST2_DOWNSHIFT (0x1FU) /*!< Rest2 to Rest3 Time. Default value : 0x2f */
#define REG_REST3_PERIOD (0x20U) /*!< Rest3 Period. Default value : 0x32 */
#define REG_POWER_LEVEL_INCREASE (0x21U) /* Increase laser power level */
#define REG_COMP_REG21   (0x22U)  /*complementary to REG 21U */
#define REG_OBSERVATION (0x2DU)
#define REG_DTEST2 (0x32U)
#define REG_PIXEL_GRAB (0x35U)
#define REG_FRAME_GRAB (0x36U)
#define REG_RESET (0x3AU)
#define REG_SHUTDOWN (0x3BU)
#define REG_NOT_REV_ID (0x3EU) /*!< Inverted Revision ID. Default value : 0xfe */
#define REG_NOT_PROD_ID (0x3fU) /*!< Inverted Revision ID. Default value : 0xff */
#define REG_SPI_CLK_ON_REQ (0x41U)
#define REG_PRBS_TEST_CTL (0x47U)  /* Default value : 0x00 */
#define REG_SPI_PAGE0 (0x7FU)
#define REG_RES_STEP (0x85U) /* Default value : 0x86 */
#define REG_VCSEL_CTL (0x9eU) /* Default value : 0x00 */
#define REG_LSR_CTL (0x9fU) /* Default value : 0x00 */
#define REG_SPI_PAGE1 (0xffU) /* Default value : 0x00 */

#define PMW3610_SS_PIN   	IO_PMW3610_CSN_PIN
#define PMW3610_SCK_PIN  	IO_PMW3610_SCK_PIN
#define PMW3610_MOSI_PIN 	IO_PMW3610_MOSI_PIN
#define PMW3610_MISO_PIN 	IO_PMW3610_MISO_PIN

#define SPI_CLOCK_ON	0xBA
#define SPI_CLOCK_OFF	0xB5
#define SPI_PAGE0 0x00
#define SPI_PAGE1 0xFF
#define SMART_ON 0x00
#define SMART_OFF 0x80
#define SET_GIN_3X 0xC0
#define SET_LASER_POWER 0xC3 /*Set laser to 650uW*/
#define COMP_REG21 0x3C 



/**
 * PMW3610 motion output pin polarity values.
 */
typedef enum
{
  PMW3610_MOTION_OUTPUT_POLARITY_LOW = 0,  /*!< Motion output polarity active low */
  PMW3610_MOTION_OUTPUT_POLARITY_HIGH = 1  /*!< Motion output polarity active high */
} motion_output_polarity_t;

/**
 * Motion output pin configuration.
 */
typedef enum
{
  PMW3610_MOTION_OUTPUT_SENSITIVITY_LEVEL = 0,  /*!< Motion output pin will be driven low/high (depending on the polarity setting) as long as there is motion data in DELTA registers */
  PMW3610_MOTION_OUTPUT_SENSITIVITY_EDGE = 1  /*!< Motion output pin will be driven low/high (depending on the polarity setting) for 380 ns when motion is detected during rest modes */
} motion_output_sensitivity_t;

/**
 * Mouse sensor resolution values.
 */
typedef enum
{
  PMW3610_RESOLUTION_800DPI = 0, /*!< 800 dpi resolution */
  PMW3610_RESOLUTION_1200DPI = 1, /*!< 1200 dpi resolution */
  PMW3610_RESOLUTION_1600DPI = 2, /*!< 1600 dpi resolution */
  PMW3610_RESOLUTION_2000DPI = 3, /*!< 2000 dpi resolution */
  PMW3610_RESOLUTION_2400DPI = 4, /*!< 2400 dpi resolution */
  PMW3610_RESOLUTION_2800DPI = 5, /*!< 2800 dpi resolution */
  PMW3610_RESOLUTION_3200DPI = 6 /*!< 3200 dpi resolution */
} pmw3610_resolution_t;

/**
 * Mouse sensor forced mode options.
 */
typedef enum
{
  PMW3610_MODE_NORMAL = 0, /*!< Normal operation mode */
  PMW3610_MODE_REST1 = 1, /*!< Rest1 operation mode */
  PMW3610_MODE_REST2 = 2, /*!< Rest2 operation mode */
  PMW3610_MODE_REST3 = 3, /*!< Rest3 operation mode */
  PMW3610_MODE_RUN1 = 4, /*!< Run1 operation mode */
  PMW3610_MODE_RUN2 = 5, /*!< Run2 operation mode */
  PMW3610_MODE_IDLE = 6 /*!< Idle operation mode */
} pmw3610_mode_t;

/**
 * Mouse sensor motion reporting bits.
 */
typedef enum
{
  PMW3610_MOTION_BITS_8 = 0,  /*!< Motion reporting uses 8 bits */
  PMW3610_MOTION_BITS_12 = 1  /*!< Motion reporting uses 12 bits */
} pmw3610_motion_bits_t;

/* Register values according to Application Note 040215 from PXI */
static const u8 pmw3610_config[][2] =
{
	/*{register, value}*/
  {REG_PERFORMANCE,0x0d},
	{REG_RUN_DOWNSHIFT,0x04},
	{REG_REST1_PERIOD,0x04},
	{REG_REST1_DOWNSHIFT,0x0f},
	{REG_SPI_CLK_ON_REQ,SPI_CLOCK_ON}, // on spi clock
	{REG_SPI_PAGE0,SPI_PAGE1},         // SPI Pg Switch
	{REG_RES_STEP, 0x48},							// 1600 cpi rotation sensor  do not swap x y  invert y
	//{0x05,0x46},                      // 1200 cpi rotation sensor  do not swap x y  invert y
	{REG_SPI_PAGE0,SPI_PAGE0},         // switch page	
	{REG_SPI_CLK_ON_REQ,SPI_CLOCK_OFF}, // off spi clock
};

static const u8 pmw3610_config1[][2] =
{
	/*{register, value}*/
  {REG_PERFORMANCE,0x0d},
	{REG_RUN_DOWNSHIFT,0x04},
	{REG_REST1_PERIOD,0x04},
	{REG_REST1_DOWNSHIFT,0x0f},
	{REG_SPI_CLK_ON_REQ,SPI_CLOCK_ON}, // on spi clock
	{REG_SPI_PAGE0,SPI_PAGE1},         // SPI Pg Switch
	{REG_RES_STEP, 0x4C},							// 2400 cpi rotation sensor  do not swap x y  invert y
	//{0x05,0x46},                      // 1200 cpi rotation sensor  do not swap x y  invert y
	{REG_SPI_PAGE0,SPI_PAGE0},         // switch page	
	{REG_SPI_CLK_ON_REQ,SPI_CLOCK_OFF}, // off spi clock
};

//uint8_t sensor_cpi_mouse[] = {0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xe0}; /*{800, 1200(default), 1600, 2000, 2400, 2800, 3200}*/
u8 sensor_cpi_mouse[] = {0x41, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50}; /*{800, 1200(default), 1600, 2000, 2400, 2800, 3200}*/
/* SPI Manipulation */

u8 data_cpi_count = 0;

//bool motion_pin_high=true;


#if 0
static u8 sdio_read_byte(uint8_t address)
{
	u8 tx_data[] = {0,0};
	u8 rx_data[] = {0,0};

	tx_data[0] = (address & ~(BIT_7));

  hal_spi_tx_rx((uint32_t*)spi_base_address, 2, tx_data, rx_data);
	
	return rx_data[1];	
}

static u8 sdio_write_byte(u8 address, u8 data_byte)
{
	u8 tx_data[] = {0, 0};
	u8 rx_data[] = {0, 0};

    tx_data[0] = address | BIT_7;
    tx_data[1] = data_byte;

    hal_spi_tx_rx((uint32_t*)spi_base_address, 2, tx_data, rx_data);
    return rx_data[1];
}

static u8 sdio_transfer()
{
	u8 rx_data;
		
    rx_data = hal_spi_transfer((uint32_t*)spi_base_address);
	
    return rx_data;
}

void smart_enable(bool m_smart_control)
{
	if(m_smart_control == true)
	{
		sdio_write_byte(REG_SPI_CLK_ON_REQ,SPI_CLOCK_ON);
		sdio_write_byte(REG_DTEST2,SMART_ON);	// SMART enable
		sdio_write_byte(REG_SPI_CLK_ON_REQ,SPI_CLOCK_OFF);
	}else{
		sdio_write_byte(REG_SPI_CLK_ON_REQ,SPI_CLOCK_ON);
		sdio_write_byte(REG_DTEST2,SMART_OFF);	// SMART disable
		sdio_write_byte(REG_SPI_CLK_ON_REQ,SPI_CLOCK_OFF);
	}
}

void sensor_config(void)
{
	sdio_write_byte(REG_SPI_CLK_ON_REQ, SPI_CLOCK_ON); // on SPI clock
	sdio_write_byte(REG_SPI_PAGE0, SPI_PAGE1); // SPI Pg Switch
	sdio_write_byte(REG_SET_GAIN_3X,SET_GIN_3X);
	sdio_write_byte(REG_POWER_LEVEL_INCREASE,SET_LASER_POWER);
	sdio_write_byte(REG_COMP_REG21,COMP_REG21);
	sdio_write_byte(REG_SPI_PAGE0, SPI_PAGE0); // switch page
	sdio_write_byte(REG_SPI_CLK_ON_REQ, SPI_CLOCK_OFF); // off SPI clock
	
}

u32 drv_mouse_sensor_read(u16* _delta_x, u16* _delta_y)
{
		u8 delta_x; /*!< Stores REG_DELTA_X contents */
		u8 delta_y; /*!< Stores REG_DELTA_Y contents */
		u8 delta_xy_high; /*!< Stores REG_DELTA_XY contents which contains upper 4 bits for both delta_x and delta_y when 12 bit mode is used */
	
		//unsigned char squal, shutterhi, shutterlo, temp;

	  sdio_read_byte(REG_MOTION_ST);
	
    delta_x = sdio_read_byte(REG_DELTA_X_L);
    delta_y = sdio_read_byte(REG_DELTA_Y_L);
    delta_xy_high = sdio_read_byte(REG_DELTA_XY_H);
	
	
	// _delta_y = (((int16_t) (((delta_xy_high & 0x0F) << 12)  | (delta_y << 4))) / 16);  /// need to negative
		
	*_delta_y = (((u16) (((delta_xy_high & 0x0F) << 12)  | (delta_y << 4))) / 16);
    *_delta_x =  ((u16) (((delta_xy_high & 0xF0) << 8)   | (delta_x << 4))) / 16;

	return NRF_SUCCESS;
}

void drv_mouse_sensor_dummy_read(void)
{
	u16 dummy_x, dummy_y;
	drv_mouse_sensor_read(&dummy_x, &dummy_y);
}

u32 drv_mouse_sensor_shutter_read(int8_t* shutter_lower, int8_t* shutter_higher, int8_t* squal)

{
	   sdio_read_byte(REG_MOTION_ST);
	
		 *shutter_higher  = sdio_read_byte(REG_SHUT_HI);
		 *shutter_lower   = sdio_read_byte(REG_SHUT_LO);
	   *squal           = sdio_read_byte(REG_SQUAL);

	   return NRF_SUCCESS;
}

void drv_motion_burst_read(burst_data *burstData)
{
    u16 deltaX_l; /*!< Stores REG_DELTA_X contents */
    u16 deltaY_l; /*!< Stores REG_DELTA_Y contents */
    u16 deltaX_h;
    u16 deltaY_h;
    u16 deltaXY_h; /*!< Stores REG_DELTA_XY contents which contains upper 4 bits for both delta_x and delta_y when 12 bit mode is used */
	
		unsigned char squal, shutterhi, shutterlo;
	
		nrf_gpio_pin_clear(IO_PMW3610_CSN_PIN);
	
		nrf_delay_us(5);
		sdio_read_byte(REG_BURST_READ);
		
		nrf_delay_us(10);
	
		sdio_transfer(); // motion

    deltaX_l = sdio_transfer(); // X Low byte
		nrf_delay_us(20);
    deltaY_l = sdio_transfer(); // Y Low byte
		nrf_delay_us(20);
    deltaXY_h = sdio_transfer(); // XY High byte
		nrf_delay_us(20);
		squal = sdio_transfer(); // squal
		nrf_delay_us(20);
		shutterhi = sdio_transfer(); // shutter hi
		nrf_delay_us(20);
		shutterlo = sdio_transfer(); // shutter lo
		nrf_delay_us(20);
		
		nrf_gpio_pin_set(IO_PMW3610_CSN_PIN);		
		
		nrf_delay_us(20);
		
		deltaX_h = ((int16_t)deltaXY_h << 4) & 0xF00;
		if(deltaX_h & 0x800)
		{
			deltaX_h |= 0xf000;
		}
		
		deltaY_h = ((int16_t)deltaXY_h << 8) & 0xF00;
		if(deltaY_h & 0x800)
		{
			deltaY_h |= 0xf000;
		}
		
		burstData->deltaX = deltaX_h | deltaX_l;
		burstData->deltaY = deltaY_h | deltaY_l;
		
		burstData->squal = squal;
		burstData->shutterhi = shutterhi;
		burstData->shutterlo = shutterlo;

}
#endif


static inline u8 pmw3610_is_motion_detected(void)
{
    static u8 temp;
	  temp = mouse_spi_ReadRegister(REG_MOTION_ST);
    return ((temp & 0x80) != 0);
}

void drv_mouse_sensor_mod_cpi(void)
{
		data_cpi_count ++;

		if(data_cpi_count > 3)
		{
				data_cpi_count = 0;
		}

		mouse_spi_WriteRegister(REG_SPI_CLK_ON_REQ, SPI_CLOCK_ON); // on SPI clock
		mouse_spi_WriteRegister(REG_SPI_PAGE0, SPI_PAGE1); // SPI Pg Switch

		switch(data_cpi_count)
		{
			case 0:
				mouse_spi_WriteRegister(0x05, sensor_cpi_mouse[PMW3610_RESOLUTION_800DPI]);
			break;
			case 1:
				mouse_spi_WriteRegister(0x05, sensor_cpi_mouse[PMW3610_RESOLUTION_1200DPI]);
			break;
			case 2:
				mouse_spi_WriteRegister(0x05, sensor_cpi_mouse[PMW3610_RESOLUTION_1600DPI]);
			break;
			case 3:
				mouse_spi_WriteRegister(0x05, sensor_cpi_mouse[PMW3610_RESOLUTION_2400DPI]);
			break;
			case 4:
				mouse_spi_WriteRegister(0x05, sensor_cpi_mouse[PMW3610_RESOLUTION_3200DPI]);
			break;
			default:
				break;
		}

		mouse_spi_WriteRegister(REG_SPI_PAGE0, SPI_PAGE0); // switch page
		mouse_spi_WriteRegister(REG_SPI_CLK_ON_REQ, SPI_CLOCK_OFF); // off SPI clock
}

void sensor_write_operation_control(bool m_sensor_control)  /////added by lvshuo to control the sensor to be read
{

	  if(m_sensor_control==true)
		  {
		  	  mouse_spi_WriteRegister(REG_SPI_CLK_ON_REQ, SPI_CLOCK_ON); // on SPI clock
		  	  mouse_spi_WriteRegister(REG_SPI_PAGE0, SPI_PAGE1); // SPI Pg Switch
			}
	  else
	    {
		  	  mouse_spi_WriteRegister(REG_SPI_PAGE0, SPI_PAGE0); // switch page
		  	  mouse_spi_WriteRegister(REG_SPI_CLK_ON_REQ, SPI_CLOCK_OFF); // off SPI clock
	    }
}

u8 sensor_spi_page()  /////added by johnny@20160426 to control the sensor to be read
{
		u8 spi_page;

		spi_page = mouse_spi_ReadRegister(REG_SPI_PAGE0);

		return spi_page;
}




/*
 *@param : None
 *@Function: Read X/Y From sensor
 *
 */

u32 drv_mouse_motion_report(s8 *pBuf, u8 no_flg )
{
	if(drv_mouse_sensor_int_get){
		s8 delta_x = mouse_spi_ReadRegister(REG_DELTA_X_L);			//x low
		s8 delta_y = mouse_spi_ReadRegister(REG_DELTA_Y_L);			//y low
		s8 delta_xy_high = mouse_spi_ReadRegister(REG_DELTA_XY_H);	//x_y high

		pBuf[1] =  ((u16) (((delta_xy_high & 0xF0) << 8)   | (delta_x << 4))) / 16;
		pBuf[0] = ((u16) (((delta_xy_high & 0x0F) << 12)  | (delta_y << 4))) / 16;
	}
	//pBuf[0] = delta_x;
	//pBuf[1] = delta_y;

}

/*
 * 初始化sensor, Reset/Motion PIN
 * 初始化MISO,MISO,SCLK,CS
 *
 *
 */

int drv_spi_init(void)
{
	mouse_spi_init();
}


int drv_sensor_reset()
{
	gpio_write(GPIO_RESET_PIN, 0);		//gpio reset
	sleep_us(30);
	gpio_write(GPIO_RESET_PIN, 1);
}

u8 product_id0;

u32 drv_mouse_sensor_init(void)
{
	drv_spi_init();
	gpio_set_output_en(GPIO_MOTION_PIN, 1);
	gpio_set_output_en(GPIO_RESET_PIN, 1);
	gpio_write(GPIO_RESET_PIN, 1);
	sleep_us(200);
	mouse_spi_WriteRegister(REG_OBSERVATION, 0);
	sleep_us(15000);	//15ms
	u8 temp = mouse_spi_ReadRegister(REG_OBSERVATION);
	while( (temp & 0x0F) != 0x0F);
	sleep_us(20);
	product_id0 = mouse_spi_ReadRegister(REG_PROD_ID);
	if(product_id0 != PMW3610_PRODUCT_ID){
		drv_sensor_reset();
		return 0;			//sensor product error
	}
	for (int i = 0; i < (sizeof(pmw3610_config) / 2); i++)
	{
		mouse_spi_WriteRegister(pmw3610_config[i][0], pmw3610_config[i][1]);
	}

	  if (pmw3610_is_motion_detected())
	    {
		  	  u8 *m_data;
		  	  drv_mouse_motion_report(m_data, 0);
	    }
	return 1;



#if 0

    u32 i;
    hal_spi_init_t spi_params;
		
	u8 temp;

    spi_params.module    = SPI0;
    spi_params.mode      = SPI_MODE3;
    spi_params.frequency = Freq_1Mbps;
    spi_params.lsb_first = false;
    spi_params.sck_pin   = IO_PMW3610_SCK_PIN;
    spi_params.mosi_pin  = IO_PMW3610_MOSI_PIN;
    spi_params.miso_pin  = IO_PMW3610_MISO_PIN;
    spi_params.ss_pin    = IO_PMW3610_CSN_PIN;
    
    spi_base_address = (NRF_SPI_Type*)hal_spi_init(&spi_params);  
    if (spi_base_address == 0)
    {
        return NRF_ERROR_TIMEOUT;
    }

		
    NRF_GPIO->PIN_CNF[IO_PMW3610_MOTION_PIN] = (GPIO_PIN_CNF_SENSE_Low       << GPIO_PIN_CNF_SENSE_Pos) |
                                                (GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos) |
                                                (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  |
                                                (GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos) |
                                                (GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos);


   // adns3000_reset();
		//pmw3610_reset();


		nrf_gpio_cfg_output(IO_PMW3610_RESETN_PIN);
		
		nrf_gpio_pin_set(IO_PMW3610_RESETN_PIN);
			
		//nrf_gpio_pin_clear(IO_PMW3610_CSN_PIN);
		
		nrf_delay_us(200);			
		
		sdio_write_byte(REG_OBSERVATION, 0x00);// add by johnny //clear observation register by writing 0x00 in register 0x2D 55
		
		nrf_delay_ms(15);
		
		temp = sdio_read_byte(REG_OBSERVATION);
		
		if(temp & 0x0F != 0x0F)
		//if(temp == 0x00)
		{
			while(1);
			//nrf_gpio_pin_clear(IO_PMW3610_CSN_PIN);
		
		  //nrf_delay_us(150);
			//m_pwr_mgmt_goto_sysoff(0, 0)
		}
		
		nrf_delay_us(20);	
		
		
		
		temp = pmw3610_product_id_read();
		
    if(temp != PMW3610_PRODUCT_ID)//if(sdio_read_byte(0x00)!=PMW3610_PRODUCT_ID)	
    {
			  //m_pwr_mgmt_goto_sysoff(0, 0);
			  pmw3610_reset();
        return NRF_ERROR_TIMEOUT;
    }
		
		sdio_read_byte(REG_MOTION_ST);
		sdio_read_byte(REG_DELTA_X_L);
		sdio_read_byte(REG_DELTA_Y_L);
		sdio_read_byte(REG_DELTA_XY_H);
		if(running_mode==GAZELL)
		{
			for (i = 0; i < (sizeof(pmw3610_config) / 2); ++i)
			{
					sdio_write_byte(pmw3610_config[i][0], pmw3610_config[i][1]);
			}
		}
		else if(running_mode==BLE)
		{
			for (i = 0; i < (sizeof(pmw3610_config) / 2); ++i)
			{
					sdio_write_byte(pmw3610_config1[i][0], pmw3610_config[i][1]);
			}
		}
    
	/* comment by johnny 20160417. There is no MOTION_CTRL reg to set MOTION polarity or sensitivity.
    if (pmw3610_motion_interrupt_set(PMW3610_MOTION_OUTPUT_POLARITY_HIGH, PMW3610_MOTION_OUTPUT_SENSITIVITY_LEVEL) != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }
    */
	//sensor_config();//////change the laser power
		
    if (pmw3610_is_motion_detected())
    {
			  drv_mouse_sensor_dummy_read();			

			  //burst_data dummy;
        //drv_motion_burst_read(&dummy);
    }
		
 		//nrf_gpio_pin_set(IO_PMW3610_CSN_PIN);
    
    return NRF_SUCCESS;
#endif
}

bool drv_mouse_sensor_int_get(void)
{
    //return ((NRF_GPIO->IN & (1 << IO_PMW3610_MOTION_PIN)) == 0);
		//return ((NRF_GPIO->IN >> IO_PMW3610_MOTION_PIN) & 1UL == 0);
		return (gpio_read(GPIO_MOTION_PIN) == 0);
}

#if 0
void drv_mouse_sensor_sense_enable(bool enable)
{
    uint32_t sense = enable ? GPIO_PIN_CNF_SENSE_Low : GPIO_PIN_CNF_SENSE_Disabled;
    //nrf_gpio_pin_clear(IO_PMW3610_CSN_PIN);
    NRF_GPIO->PIN_CNF[IO_PMW3610_MOTION_PIN] = (sense                         << GPIO_PIN_CNF_SENSE_Pos) |
                                                (GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos) |
                                                (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)  |
                                                (GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos) |
                                                (GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos);      
}

bool drv_mouse_sensor_wakeup_prepare(void)
{
    NRF_GPIO->PIN_CNF[IO_PMW3610_MOTION_PIN] = (GPIO_PIN_CNF_SENSE_Low    << GPIO_PIN_CNF_SENSE_Pos) |
                                                (GPIO_PIN_CNF_DRIVE_S0S1    << GPIO_PIN_CNF_DRIVE_Pos) |
                                                (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)  |
                                                (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                                                (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos); 
    return true;
}
#endif
