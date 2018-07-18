#ifndef _RF_DRV_H
#define _RF_DRV_H

#include "driver_config.h"
#include "bsp.h"
#include "analog.h"

#define RF_CHN_AUTO_CAP 	0xff00
#define RF_CHN_TABLE 		0x8000
#define RF_SET_TX_MANAUL	0x4000

#define FRE_OFFSET   	    0
#define FRE_STEP 	        5
#define MAX_RF_CHANNEL      16

#define RF_CHANNEL_MAX		16
#define RF_CHANNEL_MASK		(RF_CHANNEL_MAX - 1)

extern unsigned char rfhw_tx_power;
extern unsigned char cap_tp[RF_CHANNEL_MAX];
extern const unsigned char rf_chn[RF_CHANNEL_MAX];

//Use for EMI
enum{
	RF_TX_MODE_NORMAL = 0,
	RF_TX_MODE_CARRIER,
	RF_TX_MODE_CONTINUE,

	RF_POWER_LEVEL_MAX = 0,
	RF_POWER_LEVEL_M2  = 1,
	RF_POWER_LEVEL_M3  = 2,
	RF_POWER_LEVEL_MIN = 100,
};


typedef enum{
	RF_MODE_BLE_2M       = BIT(0),
	RF_MODE_BLE_1M       = BIT(1),
    RF_MODE_BLE_1M_NO_PN = BIT(2),

	RF_MODE_ZIGBEE_250K  = BIT(3),

	RF_MODE_NORDIC_1M    = BIT(4),
	RF_MODE_NORDIC_2M    = BIT(5),
}RF_ModeTypeDef;

typedef enum {
	RF_POWER_11P8dBm	= 0,
	RF_POWER_9P6dBm		= 1,
	RF_POWER_7P9dBm		= 2,
	RF_POWER_6P3dBm		= 3,
	RF_POWER_4P9dBm		= 4,
	RF_POWER_3P3dBm		= 5,
	RF_POWER_1P6dBm		= 6,
	RF_POWER_0dBm		= 7,
	RF_POWER_m1P5dBm	= 8,
	RF_POWER_m3P1dBm	= 9,
	RF_POWER_m5dBm		= 10,
	RF_POWER_m7P3dBm	= 11,
	RF_POWER_m9P6dBm	= 12,
	RF_POWER_m11P5dBm	= 13,
	RF_POWER_m13P3dBm	= 14,
	RF_POWER_m16dBm		= 15,
	RF_POWER_m17P8dBm	= 16,
	RF_POWER_m19P5dBm	= 17,
	RF_POWER_OFF		= 18,
}RF_TxPowerTypeDef;


typedef enum {
    RF_MODE_TX = 0,
    RF_MODE_RX = 1,
    RF_MODE_AUTO=2
}RF_StatusTypeDef;

#define FR_TX_PA_MAX_POWER	0x40
#define FR_TX_PA_MIN_POWER	0x41

//#define	RF_TX_PA_POWER_LOW		WriteAnalogReg (0x9e, 0x02)
//#define	RF_TX_PA_POWER_HIGH		WriteAnalogReg (0x9e, 0xf2)
#define	RF_TX_PA_POWER_LEVEL(high)		WriteAnalogReg (0x9e, high ? 0xbf : 0x02)
#define	RF_TX_PA_POWER_LOW				rfhw_tx_power = FR_TX_PA_MIN_POWER
#define	RF_TX_PA_POWER_HIGH				rfhw_tx_power = FR_TX_PA_MAX_POWER
//#define	RF_TX_PA_POWER_LEVEL(high)		rfhw_tx_power = high ? FR_TX_PA_MAX_POWER : FR_TX_PA_MIN_POWER;

#define	SET_RF_TX_DMA_ADR(a)			write_reg16 (0x80050c, a)

#define RF_TX_MODE_CARRIER_ENABLE		do {rf_tx_mode = RF_TX_MODE_CARRIER;} while(0)
#define RF_TX_MODE_CONTINUE_ENABLE		do {rf_tx_mode = RF_TX_MODE_CONTINUE;} while(0)
#define RF_TX_MODE_NORMAL_ENABLE		do {rf_tx_mode = RF_TX_MODE_NORMAL;} while(0)


#define POWER_DOWN_64MHZ_CLK    //analog_write(0x82, IS_XTAL_12M(xtalType_rfMode) ? 0x00 : 0x14)
#define POWER_ON_64MHZ_CLK		//analog_write(0x82, IS_XTAL_12M(xtalType_rfMode) ? 0x20 : 0x34)

#define PHY_POWER_DOWN   	//analog_write(0x06, sar_adc_pwdn_en ? 0xff : 0xfe)
#define PHY_POWER_UP	 	//analog_write(0x06, sar_adc_pwdn_en);//sar_adc_pwdn_en ? 0x01 : 0x00)

#define	rf_get_pipe(p)		p[7]

extern unsigned char rf_tx_mode;

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

void rf_receiving_pipe_enble(unsigned char channel_mask);

int rf_trx_state_set(RF_StatusTypeDef rf_status, signed char rf_channel);
RF_StatusTypeDef rf_trx_state_get(void);

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
    write_reg8 (0x800428, 0x80 | BIT(0));	// rx disable
    write_reg8 (0x800f02, 0x45 | BIT(5));		// RX enable
}

//manual tx mode
static inline void rf_set_txmode (void)
{
	write_reg8(0x800f02, 0x45 | BIT(4));	// TX enable
}

//maunal mode off
static inline void rf_set_tx_rx_off(void)
{
	write_reg8(0x800f16, 0x29);
	write_reg8(0x800428, 0x80);	// rx disable
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

//#define	RF_FAST_MODE_1M		1

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

#if	RF_FAST_MODE_2M
	#if	RF_LONG_PACKET_EN
		#define	RF_PACKET_LENGTH_OK(p)	(p[0] == p[12]+13)
		#define	RF_PACKET_CRC_OK(p)		((p[p[0]+3] & 0x51) == 0x40)
	#else
		#define	RF_PACKET_LENGTH_OK(p)	(p[0] == (p[12]&0x3f)+15)
		#define	RF_PACKET_CRC_OK(p)		((p[p[0]+3] & 0x51) == 0x40)
	#endif
#elif RF_FAST_MODE_1M
	#define	RF_PACKET_LENGTH_OK(p)		(p[0] == (p[13]&0x3f)+17)
	#define	RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)
#else
	#define	RF_PACKET_LENGTH_OK(p)		(p[0] == p[12]+13)
	#define	RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x10)
#endif

#define	RF_PACKET_1M_LENGTH_OK(p)		(p[0] == (p[13]&0x3f)+17)
#define	RF_PACKET_2M_LENGTH_OK(p)		(p[0] == (p[12]&0x3f)+15)

#if (RF_FAST_MODE_2M)
	#define	RF_FAST_MODE	1
	#define	RF_TRX_MODE		0x80
	#define	RF_TRX_OFF		0x44		//f02
#elif (RF_FAST_MODE_1M)
	#define	RF_FAST_MODE	1
	#define	RF_TRX_MODE		0x80
	#define	RF_TRX_OFF		0x45		//f02
#else
	#define	RF_FAST_MODE	0
	#define	RF_TRX_MODE		0xe0
	#define	RF_TRX_OFF		0x45		//f02
#endif

#define RF_TRX_OFF_MANUAL   0x55        //f02

#define	STOP_RF_STATE_MACHINE	( REG_ADDR8(0xf00) = 0x80 )

static inline void rf_ble_tx_on()
{
	write_reg8  (0x800f02, RF_TRX_OFF | BIT(4));	// TX enable
	write_reg32 (0x800f04, 0x38);
}

static inline void rf_ble_tx_done()
{
	write_reg8  (0x800f02, RF_TRX_OFF);	// TX enable
	write_reg32 (0x800f04, 0x50);
}

static inline void rf_ble_trx_off()
{
	/////////////////// turn on LDO and baseband PLL ////////////////
	//analog_write (0x06, 0xfe);
	write_reg8 (0x800f16, 0x29);
	write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
	write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine
}

static inline void rf_set_tx_pipe_long_packet (unsigned char pipe)
{
	write_reg8 (0x800f15, 0x70 | pipe);
}

static inline void rf_set_tx_pipe (unsigned char pipe)
{
	write_reg8 (0x800f15, 0xf0 | pipe);
}

static inline void rf_set_ble_crc (unsigned char *p)
{
	write_reg32 (0x80044c, p[0] | (p[1]<<8) | (p[2]<<16));
}

static inline void rf_set_ble_crc_value (unsigned int crc)
{
	write_reg32 (0x80044c, crc);
}

static inline void rf_set_ble_crc_adv()
{
	write_reg32 (0x80044c, 0x555555);
}

static inline void rf_set_ble_access_code(unsigned char *p)
{
	write_reg32 (0x800408, p[3] | (p[2]<<8) | (p[1]<<16) | (p[0]<<24));
}

static inline void rf_set_ble_access_code_value (unsigned int ac)
{
	write_reg32 (0x800408, ac);
}

static inline void rf_set_ble_access_code_adv(void)
{
#if (TEST_SPECAIL_ADV_ACCESS_CODE)
	write_reg32 (0x800408, 0x12345678);
#else
	write_reg32 (0x800408, 0xd6be898e);
#endif
}

static inline void rf_set_access_code0 (unsigned int code)
{
	write_reg32 (0x800408, (read_reg32(0x800408) & 0xff) | (code & 0xffffff00));
	write_reg8  (0x80040c, code);
}

static inline unsigned int rf_get_access_code0 (void)
{
	return read_reg8 (0x80040c) | (read_reg32(0x800408) & 0xffffff00);
}

static inline void rf_set_access_code1 (unsigned int code)
{
	write_reg32 (0x800410, (read_reg32(0x800410) & 0xff) | (code & 0xffffff00));
	write_reg8  (0x800414, code);
}

static inline unsigned int rf_get_access_code1 (void)
{
	return read_reg8 (0x800414) | (read_reg32(0x800410) & 0xffffff00);
}


static inline unsigned int light_proc_conflict_ac_32(unsigned int ac)
{
	unsigned int r = ac;
	unsigned char zero_cnt = 0;
	unsigned char one_cnt = 0;

	//foreach(i, 16)
	for(int i =0 ;i < 16; i++)
	{
		if((unsigned char)(r & 0x3) == 0x01){
		    ++one_cnt;
		}else if((unsigned char)(r & 0x3) == 0x02){
		    ++zero_cnt;
		}
		r = r >> 2;
	}

    r = ac;

	if(one_cnt > 5 || zero_cnt > 5){
		unsigned char comp_val = (one_cnt > 5)?0x01:0x02;
		unsigned char convert_val = (one_cnt > 5)?0x02:0x01;
		unsigned char off_set = 0;
		unsigned int mask_val[4] = {0x3FFFFFFF, 0xCFFFFFFF, 0xF3FFFFFF, 0xFCFFFFFF};

        //foreach(i, 4)
        for(int i = 0; i < 4; i++)
        {
            off_set = 32 - (i+1)*2;
            if(((r>>off_set) & 0x03) == comp_val){
                r = (convert_val<<off_set)|(r&mask_val[i]);
            }
        }
	}

	return r;
}

static inline unsigned short light_proc_conflict_ac_16(unsigned short ac)
{
	unsigned short r = ac;
	unsigned char zero_cnt = 0;
	unsigned char one_cnt = 0;

	//foreach(i, 16)
	for(int i = 0; i < 16; i++)
	{
		if(ac & BIT(i)){
		    ++one_cnt;
		}else{
		    ++zero_cnt;
		}
	}
	if(one_cnt < 3 || zero_cnt < 3){
	    r ^= 0xFF00;
	}
	return r;
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

/*----------------------------------------------------------------------------*/
/*------------ EMI                                               -------------*/
/*----------------------------------------------------------------------------*/
extern unsigned char emi_var[];

static inline void emi_init(  char tx_power_emi ){
    emi_var[0] = analog_read(0xa5);
    emi_var[1] = read_reg8(0x8004e8);
    //emi_var[2] = read_reg8(0x800524);
    //emi_var3 = read_reg8(0x800402);
    emi_var[4] = read_reg8(0x80050f);
    emi_var[5] = read_reg8(0x80050e);
    emi_var[6]  = read_reg8(0x800400);

    //8366 must enable, 8266 not verify
	//rf_power_enable (1);

    rf_set_power_level_index (tx_power_emi);
}

static inline void emi_carrier_init( void ){
    write_reg8 (0x800f02, 0x45);  // reset tx/rx enable reset
}

static inline void emi_cd_init( unsigned int cd_fifo){
	//reset zb & dma
	write_reg16(0x800060, 0x0480);
	write_reg16(0x800060, 0x0000);

	//TX mode
	//write_reg8 (0x800400,0x0b);//0b for 2Mbps, 03 for Zigbee, 0f for New2M and BLE Nrd 1Mbps
	//write_reg8 (0x800408,0x00);//0 for random , 1 for 0xf0f0, 2 fro 0x5555

	//txsetting
	//write_reg8(0x800402, 0x21);	//preamble length=1

	//txdma( &fifo_emi.start, FIFO_DEPTH );
    write_reg8(0x80050c,  cd_fifo & 0xff );
	write_reg8(0x80050d, (cd_fifo >> 8) & 0xff );
	write_reg8(0x80050e, (*(unsigned int*)cd_fifo -1) >> 4 );   //reg_50e = TX_buffer_size /16
	write_reg8(0x80050f, *(unsigned int*)cd_fifo -1 );

	//txpktsend
	write_reg8(0x800524, 0x08);
}

//as zhongqi's suggestion
static inline void emi_carrier_generate( void ){
    //write_reg8 (0x800400, 0x6f);//[6:5] 11: send 1, 10: send 0
    //tx_cyc1 manual
    analog_write (0xa5, 0x44);
    write_reg8 (0x8004e8, 0x04);
}

static inline void emi_carrier_recovery( void ){
    analog_write (0xa5, emi_var[0]);
    write_reg8 (0x8004e8, emi_var[1]);
}

static inline void emi_cd_recovery( void ){
    write_reg8(0x800524, 0);
    //write_reg8(0x800402, emi_var3);

    write_reg8(0x80050f, emi_var[4]);
    write_reg8(0x80050e, emi_var[5]);
    write_reg8(0x800400, emi_var[6]);
}

static inline void emi_cd_prepare( void ){
    write_reg8 (0x800f02, RF_TRX_OFF);  //must trx disable before cd switch
}


#endif
