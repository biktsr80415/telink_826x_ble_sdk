#include "driver_config.h"
#include "register.h"

// use static inline, because, spi flash code must reside in memory..
// these code may be embedd in flash code


#if  0


 static inline void mspi_wait(void){
	while(reg_master_spi_ctrl & FLD_MASTER_SPI_BUSY)
		;
}

 static inline void mspi_high(void){
	reg_master_spi_ctrl = FLD_MASTER_SPI_CS;
}

 static inline void mspi_low(void){
	reg_master_spi_ctrl = 0;
}

 static inline unsigned char mspi_get(void){
	return reg_master_spi_data;
}

 static inline void mspi_write(unsigned char c){
	reg_master_spi_data = c;
}

 static inline void mspi_ctrl_write(unsigned char c){
	reg_master_spi_ctrl = c;
}

 static inline unsigned char mspi_read(void){
	mspi_write(0);		// dummy, issue clock
	mspi_wait();
	return mspi_get();
}



#else

_attribute_ram_code_ static inline void mspi_wait(void){
	while(reg_master_spi_ctrl & FLD_MASTER_SPI_BUSY)
		;
}

_attribute_ram_code_ static inline void mspi_high(void){
	reg_master_spi_ctrl = FLD_MASTER_SPI_CS;
}

_attribute_ram_code_ static inline void mspi_low(void){
	reg_master_spi_ctrl = 0;
}

_attribute_ram_code_ static inline unsigned char mspi_get(void){
	return reg_master_spi_data;
}

_attribute_ram_code_ static inline void mspi_write(unsigned char c){
	reg_master_spi_data = c;
}

_attribute_ram_code_ static inline void mspi_ctrl_write(unsigned char c){
	reg_master_spi_ctrl = c;
}

_attribute_ram_code_ static inline unsigned char mspi_read(void){
	mspi_write(0);		// dummy, issue clock
	mspi_wait();
	return mspi_get();
}

#endif


