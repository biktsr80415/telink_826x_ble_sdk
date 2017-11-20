
#include "../tl_common.h"
#include "../drivers/spi.h"
#include "flash.h"
#include "spi_i.h"

#ifndef FLASH_PROTECT_ENABLE
#define FLASH_PROTECT_ENABLE    0
#endif



_attribute_ram_code_ static inline int flash_is_busy(){
	return mspi_read() & 0x01;				//  the busy bit, pls check flash spec
}

_attribute_ram_code_ static void flash_send_cmd(u8 cmd){
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

_attribute_ram_code_ static void flash_send_addr(u32 addr){
	mspi_write((u8)(addr>>16));
	mspi_wait();
	mspi_write((u8)(addr>>8));
	mspi_wait();
	mspi_write((u8)(addr));
	mspi_wait();
}

//  make this a asynchorous version
_attribute_ram_code_ static void flash_wait_done()
{
	sleep_us(100);
	flash_send_cmd(FLASH_READ_STATUS_CMD);

	int i;
	for(i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
	mspi_high();
}

_attribute_ram_code_ void flash_erase_sector(u32 addr){
	u8 r = irq_disable();

	WATCHDOG_CLEAR;  //in case of watchdog timeout

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_SECT_ERASE_CMD);
	flash_send_addr(addr);
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

_attribute_ram_code_ void flash_write_page(u32 addr, u32 len, u8 *buf){
	u8 r = irq_disable();

	// important:  buf must not reside at flash, such as constant string.  If that case, pls copy to memory first before write
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_CMD);
	flash_send_addr(addr);

	u32 i;
	for(i = 0; i < len; ++i){
		mspi_write(buf[i]);		/* write data */
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

_attribute_ram_code_ void flash_read_page(u32 addr, u32 len, u8 *buf){
	u8 r = irq_disable();


	flash_send_cmd(FLASH_READ_CMD);
	flash_send_addr(addr);

	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
	mspi_wait();
	/* get data */
	for(int i = 0; i < len; ++i){
		*buf++ = mspi_get();
		mspi_wait();
	}
	mspi_high();

	irq_restore(r);
}

#if 0//The internal flash model of the TLSR8261F512ET32 MCU is PN25F04C, which contains the uid register.
/*The Read Unique ID Number instruction accesses a factory-set read-only 96-bit number that is
 *unique to each PN25F04C device. The ID number can be used in conjunction with user software
 *unique methods to help prevent copying or cloning of a system.*/
_attribute_ram_code_ void flash_get_uid(u8* p)//add by tuyf
{
	u8 r = irq_disable();
	flash_send_cmd(FLASH_GET_UNIQUE_ID);
	flash_send_addr(0x000080);//reference << XTX_SPI-Nor_PN25F04C_V2.0.pdf >> page 64

	mspi_write(0);// dummy for protocol
	mspi_wait();

#if 0
	/* get data manual*/
	for(int i = 0; i < 12; i++)//UID 96bit
	{
		*p++ = mspi_read();
		mspi_wait();
	}
	mspi_high();
	irq_restore(r);
#else
	mspi_write(0x00);		/* to issue clock , for setting auto mode below*/
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
	mspi_wait();

	/* get data auto*/
	for(int i = 0; i < 12; i++){
		*p++ = mspi_get();
		mspi_wait();
	}
	mspi_high();
	irq_restore(r);
#endif

}
#endif

#if FLASH_PROTECT_ENABLE
u8 flash_protect_en = FLASH_PROTECT_ENABLE;

_attribute_ram_code_ u32 flash_get_jedec_id(){
	u8 r = irq_disable();
	flash_send_cmd(FLASH_GET_JEDEC_ID);
	u8 manufacturer = mspi_read();
	u8 mem_type = mspi_read();
	u8 cap_id = mspi_read();
	mspi_high();
	irq_restore(r);
	return (u32)((manufacturer << 24 | mem_type << 16 | cap_id));
}

static u16 T_flash_status = -1;
u32 flash_get_id(void)
{
    return flash_get_jedec_id();
}

_attribute_ram_code_ u16 flash_status_read(){
    u16 status = 0;
	u8 r = irq_disable();

	flash_send_cmd(FLASH_READ_STATUS_CMD);
	for(int i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
    status = mspi_read();
	mspi_wait();

	if(FLASH_ID_GD25Q40 == flash_get_id()){
    	flash_send_cmd(FLASH_READ_STATUS_CMD1);
        status = status + (mspi_read() << 8);
    	mspi_wait();
	}

	mspi_high();

	irq_restore(r);
	return status;
}
_attribute_ram_code_ void flash_status_write(u16 status){
	u8 r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_STATUS_CMD);
	mspi_write((u8)status);
	mspi_wait();
	if(FLASH_ID_GD25Q40 == flash_get_id()){
    	mspi_write((u8)(status >> 8));
    	mspi_wait();
	}

	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

//FLASH : GD25Q40B
/*************************** STATUS REGISTER ************************************
 S15-S10   S9   S8     S7 	  S6	 S5		 S4		 S3		 S2		 S1		 S0
 Reserved  QE   SRP1  SRP0    BP4    BP3     BP2     BP1     BP0     WEL     WIP
*********************************************************************************/

/************************ PN25F04C Protected area size *****************************************
 Status Register Content					           Memory Content
  BP3  BP3  BP2	BP1	BP0 				Blocks	         Addresses	       Density	   	   Portion
   x   	x    0 	 0	 0                    NONE              NONE              NONE        	NONE
   0    0    0 	 0	 1                   Block7         070000H-07FFFFH      64KB		  Upper 1/8
   0    0    0 	 1	 0                Block 6 to 7      060000H-07FFFFH      128KB		  Upper 2/8
   0    0    0 	 1	 1                Block 4 to 7      040000H-07FFFFH      256KB		  Upper 4/8
   0    1    0 	 0	 1                   Block 0        000000H-00FFFFH      64KB		  Upper 1/8
   0    1    0 	 1	 0                Block 0 to 1      000000H-01FFFFH      128KB		  Upper 2/8
   0    1    0 	 1	 1                Block 0 to 3      000000H-03FFFFH      256KB		  Upper 4/8
   0    x    1 	 x	 x                Block 0 to 7      000000H-07FFFFH      512KB			ALL
   1    0    0 	 0	 1                   Block 7        07F000H-07FFFFH      4KB		  Top Block
   1    0    0 	 1	 0                   Block 7        07E000H-07FFFFH      8KB		  Top Block
   1    0    0 	 1	 1                   Block 7        07C000H-07FFFFH      16KB		  Top Block
   1    0    1 	 0	 x                   Block 7        078000H-07FFFFH      32KB		  Top Block
   1    0    1 	 1	 0                   Block 7        078000H-07FFFFH      32KB		  Top Block
   1    1    0 	 0	 1                   Block 0        000000H-000FFFH      4KB		  Bottom Block
   1    1    0 	 1	 0                   Block 0        000000H-001FFFH      8KB		  Bottom Block
   1    1    0 	 1	 1                   Block 0        000000H-003FFFH      16KB		  Bottom Block
   1    1    1 	 0	 x                   Block 0        000000H-007FFFH      32KB		  Bottom Block
   1    1    1 	 1	 0                     ALL          000000H-007FFFH      32KB		  Bottom Block
   1    x    1 	 1	 1                Block 0 to 7      000000H-07FFFFH      512KB			ALL
**********************************************************************************************/
int flash_protect_GD25Q40B(u8 idx){
	u8 r = irq_disable();
	int ret = 0;
	u16 status;
    #if 1   // read status before write
	T_flash_status = status = flash_status_read();

	status &= (u16)(~((u16)(0x7c) | (BIT(6) << 8)));//1000 0011  ~0111 1100
	#else   // use default 0
	status = 0;
	#endif

	if(FLASH_NONE == idx){

	}else if(FLASH_GD25Q40_0x70000_0x7ffff == idx){
        status |= (u16)(0b00001 << 2);               //Upper 64KB
	}else if(FLASH_GD25Q40_0x60000_0x7ffff == idx){
        status |= (u16)(0b00010 << 2);               //Upper 128KB
	}else if(FLASH_GD25Q40_0x40000_0x7ffff == idx){
        status |= (u16)(0b00011 << 2);               //Upper 256KB
	}else if(FLASH_GD25Q40_0x00000_0x0ffff == idx){
        status |= (u16)(0b01001 << 2);               //lower 64KB
	}else if(FLASH_GD25Q40_0x00000_0x1ffff == idx){
        status |= (u16)(0b01010 << 2);               //lower 128KB
	}else if(FLASH_GD25Q40_0x00000_0x3ffff == idx){
        status |= (u16)(0b01011 << 2);               //lower 256KB
	}else if(FLASH_GD25Q40_0x7F000_0x7ffff == idx){
        status |= (u16)(0b10001 << 2);               //Top Block 4KB
	}else if(FLASH_GD25Q40_0x7E000_0x7ffff == idx){
        status |= (u16)(0b10010 << 2);               //Top Block 8KB
	}else if(FLASH_GD25Q40_0x7C000_0x7ffff == idx){
        status |= (u16)(0b10011 << 2);               //Top Block 16KB
	}else if(FLASH_GD25Q40_0x78000_0x7ffff == idx || idx == 0x16){
        status |= (u16)(0b10100 << 2);               //Top Block 32KB
	}else if(FLASH_GD25Q40_0x00000_0x00fff == idx){
        status |= (u16)(0b11001 << 2);              //Bottom Block 4KB
	}else if(FLASH_GD25Q40_0x00000_0x01fff == idx){
	    status |= (u16)(0b11010 << 2);              //Bottom Block 8KB
	}else if(FLASH_GD25Q40_0x00000_0x03fff == idx){
		status |= (u16)(0b11011 << 2);              //Bottom Block 16KB
	}else if(FLASH_GD25Q40_0x00000_0x07fff == idx || idx == 0x1e){
		status |= (u16)(0b11100 << 2);              //Bottom Block 32KB
	}else if(FLASH_GD25Q40_0x00000_0x7ffff == idx || idx == 0x04){
	    status |= (u16)(0b10111 << 2);              //ALL 512KB
     }else{
        ret = -1;
    	irq_restore(r);
    	return ret;
    }

    if(T_flash_status == status){
        ret = 0;
    	irq_restore(r);
    	return ret;
    }

    flash_status_write(status);

	#if 1   // check
	sleep_us(100);
	T_flash_status = flash_status_read();
	if((T_flash_status & ((u16)((u16)(0x7c) | (BIT(6) << 8)))) //0111 1100
	 != (status & ((u16)((u16)(0x7c) | (BIT(6) << 8))))){
        ret = -1;
	}
	#endif

	irq_restore(r);
	return ret;
}
//FLASH : MD25D40
/*************************** STATUS REGISTER ************************************
  S7	S6		S5		S4		S3		S2		S1		S0
 SRP Reserved Reserved  BP2     BP1     BP0     WEL     WIP
*********************************************************************************/

/************************ MD25D40 Protected area size *****************************************
 Status Register Content					           Memory Content
   BP2	BP1	BP0 				Blocks	         Addresses	       Density	   	   Portion
    0 	0	0                    NONE              NONE             NONE        	NONE
    0 	0	1               Sector 0 to 125    000000H-07DFFFH      504KB		Lower126/128
    0 	1	0               Sector 0 to 123    000000H-07BFFFH      496KB		Lower124/128
    0 	1	1               Sector 0 to 119    000000H-077FFFH      480KB		Lower120/128
    1 	0	0               Sector 0 to 111    000000H-06FFFFH      448KB		Lower112/128
    1 	0	1               Sector 0 to 95     000000H-05FFFFH      384KB		Lower96/128
    1 	1	0               Sector 0 to 63     000000H-03FFFFH      256KB		Lower64/128
    1 	1	1              	 		ALL        000000H-07FFFFH      512KB			ALL
**********************************************************************************************/
int flash_protect_MD25D40(u8 idx){
	u8 r = irq_disable();
	int ret = 0;
	u16 status;
    #if 1   // read status before write
	T_flash_status = status = flash_status_read();

	status &= (u16)(~((u16)(0x1c)));//1110 0011  ~0001 1100
	#else   // use default 0
	status = 0;
	#endif

	if(FLASH_NONE == idx){

	}else if(FLASH_MD25D40_0x00000_0x7dfff == idx){
        status |= (u16)(0b001 << 2);               //lower 504KB
	}else if(FLASH_MD25D40_0x00000_0x7bfff == idx){
        status |= (u16)(0b010 << 2);               //lower 496KB
	}else if(FLASH_MD25D40_0x00000_0x77fff == idx){
        status |= (u16)(0b011 << 2);               //lower 480KB
	}else if(FLASH_MD25D40_0x00000_0x6ffff == idx){
        status |= (u16)(0b100 << 2);               //lower 448KB
	}else if(FLASH_MD25D40_0x00000_0x5ffff == idx){
        status |= (u16)(0b101 << 2);               //lower 384KB
	}else if(FLASH_MD25D40_0x00000_0x3ffff == idx){
        status |= (u16)(0b110 << 2);               //lower 256KB
	}else if(FLASH_MD25D40_0x00000_0x7ffff == idx){
	    status |= (u16)(0b111 << 2);               //ALL 512KB
	}else{
        ret = -1;
    	irq_restore(r);
    	return ret;
    }

    if(T_flash_status == status){
        ret = 0;
    	irq_restore(r);
    	return ret;
    }

    flash_status_write(status);

	#if 1   // check
	sleep_us(100);
	T_flash_status = flash_status_read();
	if((T_flash_status & ((u16)(0x1c))) //0001 1100
	 != (status & ((u16)(0x7c)))){
        ret = -1;
	}
	#endif

	irq_restore(r);
	return ret;
}

//FLASH : PN25F04C
/*************************** STATUS REGISTER ************************************
  S7	S6		S5		S4		S3		S2		S1		S0
 SRP Reserved   BP3     BP2     BP1     BP0     WEL     WIP
*********************************************************************************/

/************************ PN25F04C Protected area size *****************************************
 Status Register Content					           Memory Content
  BP3  BP2	BP1	BP0 				Blocks	         Addresses	       Density	   	   Portion
   0    0 	0	0                    NONE              NONE              NONE        	NONE
   0    0 	0	1                   Block7         070000H-07FFFFH      64KB		  Upper 1/8
   0    0 	1	0                Block 6 to 7      060000H-07FFFFH      128KB		  Upper 2/8
   0    0 	1	1                Block 4 to 7      040000H-07FFFFH      256KB		  Upper 4/8
   0    1 	0	0                Block 2 to 7      020000H-07FFFFH      384KB		  Upper 6/8
   0    1 	0	1                Block 1 to 7      010000H-07FFFFH      448KB		  Upper 7/8
   0    1 	1	0                     ALL          000000H-07FFFFH      512KB		    ALL
   0    1 	1	1              	 	  ALL          000000H-07FFFFH      512KB			ALL
   1    0 	0	0                    NONE              NONE              NONE        	NONE
   1    0 	0	1                   Block0         000000H-00FFFFH      64KB		  Lower 1/8
   1    0 	1	0                Block 0 to 1      000000H-01FFFFH      128KB		  Lower 2/8
   1    0 	1	1                Block 0 to 3      000000H-03FFFFH      256KB		  Lower 4/8
   1    1 	0	0                Block 0 to 5      000000H-05FFFFH      384KB		  Lower 6/8
   1    1 	0	1                Block 0 to 6      000000H-06FFFFH      448KB		  Lower 7/8
   1    1 	1	0                     ALL          000000H-07FFFFH      512KB		    ALL
   1    1 	1	1              	 	  ALL          000000H-07FFFFH      512KB			ALL
**********************************************************************************************/
int flash_protect_PN25F04C(u8 idx){
	u8 r = irq_disable();
	int ret = 0;
	u16 status;
    #if 1   // read status before write
	T_flash_status = status = flash_status_read();

	status &= (u16)(~((u16)(0x3c)));//1100 0011  ~0011 1100
	#else   // use default 0
	status = 0;
	#endif

	if(FLASH_NONE == idx || idx == 0x08){
        //Do Not protect FLASH data
	}
	else if(FLASH_PN25F04C_0x00000_0x7ffff == idx || idx == 0x0e || idx == 0x06 || idx == 0x07){
		status |= (u16)(0b1111 << 2);               //ALL 512KB
	}else if(FLASH_PN25F04C_0x70000_0x7ffff == idx){
        status |= (u16)(0b0001 << 2);               //upper 64KB
	}else if(FLASH_PN25F04C_0x60000_0x7ffff == idx){
        status |= (u16)(0b0010 << 2);               //upper 128KB
	}else if(FLASH_PN25F04C_0x40000_0x7ffff == idx){
        status |= (u16)(0b0011 << 2);               //upper 256KB
	}else if(FLASH_PN25F04C_0x20000_0x7ffff == idx){
        status |= (u16)(0b0100 << 2);               //upper 384KB
	}else if(FLASH_PN25F04C_0x10000_0x7ffff == idx){
        status |= (u16)(0b1001 << 2);               //upper 448KB
	}else if(FLASH_PN25F04C_0x00000_0x0ffff == idx){
        status |= (u16)(0b1001 << 2);               //lower 64KB
	}else if(FLASH_PN25F04C_0x00000_0x1ffff == idx){
        status |= (u16)(0b1010 << 2);               //lower 128KB
	}else if(FLASH_PN25F04C_0x00000_0x3ffff == idx){
        status |= (u16)(0b1011 << 2);               //lower 256KB
	}else if(FLASH_PN25F04C_0x00000_0x5ffff == idx){
        status |= (u16)(0b1100 << 2);               //lower 384KB
	}else if(FLASH_PN25F04C_0x00000_0x6ffff == idx){
        status |= (u16)(0b1101 << 2);               //lower 448KB
	}else{
        ret = -1;
    	irq_restore(r);
    	return ret;
    }

    if(T_flash_status == status){
        ret = 0;
    	irq_restore(r);
    	return ret;
    }

    flash_status_write(status);

	#if 1   // check
	sleep_us(100);
	T_flash_status = flash_status_read();
	if((T_flash_status & ((u16)(0x3c))) //0011 1100
	 != (status & ((u16)(0x7c)))){
        ret = -1;
	}
	#endif

	irq_restore(r);
	return ret;
}

int flash_protect_cmd(u8 idx){
    int ret = 0;
    if(FLASH_ID_GD25Q40 == flash_get_id()){
        ret = flash_protect_GD25Q40B(idx);
    }else if(FLASH_ID_MD25D40 == flash_get_id()){
        ret = flash_protect_MD25D40(idx);
    }else if(FLASH_ID_PN25F04C == flash_get_id()){
        ret = flash_protect_PN25F04C(idx);
    }else{
        ret = -1;
    }

    return ret;
}

int flash_protect(u8 idx){
    if(0 == flash_protect_en){
        return -1;//old 0
    }

    for(int i = 0; i < 5; ++i){
		if(0 == flash_protect_cmd(idx)){
			return 0;
		}
	}

	return -1;
}

int flash_protect_disable(void){
    return flash_protect(FLASH_NONE);
}

int flash_protect_up64k(void){
	if(FLASH_ID_PN25F04C == flash_get_id()){
		return flash_protect(FLASH_PN25F04C_0x70000_0x7ffff);
	}

	return -1;
}

int flash_protect_down64k(void){
	if(FLASH_ID_PN25F04C == flash_get_id()){
		return flash_protect(FLASH_PN25F04C_0x00000_0x0ffff);
	}

	return -1;
}

int flash_protect_up128k(void){
	if(FLASH_ID_PN25F04C == flash_get_id()){
		return flash_protect(FLASH_PN25F04C_0x60000_0x7ffff);
	}

	return -1;
}

int flash_protect_down128k(void){
	if(FLASH_ID_PN25F04C == flash_get_id()){
		return flash_protect(FLASH_PN25F04C_0x00000_0x1ffff);
	}

	return -1;
}

int flash_protect_up256k(void){
    if(FLASH_ID_GD25Q40 == flash_get_id())
    {
        return flash_protect(FLASH_GD25Q40_0x40000_0x7ffff);
    }
    else if(FLASH_ID_PN25F04C == flash_get_id())
    {
        return flash_protect(FLASH_PN25F04C_0x40000_0x7ffff);
    }

    return -1;
}

int flash_protect_down256k(void){
    if(FLASH_ID_GD25Q40 == flash_get_id()){
        return flash_protect(FLASH_GD25Q40_0x00000_0x3ffff);
    }
    else if(FLASH_ID_MD25D40 == flash_get_id()){
        return flash_protect(FLASH_MD25D40_0x00000_0x3ffff);
    }
	else if(FLASH_ID_PN25F04C == flash_get_id()){
		return flash_protect(FLASH_PN25F04C_0x00000_0x3ffff);
	}

    return -1;
}


int flash_protect_8267_normal(void){
    if(FLASH_ID_GD25Q40 == flash_get_id()){
    	return flash_protect(FLASH_GD25Q40_0x00000_0x1ffff);
    }else if(FLASH_ID_MD25D40 == flash_get_id()){
        return flash_protect(FLASH_MD25D40_0x00000_0x6ffff);
    }
	else if(FLASH_ID_PN25F04C == flash_get_id()){
		return flash_protect(FLASH_PN25F04C_0x00000_0x1ffff);
	}
    return -1;
}

#endif


