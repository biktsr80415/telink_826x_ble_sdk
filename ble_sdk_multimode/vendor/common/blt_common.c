/********************************************************************************************************
 * @file     blt_common.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#include "tl_common.h"
#include "drivers.h"


/*
 * VVWWXX38C1A4YYZZ
 * public_mac: 		  VVWWXX 38C1A4
 * random_static_mac: VVWWXXYYZZ C0
 */

void blc_initMacAddress(int flash_addr, u8 *mac_public, u8 *mac_random_static)
{
	u8 mac_read[8];
	flash_read_page(flash_addr, 8, mac_read);

	u8 value_random[4];

	u8 ff_six_byte[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	if ( memcmp(mac_read, ff_six_byte, 6) ) {
		memcpy(mac_public, mac_read, 6);  //copy public address from flash
	}
	else{  //no public address on flash
		generateRandomNum(3, value_random);

		mac_public[0] = value_random[0];
		mac_public[1] = value_random[1];
		mac_public[2] = value_random[2];
		mac_public[3] = 0x38;             //company id: 0xA4C138
		mac_public[4] = 0xC1;
		mac_public[5] = 0xA4;

		flash_write_page (flash_addr, 6, mac_public);
	}



	if(mac_random_static != NULL){

		mac_random_static[0] = mac_public[0];
		mac_random_static[1] = mac_public[1];
		mac_random_static[2] = mac_public[2];

		u16 high_2_byte = (mac_read[6] | mac_read[7]<<8);
		if(high_2_byte != 0xFFFF){
			memcpy( (u8 *)(mac_random_static + 3), (u8 *)(mac_read + 6), 3);  //mac_random_static[3...4]
		}
		else{
			generateRandomNum(2, value_random);

			mac_random_static[3] = value_random[0];
			mac_random_static[4] = value_random[1];

			flash_write_page (flash_addr + 6, 2, (u8 *)(mac_random_static + 3) );
		}

		mac_random_static[5] = 0xC0; 			//for random static
	}
}

