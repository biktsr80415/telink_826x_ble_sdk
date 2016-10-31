/*
 * flyco_ota.c
 *
 *  Created on: 2016-10-27
 *      Author: Administrator
 */

/*
 * ble_ll_ota.c
 *
 *  Created on: 2015-7-20
 *      Author: Administrator
 */


#if 1
#include "../../proj/tl_common.h"
#include "../../proj/drivers/flash.h"

#include "../../proj_lib/ble/ble_common.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"

int flyco_ota_adr_index = -1;

int flyco_blt_ota_start_flag;
u32 flyco_blt_ota_start_tick;
u32 flyco_blt_ota_timeout_us = 15000000;  //default 15 second


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
	cpu_sleep_wakeup (1, PM_WAKEUP_TIMER, clock_time() + 5*CLOCK_SYS_CLOCK_1MS);
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

void flyco_send_ota_result(int errorcode){
	u8 result[4] = {
			U16_LO(CMD_OTA_RESULT),U16_HI(CMD_OTA_RESULT),
			U16_LO(errorcode),U16_HI(errorcode)};

	bls_att_pushIndicateData(ota_hdl, result, sizeof(result));
}

void fly_send_ota_test_rsp(void){
	u8 test_rsp[2] = { U16_LO(CMD_OTA_TEST_RSP), U16_HI(CMD_OTA_TEST_RSP)};
	bls_att_pushIndicateData(ota_hdl, test_rsp, sizeof(test_rsp));
}

void fly_send_ota_start_req_rsp(void){
	u8 start_req_rsp[2] = { U16_LO(CMD_OTA_START_RSP), U16_HI(CMD_OTA_START_RSP)};
	bls_att_pushIndicateData(ota_hdl, start_req_rsp, sizeof(start_req_rsp));
}

u8 flyco_ota_check_boot_validation(void) {
	/*For every bin, code in baseaddr+0x08 should equal to 0x544c4e4b*/
	u32 rightCode = 0x544c4e4b;
	u32 checkCode = 0;
	u32 codeAddr = OTA_BOOT_BIN_ADR + 0x08;
	flash_read_page(codeAddr, 4, (u8 *)&checkCode);
	if(checkCode != rightCode) {
		gpio_write(BLE_STA_OUT, 1);
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


		//if no index_max check, set ota success directly, otherwise we check if any index_max match
		if( req->l2cap == 9 && (adrIndex_max ^ adrIndex_max_check) == 0xffff){  //index_max valid, we can check
			if(adrIndex_max != flyco_ota_adr_index){  //last one or more packets missed
				err_flg = OTA_DATA_UNCOMPLETE;
			}
		}
		else{//To be compatible with new OTA process, once receive CMD_OTA_END, means OTA process successfully
			u32 firmware_size = 0;
			u16 addr_index_calc_from_firmware_size = 0;

			flash_read_page(NEW_FW_ADR + 0x18, 4, (u8 *)&firmware_size);

			/*Check firmware validation, >1k, <300k*/
			if((firmware_size == 0xFFFFFFFF) || (firmware_size < 1024) || (firmware_size > 300 * 1024)) {
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

		if(!err_flg){

			if(flyco_otaResIndicateCb){
				flyco_otaResIndicateCb(OTA_SUCCESS);  //OTA successed indicate
			}

			flyco_ota_set_flag ();
			flyco_start_reboot();
		}

	}
	else{
		if(flyco_ota_adr_index + 1 == ota_adr){   //correct OTA data index
			crc = (req->dat[19]<<8) | req->dat[18];
			if(crc == crc16(req->dat, 18)){
				ota_save_data (ota_adr<<4, req->dat + 2);

				u8 flash_check[16];

				flash_read_page(NEW_FW_ADR + (ota_adr<<4),16,flash_check);

				if(!memcmp(flash_check,req->dat + 2,16)){  //OK
					flyco_ota_adr_index = ota_adr;
				}
				else{ //flash write err
					err_flg = OTA_WRITE_FLASH_ERR;
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

	if(err_flg){

		if(flyco_otaResIndicateCb){
			flyco_otaResIndicateCb(err_flg);   //OTA fail indicate
		}

		if(flyco_ota_adr_index>=0){
			irq_disable();
			for(int i=0;i<=flyco_ota_adr_index;i+=256){  //4K/16 = 256
				flash_erase_sector(NEW_FW_ADR + (i<<4));
			}
			irq_enable();
		}
		//flyco_start_reboot();
	}

	return 0;
}

int flyco_otaRead(void * p)
{
	return 0;
}
#endif
