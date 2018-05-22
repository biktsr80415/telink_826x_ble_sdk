#ifndef _RF_DRV_H_
#define _RF_DRV_H_

#include "bsp.h"


#define RF_CHN_TABLE 		0x8000



/**
 *  @brief  Define RF mode
 */
typedef enum {
	 RF_MODE_BLE_2M =    		BIT(0),
	 RF_MODE_BLE_1M = 			BIT(1),
	 RF_MODE_ZIGBEE_250K = 		BIT(2),
	 RF_MODE_PRIVATE_2M = 		BIT(3),
	 RF_MODE_STANDARD_1M =  	BIT(4),
	 RF_MODE_NORDIC_1M= 		BIT(5),
	 RF_MODE_NORDIC_2M = 		BIT(6),
	 RF_MODE_LR_S2_500K=		BIT(7),
	 RF_MODE_LR_S8_125K=		BIT(8),
} RF_ModeTypeDef;


//extern  RF_ModeTypeDef g_RFMode;

//#define IS_RF_1M_MODE(rf_mode)		( (rf_mode) & (RF_MODE_BLE_1M | RF_MODE_NORDIC_1M))

typedef enum {
	RF_POWER_10m4PdBm	= 63,
	RF_POWER_8PdBm		= 50,
	RF_POWER_6m3PdBm	= 45,
	RF_POWER_5m4PdBm	= 40,
} RF_PowerTypeDef;

#ifdef		RF_MODE_250K
#define		RF_FAST_MODE_2M		0
#define		RF_FAST_MODE_1M		0
#endif

#ifndef		RF_FAST_MODE_1M
#define		RF_FAST_MODE_1M		1
#endif

#ifndef		RF_FAST_MODE_2M
#define		RF_FAST_MODE_2M		(!RF_FAST_MODE_1M)
#endif

#ifndef		RF_LONG_PACKET_EN
#define		RF_LONG_PACKET_EN		0
#endif

#if			RF_FAST_MODE_2M
	#if			RF_LONG_PACKET_EN
		#define		RF_PACKET_LENGTH_OK(p)		(p[0] == p[12]+13)
		#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)
	#else
		#define		RF_PACKET_LENGTH_OK(p)		(p[0] == (p[12]&0x3f)+15)
		#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)
	#endif
#elif		RF_FAST_MODE_1M
#define		RF_PACKET_LENGTH_OK(p)		(p[0] == p[5]+13)
#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x01) == 0x0)
#else
#define		RF_PACKET_LENGTH_OK(p)		(p[0] == p[12]+13)
#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x10)
#endif

#define		RF_PACKET_1M_LENGTH_OK(p)		(p[0] == p[5]+13)
#define		RF_PACKET_2M_LENGTH_OK(p)		(p[0] == (p[12]&0x3f)+15)


#if (RF_FAST_MODE_2M)
	#define			RF_FAST_MODE			1
	#define			RF_TRX_MODE				0x80
	#define			RF_TRX_OFF				0x44		//f02
#elif (RF_FAST_MODE_1M)
	#define			RF_FAST_MODE			1
	#define			RF_TRX_MODE				0x80
	#define			RF_TRX_OFF				0x45		//f02
#else
	#define			RF_FAST_MODE			0
	#define			RF_TRX_MODE				0xe0
	#define			RF_TRX_OFF				0x45		//f02
#endif

unsigned char is_rf_packet_crc_ok(unsigned char *p);
unsigned char is_rf_packet_length_ok(unsigned char *p);

static inline void rf_ble_tx_on ()
{
	write_reg8  (0x800f02, RF_TRX_OFF | BIT(4));	// TX enable
	write_reg32 (0x800f04, 0x38);
}

static inline void rf_ble_tx_done ()
{
	write_reg8  (0x800f02, RF_TRX_OFF);	// TX enable
	write_reg32 (0x800f04, 0x50);
}

void rf_update_tp_value (u8 tp0, u8 tp1);


/////////////////////  RF BaseBand ///////////////////////////////
static inline void reset_baseband(void)
{
	REG_ADDR8(0x61) = BIT(0);		//reset baseband
	REG_ADDR8(0x61) = 0;			//release reset signal
}

static inline void reset_sn_nesn(void)
{
	REG_ADDR8(0xf01) =  0x01;
}

static inline void 	tx_settle_adjust(u16 txstl_us)
{
	REG_ADDR16(0xf04) = txstl_us;  //adjust TX settle time
}

static inline void rf_set_tx_pipe (u8 pipe)
{
	write_reg8 (0x800f15, 0xf0 | pipe);
}

static inline void rf_set_ble_crc (u8 *p)
{
	write_reg32 (0x800424, p[0] | (p[1]<<8) | (p[2]<<16));
}

static inline void rf_set_ble_crc_value (u32 crc)
{
	write_reg32 (0x800424, crc);
}

static inline void rf_set_ble_crc_adv ()
{
	write_reg32 (0x800424, 0x555555);
}

static inline void rf_set_ble_access_code (u8 *p)
{
	write_reg32 (0x800408, p[3] | (p[2]<<8) | (p[1]<<16) | (p[0]<<24));
}

static inline void rf_set_ble_access_code_value (u32 ac)
{
	write_reg32 (0x800408, ac);
}

static inline void rf_set_ble_access_code_adv (void)
{
	write_reg32 (0x800408, 0xd6be898e);
}

static inline void rf_set_access_code0 (u32 code)
{
	write_reg32 (0x800408, (read_reg32(0x800408) & 0xff) | (code & 0xffffff00));
	write_reg8  (0x80040c, code);
}

static inline u32 rf_get_access_code0 (void)
{
	return read_reg8 (0x80040c) | (read_reg32(0x800408) & 0xffffff00);
}

static inline void rf_set_access_code1 (u32 code)
{
	write_reg32 (0x800410, (read_reg32(0x800410) & 0xff) | (code & 0xffffff00));
	write_reg8  (0x800414, code);
}

static inline u32 rf_get_access_code1 (void)
{
	return read_reg8 (0x800414) | (read_reg32(0x800410) & 0xffffff00);
}

static inline u32 rf_access_code_16to32 (u16 code)
{
	u32 r = 0;
	for (int i=0; i<16; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}
	return r;
}

static inline u16 rf_access_code_32to16 (u32 code)
{
	u16 r = 0;
	for (int i=0; i<16; i++) {
		r = r << 1;

		r |= (code & BIT(i*2)) ? 1 : 0;

	}
	return r;
}

static inline void rf_reset_sn (void)
{
	write_reg8  (0x800f01, 0x3f);
	write_reg8  (0x800f01, 0x00);
}

void rf_set_power_level_index (RF_PowerTypeDef level);     //@@@

void 	rf_drv_init (RF_ModeTypeDef rf_mode);   				//@@@

void	rf_set_channel (signed char chn, unsigned short set);

void 	rf_set_ble_channel (signed char chn);						 //@@@

//manual mode
static inline void rf_set_rxmode (void)
{
    write_reg8 (0x800428, RF_TRX_MODE | BIT(0));	// rx disable
    write_reg8 (0x800f02, RF_TRX_OFF | BIT(5));		// RX enable
}

static inline void rf_set_txmode (void)
{
	write_reg8  (0x800f02, RF_TRX_OFF | BIT(4));	// TX enable
}

void	rf_send_packet (void* addr);

static inline void rf_set_tx_rx_off(void)
{
	write_reg8 (0x800f16, 0x29);
	write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
	write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine
}

static inline void rf_set_tx_preamble_length(unsigned char len)   //len < 16
{
	write_reg8 (0x402, 0x40 | len);
}

//auto
static inline void rf_stop_trx (void)
{
	write_reg8  (0x800f00, 0x80);			// stop
}

void rf_start_btx (void* addr, u32 tick);
void rf_start_brx  (void* addr, u32 tick);

void rf_start_stx  (void* addr, u32 tick);
void rf_start_srx  (void* addr, u32 tick);

void rf_start_stx2rx  (void* addr, u32 tick);
void rf_start_srx2tx  (void* addr, u32 tick);


void rf_param_init_after_suspend_ble_1m(void);

#endif
