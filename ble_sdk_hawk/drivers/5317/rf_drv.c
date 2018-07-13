#include "rf_drv.h"
#include "register.h"
#include "analog.h"
#include "clock.h"
#include "pm.h"

#define	RF_MANUAL_AGC_MAX_GAIN		1
#define TX_GAIN    					0x93//register address of TP value

//BLE 1M mode
#define	TP_1M_G0	0x39//channel id:2402
#define	TP_1M_G1	0x33//channel id:2480

//BLE 2M mode
#define TP_2M_G0	0x47//channel id:2402
#define	TP_2M_G1	0x3f//channel id:2480

#define	TP_GET_GAIN(g0, g1)	  ((g0 - g1)*256/80)

int	rf_tp_base = TP_1M_G0;
int	rf_tp_gain = TP_GET_GAIN(TP_1M_G0, TP_1M_G1);

unsigned char emi_var[8];

const unsigned char rf_chn[MAX_RF_CHANNEL] = {
	FRE_OFFSET+ 5, FRE_OFFSET+ 9, FRE_OFFSET+13, FRE_OFFSET+17,
	FRE_OFFSET+22, FRE_OFFSET+26, FRE_OFFSET+30, FRE_OFFSET+35,
	FRE_OFFSET+40, FRE_OFFSET+45, FRE_OFFSET+50, FRE_OFFSET+55,
	FRE_OFFSET+60, FRE_OFFSET+65, FRE_OFFSET+70, FRE_OFFSET+76,
};

//////////////////////////////////////////////////////////////////////////////
//  Setting Table
//////////////////////////////////////////////////////////////////////////////
const TBLCMDSET  agc_tbl[] = {
	//Auto AGC table
	{0x489, 0x1c,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x488, 0x18,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x487, 0x2c,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x486, 0x3c,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x485, 0x38,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x484, 0x30,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x483, 0x23,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x482, 0x33,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x482, 0x23,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x481, 0x23,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x480, 0x23,	TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x499, 0x3f,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x498, 0x39,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x497, 0x36,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x496, 0x30,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x495, 0x2a,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x494, 0x24,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x493, 0x20,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x492, 0x20,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x491, 0x20,	TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x490, 0x20,	TCMD_UNDER_BOTH | TCMD_WRITE},
};

/**
 * @Brief: RF Base Initialization Table
 */
const TBLCMDSET  tbl_rf_init[] = {
	{0x01, 0x77, TCMD_UNDER_BOTH | TCMD_WAREG}, //lna/mixer ldo trim
	{0x06, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG}, //RF power-on.

	{0x80, 0x61, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x81, 0xd9, TCMD_UNDER_BOTH | TCMD_WAREG}, //frequency offset[4:0]
	{0x82, 0x5f, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x8b, 0x47, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x8d, 0x61, TCMD_UNDER_BOTH | TCMD_WAREG},

	{0xa0, 0x03, TCMD_UNDER_BOTH | TCMD_WAREG}, //dac datapath delay ******change  remington 0x26	(fix)
	//set TX power, actually no need, rf_set_power_level_index()  will update the value
	{0xa2, 0x2c, TCMD_UNDER_BOTH | TCMD_WAREG}, //pa_ramp_target ****0-5bit //TX power
	{0xa3, 0xf0, TCMD_UNDER_BOTH | TCMD_WAREG},

	{0xa8, 0x62, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xac, 0xa7, TCMD_UNDER_BOTH | TCMD_WAREG}, //RX bandwidth setting: 0xaa -> 0xa7 ,widen 15%
	{0xaa, 0x26, TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x4d4,0x8f,TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x4d5,0x02,TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x42d, 0xb3,TCMD_UNDER_BOTH | TCMD_WRITE},	// DC alpha=1/8, [6:4] r_predcoc_bw: [0,6], pwrDft 0x63,6-> 3
	{0x439, 0x6e,TCMD_UNDER_BOTH  | TCMD_WRITE},	//RX RSSI offset, pwrDft 0x6e
	{0x4cd, 0x66, TCMD_UNDER_BOTH | TCMD_WRITE},

	{0xf16, 0x29, TCMD_UNDER_BOTH | TCMD_WRITE},//TODO

	//tx/rx  timing set(must)
	{0xf04, 0x68, TCMD_UNDER_BOTH | TCMD_WRITE},	//tx settle time: 80us 0x68
	//must
	{0xf06, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE},	//rx wait settle time: 1us 0x00
	{0xf0c, 0x50, TCMD_UNDER_BOTH | TCMD_WRITE},	//rx settle time: 80us 0x50
	{0xf10, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE},	//wait time on NAK 000 0x00
};

/**
 * @Brief:Zigbee initialization table.
 */
const TBLCMDSET  tbl_rf_zigbee_250k[] = {
	{0x8f, 0xb2,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x93, 0x43,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x9e, 0xad,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xa3, 0x10,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xaa, 0xae,  TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x0400, 0x03, TCMD_UNDER_BOTH | TCMD_WRITE}, // 250K mode
	{0x0402, 0x23, TCMD_UNDER_BOTH | TCMD_WRITE}, // 11-bytes pre-amble
	{0x0404, 0xc0, TCMD_UNDER_BOTH | TCMD_WRITE}, // head_mode/crc_mode: normal c0
	{0x0405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE}, // access code length 4

	{0x0420, 0x90, TCMD_UNDER_BOTH | TCMD_WRITE}, // sync threshold: 1e (4); 26 (5)
	{0x0421, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE}, // no avg
	{0x0422, 0x1a, TCMD_UNDER_BOTH | TCMD_WRITE}, // threshold
	{0x0424, 0x52, TCMD_UNDER_BOTH | TCMD_WRITE}, // number for sync: bit[6:4]
	{0x0428, 0xe0, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x042b, 0xf3, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0464, 0x06, TCMD_UNDER_BOTH | TCMD_WRITE},//new sync
	{0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE}, // bit3: crc2_en; normal 1e
	{0x04eb, 0xe0, TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x043b, 0xfc, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x042c, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE},
};

/**
 * @Brief: BLE 1M Initialization Table
 */
const TBLCMDSET  tbl_rf_ble_1m[] = {
	{0x8f, 0xab, TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x93, 0x38, TCMD_UNDER_BOTH | TCMD_WAREG},
	//{0x9a, 0x10, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x9e, 0x56, TCMD_UNDER_BOTH | TCMD_WAREG},

	{0xa3, 0xf0, TCMD_UNDER_BOTH | TCMD_WAREG},//********[7:6] disable gauflt [5] LUT 2M or 1M
	//{0xa4, 0x90, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xaa, 0x26, TCMD_UNDER_BOTH | TCMD_WAREG},//* 1MHz */

	//BLE 1M setting
	{0x400, 0x0f,	TCMD_UNDER_BOTH | TCMD_WRITE},	// New 2M mode
	{0x401, 0x08,	TCMD_UNDER_BOTH | TCMD_WRITE},	// pn enable
	{0x402, 0x26,	TCMD_UNDER_BOTH | TCMD_WRITE},	// 6-byte pre-amble
	{0x404, 0xf5,	TCMD_UNDER_BOTH | TCMD_WRITE},	// head_mode/crc_mode: normal c0; 0xf7 for RX shockburst

	{0x405, 0x04,	TCMD_UNDER_BOTH | TCMD_WRITE},	// access code length 4
	{0x408, 0x8e,	TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte3
	{0x409, 0x89,	TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte2
	{0x40a, 0xbe,	TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte1
	{0x40b, 0xd6,	TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte0	//0xd6be898e

#if (LINK_LAYER_TEST_ENABLE)
	{0x420, 0x20,	TCMD_UNDER_BOTH | TCMD_WRITE},
#else
	{0x420, 0x1e,	TCMD_UNDER_BOTH | TCMD_WRITE},	// threshold  31/32
#endif
	{0x421, 0x04,	TCMD_UNDER_BOTH | TCMD_WRITE},	// no avg 0x04 -> 0x00
	{0x422, 0x00,	TCMD_UNDER_BOTH | TCMD_WRITE},	// threshold 0x00
	{0x424, 0x12,	TCMD_UNDER_BOTH | TCMD_WRITE},	// number fo sync: bit[6:4] 0x12
	{0x42b, 0xf1,	TCMD_UNDER_BOTH | TCMD_WRITE},	// access code: 1
	{0x428, 0x80,   TCMD_UNDER_BOTH | TCMD_WRITE},

	//0x042c, 0x30,	TCMD_UNDER_BOTH | TCMD_WRITE,	// maxiumum length 48-byte
	//{0x042c, 0xe0,TCMD_UNDER_BOTH | TCMD_WRITE},	// maxiumum length 224 byte
	{0x42c, 0xff,	TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x43b, 0xfc,   TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x464, 0x07,   TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x4cd, 0x66,	TCMD_UNDER_BOTH | TCMD_WRITE},//zhidong from 0x64 to ox66 manual rx PGA reset enable(default reset value 0)

	{0xf03, 0x1e,	TCMD_UNDER_BOTH | TCMD_WRITE},	// bit3: crc2_en; normal 1e
};

/**
 * @Brief: BLE 2M Initialization Table
 */
const TBLCMDSET  tbl_rf_ble_2m[] = {
	{0x8f, 0xb2,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x93, 0x43,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x9e, 0xad,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xa3, 0xd0,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xaa, 0xae,  TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x0400, 0x0f, TCMD_UNDER_BOTH | TCMD_WRITE}, // new 2M mode
	{0x0401, 0x08, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0402, 0x26, TCMD_UNDER_BOTH | TCMD_WRITE}, // 8-byte pre-amble
	{0x0404, 0xe5, TCMD_UNDER_BOTH | TCMD_WRITE}, //PN enable
	{0x0405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE}, //acc_len
	{0x0420, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE}, // sync threshold: 1e (4); 26 (5)
	{0x0421, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE}, // no avg
	{0x0422, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE}, // threshold
	{0x0424, 0x12, TCMD_UNDER_BOTH | TCMD_WRITE}, // number fo sync: bit[6:4]
	{0x0428, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x042c, 0xff, TCMD_UNDER_BOTH | TCMD_WRITE}, //baseband receive Max length
	{0x042b, 0xf1, TCMD_UNDER_BOTH | TCMD_WRITE}, // access code: 1

	{0x043b, 0xfc, TCMD_UNDER_BOTH | TCMD_WRITE}, //select timestamp
	{0x0464, 0x07, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE},
};

/**
 * @Brief: BLE 1M Disable PN Initialization Table
 */
const TBLCMDSET  tbl_rf_1m_no_pn[] = {
	{0x8f, 0xab, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x93, 0x38, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x9e, 0x56, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xa3, 0xf0, TCMD_UNDER_BOTH | TCMD_WAREG},//********[7:6] disable gauflt [5] LUT 2M or 1M
	{0xaa, 0xa6, TCMD_UNDER_BOTH | TCMD_WAREG},//*******filter iq_swap, adjust the bandwidth*****remington 0x2e

	{0x400, 0x0f, TCMD_UNDER_BOTH | TCMD_WRITE},	//
	{0x401, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE},	// pn enable
	{0x402, 0x26, TCMD_UNDER_BOTH | TCMD_WRITE},	// 8-byte pre-amble
	{0x404, 0xd5, TCMD_UNDER_BOTH | TCMD_WRITE},	// head_mode/crc_mode: normal c0; 0xf7 for RX shockburst ble whiting disable
	{0x405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE},	// access code length 4

	{0x408, 0x8e, TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte3
	{0x409, 0x89, TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte2
	{0x40a, 0xbe, TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte1
	{0x40b, 0xd6, TCMD_UNDER_BOTH | TCMD_WRITE},	// access code byte0	//0xd6be898e

#if (LINK_LAYER_TEST_ENABLE)
	{0x420, 0x20, TCMD_UNDER_BOTH | TCMD_WRITE},
#else
	{0x420, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE},	// threshold  31/32
#endif
	{0x421, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE},	// no avg 0x04 -> 0x00
	{0x422, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE},	// threshold 0x00
	{0x424, 0x12, TCMD_UNDER_BOTH | TCMD_WRITE},	// number fo sync: bit[6:4] 0x12
	{0x42b, 0xf1, TCMD_UNDER_BOTH | TCMD_WRITE},	// access code: 1
	{0x428, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE},

	//0x042c, 0x30,	TCMD_UNDER_BOTH | TCMD_WRITE,	// maxiumum length 48-byte
	//{0x042c, 0xe0,TCMD_UNDER_BOTH | TCMD_WRITE},	// maxiumum length 224 byte
	{0x042c, 0xff,TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x043b, 0xfc, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0464, 0x07, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE},	// bit3: crc2_en; normal 1e
};

/**
 * @Brief: Nordic 1M Initialization Table
 */
const TBLCMDSET  tbl_rf_nordic_1m[] = {
	{0x8f, 0xab,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x93, 0x38,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x9e, 0x56,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xa3, 0xf0,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xaa, 0xa6,  TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x0400, 0x0f, TCMD_UNDER_BOTH | TCMD_WRITE}, // new 2M mode
	{0x0402, 0x26, TCMD_UNDER_BOTH | TCMD_WRITE}, // 8-byte pre-amble
	{0x042b, 0xf1, TCMD_UNDER_BOTH | TCMD_WRITE}, // access code: 1
	{0x0424, 0x12, TCMD_UNDER_BOTH | TCMD_WRITE}, // number fo sync: bit[6:4]
	{0x0428, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0422, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE}, // threshold
	{0x0420, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE}, // sync threshold: 1e (4); 26 (5)
	{0x0421, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE}, // no avg
	{0x0464, 0x07, TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x0404, 0xda, TCMD_UNDER_BOTH | TCMD_WRITE}, // head_mode/crc_mode: normal c5
	{0x0f02, 0x64, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0f03, 0x36, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x04ca, 0xa0, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x04cb, 0x8c, TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x042c, 0xff, TCMD_UNDER_BOTH | TCMD_WRITE}, //baseband receive Max length
	{0x043b, 0xfc, TCMD_UNDER_BOTH | TCMD_WRITE}, //select timestamp
};

/**
 * @Brief: Nordic 2M Initialization Table
 */
const TBLCMDSET  tbl_rf_nordic_2m[] = {
	{0x8f, 0xb2,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x93, 0x50,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x9e, 0xad,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xa3, 0xd0,  TCMD_UNDER_BOTH | TCMD_WAREG},
	{0xaa, 0xae,  TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x0400, 0x0f, TCMD_UNDER_BOTH | TCMD_WRITE}, // new 2M mode
	{0x0402, 0x26, TCMD_UNDER_BOTH | TCMD_WRITE}, // 8-byte pre-amble
	{0x042b, 0xf1, TCMD_UNDER_BOTH | TCMD_WRITE}, // access code: 1
	{0x0424, 0x12, TCMD_UNDER_BOTH | TCMD_WRITE}, // number fo sync: bit[6:4]
	{0x0428, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0422, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE}, // threshold
	{0x0420, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE}, // sync threshold: 1e (4); 26 (5)
	{0x0421, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE}, // no avg
	{0x0464, 0x07, TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x0404, 0xca, TCMD_UNDER_BOTH | TCMD_WRITE}, // head_mode/crc_mode: normal c5
	{0x0f02, 0x64, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x0f03, 0x36, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x04ca, 0xa0, TCMD_UNDER_BOTH | TCMD_WRITE},
	{0x04cb, 0x8c, TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x042c, 0xff, TCMD_UNDER_BOTH | TCMD_WRITE}, //baseband receive Max length
	{0x043b, 0xfc, TCMD_UNDER_BOTH | TCMD_WRITE}, //select timestamp
};

void rf_update_tp_value(unsigned char tp0, unsigned char tp1)
{
	 rf_tp_base = tp0;
	 rf_tp_gain = TP_GET_GAIN(tp0, tp1);
}

void RF_SetManualMaxGain(void)
{
	//Disable AGC setting
	WRITE_REG8(0x430,0x17);
	WRITE_REG8(0x43d,0xfd);
	WRITE_REG8(0x438,0x37);

	//Max Gain setting
	WRITE_REG8(0x433,0x00);
	WRITE_REG8(0x434,0x01);
	WRITE_REG8(0x43a,0x77);
	WRITE_REG8(0x43e,0x89);
	WRITE_REG8(0x4cd,0x66);
	WRITE_REG8(0x4c0,0x81);
}

void RF_SetAgc(void)
{
	//Disable Max Gain setting
	WRITE_REG8(0x433,0x01);
	WRITE_REG8(0x434,0x21);
	WRITE_REG8(0x43a,0x22);
	WRITE_REG8(0x43e,0x20);

	//Enable agc
	WRITE_REG8(0x430,0x12);
	WRITE_REG8(0x43d,0xb1);
	WRITE_REG8(0x438,0xb7);
}

void rf_drv_init (RF_ModeTypeDef RF_Mode)
{
	/* RF module initialization */
	load_tbl_cmd_set(tbl_rf_init, sizeof (tbl_rf_init)/sizeof (TBLCMDSET));

	/* BLE/Zigbee/Nordic Mode configuration */
	if(RF_Mode == RF_MODE_BLE_1M)
	{
		rf_tp_base = TP_1M_G0;
		rf_tp_gain = TP_GET_GAIN(TP_1M_G0, TP_1M_G1);
		load_tbl_cmd_set(tbl_rf_ble_1m, sizeof (tbl_rf_ble_1m)/sizeof (TBLCMDSET));
	}
	else if(RF_Mode == RF_MODE_BLE_2M)
	{
		rf_tp_base = TP_2M_G0;
		rf_tp_gain = TP_GET_GAIN(TP_2M_G0, TP_2M_G1);
		load_tbl_cmd_set (tbl_rf_ble_2m, sizeof (tbl_rf_ble_2m)/sizeof (TBLCMDSET));
	}
	else if (RF_Mode == RF_MODE_BLE_1M_NO_PN)
	{
		rf_tp_base = TP_1M_G0;
		rf_tp_gain = TP_GET_GAIN(TP_1M_G0, TP_1M_G1);
		load_tbl_cmd_set (tbl_rf_1m_no_pn, sizeof (tbl_rf_1m_no_pn)/sizeof (TBLCMDSET));
	}
	else if (RF_Mode == RF_MODE_ZIGBEE_250K)
	{
		rf_tp_base = TP_2M_G0;
		rf_tp_gain = TP_GET_GAIN(TP_2M_G0, TP_2M_G1);
		load_tbl_cmd_set (tbl_rf_zigbee_250k, sizeof (tbl_rf_zigbee_250k)/sizeof (TBLCMDSET));
	}
	else if (RF_Mode == RF_MODE_NORDIC_1M)
	{
		rf_tp_base = TP_1M_G0;
		rf_tp_gain = TP_GET_GAIN(TP_1M_G0, TP_1M_G1);
		load_tbl_cmd_set (tbl_rf_nordic_1m, sizeof (tbl_rf_nordic_1m)/sizeof (TBLCMDSET));//
	}
	else if (RF_Mode == RF_MODE_NORDIC_2M)
	{
		rf_tp_base = TP_2M_G0;
		rf_tp_gain = TP_GET_GAIN(TP_2M_G0, TP_2M_G1);
		load_tbl_cmd_set (tbl_rf_nordic_2m, sizeof (tbl_rf_nordic_2m)/sizeof (TBLCMDSET));
	}

	/* Set AGC table */
	load_tbl_cmd_set(agc_tbl,sizeof(agc_tbl)/sizeof(TBLCMDSET));

	/* Set/Enable Auto AGC */
	//RF_SetAgc();

	/* Set/Enable Max Gain(Manual mode) */
	RF_SetManualMaxGain();
}

/**
 * @Brief: Tx power table.
 */
const char tbl_rf_power[] = {
	//a2    04    a7    8d
	0x25,	0xba,	0xf1,	0x60,	   //11.8
	0x25,	0x92,	0xa9,	0x62,	   //9.6
	0x25,	0x92,	0xd9,	0x62,	   //7.9
	0x25,	0xb2,	0xe9,	0x61,	   //6.3
	0x19,	0xa2,	0xe9,	0x61,	   //4.9
	0x12,	0x92,	0xe9,	0x61,	   //3.3
	0x0a,	0x92,	0xe9,	0x61,	   //1.6
	0x08,	0x8a,	0xe9,	0x61,	   //0
	0x05,	0x9a,	0xe9,	0x61,	   //-1.5
	0x04,	0xa2,	0xe9,	0x62,	   //-3.1
	0x03,	0xba,	0xe9,	0x62,	   //-5
	0x02,	0xa2,	0xd9,	0x62,	   //-7.3
	0x02,	0x82,	0xd9,	0x62,	   //-9.6
	0x02,	0xaa,	0xe9,	0x62,	   //-11.5
	0x01,	0x9a,	0xd9,	0x62,	   //-13.3
	0x01,	0xba,	0xe9,	0x62,	   //-16
	0x01,	0xa2,	0xe9,	0x62,	   //-17.8
	0x00,	0x92,	0xd9,	0x62,	   //-19.5
};

unsigned char txPower_index = 0;
void rf_set_power_level_index(RF_TxPowerTypeDef level)
{
	txPower_index = level;

	if (level + 1 > (sizeof (tbl_rf_power)>>2)) {
		level = (sizeof (tbl_rf_power)>>2) - 1;
	}

	unsigned char *p = (unsigned char*)(tbl_rf_power + level * 4);
	analog_write (0xa2, *p++);

	analog_write(0xa3,analog_read(0xa3)&0xef); //ana_0xa3<4> PA ramp enable
	analog_write(0xa3,analog_read(0xa3)|0x10);

	analog_write (0x04, *p++);
	analog_write (0xa7, *p++);
	analog_write (0x8d, *p++);
}

char rf_get_tx_power_level(void)
{
#if 0
	char txPower;
	if(txPower_index > 5){
		txPower = -20;
	}
	else{
		txPower = 8 - (txPower_index<<2);
	}

	return txPower;
#endif
}

void rf_power_enable(int en)
{

}

void rf_set_tp_gain(char chn)
{
	 analog_write(TX_GAIN, rf_tp_base - ((chn * rf_tp_gain + 128) >> 8));
}

void rf_set_channel(signed char chn, unsigned short set)
{
	analog_write(0x06, 0x00);
	write_reg8(0x800f16, 0x29);

	write_reg8(0x800428, RF_TRX_MODE);	// rx disable
	if ( set & RF_SET_TX_MANAUL ){
        write_reg8 (0x800f02, RF_TRX_OFF_MANUAL);  // reset tx/rx state machine
    }
    else{
    	write_reg8 (0x800f02, 0x44);
    }

	if (set & RF_CHN_TABLE){
		chn = rf_chn[chn];
	}

	write_reg16(0x8004d6, 2400 + chn);	// {intg_N}
	analog_write(TX_GAIN, rf_tp_base - ((chn * rf_tp_gain + 128) >> 8));
}

#if (BLS_BLE_RF_IRQ_TIMING_EXTREMELY_SHORT_EN)
_attribute_ram_code_
#endif
void rf_set_ble_channel(signed char chn)
{
	write_reg8 (0x80040d, chn);

	if (chn < 11)
    	chn += 2;
    else if (chn < 37)
    	chn += 3;
    else if (chn == 37)
    	chn = 1;
    else if (chn == 38)
    	chn = 13;
    else
    	chn = 40;

    chn = chn << 1;

	/////////////////// turn on LDO and baseband PLL ////////////////
	analog_write (0x06, 0x00);
	write_reg8 (0x800f16, 0x29);

	write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
	write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine


    write_reg16 (0x8004d6, 2400 + chn);	// {intg_N}
    analog_write (TX_GAIN, rf_tp_base - ((chn * rf_tp_gain + 128) >> 8));
}

void rf_rx_buffer_set(unsigned char *RF_RxBufAddr, int byte_size, unsigned char  PingPongEn)
{
    unsigned char mode;

	mode = PingPongEn ? 0x03 : 0x01;
	reg_dma2_addr = (unsigned short)(unsigned int)(RF_RxBufAddr);
	reg_dma2_size = (byte_size>>4);
	reg_dma2_mode = mode;
}

#if 0
void rf_set_tx_rx_off (void)
{
	/////////////////// turn on LDO and baseband PLL ////////////////
	//analog_write (0x06, 0xfe);
	write_reg8 (0x800f16, 0x29);
	write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
	write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine
}

void rf_set_rxmode ()
{
    write_reg8(0x800428, RF_TRX_MODE | BIT(0));	// RX enable
    write_reg8(0x800f02, RF_TRX_OFF | BIT(5));	// manual RX enable
}

void rf_set_txmode ()
{
	write_reg8(0x800f02, RF_TRX_OFF | BIT(4));	// TX enable
}


void rf_send_packet (void* addr, unsigned short rx_waittime, unsigned char retry)
{
	write_reg8  (0x800f00, 0x80);				// stop
	write_reg8  (0x800f14, retry);				// number of retry
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));
	write_reg16 (0x800f0a, rx_waittime);

#if 0
#if RF_FAST_MODE_2M
	write_reg16 (0x800f00, 0x3f83);				// start tx with PID = 0
#else
	write_reg16 (0x800f00, 0x3f81);				// start tx with PID = 0
	//write_reg8 (0x800524, 0x08);
#endif
#endif

	write_reg16 (0x800f00, 0x3f83);
}

void rf_send_packet_from_rx (void* addr)
{
	write_reg8 (0x800f02, RF_TRX_OFF);			// TRX manual mode off
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));

#if RF_FAST_MODE_2M
	write_reg16 (0x800f00, 0x3f85);				// start tx with PID = 0, simple tx
#else
	write_reg16 (0x800f00, 0x3f81);				// start tx with PID = 0
	//write_reg8 (0x800524, 0x08);
#endif
}

void rf_send_single_packet (void* addr)
{
	write_reg8 (0x800f02, RF_TRX_OFF);			// TRX manual mode off
	analog_write (0x06, 0x0);					//turn on RFPLL
	write_reg8  (0x800f00, 0x80);				// stop
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));

#if RF_FAST_MODE_2M
	write_reg16 (0x800f00, 0x3f85);				// start tx with PID = 0, simple tx
#else
	write_reg16 (0x800f00, 0x3f81);				// start tx with PID = 0
	//write_reg8 (0x800524, 0x08);
#endif
}

void rf_multi_receiving_init (unsigned char channel_mask)
{
//	write_reg8  (0x800f04, 0x50);			// set tx settle to 80us,
	write_reg8  (0x800f03, 0x30);			// disable rx timeout
	write_reg8  (0x800f15, 0xe0);			// disable TX manuuanl mode
	write_reg8  (0x800407, channel_mask);	// channel mask
}


void rf_receiving_pipe_enble(unsigned char channel_mask)
{
	write_reg8  (0x800407, channel_mask);	// channel mask
}

void rf_multi_receiving_start  (signed char chn, unsigned short set)
{
	write_reg8  (0x800f00, 0x80);			// stop
//	write_reg8  (0x800060, 0x80);			// reset baseband
//	write_reg8  (0x800060, 0x00);
	write_reg8 (0x800f02, RF_TRX_OFF);		// reset tx/rx state machine
	rf_set_channel (chn,  set);
	write_reg16  (0x800f00, 0x3f84);		// start rx with PID = 0
}

void rf_multi_receiving_send_packet  (void* addr)
{
//	write_reg8  (0x800060, 0x80);				// reset baseband
//	write_reg8  (0x800060, 0x00);
	write_reg8  (0x800f14, 0);					// number of retry
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));
	write_reg16 (0x800f0a, 1);					// rx wait time
	write_reg16 (0x800f00, 0x3f83);				// start tx with PID = 0
	write_reg8 (0x800524, 0x08);
}
#endif

void rf_start_stx(void* addr, unsigned int tick)
{
	//write_reg32(0x800f04, 0);						// tx wail & settle time: 0
	write_reg32(0x800f18, tick);						// Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
#if RF_FAST_MODE_2M
	write_reg16 (0x800f00, 0x3f85);						// single TX
#else
	write_reg8(0x800f00, 0x85);						// single TX
#endif
	write_reg16(0x80050c, (unsigned short)((unsigned int)addr));
}

void rf_start_srx(unsigned int tick)
{
	write_reg32(0x800f28, 0x0fffffff);//first timeout
	write_reg32(0x800f18, tick);
	write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);
	write_reg8(0xf00, 0x86);//single Rx
	//write_reg16(0x800f00, 0x3f86);//????
}

void rf_start_stx2rx(void* addr, unsigned int tick)
{
	//write_reg32 (0x800f04, 0x050);  // tx settle time: 80 us
	write_reg32(0x800f28, 0x0fffffff);//first timeout
	write_reg32(0x800f18, tick);	  // Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
#if RF_FAST_MODE_2M
	write_reg16(0x800f00, 0x3f87);
#else
	write_reg8(0x800f00, 0x87);
#endif
	// single tx2rx
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));
	//write_reg32 (0x800f04, 0x0);	 //tx settle time: 80 us
}

void rf_start_srx2tx(void* addr, unsigned int tick)
{
//	write_reg32 (0x800f04, 0x50);						// tx_wait = 0; tx_settle = 80 us
	write_reg32 (0x800f28, 0x0fffffff);					// first timeout
	write_reg32(0x800f18, tick);						// Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg16 (0x800f00, 0x3f88);						// single rx2tx
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));
}

void rf_start_btx(void* addr, unsigned int tick)
{
	//write_reg32 (0x800f04, 0x050);// tx settle time: 80 us
	write_reg32(0x800f18, tick);	// Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg8 (0x800f00, 0x81);						// ble tx
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));
}

void rf_set_ack_packet(void* addr)
{
	write_reg16 (0x80050c, (unsigned short)((unsigned int)addr));
}

void rf_tx_pkt(unsigned char *RF_TxBufAddr)
{
	write_reg8(0x50f,0x80); //dma mode
    write_reg16(0x50c, (unsigned short)((unsigned int)RF_TxBufAddr));
    write_reg8(0x524, 0x08); //dma packet ready for transfer
}

static RF_StatusTypeDef RF_TRxState = RF_MODE_TX;
int rf_trx_state_set(RF_StatusTypeDef rf_status, signed char rf_channel)
{
    int err = 0;
    rf_set_channel(rf_channel, 0);
    if (rf_status == RF_MODE_TX) {
        WRITE_REG8(0xf02, 0x45);  //reset tx_en in manual mode
        WRITE_REG8(0xf02, 0x55);  //tx_en
        WRITE_REG8(0x428, READ_REG8(0x428) & 0xfe);  //rx disable
        RF_TRxState = RF_MODE_TX;
    }
    else if (rf_status == RF_MODE_RX) {

    	WRITE_REG8(0xf02, 0x45);
    	WaitUs(5);
        WRITE_REG8(0xf02, 0x65);
        WRITE_REG8(0x428, READ_REG8(0x428) | 0x01);  //rx disable
        RF_TRxState = RF_MODE_RX;
    }
    else if (rf_status == RF_MODE_AUTO) {
        WRITE_REG8(0xf00,0x80); //stop cmd
        WRITE_REG8(0xf16, 0x29); // reg0xf16 pll_en_man and tx_en_dly_en  enable
        WRITE_REG8(0x428, READ_REG8(0x428) & 0xfe);  //rx disable
        WRITE_REG8(0xf02, READ_REG8(0xf02) & 0xcf);  //reg0xf02 disable rx_en_man and tx_en_man
        RF_TRxState = RF_MODE_AUTO;
    }
    else {
        err = -1;
    }
    return  err;
}

RF_StatusTypeDef rf_trx_state_get(void)
{
    return RF_TRxState;
}
