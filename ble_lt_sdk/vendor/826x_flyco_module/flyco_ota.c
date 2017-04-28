/*
 * flyco_ota.c
 *
 *  Created on: 2016-10-27
 *      Author: Administrator
 */

#if 1
#include "../../proj/tl_common.h"
#include "../../proj/drivers/flash.h"
#include "../../proj/drivers/uart.h"
#include "../../proj_lib/ble/ble_common.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/ble/blt_config.h"

//FullStack for 8266 OTA settings:
//If using this setting,we should use 8266 fullstack SDK's boot.link and cstartup_8266.S
//files to take place the littlestack SDK's corresponding files,in order to be compatible with
//FLYCO project's OTA function!!!
//#define			NEW_FW_ADR			0x40000
//#define			OTA_FLG_ADR			0x6a000
//#define			OTA_BOOT_BIN_ADR	0x68000

//LittleStack for 8266 OTA settings:
//Follow the telink's ble  architecture for this sdk!
//please take 826x BLE SDK Developer Handbook for reference(page 24).
//         8266 512K flash address setting:
//          0x80000 |~~~~~~~~~~~~~~~~~~|
//                  |  user data area  |
//          0x78000 |~~~~~~~~~~~~~~~~~~|
//                  |  customed value  |
//          0x77000 |~~~~~~~~~~~~~~~~~~|
//                  |    MAC address   |
//          0x76000 |~~~~~~~~~~~~~~~~~~|
//                  |    pair & sec    |
//                  |       info       |
//          0x74000 |~~~~~~~~~~~~~~~~~~|
//                  |   ota_boot_flg   |
//          0x73000 |~~~~~~~~~~~~~~~~~~|
//                  |   ota_boot.bin   |
//          0x72000 |~~~~~~~~~~~~~~~~~~|
//                  |  user data area  |
//                  |                  |
//                  |                  |
//          0x40000 |~~~~~~~~~~~~~~~~~~|
//                  |   OTA new bin    |
//                  |   storage area   |
//                  |                  |
//          0x20000 |~~~~~~~~~~~~~~~~~~|
//                  | old firmwave bin |
//                  |                  |
//                  |                  |
//          0x00000 |~~~~~~~~~~~~~~~~~~|
#define			    NEW_FW_ADR			0x20000
#define			    OTA_FLG_ADR			0x73000
#define			    OTA_BOOT_BIN_ADR	0x72000

//#define CMD_OTA_FW_VERSION		0xff00
//#define CMD_OTA_START				0xff01
//#define CMD_OTA_END				0xff02
#define CMD_OTA_START_REQ           0xff03 //Request OTA start
#define CMD_OTA_START_RSP           0xff04 //Slave allow OTA start
#define CMD_OTA_TEST                0xff05 //Master send OTA TEST to slave to check slave's state
#define CMD_OTA_TEST_RSP            0xff06 //Slave respond to master's CMD_OTA_TEST
#define CMD_OTA_RESULT              0xff07 //Slave send OTA error to master once it stop OTA process because of some error

extern int flyco_blt_ota_start_flag;
extern u32 flyco_blt_ota_start_tick;
extern u32 flyco_blt_ota_timeout_us;
extern ota_resIndicateCb_t flyco_otaResIndicateCb;
extern u8 ota_hdl;

enum{
//	OTA_SUCCESS = 0,          	   //success
//	OTA_PACKET_LOSS,          	   //lost one or more OTA PDU
//	OTA_DATA_CRC_ERR,         	   //data CRC err
//	OTA_WRITE_FLASH_ERR,           //write OTA data to flash ERR
// 	OTA_DATA_UNCOMPLETE,           //lost last one or more OTA PDU
// 	OTA_TIMEOUT, 		           //Slave OTA Timeout, OTA cost too long time for slave
	OTA_WRITE_OTA_FLAG_FLASH_ERR = 6, //Write OTA finish flag to flash error
 	OTA_PROCESS_DISCONNECTION = 7,    //Connection disconnected during OTA process
 	OTA_BOOT_BIN_NOT_EXIST    = 8,    //OTA Boot bin doesn't exist in addr 0x68000
 	OTA_ERROR_CODE_MAX,
};

void bls_ota_procTimeout(void);
void flyco_ble_setOtaStartCb(ota_startCb_t cb);
void flyco_ble_setOtaVersionCb(ota_versionCb_t cb);
void flyco_ble_setOtaResIndicateCb(ota_resIndicateCb_t cb);
void flyco_ble_OtaEraseNewFwSection(void);
void flyco_ble_setOtaTimeout(u32 timeout_us);
int flyco_otaWrite(void * p);
int flyco_otaRead(void * p);
void flyco_start_reboot(void);

int flyco_ota_adr_index = -1;

int flyco_blt_ota_start_flag;
u32 flyco_blt_ota_start_tick;
u32 flyco_blt_ota_timeout_us = 25000000;  //default 25 second


ota_startCb_t		flyco_otaStartCb       = NULL;
ota_versionCb_t 	flyco_otaVersionCb     = NULL;
ota_resIndicateCb_t flyco_otaResIndicateCb = NULL;

extern u32 ota_program_offset;

void flyco_ble_setOtaStartCb(ota_startCb_t cb)
{
	flyco_otaStartCb = cb;
}

void flyco_ble_setOtaVersionCb(ota_versionCb_t cb)
{
	flyco_otaVersionCb = cb;
}

void flyco_ble_setOtaResIndicateCb(ota_resIndicateCb_t cb)
{
	flyco_otaResIndicateCb = cb;
}

void flyco_ble_setOtaTimeout(u32 timeout_us)
{
	flyco_blt_ota_timeout_us = timeout_us;
}

void flyco_start_reboot(void)
{
	irq_disable ();
	cpu_sleep_wakeup (1, PM_WAKEUP_TIMER, clock_time() + 5*CLOCK_SYS_CLOCK_1MS);//cpu_reboot();
}


void flyco_ota_set_flag()
{
	flash_erase_sector (OTA_FLG_ADR);
	u32 flag = 0xa5;
	flash_write_page (OTA_FLG_ADR, 4, (u8*)&flag);
}

void flyco_ota_save_data(u32 adr, u8 * data){
	flash_write_page(NEW_FW_ADR + adr, 16, data);
}

extern u8 ui_ota_is_working;
void flyco_send_ota_result(int errorcode){
	if(errorcode){//error
		ui_ota_is_working = 0;

		//OTA fail,send error code to the UART,notice MCU!
		u8 ota_error[12] = {0,0,0,0,0x46,0x4c,0x59,0x43,0x4f,0x1e,0x00,0x1e};
		uart_data_t* pp = (uart_data_t*)ota_error;
		memcpy(pp->data, ota_error+4, sizeof(ota_error)-4);
		pp->len = 8;
		uart_Send_kma((u8 *)pp);
	}
	u8 result[4] = {
			U16_LO(CMD_OTA_RESULT),U16_HI(CMD_OTA_RESULT),
			U16_LO(errorcode),U16_HI(errorcode)};

	bls_att_pushNotifyData(ota_hdl, result, sizeof(result));
}

void fly_send_ota_test_rsp(void){
	u8 test_rsp[2] = { U16_LO(CMD_OTA_TEST_RSP), U16_HI(CMD_OTA_TEST_RSP)};
	bls_att_pushNotifyData(ota_hdl, test_rsp, sizeof(test_rsp));
}

void fly_send_ota_start_req_rsp(void){
	u8 start_req_rsp[2] = { U16_LO(CMD_OTA_START_RSP), U16_HI(CMD_OTA_START_RSP)};
	bls_att_pushNotifyData(ota_hdl, start_req_rsp, sizeof(start_req_rsp));
}

u8 flyco_ota_check_boot_validation(void) {
	/*For every bin, code in baseaddr+0x08 should equal to 0x544c4e4b*/
	u32 rightCode = 0x544c4e4b;
	u32 checkCode = 0;
	u32 codeAddr = OTA_BOOT_BIN_ADR + 0x08;
	flash_read_page(codeAddr, 4, (u8 *)&checkCode);
	if(checkCode != rightCode) {
		return OTA_BOOT_BIN_NOT_EXIST;
	}
	else {
		return BLE_SUCCESS;
	}
}

int flyco_otaWrite(void * p)
{
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;

	u8 err_flg = OTA_SUCCESS;

	u16 ota_adr =  req->dat[0] | (req->dat[1]<<8);
	u16 crc;
	if(ota_adr == CMD_OTA_FW_VERSION){
		//to be add
		if(flyco_otaVersionCb){
			flyco_otaVersionCb();
		}
	}
	else if(ota_adr == CMD_OTA_START){
		if(flyco_ota_check_boot_validation() == OTA_SUCCESS){
			flyco_blt_ota_start_flag = 1;   //set flag
			flyco_blt_ota_start_tick = clock_time();  //mark time
			flyco_ota_adr_index = -1;
			if(flyco_otaStartCb){
				flyco_otaStartCb();
			}
		}
		else{
			err_flg = OTA_BOOT_BIN_NOT_EXIST;
		}
	}
	else if(ota_adr == CMD_OTA_TEST) {//Master send CMD_OTA_TEST, need us to respond CMD_OTA_TEST_RSP

		fly_send_ota_test_rsp();//Send CMD_OTA_TEST_RSP
	}
	else if(ota_adr == CMD_OTA_START_REQ) {//Master request to start OTA process
		if(flyco_ota_check_boot_validation() == OTA_SUCCESS) {//Start OTA only if 8266_ota_boot.bin has been burned in 0x68000*/
			fly_send_ota_start_req_rsp();
		}
	}
	else if(ota_adr == CMD_OTA_END){

		u16 adrIndex_max	   = req->dat[2] | (req->dat[3]<<8);
		u16 adrIndex_max_check = req->dat[4] | (req->dat[5]<<8);

		if(adrIndex_max ^ adrIndex_max_check == 0xFFFF){

			//if no index_max check, set ota success directly, otherwise we check if any index_max match
			u32 firmware_size = 0;
			u16 addr_index_calc_from_firmware_size = 0;

			flash_read_page(NEW_FW_ADR + 0x18, 4, (u8 *)&firmware_size);

			/*Check firmware validation, >1k, <100k*/
			if((firmware_size == 0xFFFFFFFF) || (firmware_size < 1024) || (firmware_size > 100 * 1024)) {
				err_flg = OTA_DATA_UNCOMPLETE;
			}
			else {
				// bin size 1~16, index=0; but 16>>4=1, so we need to use  ((firmware_size+15)  >> 4)
				addr_index_calc_from_firmware_size = (((firmware_size+15)  >> 4)-1) & 0xFFFF;
				if(addr_index_calc_from_firmware_size != flyco_ota_adr_index) {//Check BIN integrity
					err_flg = OTA_DATA_UNCOMPLETE;
				}
				else {
					err_flg = OTA_SUCCESS;
				}
			}
		}
		else{
			err_flg = OTA_DATA_UNCOMPLETE;
		}

		if(!err_flg){//OTA SUCCESS

			if(flyco_otaResIndicateCb){
				flyco_otaResIndicateCb(OTA_SUCCESS);  //OTA successed indicate
			}

			sleep_us(60000);//wait for ota fail result sent
			flyco_ota_set_flag ();
			flyco_start_reboot();
		}

	}
	else{
		if(flyco_ota_adr_index + 1 == ota_adr){   //correct OTA data index
			crc = (req->dat[19]<<8) | req->dat[18];
			u8 flash_check[16];
			if(crc == crc16(req->dat, 18)){
				flash_write_page(NEW_FW_ADR + (ota_adr<<4), 16, req->dat + 2);
				flash_read_page(NEW_FW_ADR + (ota_adr<<4),16,flash_check);

				if(!memcmp(flash_check,req->dat + 2,16)){  //OK
					flyco_ota_adr_index = ota_adr;
				}
				else{
					//twice write flash
					flash_write_page(NEW_FW_ADR + (ota_adr<<4), 16, req->dat + 2);
					flash_read_page(NEW_FW_ADR + (ota_adr<<4),16,flash_check);
					if(!memcmp(flash_check,req->dat + 2,16)){  //OK
						flyco_ota_adr_index = ota_adr;
					}
					else{
						//flash write err
						err_flg = OTA_WRITE_FLASH_ERR;
					}
				}
			}
			else{  //crc err
				err_flg = OTA_DATA_CRC_ERR;
			}
		}
		else if(flyco_ota_adr_index >= ota_adr){  //maybe repeated OTA data, we neglect it, do not consider it ERR

		}
		else{  //adr index err, missing at least one OTA data
			err_flg = OTA_PACKET_LOSS;
		}

	}

	if(err_flg){//OTA FAILURE
		if(flyco_otaResIndicateCb){
			flyco_otaResIndicateCb(err_flg);   //OTA fail indicate
		}
		sleep_us(60000);//wait for ota fail result sent
		if(flyco_ota_adr_index>=0){
			irq_disable();
			for(int i=0;i<=flyco_ota_adr_index;i+=256){  //4K/16 = 256
				flash_erase_sector(NEW_FW_ADR + (i<<4));
			}
		}
		flyco_start_reboot();
	}

	return 0;
}

int flyco_otaRead(void * p)
{
	return 0;
}

void flyco_ble_OtaEraseNewFwSection(void) {
	u32 new_firmware_size = 0;
	u32 current_firmware_size = 0;
	u32 flash_erase_size = 0;
	u32 tmp[4];
	u8 checkStatus = 0;

	u8 r = irq_disable();

	/*Check if  the New firmware has already been erased, check 16 bytes for every 4k, check 80k by default*/
	for(int i = 0; i < 	20; i++) {
		flash_read_page(NEW_FW_ADR + i * 1024 * 4, 16, (u8 *)tmp);

		if((tmp[0] != 0xFFFFFFFF) || (tmp[1] != 0xFFFFFFFF) || (tmp[2] != 0xFFFFFFFF) || (tmp[3] != 0xFFFFFFFF)) {
			checkStatus = 1;
			break;
		}
	}


	if(checkStatus) {
		/*Read from flash to get size of  new firmware(0x18 offset in bin)*/
		flash_read_page(NEW_FW_ADR + 0x18, 4, &new_firmware_size);

		/*Read from flash to get size of  current firmware(0x18 offset in bin)*/
		flash_read_page(0x18, 4, &current_firmware_size);

		/*Check the validation of firmware size, between 1k ~ 80k*/
		u8 new_fw_size_valid = ((new_firmware_size > 1 * 1024)  && (new_firmware_size < 80 * 1024));
		u8 current_fw_size_valid = ((current_firmware_size > 1 * 1024)  && (current_firmware_size < 80 * 1024));

		/*Case1: Both new firmware & current firmware valid*/
		if(new_fw_size_valid && current_fw_size_valid) {
			/*If differrence between new and current fw size is smaller than 20k, think this new firmware size is valid*/
			if(((new_firmware_size - current_firmware_size) < 20 * 1024) || ((current_firmware_size  - new_firmware_size) < 20 * 1024)) {
				flash_erase_size = new_firmware_size + 16 * 1024;
			}
			else{
				flash_erase_size = current_firmware_size + 16 * 1024; //current size + 16k
			}

		}
		/*Case2: Only current firmware size is valid*/
		else if(current_fw_size_valid) {
			flash_erase_size = current_firmware_size + 16 * 1024; //current size + 16k

		}

		/*Case3: Both new firmware & current firmware not valid*/
		else {
			flash_erase_size = 80 * 1024; //80k
		}


		int i = 0;
		int j = 0;

		//should smaller than 80k
		if(flash_erase_size > 80 * 1024){
			flash_erase_size = 80 * 1024;//Max firmware size: 80k
		}

		for(i = 0; i < flash_erase_size; i += 1024 * 4) {
#if(MODULE_WATCHDOG_ENABLE)
			wd_clear();
#endif
			flash_erase_sector(NEW_FW_ADR + i);
		}


		/*Check the section after is all erased, MAX 80k*/

		for(j = i; j < 80 * 1024; j += (1024 * 4)) {
			flash_read_page(NEW_FW_ADR + j, 16, (u8 *)tmp);

			if((tmp[0] != 0xFFFFFFFF) || (tmp[1] != 0xFFFFFFFF) || (tmp[2] != 0xFFFFFFFF) || (tmp[3] != 0xFFFFFFFF)) {
#if(MODULE_WATCHDOG_ENABLE)
				wd_clear();
#endif
				flash_erase_sector(NEW_FW_ADR + j);
			}
			else {
				break;
			}
		}

	}

	irq_restore(r);

}

#endif
