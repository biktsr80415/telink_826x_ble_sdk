BLE_SUCCESS = 0


--- hci_type_t
HCI_TYPE_CMD = 0x01
HCI_TYPE_ACL_DATA = 0x02
HCI_TYPE_SCO_DATA = 0x03
HCI_TYPE_EVENT = 0x04

	

--- hci event
 HCI_CMD_DISCONNECTION_COMPLETE                               = 0x05
 HCI_EVT_ENCRYPTION_CHANGE                                    = 0x08
 HCI_EVT_CHANGE_LINK_KEY_COMPLETE							 = 0x09
 HCI_EVT_READ_REMOTE_VER_INFO_COMPLETE                        = 0x0C
 HCI_EVT_CMD_COMPLETE                                         = 0x0E
 HCI_EVT_CMD_STATUS                                           = 0x0F
 HCI_EVT_HW_ERROR                                             = 0x10
 HCI_EVT_NUM_OF_COMPLETE_PACKETS                              = 0x13
 HCI_EVT_DATA_BUF_OVERFLOW                                    = 0x1A
 HCI_EVT_ENCRYPTION_KEY_REFRESH                               = 0x30
 HCI_EVT_LE_META                                              = 0x3E
 HCI_EVT_CERT_VS                                              = 0xF0

---- LE Meta Event Codes
 HCI_SUB_EVT_LE_CONNECTION_COMPLETE                           = 0x01
 HCI_SUB_EVT_LE_ADVERTISING_REPORT                            = 0x02
 HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE                    = 0x03
 HCI_SUB_EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE            = 0x04
 HCI_SUB_EVT_LE_LONG_TERM_KEY_REQUESTED                       = 0x05   ----core_4.0
 HCI_SUB_EVT_LE_REMOTE_CONNECTION_PARAM_REQUEST               = 0x06   ----core_4.1
 HCI_SUB_EVT_LE_DATA_LENGTH_CHANGE                            = 0x07
 HCI_SUB_EVT_LE_READ_LOCAL_P256_KEY_COMPLETE                  = 0x08
 HCI_SUB_EVT_LE_GENERATE_DHKEY_COMPLETE				         = 0x09
 HCI_SUB_EVT_LE_ENHANCED_CONNECTION_COMPLETE                  = 0x0A
 HCI_SUB_EVT_LE_DIRECT_ADVERTISE_REPORT		                 = 0x0B   ----core_4.2



---- LE Event mask - last octet
 HCI_LE_EVT_MASK_NONE                                         = 0x000
 HCI_LE_EVT_MASK_CONNECTION_COMPLETE                          = 0x001
 HCI_LE_EVT_MASK_ADVERTISING_REPORT                           = 0x002
 HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE                   = 0x004
 HCI_LE_EVT_MASK_READ_REMOTE_FEATURE                          = 0x008
 HCI_LE_EVT_MASK_LONG_TERM_KEY_REQUEST                        = 0x010
 HCI_LE_EVT_MASK_REMOTE_CONNECTION_PARAM_REQUEST              = 0x020
 HCI_LE_EVT_MASK_DATA_LENGTH_CHANGE                        	 = 0x040
 HCI_LE_EVT_MASK_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE          = 0x080
 HCI_LE_EVT_MASK_GENERATE_DHKEY_COMPLETE                      = 0x100
 HCI_LE_EVT_MASK_ENHANCED_CONNECTION_COMPLETE                 = 0x200
 HCI_LE_EVT_MASK_DIRECT_ADVERTISING_REPORT                    = 0x400

 HCI_LE_EVT_MASK_DEFAULT                                      = 0x1F


 HCI_CMD_LE_OPCODE_OGF										 = 0x20  ----= 0x08 <<2 = = 0x20

---- LE Controller Commands
 HCI_CMD_LE_SET_EVENT_MASK                                    = 0x01
 HCI_CMD_LE_READ_BUF_SIZE                                     = 0x02
 HCI_CMD_LE_READ_LOCAL_SUPPORTED_FEATURES                     = 0x03
 HCI_CMD_LE_SET_RANDOM_ADDR                                   = 0x05
 HCI_CMD_LE_SET_ADVERTISE_PARAMETERS                          = 0x06
 HCI_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER                 = 0x07
 HCI_CMD_LE_SET_ADVERTISE_DATA                                = 0x08
 HCI_CMD_LE_SET_SCAN_RSP_DATA                                 = 0x09
 HCI_CMD_LE_SET_ADVERTISE_ENABLE                              = 0x0A
 HCI_CMD_LE_SET_SCAN_PARAMETERS                               = 0x0B
 HCI_CMD_LE_SET_SCAN_ENABLE                                   = 0x0C
 HCI_CMD_LE_CREATE_CONNECTION                                 = 0x0D
 HCI_CMD_LE_CREATE_CONNECTION_CANCEL                          = 0x0E
 HCI_CMD_LE_READ_WHITE_LIST_SIZE                              = 0x0F
 HCI_CMD_LE_CLEAR_WHITE_LIST                                  = 0x10
 HCI_CMD_LE_ADD_DEVICE_TO_WHITE_LIST                          = 0x11
 HCI_CMD_LE_REMOVE_DEVICE_FROM_WL                             = 0x12
 HCI_CMD_LE_CONNECTION_UPDATE                                 = 0x13
 HCI_CMD_LE_SET_HOST_CHANNEL_CLASSIFICATION                   = 0x14
 HCI_CMD_LE_READ_CHANNEL_MAP                                  = 0x15
 HCI_CMD_LE_READ_REMOTE_USED_FEATURES                         = 0x16
 HCI_CMD_LE_ENCRYPT                                           = 0x17
 HCI_CMD_LE_RANDOM                                            = 0x18
 HCI_CMD_LE_START_ENCRYPTION                                  = 0x19
 HCI_CMD_LE_LONG_TERM_KEY_REQUESTED_REPLY                     = 0x1A
 HCI_CMD_LE_LONG_TERM_KEY_REQUESTED_NEGATIVE_REPLY            = 0x1B
 HCI_CMD_LE_READ_SUPPORTED_STATES                             = 0x1C
 HCI_CMD_LE_RECEIVER_TEST                                     = 0x1D
 HCI_CMD_LE_TRANSMITTER_TEST                                  = 0x1E
 HCI_CMD_LE_TEST_END                                          = 0x1F
----core_4.0 above
 HCI_CMD_LE_REMOTE_CONNECTION_PARAM_REQ_REPLY             	 = 0x20
 HCI_CMD_LE_REMOTE_CONNECTION_PARAM_REQ_NEGATIVE_REPLY    	 = 0x21
----core_4.1 above
 HCI_CMD_LE_SET_DATA_LENGTH			                         = 0x22
 HCI_CMD_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH                = 0x23
 HCI_CMD_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH               = 0x24
 HCI_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY                        = 0x25
 HCI_CMD_LE_GENERATE_DHKEY_COMPLETE                         	 = 0x26
 HCI_CMD_LE_ADD_DEVICE_TO_RESOLVING_LIST                      = 0x27
 HCI_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                 = 0x28
 HCI_CMD_LE_CLEAR_RESOLVING_LIST                              = 0x29
 HCI_CMD_LE_READ_RESOLVING_LIST_SIZE                     	 = 0x2A
 HCI_CMD_LE_READ_PEER_RESOLVABLE_ADDRESS            			 = 0x2B
 HCI_CMD_LE_READ_LOCAL_RESOLVABLE_ADDRESS                     = 0x2C
 HCI_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE                     = 0x2D
 HCI_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT            = 0x2E
 HCI_CMD_LE_READ_MAX_DATA_LENGTH                              = 0x2F
----core_4.2 above

