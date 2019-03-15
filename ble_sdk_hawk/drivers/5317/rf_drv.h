#ifndef _RF_DRV_H
#define _RF_DRV_H

#include "bsp.h"
#include "register.h"

#define RF_CHN_AUTO_CAP 	0xff00
#define RF_CHN_TABLE 		0x8000
#define RF_SET_TX_MANAUL	0x4000

#define FRE_OFFSET   	    0
#define FRE_STEP 	        5
#define MAX_RF_CHANNEL      16

#define RF_CHANNEL_MAX		16
#define RF_CHANNEL_MASK		(RF_CHANNEL_MAX - 1)

extern unsigned char rf_tx_mode;
extern unsigned char rfhw_tx_power;
extern unsigned char cap_tp[RF_CHANNEL_MAX];
extern const unsigned char rf_chn[RF_CHANNEL_MAX];

typedef enum {
    RF_MODE_TX = 0,
    RF_MODE_RX = 1,
    RF_MODE_AUTO=2
}RF_StatusTypeDef;

typedef enum{
	RF_MODE_BLE_2M       = BIT(0),
	RF_MODE_BLE_1M       = BIT(1),
    RF_MODE_BLE_1M_NO_PN = BIT(2),

	RF_MODE_ZIGBEE_250K  = BIT(3),

	RF_MODE_NORDIC_1M    = BIT(4),
	RF_MODE_NORDIC_2M    = BIT(5),
	RF_MODE_BLE_2M_NO_PN = BIT(6),
}RF_ModeTypeDef;

typedef enum {
	RF_POWER_11P8dBm	= 0,
	RF_POWER_9P6dBm		= 1,
	RF_POWER_7P9dBm		= 2,
	RF_POWER_7dBm		= 3,
	RF_POWER_6P3dBm		= 4,
	RF_POWER_4P9dBm		= 5,
	RF_POWER_3P3dBm		= 6,
	RF_POWER_1P6dBm		= 7,
	RF_POWER_0dBm		= 8,
	RF_POWER_m1P5dBm	= 9,
	RF_POWER_m3P1dBm	= 10,
	RF_POWER_m5dBm		= 11,
	RF_POWER_m7P3dBm	= 12,
	RF_POWER_m9P6dBm	= 13,
	RF_POWER_m11P5dBm	= 14,
	RF_POWER_m13P3dBm	= 15,
	RF_POWER_m16dBm		= 16,
	RF_POWER_m17P8dBm	= 17,
	RF_POWER_m19P5dBm	= 18,
	RF_POWER_OFF		= 19,
}RF_TxPowerTypeDef;


#define FR_TX_PA_MAX_POWER	0x40
#define FR_TX_PA_MIN_POWER	0x41

//#define	RF_TX_PA_POWER_LOW		    WriteAnalogReg (0x9e, 0x02)
//#define	RF_TX_PA_POWER_HIGH		    WriteAnalogReg (0x9e, 0xf2)
//#define	RF_TX_PA_POWER_LEVEL(high)  WriteAnalogReg (0x9e, high ? 0xbf : 0x02)
//#define	RF_TX_PA_POWER_LOW			rfhw_tx_power = FR_TX_PA_MIN_POWER
//#define	RF_TX_PA_POWER_HIGH			rfhw_tx_power = FR_TX_PA_MAX_POWER
//#define	RF_TX_PA_POWER_LEVEL(high)	rfhw_tx_power = high ? FR_TX_PA_MAX_POWER : FR_TX_PA_MIN_POWER;

#define	SET_RF_TX_DMA_ADR(a)			write_reg16 (0x80050c, a)


#if 1
static inline void rf_ldo_power_down(void)
{
	analog_write(0x06, 0xff);
}

static inline void rf_ldo_power_on(void)
{
	analog_write(0x06, 0x00);
}

static inline void rf_clk_disable(void)
{
//	reg_clk_en0 &= ~FLD_CLK0_ZB_EN; //ERR
//	analog_write(0x80, 0x69);  		//ERR
//	analog_write(0x82, 0x53);  		//ERR
}

static inline void rf_clk_enable(void)
{
//	reg_clk_en0 |= FLD_CLK0_ZB_EN;
	analog_write(0x80, 0x61);
	analog_write(0x82, 0x5f);
}
#endif

#define	RF_PACKET_LENGTH_OK(p)		(p[0] == (p[13])+17)
#define	RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)

#define	RF_BLE_PACKET_LENGTH_OK(p)	(p[0] == (p[13])+17)
#define	RF_BLE_PACKET_CRC_OK(p)		((p[p[0]+3] & 0x51) == 0x40)

#define RF_TRX_OFF_MANUAL           (0x55)//f02

#define	STOP_RF_STATE_MACHINE	    (REG_ADDR8(0xf00) = 0x80)

#define	rf_get_pipe(p)		        p[7]



void rf_drv_init (RF_ModeTypeDef RF_Mode);

void rf_update_tp_value(unsigned char tp0, unsigned char tp1);

void rf_set_channel(signed char chn, unsigned short set);
void rf_set_ble_channel(signed char chn);

void rf_set_power_level_index(RF_TxPowerTypeDef level);
char rf_get_tx_power_level(void);

void rf_start_stx(void* addr, unsigned int tick);
void rf_start_srx(unsigned int start_tick);

void rf_start_stx2rx (void* addr, unsigned int tick);
void rf_start_srx2tx(void* addr, unsigned int tick);

void rf_start_btx(void* addr, unsigned int tick);

//void rf_receiving_pipe_enble(unsigned char channel_mask);

int rf_trx_state_set(RF_StatusTypeDef rf_status, signed char rf_channel);
void rf_tx_pkt(unsigned char *RF_TxBufAddr);
RF_StatusTypeDef rf_trx_state_get(void);

void rf_rx_buffer_set(unsigned char *  RF_RxAddr, int size, unsigned char  PingpongEn);
void rf_set_tp_gain (char chn);

void RF_SetGainManualMax (void);
void RF_SetAgc (void);

static inline void rf_start_brx(void* addr, unsigned int tick)
{
	//write_reg32 (0x800f04, 0x56);						// tx_wait = 0; tx_settle = 86 us
	write_reg32(0x800f28, 0x0fffffff);					// first timeout
	write_reg32(0x800f18, tick);						// Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg8(0x800f00, 0x82);						// ble rx
	write_reg16(0x80050c,(unsigned short)((unsigned int)addr));
}

//manual rx mode
static inline void rf_set_rxmode (void)
{
    write_reg8 (0x428, read_reg8(0x428) | BIT(0));
    write_reg8 (0xf02, 0x45 | BIT(5));		// RX enable
}

//manual tx mode
static inline void rf_set_txmode (void)
{
	write_reg8(0xf02, 0x45 | BIT(4));	// TX enable
}

//maunal mode off
static inline void rf_set_tx_rx_off(void)
{
	write_reg8(0x800f16, 0x29);
	write_reg8(0x800428, read_reg8(0x428)&0xfe);	// rx disable
	write_reg8(0x800f02, 0x45);	// reset tx/rx state machine
}

//auto mode off
static inline void rf_set_tx_rx_off_auto_mode(void)
{
	write_reg8 (0xf00, 0x80);
}

static inline void rf_stop_trx (void)
{
	write_reg8  (0x800f00, 0x80);			// stop
}

static inline void rf_reset_sn (void)
{
	write_reg8  (0x800f01, 0x3f);
	write_reg8  (0x800f01, 0x00);
}

static inline void reset_sn_nesn(void)
{
	REG_ADDR8(0xf01) =  0x01;
}

static inline void rf_ble_tx_on()
{
	write_reg8  (0x800f02, 0x45 | BIT(4));	// TX enable
	write_reg32 (0x800f04, 0x38);
}

static inline void rf_ble_tx_done()
{
	write_reg8  (0x800f02, 0x45);	// TX enable
	write_reg32 (0x800f04, 0x50);
}

static inline void reset_rf_baseband(void)
{
	REG_ADDR8(0x60) = BIT(7);		//reset baseband
	REG_ADDR8(0x60) = 0;			//release reset signal
}

static inline void 	tx_settle_adjust(unsigned short txstl_us)
{
	REG_ADDR16(0xf04) = txstl_us;  //adjust TX settle time
}

static inline unsigned char rf_tx_finish(void)
{
    return (READ_REG8(0xf20) & BIT(1));
}

static inline void rf_tx_finish_clear_flag(void)
{
    WRITE_REG8(0xf20, READ_REG8(0xf20) | 0x02);
}

static inline void rf_set_tx_pipe_long_packet (unsigned char pipe)
{
	write_reg8(0x800f15, 0x70 | pipe);
}

static inline void rf_set_tx_pipe (unsigned char pipe)
{
	write_reg8(0x800f15, 0xf0 | pipe);
}

static inline void rf_set_ble_crc (unsigned char *p)
{
	write_reg32(0x80044c, p[0] | (p[1]<<8) | (p[2]<<16));
}

static inline void rf_set_ble_crc_value (unsigned int crc)
{
	write_reg32(0x80044c, crc);
}

static inline void rf_set_ble_crc_adv()
{
	write_reg32(0x80044c, 0x555555);
}

static inline void rf_set_ble_access_code(unsigned char *p)
{
	write_reg32(0x800408, p[3] | (p[2]<<8) | (p[1]<<16) | (p[0]<<24));
}

static inline void rf_set_ble_access_code_value (unsigned int ac)
{
	write_reg32(0x800408, ac);
}

static inline void rf_set_ble_access_code_adv(void)
{
	write_reg32(0x800408, 0xd6be898e);
}

static inline void rf_set_access_code0 (unsigned int code)
{
	write_reg32(0x800408, (read_reg32(0x800408) & 0xff) | (code & 0xffffff00));
	write_reg8(0x80040c, code);
}

static inline unsigned int rf_get_access_code0 (void)
{
	return read_reg8(0x80040c) | (read_reg32(0x800408) & 0xffffff00);
}

static inline void rf_set_access_code1 (unsigned int code)
{
	write_reg32(0x800410, (read_reg32(0x800410) & 0xff) | (code & 0xffffff00));
	write_reg8(0x800414, code);
}

static inline unsigned int rf_get_access_code1 (void)
{
	return read_reg8(0x800414) | (read_reg32(0x800410) & 0xffffff00);
}

static inline unsigned int rf_access_code_16to32 (unsigned short code)
{
	unsigned int r = 0;
	for (int i=0; i<16; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}
	return r;
}

static inline unsigned short rf_access_code_32to16 (unsigned int code)
{
	unsigned short r = 0;
	for (int i=0; i<16; i++) {
		r = r << 1;

		r |= (code & BIT(i*2)) ? 1 : 0;

	}
	return r;
}

#endif
