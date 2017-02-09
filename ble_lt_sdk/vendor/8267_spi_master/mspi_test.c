#if (__PROJECT_8267_SPI_MASTER__)

#include "../../proj/tl_common.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj/drivers/flash.h"
#include "mspi_test.h"
#include "../../proj_lib/ble/ble_ll.h"

extern u8 spi_read_buff[64];// to store data from sspi 826x

#define SPP_CMD_FIELD \
u8 signature[2]; \
u8 cmdID; \
u8 len

#define SPP_DATA_FIELD \
u8 signature[1]; \
u8 len

typedef struct {
	SPP_CMD_FIELD;
	u8	data[1];
} spp_cmd_t;

typedef struct {
	SPP_DATA_FIELD;
	u8	data[1];
} spp_dat_t;


void spi_pin_init_8267(enum SPI_PIN gpio_pin_x){
	if(gpio_pin_x == SPI_PIN_GPIOB){
		write_reg8(0x58e,read_reg8(0x58e)&0x0f);///disable GPIO:B<4~7>
		write_reg8(0x5b1,read_reg8(0x5b1)|0xf0);///enable SPI function:B<4~7>

		write_reg8(0x5b0,read_reg8(0x5b0)&0xC3);///disable SPI function:A<2~5>
		gpio_set_input_en(GPIO_PB4,1);
		gpio_set_input_en(GPIO_PB5,1);
		gpio_set_input_en(GPIO_PB6,1);
		gpio_set_input_en(GPIO_PB7,1);
	}
	if(gpio_pin_x == SPI_PIN_GPIOA){
		write_reg8(0x586,read_reg8(0x586)&0xC3);///disable GPIO:A<2~5>
		write_reg8(0x5b0,read_reg8(0x5b0)|0x3C);///enable SPI function:A<2~5>

		write_reg8(0x5b1,read_reg8(0x5b1)&0x0f);///disable SPI function:B<4~7>
		gpio_set_input_en(GPIO_PA2,1);
		gpio_set_input_en(GPIO_PA3,1);
		gpio_set_input_en(GPIO_PA4,1);
		gpio_set_input_en(GPIO_PA5,1);
	}
	write_reg8(0x0a,read_reg8(0x0a)|0x80);////enable spi
}

void spi_master_init_8267(unsigned char div_clock,enum SPI_MODE mode){
	write_reg8(0x0a,read_reg8(0x0a)|div_clock);/////spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)|0x02);////enable master mode

	write_reg8(0x0b,read_reg8(0x0b)|mode);////select SPI mode,surpport four modes
}

void spi_slave_init_8267(unsigned char div_clock,enum SPI_MODE mode){
	write_reg8(0x0a,read_reg8(0x0a)|div_clock);/////spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)&0xfd);////disable master mode

	write_reg8(0x0b,read_reg8(0x0b)|mode);////select SPI mode,surpport four modes
}

#if 1
void spi_write_buff_8267(unsigned short addr ,unsigned char* pbuff,unsigned int len){
	unsigned int i = 0;
	write_reg8(0x09,read_reg8(0x09)&0xfe);////CS level is low
	write_reg8(0x09,read_reg8(0x09)&0xfb);///enable output
	write_reg8(0x09,read_reg8(0x09)&0xf7);///enable write

	/***send addr***/
	write_reg8(0x08,(addr>>8)&0xff);/////high addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	write_reg8(0x08,addr&0xff);/////low addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***send write command:0x00***/
	write_reg8(0x08,0x00);/////0x80:read  0x00:write
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	/***send data***/
	for(i=0;i<len;i++){
		write_reg8(0x08,pbuff[i]);
		while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	}
	/***pull up CS***/
	write_reg8(0x09,read_reg8(0x09)|0x01);///CS level is high
}


void spi_read_buff_8267(unsigned short addr,unsigned char* pbuff,unsigned int len){
	unsigned int i = 0;
	unsigned char temp = 0;

	write_reg8(0x09,read_reg8(0x09)&0xfe);////CS level is low
	write_reg8(0x09,read_reg8(0x09)&0xfb);///enable output

	/***send addr***/
	write_reg8(0x08,(addr>>8)&0xff);/////high addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	write_reg8(0x08,addr&0xff);/////low addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***send read command:0x80***/
	write_reg8(0x08,0x80);/////0x80:read  0x00:write
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***when the read_bit was set 1,you can read 0x800008 to take eight clock cycle***/
	write_reg8(0x09,read_reg8(0x09)|0x08);////set read_bit to 1
	temp = read_reg8(0x08);///first byte isn't useful data,only take 8 clock cycle
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***send one byte data to read data***/
	for(i=0;i<len;i++){
		pbuff[i] = read_reg8(0x08);///take 8 clock cycles
		while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	}
	/////pull up CS
	write_reg8(0x09,read_reg8(0x09)|0x01);///CS level is high
}
#endif


u8 spi_write_data_to_8269spi(u8* data, u8 len);

extern kb_data_t	kb_event;


u8 key_buf[8] = {0};
u8 key_type;
u8 pm_mask_temp;
#define CONSUMER_KEY   0
#define KEYBOARD_KEY   1


u8 sendTerminate_before_enterDeep = 0;

int key_not_released;
int ir_not_released;

u32 latest_user_event_tick;
u8  user_task_ongoing_flg;

//********************for test 16-8-31 by tuyf******************************
extern u8 loadramcode_success_flg;
volatile u32 fw_sieze;
u8 isNewFwSectionDetect(u32 *new_firmware_size){//detect the existence of 8269 firmware in 8267 EVK FLASH
	//u32 flash_erase_size = 0;
	u32 tmp[4];
	u8 checkStatus = 0;
	u8 new_fw_size_valid = 0;
	*new_firmware_size = 0;

	u8 r = irq_disable();

	/*Check if  the New firmware has already been erased, check 16 bytes for every 4k, check 24k by default*/
	for(int i = 0; i < 6; i++) {
		flash_read_page(TEST_FW_ADR + i * 1024 * 4, 16, (u8 *)tmp);

		if((tmp[0] != 0xFFFFFFFF) || (tmp[1] != 0xFFFFFFFF) || (tmp[2] != 0xFFFFFFFF) || (tmp[3] != 0xFFFFFFFF)) {
			checkStatus = 1;
			break;
		}
	}
	if(checkStatus) {
		/*Read from flash to get size of  new firmware(0x18 offset in bin)*/
		flash_read_page(TEST_FW_ADR + 0x18, 4, new_firmware_size);
		/*Check the validation of firmware size,  <24K*/
		fw_sieze = *new_firmware_size;
		new_fw_size_valid = ((*new_firmware_size > 1 * 1024)  && (*new_firmware_size < 24 * 1024));
	}

	irq_restore(r);
	return new_fw_size_valid;
}

void led_init(void){
	gpio_set_func(RED_LED, AS_GPIO);//LED RED EVK 8267
	gpio_set_input_en(RED_LED,0);
	gpio_set_output_en(RED_LED,1);
	gpio_write(RED_LED,1);

	gpio_set_func(GREEN_LED, AS_GPIO);//GREEN RED EVK 8267
	gpio_set_input_en(GREEN_LED,0);
	gpio_set_output_en(GREEN_LED,1);
	gpio_write(GREEN_LED,1);

	gpio_set_func(WHITE_LED, AS_GPIO);//WHITE RED EVK 8267
	gpio_set_input_en(WHITE_LED,0);
	gpio_set_output_en(WHITE_LED,1);
	gpio_write(WHITE_LED,1);

	gpio_set_func(BLUE_LED, AS_GPIO);//BLUE BLUE EVK 8267
	gpio_set_input_en(BLUE_LED,0);
	gpio_set_output_en(BLUE_LED,1);
	gpio_write(BLUE_LED,1);
    sleep_us(500000);
	gpio_write(RED_LED,0);
	gpio_write(GREEN_LED,0);
	gpio_write(WHITE_LED,0);
	gpio_write(BLUE_LED,0);
}

void led_onoff(u32 pin,u8 onoff){
	gpio_write(pin, onoff&&0x01);
}

void spi_read_data_from_8269spi(u8* data, u8 len);

void spi_master_register(int pin){

//register MSPI_RX_NOTIFY_PIN pin's irq, when SSPI's data sent complete,the NOTIFY pin changed HIGH delay hold on 100us, then LOW
//	gpio_set_input_en(pin, 1);
//	gpio_set_interrupt(pin, 1);	// falling edge
// 	reg_irq_mask |= FLD_IRQ_GPIO_RISC2_EN;

	////////////////// interrupt input pin ///////////////////
	gpio_set_func (pin, AS_GPIO);		//gpio
	gpio_set_input_en(pin, 1);
	gpio_set_output_en (pin, 0);		//output disable
	gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_10K);

	//gpio_set_interrupt (pin, 0);
}

void mspi_8267_init(void){
	spi_pin_init_8267(SPI_PIN_GPIOA);
#if spi_master_mode
	spi_master_init_8267(0x25, SPI_MODE0);///clock and enable master mode;select mode 0
	spi_master_register(MSPI_RX_NOTIFY_PIN);//GP6
#else
	spi_slave_init_8267(0x25,SPI_MODE0);
	spi_slave_rev_irq_en();
#endif
}

void mspi_8267_disable(void){
	write_reg8(0x0a,read_reg8(0x0a)&0x07);////disable spi;
}

u8 enable_sspi = 0xa5;//reg value
u8 chk_sspi = 0;//check sspi has been setting
u8 fw_bin_buf[16]={0};
u8 chk_fw_bin_buf[16]={0};
int fw_adr_index = -1;
u16 fw_adr;

u8  left_bin2send;
u8 adr_index = 0;

u8 spi_load_code2sspi8269(void){
	for(u8 i = 0; i < 2; i++){// Note:should execute twice
		spi_write_buff_8267(0x0a, &enable_sspi, 1);
		spi_read_buff_8267(0x0a, &chk_sspi,  1);
	}
    if(chk_sspi == 0xa5){//8269 EVK sspi function enable
    	u32 new_firmware_size;
    	u8 fw_detect_flg;
    	led_onoff(GREEN_LED,ON);//8269_fw_exist
    	fw_detect_flg = isNewFwSectionDetect(&new_firmware_size);

    	if(fw_detect_flg){
    		led_onoff(GREEN_LED,OFF);//8269_fw_exist

    		fw_adr = new_firmware_size >>4;
    		left_bin2send = new_firmware_size % 16;

    		for(int i =0; i< (fw_adr + (left_bin2send ? 1:0)); i++){
    			flash_read_page(TEST_FW_ADR +  i * 16, 16, fw_bin_buf);

    			spi_write_buff_8267(0x8000 +  i * 16, fw_bin_buf, 16);
    			spi_read_buff_8267(0x8000 +  i * 16, chk_fw_bin_buf, 16);

    			if(!memcmp(fw_bin_buf, chk_fw_bin_buf, 16)){//OK
    				fw_adr_index++;
    			}
    			else{
    				break;
    			}

    			if(fw_adr_index  == fw_adr - 1){
    				if(left_bin2send){//not zero
						memset(fw_bin_buf, 0, 16);//clear
						memset(chk_fw_bin_buf, 0, 16);//clear

						flash_read_page(TEST_FW_ADR +  fw_adr * 16, left_bin2send, fw_bin_buf);

						spi_write_buff_8267(0x8000 +  fw_adr * 16, fw_bin_buf, left_bin2send);
						spi_read_buff_8267(0x8000 +  fw_adr * 16, chk_fw_bin_buf, left_bin2send);
						if(!memcmp(fw_bin_buf, chk_fw_bin_buf, left_bin2send)){//OK
							fw_adr_index++;
							break;
						}
						else{
							break;
						}
    				}
    				else{
    					break;
    				}
    			}
    		}

            if(left_bin2send ? (fw_adr_index == fw_adr) : ((fw_adr_index + 1) == fw_adr)){//bin send to 8269 complete
            	u8 restart8269 = 0x88;
            	u8 chk_restart8269;
            	spi_write_buff_8267(0x0602, &restart8269, 1);//restart 8269 to run ram code
            	spi_read_buff_8267(0x0602,  &chk_restart8269, 1);
            	if(0x08 == chk_restart8269){//reg 0x602 write 0x88 to run ram code, the reg 0x602 value changed to 0x08!
            	    return 1;
            	}
            	else
            		return 0;
            }
            else
            	return 0;

    	}
    	else{
    		return 0;
    	}
    }
    else{
    	return 0;
    }
}

// spi write/read will trigger a rising edge on MSP CN's pin,which could cause a irq on Slave SPI,so disable irq in write operation
//and use the read operation to trigger a irq  16-9-8 add by tuyf
u8 chk[64];
u8 spi_write_data_to_8269spi(u8* data, u8 len){

	if(len>64)//Max len:64 Byte
		return 0;

    //8269 SSPI write_buff addr:0xff80, base address:0x800000
	spi_write_buff_8267(0xff80, data, len); //stack reserved for SPI read/write buffer ,reference .asm file on line 130:cstarup_8269.s
	spi_read_buff_8267(0xff80, chk, len);

	if(!memcmp(data, chk, len)){
		return 1;
	}
	return 0;
}

void spi_read_data_from_8269spi(u8* data, u8 len){//data size max:64
	if(len>64)//Max len:64 Byte
		return;

	//data size:64 Bytes, SSPI read_buff addr:0xff80+0x40=0xffc0, base address:0x800000
	spi_read_buff_8267(0xffc0, data, len);

}

u32 btn_pin[] = {GPIO_PD2, GPIO_PC7};
void btn_init(void){
	for (int i=0; i<(sizeof (btn_pin)/sizeof(*btn_pin)); i++)
	{
		gpio_set_wakeup(btn_pin[i],0,1);  	   //drive pin core(gpio) low wakeup suspend
		cpu_set_gpio_wakeup (btn_pin[i],0,1);  //drive pin pad low wakeup deepsleep
		gpio_setup_up_down_resistor(btn_pin[i], 2);//10K up
	}
}

void user_init(){
	led_init();
	mspi_8267_init();//Mastrt SPI A port

//	loadramcode_success_flg    = spi_load_code2sspi8269();    //load 8269 ram code onto 8269 by 8267 Master SPI A port
//	if(loadramcode_success_flg){//close SPI
//		mspi_8267_disable();
//	}

	usb_host_init();//usb host
	btn_init();
}

///////////////////////////////////////////////
void proc_button (void);
u8 tst1;
void proc_button ()
{
	int det_key = 0;
	u8 flg;
	for(u8 i=0; i<2; i++){
		if(!gpio_read(btn_pin[i])){
			WaitMs(40);
			if(!gpio_read(btn_pin[i])){
				det_key |= BIT(i);
			}
		}
	}

	if (det_key){
		if(det_key & BIT(0)){
			det_key = 0;

			u8 data1[] = {"Key1!"};
			u8 data[20]={0xBB, 0xFF, sizeof(data1)};
			memcpy(data + 3, data1, sizeof(data1));
			flg = spi_write_data_to_8269spi(data, sizeof(data1) + 3);
			if(flg){
				tst1++;
				led_onoff(GREEN_LED, ON);
			}
		}
		else if(det_key & BIT(1)){
			det_key = 0;
			u8 data1[] = {"Key2!"};
			u8 data[20]={0xBB, 0xFF, sizeof(data1)};
			memcpy(data + 3, data1, sizeof(data1));
			flg = spi_write_data_to_8269spi(data, sizeof(data1) + 3);
			if(flg){
				tst1++;
				led_onoff(GREEN_LED, OFF);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////

#endif
