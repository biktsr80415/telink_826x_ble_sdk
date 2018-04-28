#pragma once

#include "../mcu/compiler.h"
#include "../mcu/analog.h"
#include "../common/bit.h"
#include "../common/types.h"
#include "../config/user_config.h"
#if defined(__cplusplus)
	extern "C" {
#endif

#ifndef WIN32
#define REG_BASE_ADDR			0x800000
#else
extern u8 reg_simu_buffer[];
#define REG_BASE_ADDR			(&reg_simu_buffer[0])
#endif


#define REG_ADDR8(a)			(*(volatile u8*) (REG_BASE_ADDR + (a)))
#define REG_ADDR16(a)			(*(volatile u16*)(REG_BASE_ADDR + (a)))
#define REG_ADDR32(a)			(*(volatile u32*)(REG_BASE_ADDR + (a)))

#define WRITE_REG8(addr,data)           (*(volatile unsigned char  *)((addr)|REG_BASE_ADDR)=data)
#define READ_REG8(addr)                 (*(volatile unsigned char  *)((addr)|REG_BASE_ADDR))

//#define reg_gpio_datai_grp(grp) 	REG_ADDR8(0x580 + grp * 8)
//#define reg_gpio_ie_grp(grp) 		REG_ADDR8(0x581 + grp * 8)
//#define reg_gpio_oe_grp(grp) 		REG_ADDR8(0x582 + grp * 8)
//#define reg_gpio_datao_grp(grp) 	REG_ADDR8(0x583 + grp * 8)
//#define reg_gpio_ds_grp(grp) 		REG_ADDR8(0x585 + grp * 8)
//#define reg_gpio_nrm_grp(grp) 		REG_ADDR8(0x586 + grp * 8)
/****************************************************
  secondary i2c regs struct: begin  addr : 0x00
 *****************************************************/
#define reg_i2c_set				REG_ADDR32(0x00)
#define reg_i2c_speed			REG_ADDR8(0x00)
#define reg_i2c_id				REG_ADDR8(0x01)
enum{
	FLD_I2C_WRITE_READ_BIT  =  BIT(0),
	FLD_I2C_ID              =  BIT_RNG(1,7),
};

#define reg_i2c_status			REG_ADDR8(0x02)
enum{
	FLD_I2C_CMD_BUSY		= 	BIT(0),
	FLD_I2C_BUS_BUSY		= 	BIT(1),
	FLD_I2C_NAK				= 	BIT(2),
};

#define reg_i2c_mode			REG_ADDR8(0x03)
enum{
	FLD_I2C_ADDR_AUTO 		=	BIT(0),
	FLD_I2C_MODE_MASTER		= 	BIT(1),		// 1: master, 0: slave
	FLD_I2C_MEM_MAP 		=	BIT(2), 	// write i2c data to predefined memory address which set by other register
};

#define reg_i2c_adr_dat			REG_ADDR16(0x04)
#define reg_i2c_dat_ctrl		REG_ADDR32(0x04)
#define reg_i2c_di_ctrl			REG_ADDR16(0x06)
#define reg_i2c_adr				REG_ADDR8(0x04)
#define reg_i2c_do				REG_ADDR8(0x05)
#define reg_i2c_di				REG_ADDR8(0x06)
#define reg_i2c_ctrl			REG_ADDR8(0x07)
enum{
	FLD_I2C_CMD_ID		= 		BIT(0),
	FLD_I2C_CMD_ADR		= 		BIT(1),
	FLD_I2C_CMD_DO		= 		BIT(2),
	FLD_I2C_CMD_DI		= 		BIT(3),
	FLD_I2C_CMD_START	= 		BIT(4),
	FLD_I2C_CMD_STOP	= 		BIT(5),
	FLD_I2C_CMD_READ_ID	= 		BIT(6),
	FLD_I2C_CMD_NAK		= 		BIT(7),
};

/****************************************************
  secondary spi regs struct: begin  addr : 0x08
 *****************************************************/
#define reg_spi_data			REG_ADDR8(0x08)
#define reg_spi_ctrl			REG_ADDR8(0x09)
enum{
	FLD_SPI_CS = 				BIT(0),
	FLD_SPI_MASTER_MODE_EN = 	BIT(1),
	FLD_SPI_DATA_OUT_DIS = 		BIT(2),
	FLD_SPI_RD = 				BIT(3),
	FLD_SPI_ADDR_AUTO =			BIT(4),
	FLD_SPI_BUSY = 				BIT(6),		// diff from doc,  bit 6 working
};
#define reg_spi_sp				REG_ADDR8(0x0a)
enum{
	FLD_MASTER_SPI_CLK = 		BIT_RNG(0,6),
	FLD_SPI_ENABLE =			BIT(7),
	FLD_SPI_PAD_SEL = 			BIT(7),
};

#define reg_spi_inv_clk			REG_ADDR8(0x0b)
enum{
	FLD_INVERT_SPI_CLK =        BIT(0),
	FLD_DAT_DLY_HALF_CLK =      BIT(1),
};

/****************************************************
 master spi regs struct: begin  addr : 0x0c
 *****************************************************/
#define reg_master_spi_data		REG_ADDR8(0x0c)
#define reg_master_spi_ctrl		REG_ADDR8(0x0d)

enum{
	FLD_MASTER_SPI_CS = 		BIT(0),
	FLD_MASTER_SPI_SDO = 		BIT(1),
	FLD_MASTER_SPI_CONT = 		BIT(2),
	FLD_MASTER_SPI_RD = 		BIT(3),
	FLD_MASTER_SPI_BUSY = 		BIT(4),
};

/****************************************************
 otp regs struct: begin  addr : 0x10
 *****************************************************/

#define reg_otp_addr_para		REG_ADDR16(0x10)
enum{
	FLD_OTP_PARA_ADDR = 		BIT_RNG(0,12),
	FLD_OTP_PARA_PTM = 			BIT_RNG(13,15),
};

#define reg_otp_ctrl			REG_ADDR8(0x12)
enum{
	FLD_OTP_CTRL_PCEN = 		BIT(0),
	FLD_OTP_FAST_CLK = 			BIT(1),
	FLD_OTP_OEN = 				BIT(2),
	FLD_OTP_CLK = 				BIT(3),
	FLD_OTP_PCEN_PWDN = 		BIT(4),
	FLD_OTP_WEN_PWDN = 			BIT(5),
	FLD_OTP_OEN_PWDN = 			BIT(6),
	FLD_OTP_CLK_PWDN = 			BIT(7),
};

#define reg_otp_byte_dat		REG_ADDR8(0x13)
#define reg_otp_dat				REG_ADDR32(0x14)
#define reg_otp_blk_code		REG_ADDR8(0x18)

#define reg_i2c_irq_status		REG_ADDR8(0x21)
#define reg_i2c_clr_status		REG_ADDR8(0x22)
enum{
	FLD_I2C_STATUS_WR = 		BIT(1),
	FLD_SPI_STATUS_WR =         BIT(1),
	FLD_I2C_STATUS_RD = 		BIT(2),
};

#define reg_spi_irq_status      reg_i2c_irq_status
#define reg_spi_clr_status      reg_i2c_clr_status

/****************************************************
 adc regs struct: begin  addr : 0x28
 *****************************************************/

/////////// pga select, [3:0] L; [7:4] R ////////////////////
#define reg_adc_pga_sel_m		REG_ADDR8(0x28)
#define reg_adc_pga_sel_l		REG_ADDR8(0x29)
#define reg_adc_pga_sel_r		REG_ADDR8(0x2a)

#define reg_adc_ref				REG_ADDR8(0x2b)
enum{
	FLD_ADC_REF_M = 			BIT_RNG(0,1),
	FLD_ADC_REF_L = 			BIT_RNG(2,3),
	FLD_ADC_REF_R = 			BIT_RNG(4,5),
};

/////////// adc select [4:0] channel; [6:5] mode; [7] signed
#define reg_adc_chn_m_sel		REG_ADDR8(0x2c)
#define reg_adc_chn_l_sel		REG_ADDR8(0x2d)
enum{
	FLD_ADC_CHN_SEL = 			BIT_RNG(0,4),
	FLD_ADC_DIFF_CHN_SEL = 		BIT_RNG(5,6),	// datasheet  12.1
	FLD_ADC_DATA_FORMAT  =      BIT(7),
};
#define reg_adc_chn_r_sel		REG_ADDR8(0x2e)

enum{
	FLD_ADC_CHN_D0				= 0x01,
	FLD_ADC_CHN_D1				= 0x02,
	FLD_ADC_CHN_D2				= 0x03,
	FLD_ADC_CHN_D3				= 0x04,
	FLD_ADC_CHN_D4				= 0x05,
	FLD_ADC_CHN_D5				= 0x06,
	FLD_ADC_CHN_C2				= 0x07,
	FLD_ADC_CHN_C3				= 0x08,
	FLD_ADC_CHN_C4				= 0x09,
	FLD_ADC_CHN_C5				= 0x0a,
	FLD_ADC_CHN_C6				= 0x0b,
	FLD_ADC_CHN_C7				= 0x0c,

	FLD_ADC_CHN_PGA_R			= 0x0d,
	FLD_ADC_CHN_PGA_L			= 0x0e,

	FLD_ADC_CHN_TEMP_POS		= 0x0f,
	FLD_ADC_CHN_TEMP_NEG		= 0x10,
	FLD_ADC_CHN_VBUS			= 0x11,
	FLD_ADC_CHN_GND				= 0x12,

	FLD_ADC_SINGLE_IN			= 0x00,
	FLD_ADC_D5_REF				= 0x20,
	FLD_ADC_D7_REF				= 0x40,
	FLD_ADC_PGA_L_REF			= 0x60,
	FLD_ADC_DATA_SIGNED			= 0x80,

	FLD_ADC_PGA_C01				= 0x226d,
	FLD_ADC_PGA_C23				= 0x116d,
	FLD_ADC_PGA_C45				= 0x016d,

	FLD_ADC_PGA_R_C2			= 0x110d,
	FLD_ADC_PGA_R_C0			= 0x220d,
	FLD_ADC_PGA_L_C3			= 0x110e,
	FLD_ADC_PGA_L_C1			= 0x220e,

	FLD_ADC_PGA_OFF				= 0x0000,

};

#define reg_adc_res_lr			REG_ADDR8(0x2f)
enum {
	FLD_ADC_RESOLUTION_SEL   =  BIT_RNG(0,2),
};
#define reg_adc_res_m			REG_ADDR8(0x3c)
#define reg_adc_tsamp_lr		REG_ADDR8(0x3d)
enum {
	FLD_ADC_SAMPLE_TIME         = BIT_RNG(0,2),
};

enum{
	FLD_ADC_VREF_1P3V			= 0x15,
	FLD_ADC_12BITS				= 0x07,
	FLD_ADC_TSAMP_3CLOCK		= 0x00,
};


#define reg_adc_period_chn0		REG_ADDR16(0x30)
enum {
	FLD_ADC_PHASE_TICK    =    BIT_RNG(0,1),
	FLD_ADC_CHNM_PERIODL  =    BIT_RNG(2,7),
	FLD_ADC_CHNM_PEIRODH  =    BIT_RNG(8,15),
	FLD_ADC_CHNM_PERIOD   =    BIT_RNG(2,15),
};
#define reg_adc_period_chn12	REG_ADDR8(0x32)
enum {
	FLD_ADC_CHNLR_PERIOD  =    BIT_RNG(0,15),
};

#define reg_adc_ctrl			REG_ADDR8(0x33)
enum{
	FLD_ADC_CHNL_AUTO_EN = 		BIT(0),
	FLD_ADC_CHNR_AUTO_EN =		BIT(1),
	FLD_ADC_AUD_DATAPATH_EN =	BIT(2),
	FLD_ADC_CHNM_AUTO_EN =		BIT(3),
	FLD_ADC_AUD_MODE =			BITS(4,5),		// 0: no audio; 1: mono; 2: stereo; 3: reserved
	FLD_ADC_DONE_SIGNAL =		BITS(6,7),		// 1: rising; 2: falling; else: counter
};

#define reg_adc_chn1_outp		REG_ADDR16(0x34)
enum{
	FLD_ADC_OUTPUT_CHN_VAL = 	BIT_RNG(0,14),
	FLD_ADC_CHN_MANU_START = 	BIT(15),
};
#define reg_adc_chn2_outp		REG_ADDR16(0x36)
#define reg_adc_dat_byp_outp	REG_ADDR16(0x38)

#define reg_adc_chn0_input		REG_ADDR16(0x3a)
enum {
	FLD_ADC_BUSY_FLAG       =   BIT(0),
};

#define reg_adc_samp_res		REG_ADDR16(0x3c)
enum{
	FLD_ADC_CHNM_SAMP_CYCLE = 	BIT_RNG(0,2),
	FLD_ADC_CHNM_SAMP_RESOL = 	BIT_RNG(3,5),
	FLD_ADC_CHNLR_SAMP_CYCLE =	BIT_RNG(8,10),	// reg: 0x3d
};


#define reg_i2c_mem_map			REG_ADDR16(0x3e)	// check with spec again // refer to FLD_I2C_MEM_MAP

/****************************************************
 sys regs struct: begin  addr : 0x60
 *****************************************************/
#define reg_rst0				REG_ADDR8(0x60)
#define reg_rst0_16				REG_ADDR16(0x60)
#define reg_rst1				REG_ADDR8(0x61)
#define reg_rst2				REG_ADDR8(0x62)
#define reg_rst_clk0			REG_ADDR32(0x60)
enum{
	FLD_RST_SPI = 				BIT(0),
	FLD_RST_I2C = 				BIT(1),
	FLD_RST_USB = 				BIT(2),
	FLD_RST_USB_PHY = 			BIT(3),
	FLD_RST_MCU = 				BIT(4),
	FLD_RST_MAC =				BIT(5),
	FLD_RST_AIF = 				BIT(6),
	FLD_RST_BB = 				BIT(7),
	FLD_RST_ZB =				BIT(7),
	FLD_RST_GPIO = 				BIT(8),
	FLD_RST_ALGM = 				BIT(9),
	FLD_RST_DMA =				BIT(10),
	FLD_RST_UART = 				BIT(11),
	FLD_RST_PWM = 				BIT(12),
	FLD_RST_AES = 				BIT(13),
	FLD_RST_SWR_M =				BIT(14),
	FLD_RST_SWR_S =				BIT(15),
	FLD_RST_SBC =				BIT(16),
	FLD_RST_AUD =				BIT(17),
	FLD_RST_DFIFO =				BIT(18),
	FLD_RST_ADC =				BIT(19),
	FLD_RST_SOFT_MCU =			BIT(20),
	FLD_RST_MCIC = 				BIT(21),
	FLD_RST_SOFT_MCIC =			BIT(22),
	FLD_RST_RSV =				BIT(23),
	FLD_CLK_SPI_EN =			BIT(24),
	FLD_CLK_I2C_EN =			BIT(25),
	FLD_CLK_USB_EN =			BIT(26),
	FLD_CLK_USB_PHY_EN =		BIT(27),
	FLD_CLK_MCU_EN =			BIT(28),
	FLD_CLK_MAC_EN =			BIT(29),
	FLD_CLK_ADC_EN =			BIT(30),	// ADC interface
	FLD_CLK_ZB_EN =				BIT(31),
};

#define reg_clk_en				REG_ADDR16(0x64)
#define reg_clk_en1				REG_ADDR8(0x64)
enum{
	FLD_CLK_GPIO_EN = 			BIT(0),
	FLD_CLK_ALGM_EN = 			BIT(1),
	FLD_CLK_DMA_EN = 			BIT(2),
	FLD_CLK_UART_EN = 			BIT(3),
	FLD_CLK_PWM_EN = 			BIT(4),
	FLD_CLK_AES_EN = 			BIT(5),
	FLD_CLK_32K_TIMER_EN =		BIT(6),
	FLD_CLK_SWIRE_EN = 			BIT(7),
	FLD_CLK_32K_QDEC_EN =		BIT(8),
	FLD_CLK_AUD_EN =			BIT(9),
	FLD_CLK_DIFIO_EN = 			BIT(10),
	FLD_CLK_KEYSCAN_EN =		BIT(11),
	FLD_CLK_MCIC_EN =			BIT(12),
	FLD_CLK_QDEC_EN =			BIT(13),
};
#define reg_clk_en2				REG_ADDR8(0x65)
enum{
	FLD_CLK2_SBC_EN =			BIT(0),
	FLD_CLK2_AUD_EN =			BIT(1),
	FLD_CLK2_DFIFO_EN = 		BIT(2),
	FLD_CLK2_I2S =				BIT_RNG(3,4),
	FLD_CLK2_C32K =				BIT_RNG(5,7),
};

#define reg_clk_sel				REG_ADDR8(0x66)
enum{
	FLD_CLK_SEL_DIV = 			BIT_RNG(0,4),
	FLD_CLK_SEL_SRC =			BIT_RNG(5,7),
};

#define reg_i2s_step			REG_ADDR8(0x67)
enum{
	FLD_I2S_STEP = 				BIT_RNG(0,6),
	FLD_I2S_CLK_EN =			BIT(7),
};

#define reg_i2s_mod				REG_ADDR8(0x68)

static inline void SET_SDM_CLOCK_MHZ(int f_mhz)	{
	reg_i2s_step = FLD_I2S_CLK_EN | f_mhz;
	reg_i2s_mod = 0xc0;
}
/****************************************************
	 ADC: 0x69
 *****************************************************/
#define reg_adc_step_l			REG_ADDR8(0x69)
#define reg_adc_mod_l			REG_ADDR8(0x6a)
#define reg_adc_mod				REG_ADDR16(0x6a)
enum{
	FLD_ADC_MOD = 				BIT_RNG(0,11),
	FLD_ADC_STEP_H = 			BIT_RNG(12,14),
	FLD_ADC_CLK_EN =			BIT(15),
};
#define reg_adc_clk_en			REG_ADDR8(0x6b)
#define reg_adc_mod_h			REG_ADDR8(0x6b)
enum{
	FLD_ADC_MOD_H = 			BIT_RNG(0,3),
	FLD_ADC_MOD_H_STEP =		BIT_RNG(4,6),
	FLD_ADC_MOD_H_CLK =			BIT(7),
};

#define reg_dmic_step			REG_ADDR8(0x6c)
enum{
	FLD_DMIC_STEP = 			BIT_RNG(0,6),
	FLD_DMIC_CLK_EN =			BIT(7),
};
#define reg_dmic_mod			REG_ADDR8(0x6d)


#define reg_wakeup_en			REG_ADDR8(0x6e)
enum{
	FLD_WAKEUP_SRC_I2C = 		BIT(0),
	FLD_WAKEUP_SRC_SPI =		BIT(1),
	FLD_WAKEUP_SRC_USB =		BIT(2),
	FLD_WAKEUP_SRC_GPIO =		BIT(3),
	FLD_WAKEUP_SRC_I2C_SYN =	BIT(4),
	FLD_WAKEUP_SRC_GPIO_RM =	BIT(5),
	FLD_WAKEUP_SRC_USB_RESM =	BIT(6),
	FLD_WAKEUP_SRC_RST_SYS =	BIT(7),
};

#define reg_pwdn_ctrl			REG_ADDR8(0x6f)
enum{
	FLD_PWDN_CTRL_REBOOT = 		BIT(5),
	FLD_PWDN_CTRL_SLEEP =		BIT(7),
};

#define reg_fhs_sel				REG_ADDR8(0x70)
enum{
	FLD_FHS_SELECT = 			BIT_RNG(0,1),
};
enum{
	FHS_SEL_192M_PLL = 0,
//	FHS_SEL_48M_PLL = 1,
	FHS_SEL_32M_OSC = 1,
//	FHS_SEL_16M_OSC = 3,
};

/****************************************************
  OTP  addr : 0x71
 *****************************************************/
#define reg_dcdc_clk			REG_ADDR8(0x71)

/****************************************************
 dev_id regs struct: begin  addr : 0x74
 *****************************************************/
#define reg_id_wr_en			REG_ADDR8(0x74)
#define reg_product_id			REG_ADDR32(0x7c)
#define reg_func_id				REG_ADDR8(0x7c)
enum{
	FLD_ID_USB_SYS_CLOCK = 		BIT(0),
	FLD_ID_MAC_CLOCK = 			BIT(1),
	FLD_ID_SBC_CLOCK = 			BIT(2),
	FLD_ID_RISC32_ENABLE = 		BIT(3),
	FLD_ID_I2S_ENABLE = 		BIT(4),
	FLD_ID_SDM_ENABLE = 		BIT(5),
	FLD_ID_RF_ENABLE = 			BIT(6),
	FLD_ID_ZB_ENABLE = 			BIT(7),
};

#define reg_ver_id				REG_ADDR8(0x7d)
#define reg_prod_id				REG_ADDR16(0x7e)

#define reg_mcu_wakeup_mask		REG_ADDR32(0x78)

/****************************************************
 mac regs struct: begin  addr : 0xa0
 *****************************************************/
#define reg_mac_ctrl			REG_ADDR8(0x80)
enum{
	FLD_MAC_CTRL_TX_EN = 		BIT(0),
	FLD_MAC_CTRL_RX_EN =		BIT(1),
	FLD_MAC_CTRL_SPD_100M =		BIT(6),
	FLD_MAC_CTRL_PHY_RST =		BIT(7),
};

#define reg_mac_irq_sta			REG_ADDR8(0x81)
enum{
	FLD_MAC_STA_TX_DONE = 		BIT(0),
	FLD_MAC_STA_RX_DONE =		BIT(1),
	FLD_MAC_STA_TX_ERR =		BIT(2),
	FLD_MAC_STA_RX_ERR =		BIT(3),
	FLD_MAC_STA_RX_CRC =		BIT(4),
};

#define reg_mii_tx_data			REG_ADDR16(0x82)
#define reg_mii_rx_data			REG_ADDR16(0x84)
#define reg_mii_clk				REG_ADDR8(0x86)
enum{
	FLD_MII_PHY_ID  = 			BIT_RNG(0,4),
	FLD_MII_CLK_DIV =			BIT_RNG(5,7),
};
#define reg_mii_ctrl			REG_ADDR8(0x87)
enum{
	FLD_MII_INTERNAL_REG  = 	BIT_RNG(0,4),
	FLD_MII_BUSY =				BIT(5),
	FLD_MII_WR =				BIT(6),
	FLD_MII_PREAM_EN =			BIT(7),
};

/////////////  Uart  //////////////////////////////////
#define reg_uart_clk_div		REG_ADDR16(0x94)
enum{
	FLD_UART_CLK_DIV = 			BIT_RNG(0,14),
	FLD_UART_CLK_DIV_EN = 		BIT(15)
};

#define reg_uart_ctrl0			REG_ADDR16(0x96)
enum{
	FLD_UART_BPWC = 			BIT_RNG(0,3),
	FLD_UART_RX_DMA_EN = 		BIT(4),
	FLD_UART_TX_DMA_EN =		BIT(5),
	FLD_UART_RX_IRQ_EN = 		BIT(6),
	FLD_UART_TX_IRQ_EN =		BIT(7),
	FLD_UART_CTS_I_SELECT =		BIT(8),
	FLD_UART_CTS_EN = 			BIT(9),
	FLD_UART_PARITY_EN =		BIT(10),
    FLD_UART_PARITY_SEL =       BIT(11),
    FLD_UART_STOP_BIT =         BIT_RNG(12,13),
    FLD_UART_TTL =              BIT(14),
    FLD_UART_LOOPBACK =         BIT(15),
};

#define reg_uart_crtl1         		REG_ADDR8(0x97)
enum {
    FLD_UART_CTRL1_CTS_SELECT = BIT(0),
    FLD_UART_CTRL1_CTS_EN = BIT(1),
    FLD_UART_CTRL1_PARITY_EN = BIT(2),
    FLD_UART_CTRL1_PARITY = BIT(3),
    FLD_UART_CTRL1_STOP_BIT = BIT_RNG(4,5),
    FLD_UART_CTRL1_TTL = BIT(6),
    FLD_UART_CTRL1_LOOPBACK = BIT(7),
};

#define reg_uart_ctrl2			REG_ADDR16(0x98)
enum {
    FLD_UART_CTRL2_RTS_TRIG_LVL = BIT_RNG(0,3),
    FLD_UART_CTRL2_RTS_PARITY = BIT(4),
    FLD_UART_CTRL2_RTS_MANUAL_VAL = BIT(5),
    FLD_UART_CTRL2_RTS_MANUAL_EN = BIT(6),
    FLD_UART_CTRL2_RTS_EN = BIT(7),
	FLD_UART_CTRL3_RX_IRQ_TRIG_LEVEL = BIT_RNG(8,11),
	FLD_UART_CTRL3_TX_IRQ_TRIG_LEVEL = BIT_RNG(12,15),
};

#define reg_uart_rx_timeout		REG_ADDR16(0x9a)
enum{
	FLD_UART_TIMEOUT_BW = 		BIT_RNG(0,7),		//  timeout bit width
	FLD_UART_TIMEOUT_MUL = 		BIT_RNG(8,15),
};

#define reg_uart_status0       REG_ADDR8(0x9d)


enum{
	FLD_UART_IRQ_FLAG  =  BIT(3),
	FLD_UART_RX_ERR_CLR=  BIT(6),
	FLD_UART_RX_ERR_FLAG= BIT(7),
};

#define reg_uart_status1       REG_ADDR8(0x9e)
enum{
	FLD_UART_TX_DONE   =  BIT(0),
};

/****************************************************
 swire regs struct: begin  addr : 0xb0
 *****************************************************/
#define reg_swire_data			REG_ADDR8(0xb0)
#define reg_swire_ctrl1			REG_ADDR8(0xb1)
enum{
	FLD_SWIRE_WR  = 			BIT(0),
	FLD_SWIRE_RD  = 			BIT(1),
	FLD_SWIRE_CMD =				BIT(2),
	FLD_SWIRE_USB_DET =			BIT(6),
	FLD_SWIRE_USB_EN =			BIT(7),
};

#define reg_swire_clk_div		REG_ADDR8(0xb2)

//////  analog controls 0xb8 ///////
#define reg_ana_ctrl32			REG_ADDR32(0xb8)	// for performance, set addr and data at a time
#define reg_ana_addr_data		REG_ADDR16(0xb8)	// for performance, set addr and data at a time
#define reg_ana_addr			REG_ADDR8(0xb8)
#define reg_ana_data			REG_ADDR8(0xb9)
#define reg_ana_ctrl			REG_ADDR8(0xba)

// 文档不正确，请使用以下定义
enum{
	FLD_ANA_BUSY  = 			BIT(0),
	FLD_ANA_RSV	=				BIT(4),
	FLD_ANA_RW  = 				BIT(5),
	FLD_ANA_START  = 			BIT(6),
	FLD_ANA_CYC  = 				BIT(7),
};
/****************************************************
 audio regs struct: begin  addr : 0x100
 *****************************************************/
#define reg_ctrl_ep_ptr			REG_ADDR8(0x100)
#define reg_ctrl_ep_dat			REG_ADDR8(0x101)
#define reg_ctrl_ep_ctrl		REG_ADDR8(0x102)

// same for all endpoints
enum{
	FLD_EP_DAT_ACK  = 			BIT(0),
	FLD_EP_DAT_STALL =			BIT(1),
	FLD_EP_STA_ACK = 			BIT(2),
	FLD_EP_STA_STALL = 			BIT(3),
};

#define reg_ctrl_ep_irq_sta		REG_ADDR8(0x103)
enum{
	FLD_CTRL_EP_IRQ_TRANS  = 	BIT_RNG(0,3),
	FLD_CTRL_EP_IRQ_SETUP  =	BIT(4),
	FLD_CTRL_EP_IRQ_DATA  =		BIT(5),
	FLD_CTRL_EP_IRQ_STA  = 		BIT(6),
	FLD_CTRL_EP_IRQ_INTF  = 	BIT(7),
};

#define reg_ctrl_ep_irq_mode	REG_ADDR8(0x104)
enum{
	FLD_CTRL_EP_AUTO_ADDR = 	BIT(0),
	FLD_CTRL_EP_AUTO_CFG =		BIT(1),
	FLD_CTRL_EP_AUTO_INTF =		BIT(2),
	FLD_CTRL_EP_AUTO_STA =		BIT(3),
	FLD_CTRL_EP_AUTO_SYN =		BIT(4),
	FLD_CTRL_EP_AUTO_DESC =		BIT(5),
	FLD_CTRL_EP_AUTO_FEAT =		BIT(6),
	FLD_CTRL_EP_AUTO_STD =		BIT(7),
};

#define reg_usb_ctrl			REG_ADDR8(0x105)
enum{
	FLD_USB_CTRL_AUTO_CLK = 	BIT(0),
	FLD_USB_CTRL_LOW_SPD = 		BIT(1),
	FLD_USB_CTRL_LOW_JITT =		BIT(2),
	FLD_USB_CTRL_TST_MODE = 	BIT(3),
};

#define reg_usb_cyc_cali		REG_ADDR16(0x106)
#define reg_usb_mdev			REG_ADDR8(0x10a)
#define reg_usb_host_conn		REG_ADDR8(0x10b)
enum{
	FLD_USB_MDEV_SELF_PWR = 	BIT(0),
	FLD_USB_MDEV_SUSP_STA = 	BIT(1),
};

#define reg_usb_sups_cyc_cali	REG_ADDR8(0x10c)
#define reg_usb_intf_alt		REG_ADDR8(0x10d)

#define reg_usb_ep8123_ptr		REG_ADDR32(0x110)
#define reg_usb_ep8_ptr			REG_ADDR8(0x110)
#define reg_usb_ep1_ptr			REG_ADDR8(0x111)
#define reg_usb_ep2_ptr			REG_ADDR8(0x112)
#define reg_usb_ep3_ptr			REG_ADDR8(0x113)
#define reg_usb_ep4567_ptr		REG_ADDR32(0x114)
#define reg_usb_ep4_ptr			REG_ADDR8(0x114)
#define reg_usb_ep5_ptr			REG_ADDR8(0x115)
#define reg_usb_ep6_ptr			REG_ADDR8(0x116)
#define reg_usb_ep7_ptr			REG_ADDR8(0x117)
#define reg_usb_ep_ptr(i)		REG_ADDR8(0x110+((i) & 0x07))

#define reg_usb_ep8123_dat		REG_ADDR32(0x118)
#define reg_usb_ep8_dat			REG_ADDR8(0x118)
#define reg_usb_ep1_dat			REG_ADDR8(0x119)
#define reg_usb_ep2_dat			REG_ADDR8(0x11a)
#define reg_usb_ep3_dat			REG_ADDR8(0x11b)
#define reg_usb_ep4567_dat		REG_ADDR32(0x11c)
#define reg_usb_ep4_dat			REG_ADDR8(0x11c)
#define reg_usb_ep5_dat			REG_ADDR8(0x11d)
#define reg_usb_ep6_dat			REG_ADDR8(0x11e)
#define reg_usb_ep7_dat			REG_ADDR8(0x11f)
#define reg_usb_ep_dat(i)		REG_ADDR8(0x118+((i) & 0x07))

#define reg_usb_ep8_ctrl		REG_ADDR8(0x120)
#define reg_usb_ep1_ctrl		REG_ADDR8(0x121)
#define reg_usb_ep2_ctrl		REG_ADDR8(0x122)
#define reg_usb_ep3_ctrl		REG_ADDR8(0x123)
#define reg_usb_ep4_ctrl		REG_ADDR8(0x124)
#define reg_usb_ep5_ctrl		REG_ADDR8(0x125)
#define reg_usb_ep6_ctrl		REG_ADDR8(0x126)
#define reg_usb_ep7_ctrl		REG_ADDR8(0x127)
#define reg_usb_ep_ctrl(i)		REG_ADDR8(0x120+((i) & 0x07))

enum{
	FLD_USB_EP_BUSY = 			BIT(0),
	FLD_USB_EP_STALL =			BIT(1),
	FLD_USB_EP_DAT0 =			BIT(2),
	FLD_USB_EP_DAT1 =			BIT(3),
	FLD_USB_EP_MONO =			BIT(6),
	FLD_USB_EP_EOF_ISO =		BIT(7),
};

#define reg_usb_ep8123_buf_addr	REG_ADDR32(0x128)
#define reg_usb_ep8_buf_addr	REG_ADDR8(0x128)
#define reg_usb_ep1_buf_addr	REG_ADDR8(0x129)
#define reg_usb_ep2_buf_addr	REG_ADDR8(0x12a)
#define reg_usb_ep3_buf_addr	REG_ADDR8(0x12b)
#define reg_usb_ep4567_buf_addr	REG_ADDR32(0x12c)
#define reg_usb_ep4_buf_addr	REG_ADDR8(0x12c)
#define reg_usb_ep5_buf_addr	REG_ADDR8(0x12d)
#define reg_usb_ep6_buf_addr	REG_ADDR8(0x12e)
#define reg_usb_ep7_buf_addr	REG_ADDR8(0x12f)
#define reg_usb_ep_buf_addr(i)	REG_ADDR8(0x128+((i) & 0x07))

#define reg_usb_ram_ctrl		REG_ADDR8(0x130)
enum{
	FLD_USB_CEN_PWR_DN =		BIT(0),
	FLD_USB_CLK_PWR_DN =		BIT(1),
	FLD_USB_WEN_PWR_DN =		BIT(3),
	FLD_USB_CEN_FUNC =			BIT(4),
};

#define reg_usb_iso_mode		REG_ADDR8(0x138)
#define reg_usb_irq				REG_ADDR8(0x139)
#define reg_usb_mask			REG_ADDR8(0x13a)
#define reg_usb_ep8_send_max	REG_ADDR8(0x13b)
#define reg_usb_ep8_send_thre	REG_ADDR8(0x13c)
#define reg_usb_ep8_fifo_mode	REG_ADDR8(0x13d)
#define reg_usb_ep_max_size		REG_ADDR8(0x13e)

enum{
	FLD_USB_ENP8_FIFO_MODE =	BIT(0),
	FLD_USB_ENP8_FULL_FLAG =	BIT(1),
};
/****************************************************
	RF : begin  addr : 0x4e8
 *****************************************************/
#define reg_rf_tx_mode1			REG_ADDR8(0x400)
#define reg_rf_tx_mode			REG_ADDR16(0x400)
enum{
	FLD_RF_TX_DMA_EN =			BIT(0),
	FLD_RF_TX_CRC_EN =			BIT(1),
	FLD_RF_TX_BANDWIDTH =		BIT_RNG(2,3),
	FLD_RF_TX_OUTPUT = 			BIT(4),
	FLD_RF_TX_TST_OUT =			BIT(5),
	FLD_RF_TX_TST_EN =			BIT(6),
	FLD_RF_TX_TST_MODE =		BIT(7),
	FLD_RF_TX_ZB_PN_EN =		BIT(8),
	FLD_RF_TX_ZB_FEC_EN =		BIT(9),
	FLD_RF_TX_ZB_INTL_EN =		BIT(10),	// interleaving
	FLD_RF_TX_1M2M_PN_EN =		BIT(11),
	FLD_RF_TX_1M2M_FEC_EN =		BIT(12),
	FLD_RF_TX_1M2M_INTL_EN =	BIT(13), 	// interleaving
};
#define reg_rf_tx_buf_sta		REG_ADDR32(0x41c)

#define reg_rf_rx_sense_thr		REG_ADDR8(0x422)
#define reg_rf_rx_auto			REG_ADDR8(0x426)
enum{
	FLD_RF_RX_IRR_GAIN =		BIT(0),
	FLD_RF_RX_IRR_PHASE =		BIT(1),
	FLD_RF_RX_DAC_I =			BIT(2),
	FLD_RF_RX_DAC_Q =			BIT(3),
	FLD_RF_RX_LNA_GAIN =		BIT(4),
	FLD_RF_RX_MIX2_GAIN =		BIT(5),
	FLD_RF_RX_PGA_GAIN =		BIT(6),
	FLD_RF_RX_CAL_EN =			BIT(7),
};
#define reg_rf_rx_sync			REG_ADDR8(0x427)
enum{
	FLD_RF_FREQ_COMP_EN =		BIT(0),
	FLD_RF_ADC_SYNC =			BIT(1),
	FLD_RF_ADC_INP_SIGNED =		BIT(2),
	FLD_RF_SWAP_ADC_IQ =		BIT(3),
	FLD_RF_NOTCH_FREQ_SEL =		BIT(4),
	FLD_RF_NOTCH_BAND_SEL = 	BIT(5),
	FLD_RF_NOTCH_EN = 			BIT(6),
	FLD_RF_DN_CONV_FREQ_SEL =	BIT(7),
};

#define reg_rf_rx_mode			REG_ADDR8(0x428)
enum{
	FLD_RF_RX_EN =				BIT(0),
	FLD_RF_RX_MODE_1M =			BIT(1),
	FLD_RF_RX_MODE_2M =			BIT(2),
	FLD_RF_RX_LOW_IF =			BIT(3),
	FLD_RF_RX_BYPASS_DCOC =		BIT(4),
	FLD_RF_RX_MAN_FINE_TUNE = 	BIT(5),
	FLD_RF_RX_SINGLE_CAL =		BIT(6),
	FLD_RF_RX_LOW_PASS_FILTER =	BIT(7),
};

#define reg_rf_rx_pilot			REG_ADDR8(0x42b)
enum{
	FLD_RF_PILOT_LEN =			BIT_RNG(0,3),
	FLD_RF_ZB_SFD_CHK =			BIT(4),
	FLD_RF_1M_SFD_CHK =			BIT(5),
	FLD_RF_2M_SFD_CHK = 		BIT(6),
	FLD_RF_ZB_OR_AUTO = 		BIT(7),
};

#define reg_rf_rx_chn_dc		REG_ADDR32(0x42c)
#define reg_rf_rx_q_chn_cal		REG_ADDR8(0x42f)
enum{
	FLD_RF_RX_DCQ_HIGH =		BIT_RNG(0,6),
	FLD_RF_RX_DCQ_CAL_START =	BIT(7),
};
#define reg_rf_rx_pel			REG_ADDR16(0x434)
#define reg_rf_rx_pel_gain		REG_ADDR32(0x434)
#define reg_rf_rx_rssi_offset	REG_ADDR8(0x439)

#define reg_rf_rx_hdx			REG_ADDR8(0x43b)
enum{
	FLD_RX_HEADER_LEN =			BIT_RNG(0,3),
	FLD_RT_TICK_LO_SEL = 		BIT(4),
	FLD_RT_TICK_HI_SEL = 		BIT(5),
	FLD_RT_TICK_FRAME = 		BIT(6),
	FLD_PKT_LEN_OUTP_EN = 		BIT(7),
};

#define reg_rf_rx_gctl			REG_ADDR8(0x43c)
enum{
	FLD_RX_GCTL_CIC_SAT_LO_EN =	BIT(0),
	FLD_RX_GCTL_CIC_SAT_HI_EN = BIT(1),
	FLD_RX_GCTL_AUTO_PWR =		BIT(2),
	FLD_RX_GCTL_ADC_RST_VAL =	BIT(4),
	FLD_RX_GCTL_ADC_RST_EN =	BIT(5),
	FLD_RX_GCTL_PWR_CHG_DET_S =	BIT(6),
	FLD_RX_GCTL_PWR_CHG_DET_N = BIT(7),
};
#define reg_rf_rx_peak			REG_ADDR8(0x43d)
enum{
	FLD_RX_PEAK_DET_SRC_EN =	BIT_RNG(0,2),
	FLD_TX_PEAK_DET_EN =		BIT(3),
	FLD_PEAK_DET_NUM =			BIT_RNG(4,5),
	FLD_PEAK_MAX_CNT_PRD =		BIT_RNG(6,7),
};

#define reg_rf_rx_status		REG_ADDR8(0x443)
enum{
	FLD_RF_RX_STATE =			BIT_RNG(0,3),
	FLD_RF_RX_STA_RSV = 		BIT_RNG(4,5),
	FLD_RF_RX_INTR = 			BIT(6),
	FLD_RF_TX_INTR =			BIT(7),
};

#define reg_rf_irq_mask			REG_ADDR16(0xf1c)
#define reg_rf_irq_status		REG_ADDR16(0xf20)
#define reg_rf_fsm_timeout		REG_ADDR32(0xf2c)

#define		CLEAR_ALL_RFIRQ_STATUS   ( reg_rf_irq_status = 0xffff )

enum{
	FLD_RF_IRQ_RX = 			BIT(0),
	FLD_RF_IRQ_TX =				BIT(1),
	FLD_RF_IRQ_RX_TIMEOUT =		BIT(2),
	FLD_RF_IRQ_CRC =			BIT(4),
	FLD_RF_IRQ_CMD_DONE  =		BIT(5),
	FLD_RF_IRQ_FSM_TIMEOUT  =	BIT(6),
	FLD_RF_IRQ_RETRY_HIT =		BIT(7),
	FLD_RF_IRQ_FIRST_TIMEOUT =	BIT(10),
};


enum{
	FLD_RF_IRX_RX_TIMEOUT =		BIT(2),
	FLD_RF_IRX_CMD_DONE  =		BIT(5),
	FLD_RF_IRX_RETRY_HIT =		BIT(7),
};

// The value for FLD_RF_RX_STATE
enum{
	RF_RX_STA_IDLE = 0,
	RF_RX_STA_SET_GAIN = 1,
	RF_RX_STA_CIC_SETTLE = 2,
	RF_RX_STA_LPF_SETTLE = 3,
	RF_RX_STA_PE = 4,
	RF_RX_STA_SYN_START = 5,
	RF_RX_STA_GLOB_SYN = 6,
	RF_RX_STA_GLOB_LOCK = 7,
	RF_RX_STA_LOCAL_SYN = 8,
	RF_RX_STA_LOCAL_LOCK = 9,
	RF_RX_STA_ALIGN = 10,
	RF_RX_STA_ADJUST = 11,
	RF_RX_STA_DEMOD = 12,		// de modulation
	RF_RX_STA_FOOTER = 13,
};

#define reg_rx_rnd_mode			REG_ADDR8(0x447)
enum{
	FLD_RX_RND_SRC =			BIT(0),
	FLD_RX_RND_MANU_MODE =		BIT(1),
	FLD_RX_RND_AUTO_RD =		BIT(2),
	FLD_RX_RND_FREE_MODE =		BIT(3),
	FLD_RX_RND_CLK_DIV =		BIT_RNG(4,7),
};
#define reg_rnd_number			REG_ADDR16(0x448)

#define reg_bb_max_tick			REG_ADDR16(0x44c)
#define reg_rf_rtt				REG_ADDR32(0x454)
enum{
	FLD_RTT_CAL =				BIT_RNG(0,7),
	FLD_RTT_CYC1 =				BIT_RNG(8,15),
	FLD_RTT_LOCK =				BIT_RNG(16,23),
	FLD_RT_SD_DLY_40M =			BIT_RNG(24,27),
	FLD_RT_SD_DLY_BYPASS = 		BIT(28),
};

#define reg_rf_chn_rssi			REG_ADDR8(0x458)

#define reg_rf_rx_gain_agc(i)	REG_ADDR32(0x480+((i)<<2))

#define reg_rf_rx_dci			REG_ADDR8(0x4cb)	//  different from the document, why
#define reg_rf_rx_dcq			REG_ADDR8(0x4cf)	//  different from the document, why

#define reg_pll_rx_coarse_tune	REG_ADDR16(0x4d0)
#define reg_pll_rx_coarse_div	REG_ADDR8(0x4d2)
#define reg_pll_rx_fine_tune	REG_ADDR16(0x4d4)
#define reg_pll_rx_fine_div		REG_ADDR8(0x4d6)
#define reg_pll_tx_coarse_tune	REG_ADDR16(0x4d8)
#define reg_pll_tx_coarse_div	REG_ADDR8(0x4da)
#define reg_pll_tx_fine_tune	REG_ADDR16(0x4dc)
#define reg_pll_tx_fine_div		REG_ADDR8(0x4de)

#define reg_pll_rx_frac			REG_ADDR32(0x4e0)
#define reg_pll_tx_frac			REG_ADDR32(0x4e4)

#define reg_pll_tx_ctrl			REG_ADDR8(0x4e8)
#define reg_pll_ctrl16			REG_ADDR16(0x4e8)
#define reg_pll_ctrl			REG_ADDR32(0x4e8)
enum{
	FLD_PLL_TX_CYC0 =			BIT(0),
	FLD_PLL_TX_SOF =			BIT(1),
	FLD_PLL_TX_CYC1 =			BIT(2),
	FLD_PLL_TX_PRE_EN =			BIT(3),
	FLD_PLL_TX_VCO_EN =			BIT(4),
	FLD_PLL_TX_PWDN_DIV =		BIT(5),
	FLD_PLL_TX_MOD_EN =			BIT(6),
	FLD_PLL_TX_MOD_TRAN_EN =	BIT(7),
	FLD_PLL_RX_CYC0 =			BIT(8),
	FLD_PLL_RX_SOF = 			BIT(9),
	FLD_PLL_RX_CYC1 =			BIT(10),
	FLD_PLL_RX_PRES_EN = 		BIT(11),
	FLD_PLL_RX_VCO_EN =			BIT(12),
	FLD_PLL_RX_PWDN_DIV =		BIT(13),
	FLD_PLL_RX_PEAK_EN =		BIT(14),
	FLD_PLL_RX_TP_CYC = 		BIT(15),
	FLD_PLL_SD_RSTB =			BIT(16),
	FLD_PLL_SD_INTG_EN =		BIT(17),
	FLD_PLL_CP_TRI = 			BIT(18),
	FLD_PLL_PWDN_INTG1 = 		BIT(19),
	FLD_PLL_PWDN_INTG2 =		BIT(20),
	FLD_PLL_PWDN_INTG_DIV =		BIT(21),
	FLD_PLL_PEAK_DET_EN =		BIT(22),
	FLD_PLL_OPEN_LOOP_EN =		BIT(23),
	FLD_PLL_RX_TICK_EN =		BIT(24),
	FLD_PLL_TX_TICK_EN =		BIT(25),
	FLD_PLL_RX_ALWAYS_ON =		BIT(26),
	FLD_PLL_TX_ALWAYS_ON =		BIT(27),
	FLD_PLL_MANUAL_MODE_EN =	BIT(28),
	FLD_PLL_CAL_DONE_EN =		BIT(29),
	FLD_PLL_LOCK_EN =			BIT(30),
};
#define reg_pll_rx_ctrl			REG_ADDR8(0x4e9)
enum{
	FLD_PLL_RX2_CYC0 =			BIT(0),
	FLD_PLL_RX2_SOF = 			BIT(1),
	FLD_PLL_RX2_CYC1 =			BIT(2),
	FLD_PLL_RX2_PRES_EN = 		BIT(3),
	FLD_PLL_RX2_VCO_EN =		BIT(4),
	FLD_PLL_RX2_PD_DIV =		BIT(5),
	FLD_PLL_RX2_PEAK_EN =		BIT(6),
	FLD_PLL_RX2_TP_CYC = 		BIT(7),
};

#define reg_pll_ctrl_a			REG_ADDR8(0x4eb)
enum{
	FLD_PLL_A_RX_TICK_EN =		BIT(0),
	FLD_PLL_A_TX_TICK_EN =		BIT(1),
	FLD_PLL_A_RX_ALWAYS_ON =	BIT(2),
	FLD_PLL_A_TX_ALWAYS_ON =	BIT(3),
	FLD_PLL_A_MANUAL_MODE_EN =	BIT(4),
	FLD_PLL_A_CAL_DONE_EN =		BIT(5),
	FLD_PLL_A_LOCK_EN =			BIT(6),
};
// pll polarity
#define reg_pll_pol_ctrl		REG_ADDR16(0x4ec)
enum{
	FLD_PLL_POL_TX_PRE_EN =		BIT(0),
	FLD_PLL_POL_TX_VCO_EN =		BIT(1),
	FLD_PLL_POL_TX_PD_DIV =		BIT(2),
	FLD_PLL_POL_MOD_EN =		BIT(3),
	FLD_PLL_POL_MOD_TRAN_EN =	BIT(4),
	FLD_PLL_POL_RX_PRE_EN =		BIT(5),
	FLD_PLL_POL_RX_VCO_EN =		BIT(6),
	FLD_PLL_POL_RX_PD_DIV =		BIT(7),
	FLD_PLL_POL_SD_RSTB =		BIT(8),
	FLD_PLL_POL_SD_INTG_EN =	BIT(9),
	FLD_PLL_POL_CP_TRI =		BIT(10),
	FLD_PLL_POL_TX_SOF =		BIT(11),
	FLD_PLL_POL_RX_SOF =		BIT(12),
};

#define reg_rf_rx_cap			REG_ADDR16(0x4f0)		//  电容
#define reg_rf_tx_cap			REG_ADDR16(0x4f0)		//  电容

/****************************************************
 dma mac regs struct: begin  addr : 0xC00
 *****************************************************/
#define reg_dma0_addr			REG_ADDR16(0xc00)
#define reg_dma0_ctrl			REG_ADDR16(0xc02)
#define reg_dma1_addr			REG_ADDR16(0xc04)
#define reg_dma1_ctrl			REG_ADDR16(0xc06)
#define reg_dma2_addr			REG_ADDR16(0xc08)
#define reg_dma2_ctrl			REG_ADDR16(0xc0a)
#define reg_dma3_addr			REG_ADDR16(0xc0c)
#define reg_dma3_ctrl			REG_ADDR16(0xc0e)
#define reg_dma4_addr			REG_ADDR16(0xc10)
#define reg_dma4_ctrl			REG_ADDR16(0xc12)
#define reg_dma5_addr			REG_ADDR16(0xc14)
#define reg_dma5_ctrl			REG_ADDR16(0xc16)

#define reg_rf_manual_irq_status    REG_ADDR16(0xc26)   //Rx buf 0 data received
#define FLD_RF_MANUAL_IRQ_RX     BIT(2)


#define reg_dma_rx_rptr			REG_ADDR8(0xc28)
#define reg_dma_rx_wptr			REG_ADDR8(0xc29)

#define reg_dma_tx_rptr			REG_ADDR8(0xc2a)
#define reg_dma_tx_wptr			REG_ADDR8(0xc2b)
#define reg_dma_tx_fifo			REG_ADDR16(0xc2c)
enum{
	FLD_DMA_BUF_SIZE =			BIT_RNG(0,7),
	FLD_DMA_WR_MEM =			BIT(8),
	FLD_DMA_PINGPONG_EN =		BIT(9),
	FLD_DMA_FIFO_EN =			BIT(10),
	FLD_DMA_AUTO_MODE =			BIT(11),
	FLD_DMA_BYTE_MODE =			BIT(12),
	FLD_DMA_FIFO8 	=			(BIT(15) | BIT(14) | FLD_DMA_WR_MEM | FLD_DMA_PINGPONG_EN),

	FLD_DMA_RPTR_CLR =			BIT(4),
	FLD_DMA_RPTR_NEXT =			BIT(5),
	FLD_DMA_RPTR_SET =			BIT(6),
};

#define reg_dma_chn_en			REG_ADDR8(0xc20)
#define reg_dma_chn_irq_msk		REG_ADDR8(0xc21)
#define reg_dma_tx_rdy0			REG_ADDR8(0xc24)
#define reg_dma_tx_rdy1			REG_ADDR8(0xc25)
#define reg_dma_rx_rdy0			REG_ADDR8(0xc26)
#define reg_dma_irq_src			reg_dma_rx_rdy0
#define reg_dma_rx_rdy1			REG_ADDR8(0xc27)
enum{
	FLD_DMA_UART_RX =			BIT(0),		//  not sure ???
	FLD_DMA_UART_TX =			BIT(1),
	FLD_DMA_RF_RX =				BIT(2),		//  not sure ???
	FLD_DMA_RF_TX =				BIT(3),
};

#define reg_dma0_addrHi			REG_ADDR8(0xc40)

//  The default channel assignment
#define reg_dma_uart_rx_addr	reg_dma0_addr
#define reg_dma_uart_rx_ctrl	reg_dma0_ctrl
#define reg_dma_uart_tx_addr	reg_dma1_addr
#define reg_dma_uart_tx_ctrl	reg_dma1_ctrl

#define reg_dma_rf_rx_addr		reg_dma2_addr
#define reg_dma_rf_rx_ctrl		reg_dma2_ctrl
#define reg_dma_rf_tx_addr		reg_dma3_addr
#define reg_dma_rf_tx_ctrl		reg_dma3_ctrl

#define reg_aes_ctrl            REG_ADDR8(0x540)
#define reg_aes_data            REG_ADDR32(0x548)
#define reg_aes_key(key_id)     reg_aes_key##key_id
#define reg_aes_key0            REG_ADDR8(0x550)
#define reg_aes_key1            REG_ADDR8(0x551)
#define reg_aes_key2            REG_ADDR8(0x552)
#define reg_aes_key3            REG_ADDR8(0x553)
#define reg_aes_key4            REG_ADDR8(0x554)
#define reg_aes_key5            REG_ADDR8(0x555)
#define reg_aes_key6            REG_ADDR8(0x556)
#define reg_aes_key7            REG_ADDR8(0x557)
#define reg_aes_key8            REG_ADDR8(0x558)
#define reg_aes_key9            REG_ADDR8(0x559)
#define reg_aes_key10            REG_ADDR8(0x55a)
#define reg_aes_key11            REG_ADDR8(0x55b)
#define reg_aes_key12            REG_ADDR8(0x55c)
#define reg_aes_key13            REG_ADDR8(0x55d)
#define reg_aes_key14            REG_ADDR8(0x55e)
#define reg_aes_key15            REG_ADDR8(0x55f)
/****************************************************
 audio regs struct: begin  addr : 0x560
 *****************************************************/
#define reg_aud_ctrl			REG_ADDR8(0x560)
enum{
	FLD_AUD_ENABLE	 =			BIT(0),
	FLD_AUD_SDM_PLAY_EN = 		BIT(1),
	FLD_AUD_SHAPPING_EN =		BIT(2),
	FLD_AUD_PN_SHAPPING_BYPASS =    BIT(2),
	FLD_AUD_SHAPING_EN         =    BIT(3),
	FLD_AUD_PN2_GENERATOR_EN   =    BIT(4),
	FLD_AUD_PN1_GENERATOR_EN   =    BIT(5),
	FLD_AUD_CONST_VAL_INPUT_EN =    BIT(6),
};

#define reg_aud_vol_ctrl		REG_ADDR8(0x561)
#define reg_aud_pn1				REG_ADDR8(0x562)
#define reg_aud_pn2				REG_ADDR8(0x563)

#define reg_ascl_step			REG_ADDR16(0x564)
#define reg_aud_base_adr		REG_ADDR16(0x568)
#define reg_aud_buff_size		REG_ADDR8(0x56a)
#define reg_aud_rptr			REG_ADDR16(0x56c)
#define reg_sdm_ptr				reg_aud_rptr

#define	AUD_SDM_STEP(i,o)		(0x8000*(i/100)/(o/100))

static inline u16 get_sdm_rd_ptr (void) {
	return reg_aud_rptr << 0;
}

enum {
	FLD_AUD_PN_1BIT		= 0x104050,
	FLD_AUD_PN_4BITS	= 0x0c404c,
};

#define reg_aud_const			REG_ADDR32(0x56)


#define reg_aud_filter			REG_ADDR16(0xb80)
enum {
	FLD_AUD_FLT_IBFT	=	BIT(0),
	FLD_AUD_FLT_AUTO	=	BIT(1),
	FLD_AUD_FLT_FIFO0	=	BIT(2),
	FLD_AUD_FLT_FIFO1	=	BIT(3),
	FLD_AUD_FLT_BYPASS0	=	BIT(5),
	FLD_AUD_FLT_BYPASS1	=	BIT(6),
	FLD_AUD_FLT_BYPASS		=   0x60,
	FLD_AUD_FLT_I0			=   0x47,
	FLD_AUD_FLT_I1			=   0x2b,
	FLD_AUD_FLT_I0I1		=   0x0f,
	FLD_AUD_FLT_CFGHIGH		= 	0x1000,
};
#define reg_aud_sram			REG_ADDR8(0xb81)


/****************************************************
 gpio regs struct: begin  0x580
 *****************************************************/
#define reg_gpio_pa_in			REG_ADDR8(0x580)
#define reg_gpio_pa_ie			REG_ADDR8(0x581)
#define reg_gpio_pa_oen			REG_ADDR8(0x582)
#define reg_gpio_pa_out			REG_ADDR8(0x583)
#define reg_gpio_pa_pol			REG_ADDR8(0x584)
#define reg_gpio_pa_ds			REG_ADDR8(0x585)
#define reg_gpio_pa_gpio		REG_ADDR8(0x586)
#define reg_gpio_pa_irq_en		REG_ADDR8(0x587)

#define reg_gpio_pb_in			REG_ADDR8(0x588)
#define reg_gpio_pb_ie			REG_ADDR8(0x589)
#define reg_gpio_pb_oen			REG_ADDR8(0x58a)
#define reg_gpio_pb_out			REG_ADDR8(0x58b)
#define reg_gpio_pb_pol			REG_ADDR8(0x58c)
#define reg_gpio_pb_ds			REG_ADDR8(0x58d)
#define reg_gpio_pb_gpio		REG_ADDR8(0x58e)
#define reg_gpio_pb_irq_en		REG_ADDR8(0x58f)

#define reg_gpio_pc_in			REG_ADDR8(0x590)
#define reg_gpio_pc_ie			REG_ADDR8(0x591)
#define reg_gpio_pc_oen			REG_ADDR8(0x592)
#define reg_gpio_pc_out			REG_ADDR8(0x593)
#define reg_gpio_pc_pol			REG_ADDR8(0x594)
#define reg_gpio_pc_ds			REG_ADDR8(0x595)
#define reg_gpio_pc_gpio		REG_ADDR8(0x596)
#define reg_gpio_pc_irq_en		REG_ADDR8(0x597)

#define reg_gpio_pd_in			REG_ADDR8(0x598)
#define reg_gpio_pd_ie			REG_ADDR8(0x599)
#define reg_gpio_pd_oen			REG_ADDR8(0x59a)
#define reg_gpio_pd_out			REG_ADDR8(0x59b)
#define reg_gpio_pd_pol			REG_ADDR8(0x59c)
#define reg_gpio_pd_ds			REG_ADDR8(0x59d)
#define reg_gpio_pd_gpio		REG_ADDR8(0x59e)
#define reg_gpio_pd_irq_en		REG_ADDR8(0x59f)

#define reg_gpio_pe_in			REG_ADDR8(0x5a0)
#define reg_gpio_pe_ie			REG_ADDR8(0x5a1)
#define reg_gpio_pe_oen			REG_ADDR8(0x5a2)
#define reg_gpio_pe_out			REG_ADDR8(0x5a3)
#define reg_gpio_pe_pol			REG_ADDR8(0x5a4)
#define reg_gpio_pe_ds			REG_ADDR8(0x5a5)
#define reg_gpio_pe_gpio		REG_ADDR8(0x5a6)
#define reg_gpio_pe_irq_en		REG_ADDR8(0x5a7)
#define reg_gpio_pf_in			REG_ADDR8(0x5a8)
#define reg_gpio_pf_ie			REG_ADDR8(0x5a9)
#define reg_gpio_pf_oen			REG_ADDR8(0x5aa)
#define reg_gpio_pf_out			REG_ADDR8(0x5ab)
#define reg_gpio_pf_pol			REG_ADDR8(0x5ac)
#define reg_gpio_pf_ds			REG_ADDR8(0x5ad)
#define reg_gpio_pf_gpio		REG_ADDR8(0x5ae)
#define reg_gpio_pf_irq_en		REG_ADDR8(0x5af)


#define reg_gpio_pa_setting1	REG_ADDR32(0x580)
#define reg_gpio_pa_setting2	REG_ADDR32(0x584)
#define reg_gpio_pb_setting1	REG_ADDR32(0x588)
#define reg_gpio_pb_setting2	REG_ADDR32(0x58c)
#define reg_gpio_pc_setting1	REG_ADDR32(0x590)
#define reg_gpio_pc_setting2	REG_ADDR32(0x594)
#define reg_gpio_pd_setting1	REG_ADDR32(0x598)
#define reg_gpio_pd_setting2	REG_ADDR32(0x59c)
#define reg_gpio_pe_setting1	REG_ADDR32(0x5a0)
#define reg_gpio_pe_setting2	REG_ADDR32(0x5a4)


#define reg_gpio_ctrl			REG_ADDR32(0x5a4)
enum{
	GPIO_WAKEUP_EN = BIT(0),
	GPIO_IRQ_EN	   = BIT(1),
	I2S_SLAVE_EN   = BIT(2),
	RMII_REFCLK_OUTPUT_EN = BIT(3),
};


#define reg_gpio_config_func0 REG_ADDR8(0x5b0)
enum{
	FLD_I2S_REFCLK_DMIC	=	BIT(0),
	FLD_I2S_BCK_BB_PEAK	=	BIT(1),
	FLD_I2S_BCK_PWM1	=	BIT(2),
	FLD_I2S_LCK_UART_RX	=	BIT(3),
	FLD_I2S_LCK_PWM2	=	BIT(4),
	FLD_I2S_DO_UART_TX	=	BIT(5),
	FLD_I2S_DO_PWM3		=	BIT(6),
	FLD_I2S_DI_DMIC		=	BIT(7),
};
#define reg_gpio_config_func1 REG_ADDR8(0x5b1)
enum{
	FLD_RP_TX_CYC1		=	BIT(0),
	FLD_RN_BB_RSSI		=	BIT(1),
	FLD_GP6_BB_SS2		=	BIT(2),
	FLD_GP7_RXADC_CLK	=	BIT(3),
	FLD_RP_T0			=	BIT(4),
	FLD_RN_T1			=	BIT(5),
	FLD_GP6_TE			=	BIT(6),
	FLD_GP7_MDC			=	BIT(7),
};
#define reg_gpio_config_func2 REG_ADDR8(0x5b2)
enum{
	FLD_GP8_RXADC_DAT	=	BIT(0),
	FLD_GP9_BB_SS1		=	BIT(1),
	FLD_GP10_BBSS0		=	BIT(2),
	FLD_SWS_BB_GAIN4	=	BIT(3),
	FLD_DMIC_CK_BBCLK_BB	=	BIT(4),
	FLD_DMIC_CK_REFCLK	=	BIT(5),
	FLD_UART_TX			=	BIT(6),
	FLD_UART_RX			=	BIT(7),
};
#define reg_gpio_config_func3 REG_ADDR8(0x5b3)
enum{
	FLD_CN_BB_GAIN3		=	BIT(0),
	FLD_CK_BB_GAIN2		=	BIT(1),
	FLD_DO_BB_GAIN1		=	BIT(2),
	FLD_DI_BB_GAIN0		=	BIT(3),
//	FLD_I2S_LCK_PWM2	=	BIT(4),//NOT SURE
	FLD_I2S_DO_RXDV		=	BIT(5),
	FLD_I2S_DI_RXER		=	BIT(6),
	FLD_I2S_DI_TXSD		=	BIT(7),
};
#define reg_gpio_config_func4 REG_ADDR8(0x5b4)
enum{
	FLD_DMIC_CK_RX_CLK	=	BIT(0),
	FLD_I2S_DI_RX_DAT	=	BIT(1),
};

/****************************************************
 timer regs struct: begin  0x620
 *****************************************************/
#define reg_tmr_ctrl			REG_ADDR32(0x620)
#define reg_tmr_ctrl16			REG_ADDR16(0x620)
#define reg_tmr_ctrl8			REG_ADDR8(0x620)
enum{
	FLD_TMR0_EN =				BIT(0),
	FLD_TMR0_MODE =				BIT_RNG(1,2),
	FLD_TMR1_EN = 				BIT(3),
	FLD_TMR1_MODE =				BIT_RNG(4,5),
	FLD_TMR2_EN =				BIT(6),
	FLD_TMR2_MODE = 			BIT_RNG(7,8),
	FLD_TMR_WD_CAPT = 			BIT_RNG(9,22),
	FLD_TMR_WD_EN =				BIT(23),
	FLD_TMR0_STA =				BIT(24),
	FLD_TMR1_STA =				BIT(25),
	FLD_TMR2_STA =				BIT(26),
	FLD_CLR_WD =				BIT(27),
};
#define WATCHDOG_TIMEOUT_COEFF	18		//  check register definiton, 0x622

#define WATCHDOG_DISABLE	( reg_tmr_ctrl &= ~FLD_TMR_WD_EN )

#define reg_tmr_sta				REG_ADDR8(0x623)
enum{
	FLD_TMR_STA_TMR0 =			BIT(0),
	FLD_TMR_STA_TMR1 =			BIT(1),
	FLD_TMR_STA_TMR2 =			BIT(2),
	FLD_TMR_STA_WD =			BIT(3),
};

#define reg_tmr0_capt			REG_ADDR32(0x624)
#define reg_tmr1_capt			REG_ADDR32(0x628)
#define reg_tmr2_capt			REG_ADDR32(0x62c)
#define reg_tmr_capt(i)			REG_ADDR32(0x624 + ((i) << 2))
#define reg_tmr0_tick			REG_ADDR32(0x630)
#define reg_tmr1_tick			REG_ADDR32(0x634)
#define reg_tmr2_tick			REG_ADDR32(0x638)
#define reg_tmr_tick(i)			REG_ADDR32(0x630 + ((i) << 2))

/****************************************************
 interrupt regs struct: begin  0x640
 *****************************************************/
#define reg_irq_mask			REG_ADDR32(0x640)
#define reg_irq_pri				REG_ADDR32(0x644)
#define reg_irq_src				REG_ADDR32(0x648)
#define reg_irq_src3			REG_ADDR8(0x64a)
enum{
	FLD_IRQ_TMR0_EN =			BIT(0),
	FLD_IRQ_TMR1_EN =			BIT(1),
	FLD_IRQ_TMR2_EN =			BIT(2),
	FLD_IRQ_USB_PWDN_EN =		BIT(3),
	FLD_IRQ_DMA_EN =			BIT(4),
	FLD_IRQ_DAM_FIFO_EN =		BIT(5),
	FLD_IRQ_UART_EN 	=		BIT(6),
	FLD_IRQ_HOST_CMD_EN =		BIT(7),

	FLD_IRQ_EP0_SETUP_EN =		BIT(8),
	FLD_IRQ_EP0_DAT_EN =		BIT(9),
	FLD_IRQ_EP0_STA_EN =		BIT(10),
	FLD_IRQ_SET_INTF_EN =		BIT(11),
	FLD_IRQ_EP_DATA_EN =		BIT(12),
	FLD_IRQ_IRQ4_EN =			BIT(12),
	FLD_IRQ_ZB_RT_EN =			BIT(13),
	FLD_IRQ_SW_PWM_EN =			BIT(14),  //irq_software | irq_pwm
	FLD_IRQ_AN_EN =				BIT(15),

	FLD_IRQ_USB_250US_EN =		BIT(16),
	FLD_IRQ_USB_RST_EN =		BIT(17),
	FLD_IRQ_GPIO_EN =			BIT(18),
	FLD_IRQ_PM_EN =				BIT(19),
	FLD_IRQ_SYSTEM_TIMER =		BIT(20),
	FLD_IRQ_GPIO_RISC0_EN =		BIT(21),
	FLD_IRQ_GPIO_RISC1_EN =		BIT(22),
	FLD_IRQ_CMP_EN		  = 	BIT(23),

	FLD_IRQ_EN =				BIT_RNG(24,31),
};
#define reg_irq_en				REG_ADDR8(0x643)

#define reg_system_tick			REG_ADDR32(0x740)
#define reg_system_tick_irq		REG_ADDR32(0x744)
#define reg_system_wakeup_tick	REG_ADDR32(0x748)
#define reg_system_tick_mode	REG_ADDR8(0x74c)
#define reg_system_tick_ctrl	REG_ADDR8(0x74f)

enum {
	FLD_SYSTEM_TICK_START	=		BIT(0),
	FLD_SYSTEM_TICK_STOP	=		BIT(1),
	FLD_SYSTEM_TICK_RUNNING	=		BIT(1),

	FLD_SYSTEM_TICK_IRQ_EN  = 		BIT(1),
};


/****************************************************
 PWM regs define:  begin  0x780
 *****************************************************/

typedef enum {
	PWM0_ID = 0,
	PWM1_ID,
	PWM2_ID,
	PWM3_ID,
	PWM4_ID,
	PWM5_ID,
}pwm_id;


#define reg_pwm_enable			REG_ADDR8(0x780)
#define reg_pwm_clk				REG_ADDR8(0x781)

#define reg_pwm_mode			REG_ADDR8(0x782)
enum{
	PWM_NORMAL_MODE = 0x00,
	PWM_COUNT_MODE  = 0x01,
	PWM_IR_MODE     = 0x03,
	PWM_IR_FIFO_MODE  = 0x13,
};

#define reg_pwm_invert			REG_ADDR8(0x783)
#define reg_pwm_n_invert		REG_ADDR8(0x784)
#define reg_pwm_pol				REG_ADDR8(0x785)

#define reg_pwm_phase(i)		REG_ADDR16(0x788 + (i << 1))
#define reg_pwm_cycle(i)		REG_ADDR32(0x794 + (i << 2))
#define reg_pwm_cmp(i)			REG_ADDR16(0x794 + (i << 2))
#define reg_pwm_max(i)			REG_ADDR16(0x796 + (i << 2))
enum{
	FLD_PWM_CMP  = 				BIT_RNG(0,15),
	FLD_PWM_MAX  = 				BIT_RNG(16,31),
};

#define reg_pwm_pulse_num		REG_ADDR16(0x7ac)
#define reg_pwm_irq_mask		REG_ADDR8(0x7b0)
#define reg_pwm_irq_sta			REG_ADDR8(0x7b1)

typedef enum{
	FLD_IRQ_PWM0_PNUM =			BIT(0),
	FLD_IRQ_PWM0_FRAME =		BIT(2),
	FLD_IRQ_PWM1_FRAME =		BIT(3),
	FLD_IRQ_PWM2_FRAME =		BIT(4),
	FLD_IRQ_PWM3_FRAME =		BIT(5),
	FLD_IRQ_PWM4_FRAME 	=		BIT(6),
	FLD_IRQ_PWM5_FRAME =		BIT(7),
}PWM_IRQ;


#define reg_pwm0_fifo_mode_irq_mask		REG_ADDR8(0x7b2)
#define reg_pwm0_fifo_mode_irq_sta		REG_ADDR8(0x7b3)
enum{
	FLD_IRQ_PWM0_FIFO_MODE_STOP =		BIT(0),
	FLD_IRQ_PWM0_FIFO_MODE_CNT  =		BIT(1),
};






static inline void pwm_set_mode(pwm_id id, int mode){
	if(PWM0_ID == id){
		reg_pwm_mode = mode;  //only PWM0 has count/IR/fifo IR mode
	}
}

//Set PWM clock frequency
static inline void pwm_set_clk(int system_clock_hz, int pwm_clk){
	reg_pwm_clk = (int)system_clock_hz /pwm_clk - 1;
}


static inline void pwm_set_cmp(pwm_id id, u16 cmp_tick){
	reg_pwm_cmp(id) = cmp_tick;
}

static inline void pwm_set_cycle(pwm_id id, u16 cycle_tick){
	reg_pwm_max(id) = cycle_tick;
}


static inline void pwm_set_cycle_and_duty(pwm_id id, u16 cycle_tick, u16 cmp_tick){
	reg_pwm_cycle(id) = MASK_VAL(FLD_PWM_CMP, cmp_tick, FLD_PWM_MAX, cycle_tick);
}


static inline void pwm_set_phase(pwm_id id, u16 phase){
	reg_pwm_phase(id) = phase;
}


static inline void pwm_set_pulse_num(pwm_id id, u16 pulse_num){
	if(PWM0_ID == id){
		reg_pwm_pulse_num = pulse_num;
	}

}

static inline void pwm_start(pwm_id id){
	BM_SET(reg_pwm_enable, BIT(id));
}

static inline void pwm_stop(pwm_id id){
	BM_CLR(reg_pwm_enable, BIT(id));
}

//revert PWMx
static inline void pwm_revert(pwm_id id){
	reg_pwm_invert |= BIT(id);
}


//revert PWMx_N
static inline void pwm_n_revert(pwm_id id){
	reg_pwm_n_invert |= BIT(id);
}

static inline void pwm_polo_enable(pwm_id id, int en){
	if(en){
		BM_SET(reg_pwm_pol, BIT(id));
	}else{
		BM_CLR(reg_pwm_pol, BIT(id));
	}
}


static inline void pwm_set_interrupt_enable(PWM_IRQ irq){
//	if(en){
//		BM_SET(reg_pwm_irq_mask, irq);
//	}else{
//		BM_CLR(reg_pwm_irq_mask, irq);
//	}
	BM_SET(reg_pwm_irq_mask, irq);
}

static inline void pwm_set_interrupt_disable(PWM_IRQ irq){
	BM_CLR(reg_pwm_irq_mask, irq);
}

static inline void pwm_clear_interrupt_status( PWM_IRQ irq){
	reg_pwm_irq_sta = irq;
}


typedef enum{
	WAVEFORM0_ID = 0x00,
	WAVEFORM1_ID = 0x01,
	WAVEFORM2_ID = 0x02,
	WAVEFORM3_ID = 0x03,
	WAVEFORM4_ID = 0x04,
	WAVEFORM5_ID = 0x05,
	WAVEFORM6_ID = 0x06,
	WAVEFORM7_ID = 0x07,
}WAVEFORM_ID;


typedef enum{
	WAVEFORM0_TAIL = 0x00,
	WAVEFORM0_HEAD = 0x01,
	WAVEFORM1_TAIL = 0x02,
	WAVEFORM1_HEAD = 0x03,
	WAVEFORM2_TAIL = 0x04,
	WAVEFORM2_HEAD = 0x05,
	WAVEFORM3_TAIL = 0x06,
	WAVEFORM3_HEAD = 0x07,
	WAVEFORM4_TAIL = 0x08,
	WAVEFORM4_HEAD = 0x09,
	WAVEFORM5_TAIL = 0x0a,
	WAVEFORM5_HEAD = 0x0b,
	WAVEFORM6_TAIL = 0x0c,
	WAVEFORM6_HEAD = 0x0d,
	WAVEFORM7_TAIL = 0x0e,
	WAVEFORM7_HEAD = 0x0f,
}WAVEFORM_TypeDef;


#define reg_pwm0_waveform_carrier_en					REG_ADDR16(0x7cc)
#define reg_pwm0_waveform_carrier_pulse_num(i)			REG_ADDR16(0x7d0 + (i<<1))


#define reg_pwm0_fifo_input								REG_ADDR16(0x7f0)


static inline void pwm_set_waveform_carrier_en(WAVEFORM_TypeDef wf_type, int en)
{
	u16 mask = BIT((wf_type^0x01));
	if(en){
		BM_SET(reg_pwm0_waveform_carrier_en, mask);
	}
	else{
		BM_CLR(reg_pwm0_waveform_carrier_en, mask);
	}
}


static inline void pwm_config_waveform_carrier_num(WAVEFORM_TypeDef wf_type, u16 carrier_num)
{
	reg_pwm0_waveform_carrier_pulse_num(wf_type) = carrier_num;
}



static inline void pwm_set_waveform_input_1(WAVEFORM_ID wf_id)
{
	reg_pwm0_fifo_input = (0xfff0 | wf_id);
}

static inline void pwm_set_waveform_input_2(WAVEFORM_ID wf_id1, WAVEFORM_ID wf_id2)
{
	reg_pwm0_fifo_input = (0xff00 | wf_id2<<4 | wf_id1);
}

static inline void pwm_set_waveform_input_3(WAVEFORM_ID wf_id1, WAVEFORM_ID wf_id2, WAVEFORM_ID wf_id3)
{
	reg_pwm0_fifo_input = (0xf000 | wf_id3<<8 | wf_id2<<4 | wf_id1);
}

static inline void pwm_set_waveform_input_4(WAVEFORM_ID wf_id1, WAVEFORM_ID wf_id2, WAVEFORM_ID wf_id3, WAVEFORM_ID wf_id4)
{
	reg_pwm0_fifo_input = (wf_id4<<12 | wf_id3<<8 | wf_id2<<4 | wf_id1);
}





//////////////////////////////////////////////////////////////
// DFIFO
//////////////////////////////////////////////////////////////

#define reg_fifo0_data			REG_ADDR32(0x800)
#define reg_fifo1_data			REG_ADDR32(0x900)
#define reg_fifo2_data			REG_ADDR32(0xa00)

/****************************************************
 dfifo regs define:  begin  0xb00
 *****************************************************/
#define reg_dfifo0_buf			REG_ADDR32(0xb00)
#define reg_dfifo1_buf			REG_ADDR32(0xb04)
#define reg_dfifo2_buf			REG_ADDR32(0xb08)
enum{
	FLD_DFIFO_BUF_ADDR =		BIT_RNG(0,15),
	FLD_DFIFO_BUF_SIZE =		BIT_RNG(16,23),
};

#define reg_dfifo0_addr			REG_ADDR16(0xb00)
#define reg_dfifo0_addr2		REG_ADDR8(0xb03)

#define reg_dfifo0_size			REG_ADDR8(0xb02)



#define reg_dfifo_ana_in		REG_ADDR8(0xb03)
enum{
	FLD_DFIFO_MIC0_RISING_EDGE = BIT(0),
	FLD_DFIFO_MIC_ADC_IN 	= BIT(1),
	FLD_DFIFO_EN               = BIT(4),
	FLD_DFIFO_WPTR_EN          = BIT(5),
	FLD_DFIFO_WPTR_CLR         = BIT(6),
	FLD_DFIFO_AUD_INPUT_MONO =	BIT(4) | BIT(5),
//	FLD_DFIFO_AUD_INPUT_BYPASS = BIT(5),
};
enum{
	REG_AUD_INPUT_SEL_USB = 0,
	REG_AUD_INPUT_SEL_I2S = 1,
	REG_AUD_INPUT_SEL_ADC = 2,
	REG_AUD_INPUT_SEL_DMIC = 3,
};

#define reg_dfifo_scale			REG_ADDR8(0xb04)
enum{
	FLD_DFIFO2_DEC_CIC =		BIT_RNG(0,3),
	FLD_DFIFO0_DEC_SCALE =		BIT_RNG(4,6),
};


#define reg_aud_vol_step        REG_ADDR8(0xb0b)
#define reg_aud_tick_interval   REG_ADDR16(0xb0c)
enum {
	FLD_AUD_ALC_VOL_TICK_L    = BIT_RNG(0,7),
	FLD_AUD_ALC_VOL_TICK_H    = BIT_RNG(8,13),
};



#define	reg_audio_dfifo_mode	REG_ADDR8(0xb10)
#define	REG_AUDIO_DFIFO_MODE    reg_audio_dfifo_mode
enum{
	FLD_AUD_DFIFO0_IN 		= BIT(0),
	FLD_AUD_DFIFO1_IN 		= BIT(1),
	FLD_AUD_DFIFO2_IN 		= BIT(2),
	FLD_AUD_DFIFO0_OUT 		= BIT(3),
	FLD_AUD_DFIFO0_L_INT	= BIT(4),
	FLD_AUD_DFIFO0_H_INT	= BIT(5),
	FLD_AUD_DFIFO1_H_INT	= BIT(6),
	FLD_AUD_DFIFO2_H_INT	= BIT(7),
};


#define	reg_audio_dfifo_ain		REG_ADDR8(0xb11)
#define	REG_AUDIO_DFIFO_AIN		reg_audio_dfifo_ain
enum{
	FLD_AUD_DMIC0_DATA_IN_RISING_EDGE = BIT(0),
	FLD_AUD_DMIC1_DATA_IN_RISING_EDGE = BIT(1),
	FLD_AUD_INPUT_SELECT     		  = BIT_RNG(2,3),
	FLD_AUD_INPUT_MONO_MODE	   		  = BIT(4),
	FLD_AUD_DECIMATION_FILTER_BYPASS  = BIT(5),
	FLD_AUD_DMIC_RISING_EDGE_BYPASS   = BIT(6),
	FLD_AUD_DMIC_FALLING_EDGE_BYPASS  = BIT(7),

};

//enum{
//	AUDIO_DMIC_DATA_IN_RISING_EDGE 		= 0,
//	AUDIO_DMIC_DATA_IN_FALLING_EDGE 	= 1,
//}

enum{
	AUDIO_INPUT_USB  = 0x00,
	AUDIO_INPUT_I2S  = 0x01,
	AUDIO_INPUT_AMIC = 0x02,
	AUDIO_INPUT_DMIC = 0x03,
};



#define reg_audio_dec_ratio		REG_ADDR8(0xb12)
#define REG_AUDIO_DEC_RATIO  	reg_audio_dec_ratio





#define reg_audio_wr_ptr		REG_ADDR16(0xb16)
#define reg_mic_ptr				reg_audio_wr_ptr

static inline u16 get_mic_wr_ptr (void) {
	return reg_audio_wr_ptr >>1;
}



#define	reg_audio_dec_mode		REG_ADDR8(0xb35)
#define	REG_AUDIO_DEC_MODE		reg_audio_dec_mode
enum{
	FLD_AUD_LNR_VALID_SEL	= BIT(0),
	FLD_AUD_CIC_MODE  	= BIT(3)
};


#define	reg_audio_alc_hpf_lpf_ctrl	 REG_ADDR8(0xb40)
#define reg_aud_hpf_alc				 reg_audio_alc_hpf_lpf_ctrl
enum {
	FLD_AUD_IN_HPF_SFT		=	BIT_RNG(0,3),
	FLD_AUD_IN_HPF_BYPASS	=	BIT(4),
	FLD_AUD_IN_ALC_BYPASS	=	BIT(5),
	FLD_AUD_IN_LPF_BYPASS   =   BIT(6),
	FLD_DOUBLE_DOWN_SAMPLING_ON =   BIT(7)
};



#define reg_aud_alc_vol_l_chn		REG_ADDR8(0xb41)  //default 0x00, will be 0x20 after ana_34 set to 0x80
#define reg_aud_alc_vol_r_chn		REG_ADDR8(0xb42)  //default 0x00, will be 0x20 after ana_34 set to 0x80
enum{
	FLD_AUD_ALC_MIN_VOLUME_IN_DIGITAL_MODE 	= BIT_RNG(0,5),
	FLD_AUD_ALC_MIN_PGA_IN_ANALOG_MODE	    = BIT_RNG(0,6),
	//alc digital mode left/right channel regulate mode select: 1 for auto regulate; 0 for manual regulate
	FLD_AUD_ALC_DIGITAL_MODE_AUTO_REGULATE_EN	= BIT(7),
};


#define reg_aud_alc_vol_h			REG_ADDR8(0xb43)
enum{
	FLD_AUD_ALC_MAX_VOLUME_IN_DIGITAL_MODE 	= BIT_RNG(0,5),
	FLD_AUD_ALC_MAX_PGA_IN_ANALOG_MODE	    = BIT_RNG(0,6),
};





#define reg_aud_alc_cfg			REG_ADDR8(0xb54)   //default 0x00, will be 0x02 after ana_34 set to 0x80
enum{
	FLD_AUD_ALC_ANALOG_MODE_EN 		= BIT(0),    //alc mode select:   1 for analog mode;  0 for digital mode
	FLD_AUD_ALC_NOISE_EN 			= BIT(1),


};





#define reg_pga_gain_init		REG_ADDR8(0xb5d)
#define reg_pga_gain_l			REG_ADDR8(0xb5e)   //used to check current left  channel gain in analog mode auto regulate
#define reg_pga_gain_r			REG_ADDR8(0xb5f)   //used to check current right channel gain in analog mode auto regulate
#define reg_pga_man_speed		REG_ADDR8(0xb60)








#if defined(__cplusplus)
}
#endif

