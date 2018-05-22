#ifndef _RF_DRV_H_
#define _RF_DRV_H_

#include "bsp.h"


#define RF_CHN_AUTO_CAP 	0xff00
#define RF_CHN_TABLE 		0x8000
#define RF_SET_TX_MANAUL	0x4000

#define FRE_OFFSET 	0
#define FRE_STEP 	5
#define MAX_RF_CHANNEL  16



enum {
	RF_POWER_11PdBm		= 0,  // 11
	RF_POWER_10m4PdBm	= 1,  // 10.4
	RF_POWER_9m8PdBm	= 2,  // 9.8
	RF_POWER_9m34PdBm	= 3,  // 9.34
	RF_POWER_8m5PdBm	= 4,  // 8.5
	RF_POWER_7m9PdBm	= 5,  // 7.9
	RF_POWER_7m3PdBm	= 6,  // 7.3
	RF_POWER_6m3PdBm	= 7,  // 6.3
	RF_POWER_5m4PdBm	= 8,  // 5.4
	RF_POWER_4m5PdBm	= 9,  // 4.5
	RF_POWER_3m5PdBm	= 10,  // 3.5
	RF_POWER_1m9PdBm	= 11,  // 1.9
	RF_POWER_1PdBm		= 12, // 1
	RF_POWER_0m28NdBm	= 13, // -0.28
	RF_POWER_1m9NdBm	= 14, // -1.9
	RF_POWER_3m5NdBm	= 15, // -3.5
	RF_POWER_5m5NdBm	= 16, // -5.5
	RF_POWER_6m8NdBm	= 17, // -6.8
	RF_POWER_8m23NdBm	= 18, // -8.23
	RF_POWER_9m1NdBm	= 19, // -9.1
	RF_POWER_11m8NdBm	= 20, // -11.8
	RF_POWER_13m1NdBm	= 21, // -13.1
	RF_POWER_14m5NdBm	= 22, // -14.5
	RF_POWER_15m3NdBm	= 23, // -15.3
	RF_POWER_17m75NdBm	= 24, // -17.75
	RF_POWER_19m8NdBm	= 25, // -19
	RF_POWER_20m3NdBm	= 26, // -20.3
};




#define POWER_DOWN_64MHZ_CLK
#define POWER_ON_64MHZ_CLK

#define PHY_POWER_DOWN
#define PHY_POWER_UP



void SetRxMode (signed char chn, unsigned short set);
void SetTxMode (signed char chn, unsigned short set);
void TxPkt (void* addr);

void rf_set_ble_channel (signed char chn);
void rf_start_stx2rx  (void* addr, u32 tick);



static inline void rf_set_tx_pipe_long_packet (u8 pipe)
{
	write_reg8 (0x800f15, 0x70 | pipe);
}

static inline void rf_set_tx_pipe (u8 pipe)
{
	write_reg8 (0x800f15, 0xf0 | pipe);
}

static inline void rf_set_ble_crc (u8 *p)
{
	write_reg32 (0x80042c, p[0] | (p[1]<<8) | (p[2]<<16));
}

static inline void rf_set_ble_crc_value (u32 crc)
{
	write_reg32 (0x80042c, crc);
}

static inline void rf_set_ble_crc_adv ()
{
	write_reg32 (0x80042c, 0x555555);
}

static inline void rf_set_ble_access_code (u8 *p)
{
	write_reg32 (0x800408, p[3] | (p[2]<<8) | (p[1]<<16) | (p[0]<<24));
}

static inline void rf_set_ble_access_code_value (u32 ac)
{
	write_reg32 (0x800408, ac);
}

static inline void rf_set_ble_access_code_adv ()
{
#if (TEST_SPECAIL_ADV_ACCESS_CODE)
	write_reg32 (0x800408, 0x12345678);
#else
	write_reg32 (0x800408, 0xd6be898e);
#endif
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

static inline void rf_stop_trx (void)
{
	write_reg8  (0x800f00, 0x80);			// stop
}

static inline void rf_reset_sn (void)
{
	write_reg8  (0x800f01, 0x3f);
	write_reg8  (0x800f01, 0x00);
}

extern u8 emi_var[];

void 	rf_set_power_level_index (int level);
s8 		rf_get_tx_power_level(void);




static inline void rf_start_brx  (void* addr, u32 tick)
{
//	write_reg32 (0x800f04, 0x56);						// tx_wait = 0; tx_settle = 86 us
	write_reg32 (0x800f28, 0x0fffffff);					// first timeout
//	write_reg32(0x800f18, tick);						// Setting schedule trigger time
	write_reg32(0x800f18, (tick - pm_systick_offset));
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg8 (0x800f00, 0x82);						// ble rx
	write_reg16 (0x800c0c, (u16)((u32)addr));
}

void 	rf_drv_init (void);
void	rf_power_down ();
void	rf_set_channel (signed char chn, unsigned short set);
void	rf_set_rxmode (void);
void	rf_set_txmode (void);
void	rf_send_packet (void* addr, u16 rx_waittime, u8 retry);
void 	rf_multi_receiving_init (u8 channel_mask);
void	rf_multi_receiving_start  (signed char chn, unsigned short set);
void	rf_set_ack_packet  (void* addr);
void 	rf_receiving_pipe_enble(u8 channel_mask);




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

#define             RF_TRX_OFF_MANUAL       0x55        //f02


#define				STOP_RF_STATE_MACHINE	( REG_ADDR8(0xf00) = 0x80 )


void rf_doubler_calibration(void);


static inline void  ()
{rf_ble_tx_on
	write_reg8  (0x800f02, RF_TRX_OFF | BIT(4));	// TX enable
	write_reg32 (0x800f04, 0x38);
}

static inline void rf_ble_tx_done ()
{
	write_reg8  (0x800f02, RF_TRX_OFF);	// TX enable
	write_reg32 (0x800f04, 0x50);
}

static inline void rf_ble_trx_off ()
{
	/////////////////// turn on LDO and baseband PLL ////////////////
	//analog_write (0x06, 0xfe);
	write_reg8 (0x800f16, 0x29);
	write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
	write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine
}

static inline void emi_cd_prepare( void ){
    write_reg8 (0x800f02, RF_TRX_OFF);  //must trx disable before cd switch
}

static inline void rf_ble_stx  (void* addr, u32 tick)
{
	//write_reg32 (0x800f04, 0);						// tx wail & settle time: 0
//	write_reg32(0x800f18, tick);						// Setting schedule trigger time
	write_reg32(0x800f18, (tick - pm_systick_offset));
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg8 (0x800f00, 0x87);						// single TX2RX
	write_reg16 (0x800c0c, (u16)((u32)addr));
}

void rf_start_stx  (void* addr, u32 tick);

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


#endif
