
#pragma once

#include "../common/types.h"
#include "../mcu/compiler.h"

enum{
	FLASH_WRITE_STATUS_CMD	=	0x01,
	FLASH_WRITE_CMD			=	0x02,
	FLASH_READ_CMD			=	0x03,
	FLASH_WRITE_ENABLE_CMD 	= 	0x06,
	FLASH_WRITE_DISABLE_CMD = 	0x04,
	FLASH_READ_STATUS_CMD	=	0x05,
	FLASH_READ_STATUS_CMD1	=	0x35,
	FLASH_SECT_ERASE_CMD	=	0x20,
	FLASH_BLK_ERASE_CMD		=	0xD8,
	FLASH_POWER_DOWN		=	0xB9,
	FLASH_GET_JEDEC_ID		=	0x9F,
	//PN25F04C UID. Add by tuyf
	FLASH_GET_UNIQUE_ID		=	0x5A,//Read SFDP mode and Unique ID Number

};

#if FLASH_PROTECT_ENABLE
enum{
	FLASH_NONE	                   =	0,
	//GD25Q40
	FLASH_GD25Q40_0x70000_0x7ffff  = 1,    //Upper 64KB
	FLASH_GD25Q40_0x60000_0x7ffff  = 2,    //Upper 128KB
	FLASH_GD25Q40_0x40000_0x7ffff  = 3,    //Upper 256KB
	FLASH_GD25Q40_0x00000_0x0ffff  = 9,    //lower 64KB
	FLASH_GD25Q40_0x00000_0x1ffff  = 10,   //lower 128KB
	FLASH_GD25Q40_0x00000_0x3ffff  = 11,   //lower 256KB
	FLASH_GD25Q40_0x7F000_0x7ffff  = 0x11, //Top Block 4KB
	FLASH_GD25Q40_0x7E000_0x7ffff  = 0x12, //Top Block 8KB
	FLASH_GD25Q40_0x7C000_0x7ffff  = 0x13, //Top Block 16KB
	FLASH_GD25Q40_0x78000_0x7ffff  = 0x14, //0x16 //Top Block 32KB
	FLASH_GD25Q40_0x00000_0x00fff  = 0x19, //Bottom Block 4KB
	FLASH_GD25Q40_0x00000_0x01fff  = 0x1a, //Bottom Block 8KB
	FLASH_GD25Q40_0x00000_0x03fff  = 0x1b, //Bottom Block 16KB
	FLASH_GD25Q40_0x00000_0x07fff  = 0x1c, //0x1e //Bottom Block 32KB
	FLASH_GD25Q40_0x00000_0x7ffff  = 0x17, //4 //ALL 512KB

    //MD25D40
	FLASH_MD25D40_0x00000_0x7dfff  = 1, //lower 504KB
	FLASH_MD25D40_0x00000_0x7bfff  = 2, //lower 496KB
	FLASH_MD25D40_0x00000_0x77fff  = 3, //lower 480KB
	FLASH_MD25D40_0x00000_0x6ffff  = 4, //lower 448KB
	FLASH_MD25D40_0x00000_0x5ffff  = 5, //lower 384KB
	FLASH_MD25D40_0x00000_0x3ffff  = 6, //lower 256KB
	FLASH_MD25D40_0x00000_0x7ffff  = 7, //lower 512KB

	//PN25F04C add by tuyf
	FLASH_PN25F04C_0x70000_0x7ffff = 1, //upper 64KB
	FLASH_PN25F04C_0x60000_0x7ffff = 2, //upper 128KB
	FLASH_PN25F04C_0x40000_0x7ffff = 3, //upper 256KB
	FLASH_PN25F04C_0x20000_0x7ffff = 4, //upper 384KB
	FLASH_PN25F04C_0x10000_0x7ffff = 5, //upper 448KB
	FLASH_PN25F04C_0x00000_0x0ffff = 9, //lower 64KB
	FLASH_PN25F04C_0x00000_0x1ffff = 10,//lower 128KB
	FLASH_PN25F04C_0x00000_0x3ffff = 11,//lower 256KB
	FLASH_PN25F04C_0x00000_0x5ffff = 12,//lower 384KB
	FLASH_PN25F04C_0x00000_0x6ffff = 13,//lower 448KB
	FLASH_PN25F04C_0x00000_0x7ffff = 15,//ALL 512KB

};

_attribute_ram_code_ u32 flash_get_jedec_id();
_attribute_ram_code_ u16 flash_read_manufacture_id();
u32 flash_get_id(void);
int flash_protect_up256k(void);
int flash_protect_disable(void);
int flash_protect_down256k(void);
int flash_protect_8267_normal(void);
int flash_protect_up64k(void);
int flash_protect_down64k(void);
int flash_protect_up128k(void);
int flash_protect_down128k(void);
#endif

enum{
	FLASH_ID_GD25Q40  = 0xc8400013,
	FLASH_ID_MD25D40  = 0x51400013,
	FLASH_ID_PN25F04C = 0x1c310013,
};


_attribute_ram_code_ void flash_erase_sector(u32 addr);
_attribute_ram_code_ void flash_write_page(u32 addr, u32 len, u8 *buf);
_attribute_ram_code_ void flash_read_page(u32 addr, u32 len, u8 *buf);

#if 0//The internal flash model of the TLSR8261F512ET32 MCU is PN25F04C, which contains the uid register.
/*The Read Unique ID Number instruction accesses a factory-set read-only 96-bit number that is
 *unique to each PN25F04C device. The ID number can be used in conjunction with user software
 *unique methods to help prevent copying or cloning of a system.*/
_attribute_ram_code_ void flash_get_uid(u8* p);//FLASH mode: PN25F04C
#endif


