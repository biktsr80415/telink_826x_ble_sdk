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

#include "../btstack_config.h"

#include "../stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <inttypes.h>

#include "../btstack_debug.h"
#include "../btstack_event.h"
#include "../btstack_memory.h"
#include "../btstack_run_loop.h"
#include "../classic/core.h"
#include "../classic/sdp_client_rfcomm.h"
#include "../classic/sdp_server.h"
#include "../classic/sdp_util.h"
#include "../hci.h"
#include "../hci_cmd.h"
#include "../hci_dump.h"
#include "../l2cap.h"

#define HFP_HF_FEATURES_SIZE 10
#define HFP_AG_FEATURES_SIZE 12


static const char * hfp_hf_features[] = {
    "EC and/or NR function",
    "Three-way calling",
    "CLI presentation capability",
    "Voice recognition activation",
    "Remote volume control",

    "Enhanced call status",
    "Enhanced call control",
    
    "Codec negotiation",
    
    "HF Indicators",
    "eSCO S4 (and T2) Settings Supported",
    "Reserved for future definition"
};

static const char * hfp_ag_features[] = {
    "Three-way calling",
    "EC and/or NR function",
    "Voice recognition function",
    "In-band ring tone capability",
    "Attach a number to a voice tag",
    "Ability to reject a call",
    "Enhanced call status",
    "Enhanced call control",
    "Extended Error Result Codes",
    "Codec negotiation",
    "HF Indicators",
    "eSCO S4 (and T2) Settings Supported",
    "Reserved for future definition"
};

static btstack_linked_list_t hfp_connections = NULL;
static void parse_sequence(hfp_connection_t * context);
static btstack_packet_handler_t hfp_callback;
static btstack_packet_handler_t rfcomm_packet_handler;

static hfp_connection_t * sco_establishment_active;

void hfp_set_callback(btstack_packet_handler_t callback){
    hfp_callback = callback;
}

const char * hfp_hf_feature(int index){
    if (index > HFP_HF_FEATURES_SIZE){
        return hfp_hf_features[HFP_HF_FEATURES_SIZE];
    }
    return hfp_hf_features[index];
}

const char * hfp_ag_feature(int index){
    if (index > HFP_AG_FEATURES_SIZE){
        return hfp_ag_features[HFP_AG_FEATURES_SIZE];
    }
    return hfp_ag_features[index];
}

int send_str_over_rfcomm(uint16_t cid, char * command){
    if (!rfcomm_can_send_packet_now(cid)) return 1;
    log_info("HFP_TX %s", command);
    int err = rfcomm_send(cid, (uint8_t*) command, strlen(command));
    if (err){
        log_error("rfcomm_send -> error 0x%02x \n", err);
    } 
    return 1;
}

#if 0
void hfp_set_codec(hfp_connection_t * hfp_connection, uint8_t *packet, uint16_t size){
    // parse available codecs
    int pos = 0;
    int i;
    for (i=0; i<size; i++){
        pos+=8;
        if (packet[pos] > hfp_connection->negotiated_codec){
            hfp_connection->negotiated_codec = packet[pos];
        }
    }
    printf("Negotiated Codec 0x%02x\n", hfp_connection->negotiated_codec);
}
#endif

// UTILS
int get_bit(uint16_t bitmap, int position){
    return (bitmap >> position) & 1;
}

int store_bit(uint32_t bitmap, int position, uint8_t value){
    if (value){
        bitmap |= 1 << position;
    } else {
        bitmap &= ~ (1 << position);
    }
    return bitmap;
}

int join(char * buffer, int buffer_size, uint8_t * values, int values_nr){
    if (buffer_size < values_nr * 3) return 0;
    int i;
    int offset = 0;
    for (i = 0; i < values_nr-1; i++) {
      offset += snprintf(buffer+offset, buffer_size-offset, "%d,", values[i]); // puts string into buffer
    }
    if (i<values_nr){
        offset += snprintf(buffer+offset, buffer_size-offset, "%d", values[i]);
    }
    return offset;
}

int join_bitmap(char * buffer, int buffer_size, uint32_t values, int values_nr){
    if (buffer_size < values_nr * 3) return 0;

    int i;
    int offset = 0;
    for (i = 0; i < values_nr-1; i++) {
      offset += snprintf(buffer+offset, buffer_size-offset, "%d,", get_bit(values,i)); // puts string into buffer
    }
    
    if (i<values_nr){
        offset += snprintf(buffer+offset, buffer_size-offset, "%d", get_bit(values,i));
    }
    return offset;
}

void hfp_emit_simple_event(btstack_packet_handler_t callback, uint8_t event_subtype){
    if (!callback) return;
    uint8_t event[3];
    event[0] = HCI_EVENT_HFP_META;
    event[1] = sizeof(event) - 2;
    event[2] = event_subtype;
    (*callback)(HCI_EVENT_PACKET, 0, event, sizeof(event));
}

void hfp_emit_event(btstack_packet_handler_t callback, uint8_t event_subtype, uint8_t value){
    if (!callback) return;
    uint8_t event[4];
    event[0] = HCI_EVENT_HFP_META;
    event[1] = sizeof(event) - 2;
    event[2] = event_subtype;
    event[3] = value; // status 0 == OK
    (*callback)(HCI_EVENT_PACKET, 0, event, sizeof(event));
}

void hfp_emit_connection_event(btstack_packet_handler_t callback, uint8_t event_subtype, uint8_t status, hci_con_handle_t con_handle){
    if (!callback) return;
    uint8_t event[6];
    event[0] = HCI_EVENT_HFP_META;
    event[1] = sizeof(event) - 2;
    event[2] = event_subtype;
    event[3] = status; // status 0 == OK
    little_endian_store_16(event, 4, con_handle);
    (*callback)(HCI_EVENT_PACKET, 0, event, sizeof(event));
}

void hfp_emit_string_event(btstack_packet_handler_t callback, uint8_t event_subtype, const char * value){
    if (!callback) return;
    uint8_t event[40];
    event[0] = HCI_EVENT_HFP_META;
    event[1] = sizeof(event) - 2;
    event[2] = event_subtype;
    int size = (strlen(value) < sizeof(event) - 4) ? strlen(value) : sizeof(event) - 4;
    strncpy((char*)&event[3], value, size);
    event[3 + size] = 0;
    (*callback)(HCI_EVENT_PACKET, 0, event, sizeof(event));
}

btstack_linked_list_t * hfp_get_connections(void){
    return (btstack_linked_list_t *) &hfp_connections;
} 

hfp_connection_t * get_hfp_connection_context_for_rfcomm_cid(uint16_t cid){
    btstack_linked_list_iterator_t it;    
    btstack_linked_list_iterator_init(&it, hfp_get_connections());
    while (btstack_linked_list_iterator_has_next(&it)){
        hfp_connection_t * hfp_connection = (hfp_connection_t *)btstack_linked_list_iterator_next(&it);
        if (hfp_connection->rfcomm_cid == cid){
            return hfp_connection;
        }
    }
    return NULL;
}

hfp_connection_t * get_hfp_connection_context_for_bd_addr(bd_addr_t bd_addr){
    btstack_linked_list_iterator_t it;  
    btstack_linked_list_iterator_init(&it, hfp_get_connections());
    while (btstack_linked_list_iterator_has_next(&it)){
        hfp_connection_t * hfp_connection = (hfp_connection_t *)btstack_linked_list_iterator_next(&it);
        if (memcmp(hfp_connection->remote_addr, bd_addr, 6) == 0) {
            return hfp_connection;
        }
    }
    return NULL;
}

hfp_connection_t * get_hfp_connection_context_for_sco_handle(uint16_t handle){
    btstack_linked_list_iterator_t it;    
    btstack_linked_list_iterator_init(&it, hfp_get_connections());
    while (btstack_linked_list_iterator_has_next(&it)){
        hfp_connection_t * hfp_connection = (hfp_connection_t *)btstack_linked_list_iterator_next(&it);
        if (hfp_connection->sco_handle == handle){
            return hfp_connection;
        }
    }
    return NULL;
}

void hfp_reset_context_flags(hfp_connection_t * hfp_connection){
    if (!hfp_connection) return;
    hfp_connection->ok_pending = 0;
    hfp_connection->send_error = 0;

    hfp_connection->keep_byte = 0;

    hfp_connection->change_status_update_for_individual_ag_indicators = 0; 
    hfp_connection->operator_name_changed = 0;      

    hfp_connection->enable_extended_audio_gateway_error_report = 0;
    hfp_connection->extended_audio_gateway_error = 0;

    // establish codecs hfp_connection
    hfp_connection->suggested_codec = 0;
    hfp_connection->negotiated_codec = 0;
    hfp_connection->codec_confirmed = 0;

    hfp_connection->establish_audio_connection = 0; 
    hfp_connection->call_waiting_notification_enabled = 0;
    hfp_connection->command = HFP_CMD_NONE;
    hfp_connection->enable_status_update_for_ag_indicators = 0xFF;
}

static hfp_connection_t * create_hfp_connection_context(void){
    hfp_connection_t * hfp_connection = btstack_memory_hfp_connection_get();
    if (!hfp_connection) return NULL;
    // init state
    memset(hfp_connection,0, sizeof(hfp_connection_t));

    hfp_connection->state = HFP_IDLE;
    hfp_connection->call_state = HFP_CALL_IDLE;
    hfp_connection->codecs_state = HFP_CODECS_IDLE;

    hfp_connection->parser_state = HFP_PARSER_CMD_HEADER;
    hfp_connection->command = HFP_CMD_NONE;
    
    hfp_reset_context_flags(hfp_connection);

    btstack_linked_list_add(&hfp_connections, (btstack_linked_item_t*)hfp_connection);
    return hfp_connection;
}

static void remove_hfp_connection_context(hfp_connection_t * hfp_connection){
    btstack_linked_list_remove(&hfp_connections, (btstack_linked_item_t*) hfp_connection);   
}

static hfp_connection_t * provide_hfp_connection_context_for_bd_addr(bd_addr_t bd_addr){
    hfp_connection_t * hfp_connection = get_hfp_connection_context_for_bd_addr(bd_addr);
    if (hfp_connection) return  hfp_connection;
    hfp_connection = create_hfp_connection_context();
    printf("created hfp_connection for address %s\n", bd_addr_to_str(bd_addr));
    memcpy(hfp_connection->remote_addr, bd_addr, 6);
    return hfp_connection;
}

/* @param network.
 * 0 == no ability to reject a call. 
 * 1 == ability to reject a call.
 */

/* @param suported_features
 * HF bit 0: EC and/or NR function (yes/no, 1 = yes, 0 = no)
 * HF bit 1: Call waiting or three-way calling(yes/no, 1 = yes, 0 = no)
 * HF bit 2: CLI presentation capability (yes/no, 1 = yes, 0 = no)
 * HF bit 3: Voice recognition activation (yes/no, 1= yes, 0 = no)
 * HF bit 4: Remote volume control (yes/no, 1 = yes, 0 = no)
 * HF bit 5: Wide band speech (yes/no, 1 = yes, 0 = no)
 */
 /* Bit position:
 * AG bit 0: Three-way calling (yes/no, 1 = yes, 0 = no)
 * AG bit 1: EC and/or NR function (yes/no, 1 = yes, 0 = no)
 * AG bit 2: Voice recognition function (yes/no, 1 = yes, 0 = no)
 * AG bit 3: In-band ring tone capability (yes/no, 1 = yes, 0 = no)
 * AG bit 4: Attach a phone number to a voice tag (yes/no, 1 = yes, 0 = no)
 * AG bit 5: Wide band speech (yes/no, 1 = yes, 0 = no)
 */

void hfp_create_sdp_record(uint8_t * service, uint32_t service_record_handle, uint16_t service_uuid, int rfcomm_channel_nr, const char * name){
    uint8_t* attribute;
    de_create_sequence(service);

    // 0x0000 "Service Record Handle"
    de_add_number(service, DE_UINT, DE_SIZE_16, SDP_ServiceRecordHandle);
    de_add_number(service, DE_UINT, DE_SIZE_32, service_record_handle);

    // 0x0001 "Service Class ID List"
    de_add_number(service,  DE_UINT, DE_SIZE_16, SDP_ServiceClassIDList);
    attribute = de_push_sequence(service);
    {
        //  "UUID for Service"
        de_add_number(attribute, DE_UUID, DE_SIZE_16, service_uuid);
        de_add_number(attribute, DE_UUID, DE_SIZE_16, SDP_GenericAudio);
    }
    de_pop_sequence(service, attribute);

    // 0x0004 "Protocol Descriptor List"
    de_add_number(service,  DE_UINT, DE_SIZE_16, SDP_ProtocolDescriptorList);
    attribute = de_push_sequence(service);
    {
        uint8_t* l2cpProtocol = de_push_sequence(attribute);
        {
            de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, SDP_L2CAPProtocol);
        }
        de_pop_sequence(attribute, l2cpProtocol);
        
        uint8_t* rfcomm = de_push_sequence(attribute);
        {
            de_add_number(rfcomm,  DE_UUID, DE_SIZE_16, SDP_RFCOMMProtocol);  // rfcomm_service
            de_add_number(rfcomm,  DE_UINT, DE_SIZE_8,  rfcomm_channel_nr);  // rfcomm channel
        }
        de_pop_sequence(attribute, rfcomm);
    }
    de_pop_sequence(service, attribute);

    
    // 0x0005 "Public Browse Group"
    de_add_number(service,  DE_UINT, DE_SIZE_16, SDP_BrowseGroupList); // public browse group
    attribute = de_push_sequence(service);
    {
        de_add_number(attribute,  DE_UUID, DE_SIZE_16, SDP_PublicBrowseGroup);
    }
    de_pop_sequence(service, attribute);

    // 0x0009 "Bluetooth Profile Descriptor List"
    de_add_number(service,  DE_UINT, DE_SIZE_16, SDP_BluetoothProfileDescriptorList);
    attribute = de_push_sequence(service);
    {
        uint8_t *sppProfile = de_push_sequence(attribute);
        {
            de_add_number(sppProfile,  DE_UUID, DE_SIZE_16, SDP_Handsfree); 
            de_add_number(sppProfile,  DE_UINT, DE_SIZE_16, 0x0107); // Verision 1.7
        }
        de_pop_sequence(attribute, sppProfile);
    }
    de_pop_sequence(service, attribute);

    // 0x0100 "Service Name"
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0100);
    de_add_data(service,  DE_STRING, strlen(name), (uint8_t *) name);
}

static hfp_connection_t * connection_doing_sdp_query = NULL;

static void handle_query_rfcomm_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    hfp_connection_t * hfp_connection = connection_doing_sdp_query;
    
    if ( hfp_connection->state != HFP_W4_SDP_QUERY_COMPLETE) return;
    
    switch (hci_event_packet_get_type(packet)){
        case SDP_EVENT_QUERY_RFCOMM_SERVICE:
            if (!hfp_connection) {
                log_error("handle_query_rfcomm_event alloc connection for RFCOMM port %u failed", sdp_event_query_rfcomm_service_get_rfcomm_channel(packet));
                return;
            }
            hfp_connection->rfcomm_channel_nr = sdp_event_query_rfcomm_service_get_rfcomm_channel(packet);
            break;
        case SDP_EVENT_QUERY_COMPLETE:
            connection_doing_sdp_query = NULL;
            if (hfp_connection->rfcomm_channel_nr > 0){
                hfp_connection->state = HFP_W4_RFCOMM_CONNECTED;
                log_info("HFP: SDP_EVENT_QUERY_COMPLETE context %p, addr %s, state %d", hfp_connection, bd_addr_to_str( hfp_connection->remote_addr),  hfp_connection->state);
                rfcomm_create_channel(rfcomm_packet_handler, hfp_connection->remote_addr, hfp_connection->rfcomm_channel_nr, NULL); 
                break;
            }
            log_info("rfcomm service not found, status %u.", sdp_event_query_complete_get_status(packet));
            break;
        default:
            break;
    }
}

static void hfp_handle_failed_sco_connection(uint8_t status){
               
    if (!sco_establishment_active){
        log_error("(e)SCO Connection failed but not started by us");
        return;
    }
    log_error("(e)SCO Connection failed status 0x%02x", status);

    // invalid params / unspecified error
    if (status != 0x11 && status != 0x1f) return;
                
     switch (sco_establishment_active->link_setting){
        case HFP_LINK_SETTINGS_D0:
            return; // no other option left
        case HFP_LINK_SETTINGS_D1:
            sco_establishment_active->link_setting = HFP_LINK_SETTINGS_D0;
            break;
        case HFP_LINK_SETTINGS_S1:
            sco_establishment_active->link_setting = HFP_LINK_SETTINGS_D1;
            break;                    
        case HFP_LINK_SETTINGS_S2:
        case HFP_LINK_SETTINGS_S3:
        case HFP_LINK_SETTINGS_S4:
            sco_establishment_active->link_setting = HFP_LINK_SETTINGS_S1;
            break;
        case HFP_LINK_SETTINGS_T1:
        case HFP_LINK_SETTINGS_T2:
            sco_establishment_active->link_setting = HFP_LINK_SETTINGS_S3;
            break;
    }
    sco_establishment_active->establish_audio_connection = 1;
    sco_establishment_active = 0;
}


void hfp_handle_hci_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    bd_addr_t event_addr;
    uint16_t rfcomm_cid, handle;
    hfp_connection_t * hfp_connection = NULL;
    uint8_t status;

    log_info("AG packet_handler type %u, event type %x, size %u", packet_type, hci_event_packet_get_type(packet), size);

    switch (hci_event_packet_get_type(packet)) {
        
        case RFCOMM_EVENT_INCOMING_CONNECTION:
            // data: event (8), len(8), address(48), channel (8), rfcomm_cid (16)
            rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr); 
            hfp_connection = provide_hfp_connection_context_for_bd_addr(event_addr);
            if (!hfp_connection || hfp_connection->state != HFP_IDLE) return;

            hfp_connection->rfcomm_cid = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
            hfp_connection->state = HFP_W4_RFCOMM_CONNECTED;
            // printf("RFCOMM channel %u requested for %s\n", hfp_connection->rfcomm_cid, bd_addr_to_str(hfp_connection->remote_addr));
            rfcomm_accept_connection(hfp_connection->rfcomm_cid);
            break;

        case RFCOMM_EVENT_CHANNEL_OPENED:
            // data: event(8), len(8), status (8), address (48), handle(16), server channel(8), rfcomm_cid(16), max frame size(16)

            rfcomm_event_channel_opened_get_bd_addr(packet, event_addr); 
            status = rfcomm_event_channel_opened_get_status(packet);          
            // printf("RFCOMM_EVENT_CHANNEL_OPENED packet_handler adddr %s, status %u\n", bd_addr_to_str(event_addr), status);

            hfp_connection = get_hfp_connection_context_for_bd_addr(event_addr);
            if (!hfp_connection || hfp_connection->state != HFP_W4_RFCOMM_CONNECTED) return;

            if (status) {
                hfp_emit_connection_event(hfp_callback, HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED, status, rfcomm_event_channel_opened_get_con_handle(packet));
                remove_hfp_connection_context(hfp_connection);
            } else {
                hfp_connection->acl_handle = rfcomm_event_channel_opened_get_con_handle(packet);
                hfp_connection->rfcomm_cid = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
                // uint16_t mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
                // printf("RFCOMM channel open succeeded. hfp_connection %p, RFCOMM Channel ID 0x%02x, max frame size %u\n", hfp_connection, hfp_connection->rfcomm_cid, mtu);
                        
                switch (hfp_connection->state){
                    case HFP_W4_RFCOMM_CONNECTED:
                        hfp_connection->state = HFP_EXCHANGE_SUPPORTED_FEATURES;
                        break;
                    case HFP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN:
                        hfp_connection->state = HFP_W2_DISCONNECT_RFCOMM;
                        // printf("Shutting down RFCOMM.\n");
                        break;
                    default:
                        break;
                }
                rfcomm_request_can_send_now_event(hfp_connection->rfcomm_cid);
            }
            break;
        
        case HCI_EVENT_COMMAND_STATUS:
            if (hci_event_command_status_get_command_opcode(packet) == hci_setup_synchronous_connection.opcode) {
                status = hci_event_command_status_get_status(packet);
                if (status) {
                    hfp_handle_failed_sco_connection(hci_event_command_status_get_status(packet));
               }
            }
            break;

        case HCI_EVENT_SYNCHRONOUS_CONNECTION_COMPLETE:{

            reverse_bd_addr(&packet[5], event_addr);
            int index = 2;
            status = packet[index++];

            if (status != 0){
                hfp_handle_failed_sco_connection(status);
                break;
            }
            
            uint16_t sco_handle = little_endian_read_16(packet, index);
            index+=2;

            reverse_bd_addr(&packet[index], event_addr);
            index+=6;

            uint8_t link_type = packet[index++];
            uint8_t transmission_interval = packet[index++];  // measured in slots
            uint8_t retransmission_interval = packet[index++];// measured in slots
            uint16_t rx_packet_length = little_endian_read_16(packet, index); // measured in bytes
            index+=2;
            uint16_t tx_packet_length = little_endian_read_16(packet, index); // measured in bytes
            index+=2;
            uint8_t air_mode = packet[index];

            switch (link_type){
                case 0x00:
                    log_info("SCO Connection established.");
                    if (transmission_interval != 0) log_error("SCO Connection: transmission_interval not zero: %d.", transmission_interval);
                    if (retransmission_interval != 0) log_error("SCO Connection: retransmission_interval not zero: %d.", retransmission_interval);
                    if (rx_packet_length != 0) log_error("SCO Connection: rx_packet_length not zero: %d.", rx_packet_length);
                    if (tx_packet_length != 0) log_error("SCO Connection: tx_packet_length not zero: %d.", tx_packet_length);
                    break;
                case 0x02:
                    log_info("eSCO Connection established. \n");
                    break;
                default:
                    log_error("(e)SCO reserved link_type 0x%2x", link_type);
                    break;
            }
            log_info("sco_handle 0x%2x, address %s, transmission_interval %u slots, retransmission_interval %u slots, " 
                 " rx_packet_length %u bytes, tx_packet_length %u bytes, air_mode 0x%2x (0x02 == CVSD)\n", sco_handle,
                 bd_addr_to_str(event_addr), transmission_interval, retransmission_interval, rx_packet_length, tx_packet_length, air_mode);

            hfp_connection = get_hfp_connection_context_for_bd_addr(event_addr);
            
            if (!hfp_connection) {
                log_error("SCO link created, hfp_connection for address %s not found.", bd_addr_to_str(event_addr));
                break;
            }

            if (hfp_connection->state == HFP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN){
                log_info("SCO about to disconnect: HFP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN");
                hfp_connection->state = HFP_W2_DISCONNECT_SCO;
                break;
            }
            hfp_connection->sco_handle = sco_handle;
            hfp_connection->establish_audio_connection = 0;
            hfp_connection->state = HFP_AUDIO_CONNECTION_ESTABLISHED;
            hfp_emit_connection_event(hfp_callback, HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED, packet[2], sco_handle);
            break;                
        }

        case RFCOMM_EVENT_CHANNEL_CLOSED:
            rfcomm_cid = little_endian_read_16(packet,2);
            hfp_connection = get_hfp_connection_context_for_rfcomm_cid(rfcomm_cid);
            if (!hfp_connection) break;
            if (hfp_connection->state == HFP_W4_RFCOMM_DISCONNECTED_AND_RESTART){
                hfp_connection->state = HFP_IDLE;
                hfp_establish_service_level_connection(hfp_connection->remote_addr, hfp_connection->service_uuid);
                break;
            }
            
            hfp_emit_event(hfp_callback, HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED, 0);
            remove_hfp_connection_context(hfp_connection);
            break;

        case HCI_EVENT_DISCONNECTION_COMPLETE:
            handle = little_endian_read_16(packet,3);
            hfp_connection = get_hfp_connection_context_for_sco_handle(handle);
            
            if (!hfp_connection) break;

            if (hfp_connection->state != HFP_W4_SCO_DISCONNECTED){
                log_info("Received gap disconnect in wrong hfp state");
            }
            log_info("Check SCO handle: incoming 0x%02x, hfp_connection 0x%02x\n", handle, hfp_connection->sco_handle);
                
            if (handle == hfp_connection->sco_handle){
                log_info("SCO disconnected, w2 disconnect RFCOMM\n");
                hfp_connection->sco_handle = 0;
                hfp_connection->release_audio_connection = 0;
                hfp_connection->state = HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED;
                hfp_emit_event(hfp_callback, HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED, 0);
                break;
            }
            break;

        default:
            break;
    }
}

// translates command string into hfp_command_t CMD
static hfp_command_t parse_command(const char * line_buffer, int isHandsFree){
    int offset = isHandsFree ? 0 : 2;

    if (strncmp(line_buffer+offset, HFP_LIST_CURRENT_CALLS, strlen(HFP_LIST_CURRENT_CALLS)) == 0){
        return HFP_CMD_LIST_CURRENT_CALLS;
    }

    if (strncmp(line_buffer+offset, HFP_SUBSCRIBER_NUMBER_INFORMATION, strlen(HFP_SUBSCRIBER_NUMBER_INFORMATION)) == 0){
        return HFP_CMD_GET_SUBSCRIBER_NUMBER_INFORMATION;
    }

    if (strncmp(line_buffer+offset, HFP_PHONE_NUMBER_FOR_VOICE_TAG, strlen(HFP_PHONE_NUMBER_FOR_VOICE_TAG)) == 0){
        if (isHandsFree) return HFP_CMD_AG_SENT_PHONE_NUMBER;
        return HFP_CMD_HF_REQUEST_PHONE_NUMBER;
    }

    if (strncmp(line_buffer+offset, HFP_TRANSMIT_DTMF_CODES, strlen(HFP_TRANSMIT_DTMF_CODES)) == 0){
        return HFP_CMD_TRANSMIT_DTMF_CODES;
    }

    if (strncmp(line_buffer+offset, HFP_SET_MICROPHONE_GAIN, strlen(HFP_SET_MICROPHONE_GAIN)) == 0){
        return HFP_CMD_SET_MICROPHONE_GAIN;
    }

    if (strncmp(line_buffer+offset, HFP_SET_SPEAKER_GAIN, strlen(HFP_SET_SPEAKER_GAIN)) == 0){
        return HFP_CMD_SET_SPEAKER_GAIN;
    }
    
    if (strncmp(line_buffer+offset, HFP_ACTIVATE_VOICE_RECOGNITION, strlen(HFP_ACTIVATE_VOICE_RECOGNITION)) == 0){
        if (isHandsFree) return HFP_CMD_AG_ACTIVATE_VOICE_RECOGNITION;
        return HFP_CMD_HF_ACTIVATE_VOICE_RECOGNITION;
    }

    if (strncmp(line_buffer+offset, HFP_TURN_OFF_EC_AND_NR, strlen(HFP_TURN_OFF_EC_AND_NR)) == 0){
        return HFP_CMD_TURN_OFF_EC_AND_NR;
    }

    if (strncmp(line_buffer, HFP_CALL_ANSWERED, strlen(HFP_CALL_ANSWERED)) == 0){
        return HFP_CMD_CALL_ANSWERED;
    }

    if (strncmp(line_buffer, HFP_CALL_PHONE_NUMBER, strlen(HFP_CALL_PHONE_NUMBER)) == 0){
        return HFP_CMD_CALL_PHONE_NUMBER;
    }

    if (strncmp(line_buffer+offset, HFP_REDIAL_LAST_NUMBER, strlen(HFP_REDIAL_LAST_NUMBER)) == 0){
        return HFP_CMD_REDIAL_LAST_NUMBER;
    }

    if (strncmp(line_buffer+offset, HFP_CHANGE_IN_BAND_RING_TONE_SETTING, strlen(HFP_CHANGE_IN_BAND_RING_TONE_SETTING)) == 0){
        return HFP_CMD_CHANGE_IN_BAND_RING_TONE_SETTING;
    }

    if (strncmp(line_buffer+offset, HFP_HANG_UP_CALL, strlen(HFP_HANG_UP_CALL)) == 0){
        return HFP_CMD_HANG_UP_CALL;
    }

    if (strncmp(line_buffer+offset, HFP_ERROR, strlen(HFP_ERROR)) == 0){
        return HFP_CMD_ERROR;
    }

    if (strncmp(line_buffer+offset, HFP_RING, strlen(HFP_RING)) == 0){
        return HFP_CMD_RING;
    }

    if (isHandsFree && strncmp(line_buffer+offset, HFP_OK, strlen(HFP_OK)) == 0){
        return HFP_CMD_OK;
    }

    if (strncmp(line_buffer+offset, HFP_SUPPORTED_FEATURES, strlen(HFP_SUPPORTED_FEATURES)) == 0){
        return HFP_CMD_SUPPORTED_FEATURES;
    }

    if (strncmp(line_buffer+offset, HFP_TRANSFER_HF_INDICATOR_STATUS, strlen(HFP_TRANSFER_HF_INDICATOR_STATUS)) == 0){
        return HFP_CMD_HF_INDICATOR_STATUS;
    }
    
    if (strncmp(line_buffer+offset, HFP_RESPONSE_AND_HOLD, strlen(HFP_RESPONSE_AND_HOLD)) == 0){
        if (strncmp(line_buffer+strlen(HFP_RESPONSE_AND_HOLD)+offset, "?", 1) == 0){
            return HFP_CMD_RESPONSE_AND_HOLD_QUERY;
        }
        if (strncmp(line_buffer+strlen(HFP_RESPONSE_AND_HOLD)+offset, "=", 1) == 0){
            return HFP_CMD_RESPONSE_AND_HOLD_COMMAND;
        }
        return HFP_CMD_RESPONSE_AND_HOLD_STATUS;
    }

    if (strncmp(line_buffer+offset, HFP_INDICATOR, strlen(HFP_INDICATOR)) == 0){
        if (strncmp(line_buffer+strlen(HFP_INDICATOR)+offset, "?", 1) == 0){
            return HFP_CMD_RETRIEVE_AG_INDICATORS_STATUS;
        }

        if (strncmp(line_buffer+strlen(HFP_INDICATOR)+offset, "=?", 2) == 0){
            return HFP_CMD_RETRIEVE_AG_INDICATORS;
        }
    }

    if (strncmp(line_buffer+offset, HFP_AVAILABLE_CODECS, strlen(HFP_AVAILABLE_CODECS)) == 0){
        return HFP_CMD_AVAILABLE_CODECS;
    }

    if (strncmp(line_buffer+offset, HFP_ENABLE_STATUS_UPDATE_FOR_AG_INDICATORS, strlen(HFP_ENABLE_STATUS_UPDATE_FOR_AG_INDICATORS)) == 0){
        return HFP_CMD_ENABLE_INDICATOR_STATUS_UPDATE;
    }

    if (strncmp(line_buffer+offset, HFP_ENABLE_CLIP, strlen(HFP_ENABLE_CLIP)) == 0){
        if (isHandsFree) return HFP_CMD_AG_SENT_CLIP_INFORMATION;
        return HFP_CMD_ENABLE_CLIP;
    }

    if (strncmp(line_buffer+offset, HFP_ENABLE_CALL_WAITING_NOTIFICATION, strlen(HFP_ENABLE_CALL_WAITING_NOTIFICATION)) == 0){
        if (isHandsFree) return HFP_CMD_AG_SENT_CALL_WAITING_NOTIFICATION_UPDATE;
        return HFP_CMD_ENABLE_CALL_WAITING_NOTIFICATION;
    }

    if (strncmp(line_buffer+offset, HFP_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES, strlen(HFP_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES)) == 0){
        
        if (isHandsFree) return HFP_CMD_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES;

        if (strncmp(line_buffer+strlen(HFP_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES)+offset, "=?", 2) == 0){
            return HFP_CMD_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES;
        }
        if (strncmp(line_buffer+strlen(HFP_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES)+offset, "=", 1) == 0){
            return HFP_CMD_CALL_HOLD;    
        }

        return HFP_CMD_UNKNOWN;
    } 

    if (strncmp(line_buffer+offset, HFP_GENERIC_STATUS_INDICATOR, strlen(HFP_GENERIC_STATUS_INDICATOR)) == 0){
        if (isHandsFree) {
            return HFP_CMD_SET_GENERIC_STATUS_INDICATOR_STATUS;
        }
        if (strncmp(line_buffer+strlen(HFP_GENERIC_STATUS_INDICATOR)+offset, "=?", 2) == 0){
            return HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS;
        } 
        if (strncmp(line_buffer+strlen(HFP_GENERIC_STATUS_INDICATOR)+offset, "=", 1) == 0){
            return HFP_CMD_LIST_GENERIC_STATUS_INDICATORS;    
        }
        return HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS_STATE;
    } 

    if (strncmp(line_buffer+offset, HFP_UPDATE_ENABLE_STATUS_FOR_INDIVIDUAL_AG_INDICATORS, strlen(HFP_UPDATE_ENABLE_STATUS_FOR_INDIVIDUAL_AG_INDICATORS)) == 0){
        return HFP_CMD_ENABLE_INDIVIDUAL_AG_INDICATOR_STATUS_UPDATE;
    } 
    

    if (strncmp(line_buffer+offset, HFP_QUERY_OPERATOR_SELECTION, strlen(HFP_QUERY_OPERATOR_SELECTION)) == 0){
        if (strncmp(line_buffer+strlen(HFP_QUERY_OPERATOR_SELECTION)+offset, "=", 1) == 0){
            return HFP_CMD_QUERY_OPERATOR_SELECTION_NAME_FORMAT;
        } 
        return HFP_CMD_QUERY_OPERATOR_SELECTION_NAME;
    }

    if (strncmp(line_buffer+offset, HFP_TRANSFER_AG_INDICATOR_STATUS, strlen(HFP_TRANSFER_AG_INDICATOR_STATUS)) == 0){
        return HFP_CMD_TRANSFER_AG_INDICATOR_STATUS;
    } 

    if (isHandsFree && strncmp(line_buffer+offset, HFP_EXTENDED_AUDIO_GATEWAY_ERROR, strlen(HFP_EXTENDED_AUDIO_GATEWAY_ERROR)) == 0){
        return HFP_CMD_EXTENDED_AUDIO_GATEWAY_ERROR;
    }

    if (!isHandsFree && strncmp(line_buffer+offset, HFP_ENABLE_EXTENDED_AUDIO_GATEWAY_ERROR, strlen(HFP_ENABLE_EXTENDED_AUDIO_GATEWAY_ERROR)) == 0){
        return HFP_CMD_ENABLE_EXTENDED_AUDIO_GATEWAY_ERROR;
    }

    if (strncmp(line_buffer+offset, HFP_TRIGGER_CODEC_CONNECTION_SETUP, strlen(HFP_TRIGGER_CODEC_CONNECTION_SETUP)) == 0){
        return HFP_CMD_TRIGGER_CODEC_CONNECTION_SETUP;
    } 

    if (strncmp(line_buffer+offset, HFP_CONFIRM_COMMON_CODEC, strlen(HFP_CONFIRM_COMMON_CODEC)) == 0){
        if (isHandsFree){
            return HFP_CMD_AG_SUGGESTED_CODEC;
        } else {
            return HFP_CMD_HF_CONFIRMED_CODEC;
        }
    } 
    
    if (strncmp(line_buffer+offset, "AT+", 3) == 0){
        log_info("process unknown HF command %s \n", line_buffer);
        return HFP_CMD_UNKNOWN;
    } 
    
    if (strncmp(line_buffer+offset, "+", 1) == 0){
        log_info(" process unknown AG command %s \n", line_buffer);
        return HFP_CMD_UNKNOWN;
    }
    
    if (strncmp(line_buffer+offset, "NOP", 3) == 0){
        return HFP_CMD_NONE;
    } 
    
    return HFP_CMD_NONE;
}

static void hfp_parser_store_byte(hfp_connection_t * hfp_connection, uint8_t byte){
    // printf("hfp_parser_store_byte %c at pos %u\n", (char) byte, context->line_size);
    // TODO: add limit
    hfp_connection->line_buffer[hfp_connection->line_size++] = byte;
    hfp_connection->line_buffer[hfp_connection->line_size] = 0;
}
static int hfp_parser_is_buffer_empty(hfp_connection_t * hfp_connection){
    return hfp_connection->line_size == 0;
}

static int hfp_parser_is_end_of_line(uint8_t byte){
    return byte == '\n' || byte == '\r';
}

static int hfp_parser_is_end_of_header(uint8_t byte){
    return hfp_parser_is_end_of_line(byte) || byte == ':' || byte == '?';
}

static int hfp_parser_found_separator(hfp_connection_t * hfp_connection, uint8_t byte){
    if (hfp_connection->keep_byte == 1) return 1;

    int found_separator =   byte == ',' || byte == '\n'|| byte == '\r'||
                            byte == ')' || byte == '(' || byte == ':' || 
                            byte == '-' || byte == '"' ||  byte == '?'|| byte == '=';
    return found_separator;
}

static void hfp_parser_next_state(hfp_connection_t * hfp_connection, uint8_t byte){
    hfp_connection->line_size = 0;
    if (hfp_parser_is_end_of_line(byte)){
        hfp_connection->parser_item_index = 0;
        hfp_connection->parser_state = HFP_PARSER_CMD_HEADER;
        return;
    }
    switch (hfp_connection->parser_state){
        case HFP_PARSER_CMD_HEADER:
            hfp_connection->parser_state = HFP_PARSER_CMD_SEQUENCE;
            if (hfp_connection->keep_byte == 1){
                hfp_parser_store_byte(hfp_connection, byte);
                hfp_connection->keep_byte = 0;
            }
            break;
        case HFP_PARSER_CMD_SEQUENCE:
            switch (hfp_connection->command){
                case HFP_CMD_AG_SENT_PHONE_NUMBER:
                case HFP_CMD_AG_SENT_CALL_WAITING_NOTIFICATION_UPDATE:
                case HFP_CMD_AG_SENT_CLIP_INFORMATION:
                case HFP_CMD_TRANSFER_AG_INDICATOR_STATUS:
                case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME:
                case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME_FORMAT:
                case HFP_CMD_RETRIEVE_AG_INDICATORS:
                case HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS_STATE:
                case HFP_CMD_HF_INDICATOR_STATUS:
                    hfp_connection->parser_state = HFP_PARSER_SECOND_ITEM;
                    break;
                default:
                    break;
            }
            break;
        case HFP_PARSER_SECOND_ITEM:
            hfp_connection->parser_state = HFP_PARSER_THIRD_ITEM;
            break;
        case HFP_PARSER_THIRD_ITEM:
            if (hfp_connection->command == HFP_CMD_RETRIEVE_AG_INDICATORS){
                hfp_connection->parser_state = HFP_PARSER_CMD_SEQUENCE;
                break;
            }
            hfp_connection->parser_state = HFP_PARSER_CMD_HEADER;
            break;
    }
}

void hfp_parse(hfp_connection_t * hfp_connection, uint8_t byte, int isHandsFree){
    // handle ATD<dial_string>;
    if (strncmp((const char*)hfp_connection->line_buffer, HFP_CALL_PHONE_NUMBER, strlen(HFP_CALL_PHONE_NUMBER)) == 0){
        // check for end-of-line or ';'
        if (byte == ';' || hfp_parser_is_end_of_line(byte)){
            hfp_connection->line_buffer[hfp_connection->line_size] = 0;
            hfp_connection->line_size = 0;
            hfp_connection->command = HFP_CMD_CALL_PHONE_NUMBER;
        } else {
            hfp_connection->line_buffer[hfp_connection->line_size++] = byte;
        }
        return;
    }

    // TODO: handle space inside word        
    if (byte == ' ' && hfp_connection->parser_state > HFP_PARSER_CMD_HEADER) return;
    
    if (byte == ',' && hfp_connection->command == HFP_CMD_ENABLE_INDIVIDUAL_AG_INDICATOR_STATUS_UPDATE){
        if (hfp_connection->line_size == 0){
            hfp_connection->line_buffer[0] = 0;
            hfp_connection->ignore_value = 1;
            parse_sequence(hfp_connection);
            return;    
        } 
    }

    if (!hfp_parser_found_separator(hfp_connection, byte)){
        hfp_parser_store_byte(hfp_connection, byte);
        return;
    } 

    if (hfp_parser_is_end_of_line(byte)) {
        if (hfp_parser_is_buffer_empty(hfp_connection)){
            hfp_connection->parser_state = HFP_PARSER_CMD_HEADER;
        }
    }
    if (hfp_parser_is_buffer_empty(hfp_connection)) return;

    switch (hfp_connection->parser_state){
        case HFP_PARSER_CMD_HEADER: // header
            if (byte == '='){
                hfp_connection->keep_byte = 1;
                hfp_parser_store_byte(hfp_connection, byte);
                return;
            }
            
            if (byte == '?'){
                hfp_connection->keep_byte = 0;
                hfp_parser_store_byte(hfp_connection, byte);
                return;
            }

            if (byte == ','){
                hfp_connection->resolve_byte = 1;
            }

            // printf(" parse header 2 %s, keep separator $ %d\n", hfp_connection->line_buffer, hfp_connection->keep_byte);
            if (hfp_parser_is_end_of_header(byte) || hfp_connection->keep_byte == 1){
                // printf(" parse header 3 %s, keep separator $ %d\n", hfp_connection->line_buffer, hfp_connection->keep_byte);
                char * line_buffer = (char *)hfp_connection->line_buffer;
                hfp_connection->command = parse_command(line_buffer, isHandsFree);
                
                /* resolve command name according to hfp_connection */
                if (hfp_connection->command == HFP_CMD_UNKNOWN){
                    switch(hfp_connection->state){
                        case HFP_W4_LIST_GENERIC_STATUS_INDICATORS:
                            hfp_connection->command = HFP_CMD_LIST_GENERIC_STATUS_INDICATORS;
                            break;
                        case HFP_W4_RETRIEVE_GENERIC_STATUS_INDICATORS:
                            hfp_connection->command = HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS;
                            break;
                        case HFP_W4_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS:
                            hfp_connection->command = HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS_STATE;
                            break;
                        case HFP_W4_RETRIEVE_INDICATORS_STATUS:
                            hfp_connection->command = HFP_CMD_RETRIEVE_AG_INDICATORS_STATUS;
                            break;
                        case HFP_W4_RETRIEVE_INDICATORS:
                            hfp_connection->send_ag_indicators_segment = 0;
                            hfp_connection->command = HFP_CMD_RETRIEVE_AG_INDICATORS;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;

        case HFP_PARSER_CMD_SEQUENCE: 
            parse_sequence(hfp_connection);
            break;
        case HFP_PARSER_SECOND_ITEM:
            switch (hfp_connection->command){
                case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME:
                    log_info("format %s, ", hfp_connection->line_buffer);
                    hfp_connection->network_operator.format =  atoi((char *)&hfp_connection->line_buffer[0]);
                    break;
                case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME_FORMAT:
                    log_info("format %s \n", hfp_connection->line_buffer);
                    hfp_connection->network_operator.format =  atoi((char *)&hfp_connection->line_buffer[0]);
                    break;
                case HFP_CMD_LIST_GENERIC_STATUS_INDICATORS:
                case HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS:
                case HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS_STATE:
                    hfp_connection->generic_status_indicators[hfp_connection->parser_item_index].state = (uint8_t)atoi((char*)hfp_connection->line_buffer);
                    break;
                case HFP_CMD_TRANSFER_AG_INDICATOR_STATUS:
                    hfp_connection->ag_indicators[hfp_connection->parser_item_index].status = (uint8_t)atoi((char*)hfp_connection->line_buffer);
                    log_info("%d \n", hfp_connection->ag_indicators[hfp_connection->parser_item_index].status);
                    hfp_connection->ag_indicators[hfp_connection->parser_item_index].status_changed = 1;
                    break;
                case HFP_CMD_RETRIEVE_AG_INDICATORS:
                    hfp_connection->ag_indicators[hfp_connection->parser_item_index].min_range = atoi((char *)hfp_connection->line_buffer);
                    log_info("%s, ", hfp_connection->line_buffer);
                    break;
                case HFP_CMD_AG_SENT_PHONE_NUMBER:
                case HFP_CMD_AG_SENT_CALL_WAITING_NOTIFICATION_UPDATE:
                case HFP_CMD_AG_SENT_CLIP_INFORMATION:
                    hfp_connection->bnip_type = (uint8_t)atoi((char*)hfp_connection->line_buffer);
                    break;
                default:
                    break;
            }
            break;

        case HFP_PARSER_THIRD_ITEM:
             switch (hfp_connection->command){
                case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME:
                    strcpy(hfp_connection->network_operator.name, (char *)hfp_connection->line_buffer);
                    log_info("name %s\n", hfp_connection->line_buffer);
                    break;
                case HFP_CMD_RETRIEVE_AG_INDICATORS:
                    hfp_connection->ag_indicators[hfp_connection->parser_item_index].max_range = atoi((char *)hfp_connection->line_buffer);
                    hfp_connection->parser_item_index++;
                    hfp_connection->ag_indicators_nr = hfp_connection->parser_item_index;
                    log_info("%s)\n", hfp_connection->line_buffer);
                    break;
                default:
                    break;
            }
            break;
    }
    hfp_parser_next_state(hfp_connection, byte);

    if (hfp_connection->resolve_byte && hfp_connection->command == HFP_CMD_ENABLE_INDIVIDUAL_AG_INDICATOR_STATUS_UPDATE){
        hfp_connection->resolve_byte = 0;
        hfp_connection->ignore_value = 1;
        parse_sequence(hfp_connection);
        hfp_connection->line_buffer[0] = 0;
        hfp_connection->line_size = 0;
    }
}

static void parse_sequence(hfp_connection_t * hfp_connection){
    int value;
    switch (hfp_connection->command){
        case HFP_CMD_SET_GENERIC_STATUS_INDICATOR_STATUS:
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            int i;
            switch (hfp_connection->parser_item_index){
                case 0:
                    for (i=0;i<hfp_connection->generic_status_indicators_nr;i++){
                        if (hfp_connection->generic_status_indicators[i].uuid == value){
                            hfp_connection->parser_indicator_index = i;
                            break;
                        }
                    }
                    break;
                case 1:
                    if (hfp_connection->parser_indicator_index <0) break;
                    hfp_connection->generic_status_indicators[hfp_connection->parser_indicator_index].state = value;
                    log_info("HFP_CMD_SET_GENERIC_STATUS_INDICATOR_STATUS set indicator at index %u, to %u\n",
                     hfp_connection->parser_item_index, value);
                    break;
                default:
                    break;
            }
            hfp_connection->parser_item_index++;
            break;

        case HFP_CMD_GET_SUBSCRIBER_NUMBER_INFORMATION:
            switch(hfp_connection->parser_item_index){
                case 0:
                    strncpy(hfp_connection->bnip_number, (char *)hfp_connection->line_buffer, sizeof(hfp_connection->bnip_number));
                    hfp_connection->bnip_number[sizeof(hfp_connection->bnip_number)-1] = 0;
                    break;
                case 1:
                    value = atoi((char *)&hfp_connection->line_buffer[0]);
                    hfp_connection->bnip_type = value;
                    break;
                default:
                    break;
            }
            hfp_connection->parser_item_index++;
            break;            
        case HFP_CMD_LIST_CURRENT_CALLS:
            switch(hfp_connection->parser_item_index){
                case 0:
                    value = atoi((char *)&hfp_connection->line_buffer[0]);
                    hfp_connection->clcc_idx = value;
                    break;
                case 1:
                    value = atoi((char *)&hfp_connection->line_buffer[0]);
                    hfp_connection->clcc_dir = value;
                    break;
                case 2:
                    value = atoi((char *)&hfp_connection->line_buffer[0]);
                    hfp_connection->clcc_status = value;
                    break;
                case 3:
                    value = atoi((char *)&hfp_connection->line_buffer[0]);
                    hfp_connection->clcc_mpty = value;
                    break;
                case 4:
                    strncpy(hfp_connection->bnip_number, (char *)hfp_connection->line_buffer, sizeof(hfp_connection->bnip_number));
                    hfp_connection->bnip_number[sizeof(hfp_connection->bnip_number)-1] = 0;
                    break;
                case 5:
                    value = atoi((char *)&hfp_connection->line_buffer[0]);
                    hfp_connection->bnip_type = value;
                    break;
                default:
                    break;
            }
            hfp_connection->parser_item_index++;
            break;
        case HFP_CMD_SET_MICROPHONE_GAIN:
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            hfp_connection->microphone_gain = value;
            log_info("hfp parse HFP_CMD_SET_MICROPHONE_GAIN %d\n", value);
            break;
        case HFP_CMD_SET_SPEAKER_GAIN:
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            hfp_connection->speaker_gain = value;
            log_info("hfp parse HFP_CMD_SET_SPEAKER_GAIN %d\n", value);
            break;
        case HFP_CMD_HF_ACTIVATE_VOICE_RECOGNITION:
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            hfp_connection->ag_activate_voice_recognition = value;
            log_info("hfp parse HFP_CMD_HF_ACTIVATE_VOICE_RECOGNITION %d\n", value);
            break;
        case HFP_CMD_TURN_OFF_EC_AND_NR:
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            hfp_connection->ag_echo_and_noise_reduction = value;
            log_info("hfp parse HFP_CMD_TURN_OFF_EC_AND_NR %d\n", value);
            break;
        case HFP_CMD_CHANGE_IN_BAND_RING_TONE_SETTING:
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            hfp_connection->remote_supported_features = store_bit(hfp_connection->remote_supported_features, HFP_AGSF_IN_BAND_RING_TONE, value);
            log_info("hfp parse HFP_CHANGE_IN_BAND_RING_TONE_SETTING %d\n", value);
            break;
        case HFP_CMD_HF_CONFIRMED_CODEC:
            hfp_connection->codec_confirmed = atoi((char*)hfp_connection->line_buffer);
            log_info("hfp parse HFP_CMD_HF_CONFIRMED_CODEC %d\n", hfp_connection->codec_confirmed);
            break;
        case HFP_CMD_AG_SUGGESTED_CODEC:
            hfp_connection->suggested_codec = atoi((char*)hfp_connection->line_buffer);
            log_info("hfp parse HFP_CMD_AG_SUGGESTED_CODEC %d\n", hfp_connection->suggested_codec);
            break;
        case HFP_CMD_SUPPORTED_FEATURES:
            hfp_connection->remote_supported_features = atoi((char*)hfp_connection->line_buffer);
            log_info("Parsed supported feature %d\n", hfp_connection->remote_supported_features);
            break;
        case HFP_CMD_AVAILABLE_CODECS:
            log_info("Parsed codec %s\n", hfp_connection->line_buffer);
            hfp_connection->remote_codecs[hfp_connection->parser_item_index] = (uint16_t)atoi((char*)hfp_connection->line_buffer);
            hfp_connection->parser_item_index++;
            hfp_connection->remote_codecs_nr = hfp_connection->parser_item_index;
            break;
        case HFP_CMD_RETRIEVE_AG_INDICATORS:
            strcpy((char *)hfp_connection->ag_indicators[hfp_connection->parser_item_index].name,  (char *)hfp_connection->line_buffer);
            hfp_connection->ag_indicators[hfp_connection->parser_item_index].index = hfp_connection->parser_item_index+1;
            log_info("Indicator %d: %s (", hfp_connection->ag_indicators_nr+1, hfp_connection->line_buffer);
            break;
        case HFP_CMD_RETRIEVE_AG_INDICATORS_STATUS:
            log_info("Parsed Indicator %d with status: %s\n", hfp_connection->parser_item_index+1, hfp_connection->line_buffer);
            hfp_connection->ag_indicators[hfp_connection->parser_item_index].status = atoi((char *) hfp_connection->line_buffer);
            hfp_connection->parser_item_index++;
            break;
        case HFP_CMD_ENABLE_INDICATOR_STATUS_UPDATE:
            hfp_connection->parser_item_index++;
            if (hfp_connection->parser_item_index != 4) break;
            log_info("Parsed Enable indicators: %s\n", hfp_connection->line_buffer);
            value = atoi((char *)&hfp_connection->line_buffer[0]);
            hfp_connection->enable_status_update_for_ag_indicators = (uint8_t) value;
            break;
        case HFP_CMD_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES:
            log_info("Parsed Support call hold: %s\n", hfp_connection->line_buffer);
            if (hfp_connection->line_size > 2 ) break;
            strcpy((char *)hfp_connection->remote_call_services[hfp_connection->remote_call_services_nr].name,  (char *)hfp_connection->line_buffer);
            hfp_connection->remote_call_services_nr++;
            break;
        case HFP_CMD_LIST_GENERIC_STATUS_INDICATORS:
        case HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS:
            log_info("Parsed Generic status indicator: %s\n", hfp_connection->line_buffer);
            hfp_connection->generic_status_indicators[hfp_connection->parser_item_index].uuid = (uint16_t)atoi((char*)hfp_connection->line_buffer);
            hfp_connection->parser_item_index++;
            hfp_connection->generic_status_indicators_nr = hfp_connection->parser_item_index;
            break;
        case HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS_STATE:
            // HF parses inital AG gen. ind. state
            log_info("Parsed List generic status indicator %s state: ", hfp_connection->line_buffer);
            hfp_connection->parser_item_index = (uint8_t)atoi((char*)hfp_connection->line_buffer);
            break;
        case HFP_CMD_HF_INDICATOR_STATUS:
            hfp_connection->parser_indicator_index = (uint8_t)atoi((char*)hfp_connection->line_buffer);
            log_info("Parsed HF indicator index %u", hfp_connection->parser_indicator_index);
            break;
        case HFP_CMD_ENABLE_INDIVIDUAL_AG_INDICATOR_STATUS_UPDATE:
            // AG parses new gen. ind. state
            if (hfp_connection->ignore_value){
                hfp_connection->ignore_value = 0;
                log_info("Parsed Enable AG indicator pos %u('%s') - unchanged (stays %u)\n", hfp_connection->parser_item_index,
                    hfp_connection->ag_indicators[hfp_connection->parser_item_index].name, hfp_connection->ag_indicators[hfp_connection->parser_item_index].enabled);
            }
            else if (hfp_connection->ag_indicators[hfp_connection->parser_item_index].mandatory){
                log_info("Parsed Enable AG indicator pos %u('%s') - ignore (mandatory)\n", 
                    hfp_connection->parser_item_index, hfp_connection->ag_indicators[hfp_connection->parser_item_index].name);
            } else {
                value = atoi((char *)&hfp_connection->line_buffer[0]);
                hfp_connection->ag_indicators[hfp_connection->parser_item_index].enabled = value;
                log_info("Parsed Enable AG indicator pos %u('%s'): %u\n", hfp_connection->parser_item_index,
                    hfp_connection->ag_indicators[hfp_connection->parser_item_index].name, value);
            }
            hfp_connection->parser_item_index++;
            break;
        case HFP_CMD_TRANSFER_AG_INDICATOR_STATUS:
            // indicators are indexed starting with 1
            hfp_connection->parser_item_index = atoi((char *)&hfp_connection->line_buffer[0]) - 1;
            log_info("Parsed status of the AG indicator %d, status ", hfp_connection->parser_item_index);
            break;
        case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME:
            hfp_connection->network_operator.mode = atoi((char *)&hfp_connection->line_buffer[0]);
            log_info("Parsed network operator mode: %d, ", hfp_connection->network_operator.mode);
            break;
        case HFP_CMD_QUERY_OPERATOR_SELECTION_NAME_FORMAT:
            if (hfp_connection->line_buffer[0] == '3'){
                log_info("Parsed Set network operator format : %s, ", hfp_connection->line_buffer);
                break;
            }
            // TODO emit ERROR, wrong format
            log_info("ERROR Set network operator format: index %s not supported\n", hfp_connection->line_buffer);
            break;
        case HFP_CMD_ERROR:
            break;
        case HFP_CMD_EXTENDED_AUDIO_GATEWAY_ERROR:
            hfp_connection->extended_audio_gateway_error = 1;
            hfp_connection->extended_audio_gateway_error_value = (uint8_t)atoi((char*)hfp_connection->line_buffer);
            break;
        case HFP_CMD_ENABLE_EXTENDED_AUDIO_GATEWAY_ERROR:
            hfp_connection->enable_extended_audio_gateway_error_report = (uint8_t)atoi((char*)hfp_connection->line_buffer);
            hfp_connection->ok_pending = 1;
            hfp_connection->extended_audio_gateway_error = 0;
            break;
        case HFP_CMD_AG_SENT_PHONE_NUMBER:
        case HFP_CMD_AG_SENT_CALL_WAITING_NOTIFICATION_UPDATE:
        case HFP_CMD_AG_SENT_CLIP_INFORMATION:
            strncpy(hfp_connection->bnip_number, (char *)hfp_connection->line_buffer, sizeof(hfp_connection->bnip_number));
            hfp_connection->bnip_number[sizeof(hfp_connection->bnip_number)-1] = 0;
            break;
        default:
            break;
    }  
}

void hfp_establish_service_level_connection(bd_addr_t bd_addr, uint16_t service_uuid){
    hfp_connection_t * hfp_connection = provide_hfp_connection_context_for_bd_addr(bd_addr);
    log_info("hfp_connect %s, hfp_connection %p", bd_addr_to_str(bd_addr), hfp_connection);
    
    if (!hfp_connection) {
        log_error("hfp_establish_service_level_connection for addr %s failed", bd_addr_to_str(bd_addr));
        return;
    }

    switch (hfp_connection->state){
        case HFP_W2_DISCONNECT_RFCOMM:
            hfp_connection->state = HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED;
            return;
        case HFP_W4_RFCOMM_DISCONNECTED:
            hfp_connection->state = HFP_W4_RFCOMM_DISCONNECTED_AND_RESTART;
            return;
        case HFP_IDLE:
            memcpy(hfp_connection->remote_addr, bd_addr, 6);
            hfp_connection->state = HFP_W4_SDP_QUERY_COMPLETE;
            connection_doing_sdp_query = hfp_connection;
            hfp_connection->service_uuid = service_uuid;
            sdp_client_query_rfcomm_channel_and_name_for_uuid(&handle_query_rfcomm_event, hfp_connection->remote_addr, service_uuid);
            break;
        default:
            break;
    }
}

void hfp_release_service_level_connection(hfp_connection_t * hfp_connection){
    if (!hfp_connection) return;
    hfp_release_audio_connection(hfp_connection);

    if (hfp_connection->state < HFP_W4_RFCOMM_CONNECTED){
        hfp_connection->state = HFP_IDLE;
        return;
    }

    if (hfp_connection->state == HFP_W4_RFCOMM_CONNECTED){
        hfp_connection->state = HFP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN;
        return;
    }

    if (hfp_connection->state < HFP_W4_SCO_CONNECTED){
        hfp_connection->state = HFP_W2_DISCONNECT_RFCOMM;
        return;
    }

    if (hfp_connection->state < HFP_W4_SCO_DISCONNECTED){
        hfp_connection->state = HFP_W2_DISCONNECT_SCO;
        return;
    }

    return;
}

void hfp_release_audio_connection(hfp_connection_t * hfp_connection){
    if (!hfp_connection) return;
    if (hfp_connection->state >= HFP_W2_DISCONNECT_SCO) return;
    hfp_connection->release_audio_connection = 1; 
}

static const struct link_settings {
    const uint16_t max_latency;
    const uint8_t  retransmission_effort;
    const uint16_t packet_types;
} hfp_link_settings [] = {
    { 0xffff, 0xff, 0x03c1 }, // HFP_LINK_SETTINGS_D0,   HV1
    { 0xffff, 0xff, 0x03c4 }, // HFP_LINK_SETTINGS_D1,   HV3
    { 0x0007, 0x01, 0x03c8 }, // HFP_LINK_SETTINGS_S1,   EV3
    { 0x0007, 0x01, 0x0380 }, // HFP_LINK_SETTINGS_S2, 2-EV3
    { 0x000a, 0x01, 0x0380 }, // HFP_LINK_SETTINGS_S3, 2-EV3
    { 0x000c, 0x02, 0x0380 }, // HFP_LINK_SETTINGS_S4, 2-EV3
    { 0x0008, 0x02, 0x03c8 }, // HFP_LINK_SETTINGS_T1,   EV3
    { 0x000d, 0x02, 0x0380 }  // HFP_LINK_SETTINGS_T2, 2-EV3
};

void hfp_setup_synchronous_connection(hfp_connection_t * hfp_connection){
    // all packet types, fixed bandwidth
    int setting = hfp_connection->link_setting;
    log_info("hfp_setup_synchronous_connection using setting nr %u", setting);
    sco_establishment_active = hfp_connection;
    hci_send_cmd(&hci_setup_synchronous_connection, hfp_connection->acl_handle, 8000, 8000, hfp_link_settings[setting].max_latency,
        hci_get_sco_voice_setting(), hfp_link_settings[setting].retransmission_effort, hfp_link_settings[setting].packet_types); // all types 0x003f, only 2-ev3 0x380
}

void hfp_set_packet_handler_for_rfcomm_connections(btstack_packet_handler_t handler){
    rfcomm_packet_handler = handler;
}

#endif