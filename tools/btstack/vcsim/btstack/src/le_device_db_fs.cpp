/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#if 0
 
#include "ble/le_device_db.h"

#include "ble/core.h"

#include <stdio.h>
#include <string.h>
#include "btstack_debug.h"

// Central Device db implemenation using static memory
typedef struct le_device_memory_db {

    // Identification
    int addr_type;
    bd_addr_t addr;
    sm_key_t irk;

    // Stored pairing information allows to re-establish an enncrypted connection
    // with a peripheral that doesn't have any persistent memory
    sm_key_t ltk;
    uint16_t ediv;
    uint8_t  rand[8];
    uint8_t  key_size;
    uint8_t  authenticated;
    uint8_t  authorized;

    // Signed Writes by remote
    sm_key_t remote_csrk;
    uint32_t remote_counter;

    // Signed Writes by us
    sm_key_t local_csrk;
    uint32_t local_counter;

} le_device_memory_db_t;

#define LE_DEVICE_MEMORY_SIZE 20
#define INVALID_ENTRY_ADDR_TYPE 0xff
#define DB_PATH_TEMPLATE "/tmp/btstack_at_%s_le_device_db.txt"
const  char * csv_header = "# addr_type, addr, irk, ltk, ediv, rand[8], key_size, authenticated, authorized, remote_csrk, remote_counter, local_csrk, local_counter";
static char db_path[sizeof(DB_PATH_TEMPLATE) - 2 + 17 + 1];

static le_device_memory_db_t le_devices[LE_DEVICE_MEMORY_SIZE];

static char bd_addr_to_dash_str_buffer[6*3];  // 12-45-78-01-34-67\0
static char * bd_addr_to_dash_str(bd_addr_t addr){
    char * p = bd_addr_to_dash_str_buffer;
    int i;
    for (i = 0; i < 6 ; i++) {
        *p++ = char_for_nibble((addr[i] >> 4) & 0x0F);
        *p++ = char_for_nibble((addr[i] >> 0) & 0x0F);
        *p++ = '-';
    }
    *--p = 0;
    return (char *) bd_addr_to_dash_str_buffer;
}

static inline void write_delimiter(FILE * wFile){
    fwrite(", ", 1, 1, wFile);
}
static inline void write_hex_byte(FILE * wFile, uint8_t value){
    char buffer[2];
    buffer[0] = char_for_nibble(value >>   4);
    buffer[1] = char_for_nibble(value & 0x0f);
    fwrite(buffer, 2, 1, wFile);
}

static inline void write_str(FILE * wFile, const char * str){
    fwrite(str, strlen(str), 1, wFile);
    write_delimiter(wFile);
}
static void write_hex(FILE * wFile, const uint8_t * value, int len){
    int i;
    for (i = 0; i < len; i++){
        write_hex_byte(wFile, value[i]);
    }
    write_delimiter(wFile);
}
static void write_value(FILE * wFile, uint32_t value, int len){
    switch (len){
        case 4:
            write_hex_byte(wFile, value >> 24);
        case 3:
            write_hex_byte(wFile, value >> 16);
        case 2:
            write_hex_byte(wFile, value >> 8);
        case 1:
        default:
            write_hex_byte(wFile, value);
    }
    write_delimiter(wFile);
}

static void le_device_db_store(void) {
    int i;
    // open file
    FILE * wFile = fopen(db_path,"w+");
    if (wFile == NULL) return;
    fwrite(csv_header, strlen(csv_header), 1, wFile);
    fwrite("\n", 1, 1, wFile);
    for (i=0;i<LE_DEVICE_MEMORY_SIZE;i++){
        if (le_devices[i].addr_type == INVALID_ENTRY_ADDR_TYPE) continue;
        write_value(wFile, le_devices[i].addr_type, 1);
        write_str(wFile,   bd_addr_to_str(le_devices[i].addr));
        write_hex(wFile,   le_devices[i].irk, 16);
        write_hex(wFile,   le_devices[i].ltk, 16);
        write_value(wFile, le_devices[i].ediv, 2);
        write_hex(wFile,   le_devices[i].rand, 8);
        write_value(wFile, le_devices[i].key_size, 1);
        write_value(wFile, le_devices[i].authenticated, 1);
        write_value(wFile, le_devices[i].authorized, 1);
        write_hex(wFile,   le_devices[i].remote_csrk, 16);
        write_value(wFile, le_devices[i].remote_counter, 2);
        write_hex(wFile,   le_devices[i].local_csrk, 16);
        write_value(wFile, le_devices[i].local_counter, 2);
        fwrite("\n", 1, 1, wFile);
    }
    fclose(wFile);
}
static void read_delimiter(FILE * wFile){
    fgetc(wFile);
}

static uint8_t read_hex_byte(FILE * wFile){
    int c = fgetc(wFile);
    if (c == ':') {
        c = fgetc(wFile);
    }
    int d = fgetc(wFile);
    return nibble_for_char(c) << 4 | nibble_for_char(d);
}

static void read_hex(FILE * wFile, uint8_t * buffer, int len){
    int i;
    for (i=0;i<len;i++){
        buffer[i] = read_hex_byte(wFile);
    }
    read_delimiter(wFile);
}

static uint32_t read_value(FILE * wFile, int len){
    uint32_t res = 0;
    int i;
    for (i=0;i<len;i++){
        res = res << 8 | read_hex_byte(wFile);
    }
    read_delimiter(wFile);
    return res;
}

static void le_device_db_read(void){
    // open file
    FILE * wFile = fopen(db_path,"r");
    if (wFile == NULL) return;
    // skip header
    while (1) {
        int c = fgetc(wFile);
        if (feof(wFile)) goto exit;
        if (c == '\n') break;
    }
    // read entries
    int i;
    for (i=0;i<LE_DEVICE_MEMORY_SIZE && !feof(wFile);i++){
        le_devices[i].addr_type = read_value(wFile, 1);
        read_hex(wFile,   le_devices[i].addr, 6);
        read_hex(wFile,   le_devices[i].irk, 16);
        read_hex(wFile,   le_devices[i].ltk, 16);
        le_devices[i].ediv = read_value(wFile, 2);
        read_hex(wFile,   le_devices[i].rand, 8);
        le_devices[i].key_size      = read_value(wFile, 1);
        le_devices[i].authenticated = read_value(wFile, 1);
        le_devices[i].authorized    = read_value(wFile, 1);
        read_hex(wFile,   le_devices[i].remote_csrk, 16);
        le_devices[i].remote_counter = read_value(wFile, 2);
        read_hex(wFile,   le_devices[i].local_csrk, 16);
        le_devices[i].local_counter = read_value(wFile, 2);
        // read newling
        fgetc(wFile);
    }
exit:
    fclose(wFile);
}

void le_device_db_init(void){
    int i;
    for (i=0;i<LE_DEVICE_MEMORY_SIZE;i++){
        le_devices[i].addr_type = INVALID_ENTRY_ADDR_TYPE;
    }
    sprintf(db_path, DB_PATH_TEMPLATE, "00-00-00-00-00-00");
}

void le_device_db_set_local_bd_addr(bd_addr_t addr){
    sprintf(db_path, DB_PATH_TEMPLATE, bd_addr_to_dash_str(addr));
    log_info("le_device_db_fs: path %s", db_path);
    le_device_db_read();
    le_device_db_dump();
}

// @returns number of device in db
int le_device_db_count(void){
    int i;
    int counter = 0;
    for (i=0;i<LE_DEVICE_MEMORY_SIZE;i++){
        if (le_devices[i].addr_type != INVALID_ENTRY_ADDR_TYPE) counter++;
    }
    return counter;
}

// free device
void le_device_db_remove(int index){
    le_devices[index].addr_type = INVALID_ENTRY_ADDR_TYPE;
    le_device_db_store();
}

int le_device_db_add(int addr_type, bd_addr_t addr, sm_key_t irk){
    int i;
    int index = -1;
    for (i=0;i<LE_DEVICE_MEMORY_SIZE;i++){
         if (le_devices[i].addr_type == INVALID_ENTRY_ADDR_TYPE){
            index = i;
            break;
         }
    }

    if (index < 0) return -1;

    log_info("Central Device DB adding type %u - %s", addr_type, bd_addr_to_str(addr));
    log_info_key("irk", irk);

    le_devices[index].addr_type = addr_type;
    memcpy(le_devices[index].addr, addr, 6);
    memcpy(le_devices[index].irk, irk, 16);
    le_devices[index].remote_counter = 0; 

    le_device_db_store();

    return index;
}


// get device information: addr type and address
void le_device_db_info(int index, int * addr_type, bd_addr_t addr, sm_key_t irk){
    if (addr_type) *addr_type = le_devices[index].addr_type;
    if (addr) memcpy(addr, le_devices[index].addr, 6);
    if (irk) memcpy(irk, le_devices[index].irk, 16);
}

void le_device_db_encryption_set(int index, uint16_t ediv, uint8_t rand[8], sm_key_t ltk, int key_size, int authenticated, int authorized){
    log_info("Central Device DB set encryption for %u, ediv x%04x, key size %u, authenticated %u, authorized %u",
        index, ediv, key_size, authenticated, authorized);
    le_device_memory_db_t * device = &le_devices[index];
    device->ediv = ediv;
    if (rand) memcpy(device->rand, rand, 8);
    if (ltk) memcpy(device->ltk, ltk, 16);
    device->key_size = key_size;
    device->authenticated = authenticated;
    device->authorized = authorized;

    le_device_db_store();
}

void le_device_db_encryption_get(int index, uint16_t * ediv, uint8_t rand[8], sm_key_t ltk, int * key_size, int * authenticated, int * authorized){
    le_device_memory_db_t * device = &le_devices[index];
    log_info("Central Device DB encryption for %u, ediv x%04x, keysize %u, authenticated %u, authorized %u",
        index, device->ediv, device->key_size, device->authenticated, device->authorized);
    if (ediv) *ediv = device->ediv;
    if (rand) memcpy(rand, device->rand, 8);
    if (ltk)  memcpy(ltk, device->ltk, 16);    
    if (key_size) *key_size = device->key_size;
    if (authenticated) *authenticated = device->authenticated;
    if (authorized) *authorized = device->authorized;
}

// get signature key
void le_device_db_remote_csrk_get(int index, sm_key_t csrk){
    if (index < 0 || index >= LE_DEVICE_MEMORY_SIZE){
        log_error("le_device_db_remote_csrk_get called with invalid index %d", index);
        return;
    }
    if (csrk) memcpy(csrk, le_devices[index].remote_csrk, 16);
}

void le_device_db_remote_csrk_set(int index, sm_key_t csrk){
    if (index < 0 || index >= LE_DEVICE_MEMORY_SIZE){
        log_error("le_device_db_remote_csrk_set called with invalid index %d", index);
        return;
    }
    if (csrk) memcpy(le_devices[index].remote_csrk, csrk, 16);

    le_device_db_store();
}

void le_device_db_local_csrk_get(int index, sm_key_t csrk){
    if (index < 0 || index >= LE_DEVICE_MEMORY_SIZE){
        log_error("le_device_db_local_csrk_get called with invalid index %d", index);
        return;
    }
    if (csrk) memcpy(csrk, le_devices[index].local_csrk, 16);
}

void le_device_db_local_csrk_set(int index, sm_key_t csrk){
    if (index < 0 || index >= LE_DEVICE_MEMORY_SIZE){
        log_error("le_device_db_local_csrk_set called with invalid index %d", index);
        return;
    }
    if (csrk) memcpy(le_devices[index].local_csrk, csrk, 16);

    le_device_db_store();
}

// query last used/seen signing counter
uint32_t le_device_db_remote_counter_get(int index){
    return le_devices[index].remote_counter;
}

// update signing counter
void le_device_db_remote_counter_set(int index, uint32_t counter){
    le_devices[index].remote_counter = counter;

    le_device_db_store();
}

// query last used/seen signing counter
uint32_t le_device_db_local_counter_get(int index){
    return le_devices[index].local_counter;
}

// update signing counter
void le_device_db_local_counter_set(int index, uint32_t counter){
    le_devices[index].local_counter = counter;

    le_device_db_store();
}

void le_device_db_dump(void){
    log_info("Central Device DB dump, devices: %d", le_device_db_count());
    int i;
    for (i=0;i<LE_DEVICE_MEMORY_SIZE;i++){
        if (le_devices[i].addr_type == INVALID_ENTRY_ADDR_TYPE) continue;
        log_info("%u: %u %s", i, le_devices[i].addr_type, bd_addr_to_str(le_devices[i].addr));
        log_info_key("ltk", le_devices[i].ltk);
        log_info_key("irk", le_devices[i].irk);
        log_info_key("local csrk", le_devices[i].local_csrk);
        log_info_key("remote csrk", le_devices[i].remote_csrk);
    }
}

#endif