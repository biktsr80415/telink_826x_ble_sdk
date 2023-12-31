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

#if 1

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>

#include "btstack_link_key_db_fs.h"
#include "btstack_debug.h"

#include "btstack_util.h"

#define LINK_KEY_PATH "/tmp/"
#define LINK_KEY_PREFIX "btstack_at_"
#define LINK_KEY_FOR "_link_key_for_"
#define LINK_KEY_SUFFIX ".txt"
#define LINK_KEY_STRING_LEN 17

static bd_addr_t local_addr;
// note: sizeof for string literals works at compile time while strlen only works with some optimizations turned on. sizeof inlcudes the  \0
static char keypath[sizeof(LINK_KEY_PATH) + sizeof(LINK_KEY_PREFIX) + LINK_KEY_STRING_LEN + sizeof(LINK_KEY_FOR) + LINK_KEY_STRING_LEN + sizeof(LINK_KEY_SUFFIX) + 1];

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

static char link_key_to_str_buffer[LINK_KEY_STR_LEN+1];  // 11223344556677889900112233445566\0
char *link_key_to_str(link_key_t link_key){
    char * p = link_key_to_str_buffer;
    int i;
    for (i = 0; i < LINK_KEY_LEN ; i++) {
        *p++ = char_for_nibble((link_key[i] >> 4) & 0x0F);
        *p++ = char_for_nibble((link_key[i] >> 0) & 0x0F);
    }
    *p = 0;
    return (char *) link_key_to_str_buffer;
}

static char link_key_type_to_str_buffer[2];
char *link_key_type_to_str(link_key_type_t link_key){
    //snprintf(link_key_type_to_str_buffer, sizeof(link_key_type_to_str_buffer), "%d", link_key);
	sprintf(link_key_type_to_str_buffer, "%d", link_key);
    return (char *) link_key_type_to_str_buffer;
}

int sscanf_link_key(char * addr_string, link_key_t link_key){
    unsigned int buffer[LINK_KEY_LEN];

    // reset result buffer
    memset(&buffer, 0, sizeof(buffer));

    // parse
    int result = sscanf( (char *) addr_string, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
                                    &buffer[0], &buffer[1], &buffer[2], &buffer[3],
                                    &buffer[4], &buffer[5], &buffer[6], &buffer[7],
                                    &buffer[8], &buffer[9], &buffer[10], &buffer[11],
                                    &buffer[12], &buffer[13], &buffer[14], &buffer[15] );

    if (result != LINK_KEY_LEN) return 0;

    // store
    int i;
    uint8_t *p = (uint8_t *) link_key;
    for (i=0; i<LINK_KEY_LEN; i++ ) {
        *p++ = (uint8_t) buffer[i];
    }
    return 1;
}

static void set_path(bd_addr_t bd_addr){
    strcpy(keypath, LINK_KEY_PATH);
    strcat(keypath, LINK_KEY_PREFIX);
    strcat(keypath, bd_addr_to_dash_str(local_addr));
    strcat(keypath, LINK_KEY_FOR);
    strcat(keypath, bd_addr_to_dash_str(bd_addr));
    strcat(keypath, LINK_KEY_SUFFIX);
}

// Device info
static void db_open(void){
}

static void db_set_local_bd_addr(bd_addr_t bd_addr){
    memcpy(local_addr, bd_addr, 6);
}

static void db_close(void){ 
}

static void put_link_key(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t link_key_type){
    set_path(bd_addr);
    char * link_key_str = link_key_to_str(link_key);
    char * link_key_type_str = link_key_type_to_str(link_key_type); 

    FILE * wFile = fopen(keypath,"w+");
    fwrite(link_key_str, strlen(link_key_str), 1, wFile);
    fwrite(link_key_type_str, strlen(link_key_type_str), 1, wFile);
    fclose(wFile);
}

static int get_link_key(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t * link_key_type) {

#if 0
    set_path(bd_addr);
    //if (access(keypath, R_OK)) return 0;
    
    char link_key_str[LINK_KEY_STR_LEN + 1];
    char link_key_type_str[2];

    FILE * rFile = fopen(keypath,"r+");
    size_t objects_read = fread(link_key_str, LINK_KEY_STR_LEN, 1, rFile );
    if (objects_read == 1){
        link_key_str[LINK_KEY_STR_LEN] = 0;
        log_info("Found link key %s\n", link_key_str);
        objects_read = fread(link_key_type_str, 1, 1, rFile );
    }
    fclose(rFile);
    
    if (objects_read != 1) return 0;
    link_key_type_str[1] = 0;
    log_info("Found link key type %s\n", link_key_type_str);
    
    int scan_result = sscanf_link_key(link_key_str, link_key);
    if (scan_result == 0 ) return 0;

    int link_key_type_buffer;
    scan_result = sscanf( (char *) link_key_type_str, "%d", &link_key_type_buffer);
    if (scan_result == 0 ) return 0;
    *link_key_type = (link_key_type_t) link_key_type_buffer;
#else

#endif
    return 1;
}

static void delete_link_key(bd_addr_t bd_addr){
    set_path(bd_addr);
    //if (access(keypath, R_OK)) return;

    if(remove(keypath) != 0){
        log_error("File %s could not be deleted.\n", keypath);
    }
}

const btstack_link_key_db_t btstack_link_key_db_fs = {
    &db_open,
    //&db_set_local_bd_addr,
    &db_close,
    &get_link_key,
    &put_link_key,
    &delete_link_key,
};

const btstack_link_key_db_t * btstack_link_key_db_fs_instance(void){
    return &btstack_link_key_db_fs;
}

#endif
