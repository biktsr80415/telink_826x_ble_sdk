

----------------------- BLE basic ----------------------- 
LL_ROLE_MASTER     =         0
LL_ROLE_SLAVE      =         1

BLM_CONN_HANDLE = 0x80
BLS_CONN_HANDLE = 0x40


ADV_ENABLE	= 1
ADV_DISABLE	= 0
----------------------- ADV interval ----------------------- 
ADV_INTERVAL_3_125MS =                       5
ADV_INTERVAL_3_75MS =                        6
ADV_INTERVAL_10MS =                          16
ADV_INTERVAL_20MS =                          32
ADV_INTERVAL_30MS =                          48
ADV_INTERVAL_100MS =                         160


----------------------- ADV Channel ----------------------- 
BLT_ENABLE_ADV_37	=		0x01
BLT_ENABLE_ADV_38	=		0x02
BLT_ENABLE_ADV_39	=		0x04
BLT_ENABLE_ADV_ALL	=		0x07

----------------------- Advertisement Type ----------------------- 
ADV_TYPE_CONNECTABLE_UNDIRECTED             = 0x00
ADV_TYPE_CONNECTABLE_DIRECTED_HIGH_DUTY     = 0x01
ADV_TYPE_SCANNABLE_UNDIRECTED               = 0x02
ADV_TYPE_NONCONNECTABLE_UNDIRECTED          = 0x03
ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY      = 0x04



-- Advertise channel PDU Type
	LL_TYPE_ADV_IND = 0x00
	LL_TYPE_ADV_DIRECT_IND = 0x01
	LL_TYPE_ADV_NONCONN_IND = 0x02
	LL_TYPE_SCAN_REQ = 0x03
	LL_TYPE_SCAN_RSP = 0x04
	LL_TYPE_CONNNECT_REQ = 0x05
	LL_TYPE_ADV_SCAN_IND = 0x06

 ----------------------- Device Address Type ----------------------- 
BLE_ADDR_PUBLIC     =             0
BLE_ADDR_RANDOM      =            1
BLE_ADDR_STATIC       =           1
BLE_ADDR_PRIVATE_NONRESOLVE  =    2
BLE_ADDR_PRIVATE_RESOLVE     =    3
BLE_ADDR_INVALID             =    0xff


----------------------- own_addr_type ----------------------- 
OWN_ADDRESS_PUBLIC = 0
OWN_ADDRESS_RANDOM = 1
OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC = 2
OWN_ADDRESS_RESOLVE_PRIVATE_RANDOM = 3

----------------------- adv filter policy ----------------------- 
ALLOW_SCAN_WL	=							0x01
ALLOW_CONN_WL	=							0x02
ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_ANY   =     0x00  -- Process scan and connection requests from all devices
ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_ANY    =     0x01  -- Process connection requests from all devices and only scan requests from devices that are in the White List.
ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_WL    =     0x02  -- Process scan requests from all devices and only connection requests from devices that are in the White List..
ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL     =     0x03  -- Process scan and connection requests only from devices in the White List.

ADV_FP_NONE							=    ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_ANY



RxOctets_Max 		=  			40
TxOctets_Max 		=  			40
Rxtime_Max 			=  			432
Txtime_Max 			=  			432


----------------------- SCAN  ----------------------- 
SCAN_INTERVAL_30MS                =           48
SCAN_INTERVAL_60MS                =           96
SCAN_INTERVAL_70MS                =           112
SCAN_INTERVAL_90MS                =           144
SCAN_INTERVAL_100MS               =           160
SCAN_INTERVAL_200MS               =           320
SCAN_INTERVAL_300MS               =           480
SCAN_INTERVAL_330MS               =           528

SCAN_FP_ALLOW_ADV_ANY			=			0x00
SCAN_FP_ALLOW_ADV_WL        	=			0x01


SCAN_TYPE_PASSIVE = 0
SCAN_TYPE_ACTIVE  = 1

SCAN_ENABLE		= 1
SCAN_DISABLE	= 0

FILTER_DUPLICATE_ENABLE	= 1
FILTER_DUPLICATE_DISABLE	= 0



INITIATE_FP_ADV_ANY        			=		0x00
INITIATE_FP_ADV_WL         			=		0x01


--------------- ble_sts_t ----------------------------------------------------

	BLE_SUCCESS = 0
 
    HCI_ERR_UNKNOWN_HCI_CMD                                        = 0x01
    HCI_ERR_UNKNOWN_CONN_ID                                        = 0x02
    HCI_ERR_HW_FAILURE                                             = 0x03
    HCI_ERR_PAGE_TIMEOUT                                           = 0x04
    HCI_ERR_AUTH_FAILURE                                           = 0x05
    HCI_ERR_PIN_KEY_MISSING                                        = 0x06
    HCI_ERR_MEM_CAP_EXCEEDED                                       = 0x07
    HCI_ERR_CONN_TIMEOUT                                           = 0x08
    HCI_ERR_CONN_LIMIT_EXCEEDED                                    = 0x09
    HCI_ERR_SYNCH_CONN_LIMIT_EXCEEDED                              = 0x0A
    HCI_ERR_ACL_CONN_ALREADY_EXISTS                                = 0x0B
    HCI_ERR_CMD_DISALLOWED                                         = 0x0C
    HCI_ERR_CONN_REJ_LIMITED_RESOURCES                             = 0x0D
    HCI_ERR_CONN_REJECTED_SECURITY_REASONS                         = 0x0E
    HCI_ERR_CONN_REJECTED_UNACCEPTABLE_BDADDR                      = 0x0F
    HCI_ERR_CONN_ACCEPT_TIMEOUT_EXCEEDED                           = 0x10
    HCI_ERR_UNSUPPORTED_FEATURE_PARAM_VALUE                        = 0x11
    HCI_ERR_INVALID_HCI_CMD_PARAMS                                 = 0x12
    HCI_ERR_REMOTE_USER_TERM_CONN                                  = 0x13
    HCI_ERR_REMOTE_DEVICE_TERM_CONN_LOW_RESOURCES                  = 0x14
    HCI_ERR_REMOTE_DEVICE_TERM_CONN_POWER_OFF                      = 0x15
    HCI_ERR_CONN_TERM_BY_LOCAL_HOST                                = 0x16
    HCI_ERR_REPEATED_ATTEMPTS                                      = 0x17
    HCI_ERR_PAIRING_NOT_ALLOWED                                    = 0x18
    HCI_ERR_UNKNOWN_LMP_PDU                                        = 0x19
    HCI_ERR_UNSUPPORTED_REMOTE_FEATURE                             = 0x1A
    HCI_ERR_SCO_OFFSET_REJ                                         = 0x1B
    HCI_ERR_SCO_INTERVAL_REJ                                       = 0x1C
    HCI_ERR_SCO_AIR_MODE_REJ                                       = 0x1D
    HCI_ERR_INVALID_LMP_PARAMS                                     = 0x1E
    HCI_ERR_UNSPECIFIED_ERROR                                      = 0x1F
    HCI_ERR_UNSUPPORTED_LMP_PARAM_VAL                              = 0x20
    HCI_ERR_ROLE_CHANGE_NOT_ALLOWED                                = 0x21
    HCI_ERR_LMP_LL_RESP_TIMEOUT                                    = 0x22
    HCI_ERR_LMP_ERR_TRANSACTION_COLLISION                          = 0x23
    HCI_ERR_LMP_PDU_NOT_ALLOWED                                    = 0x24
    HCI_ERR_ENCRYPT_MODE_NOT_ACCEPTABLE                            = 0x25
    HCI_ERR_LINK_KEY_CAN_NOT_BE_CHANGED                            = 0x26
    HCI_ERR_REQ_QOS_NOT_SUPPORTED                                  = 0x27
    HCI_ERR_INSTANT_PASSED                                         = 0x28
    HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED                    = 0x29
    HCI_ERR_DIFFERENT_TRANSACTION_COLLISION                        = 0x2A
    HCI_ERR_RESERVED1                                              = 0x2B
    HCI_ERR_QOS_UNACCEPTABLE_PARAM                                 = 0x2C
    HCI_ERR_QOS_REJ                                                = 0x2D
    HCI_ERR_CHAN_ASSESSMENT_NOT_SUPPORTED                          = 0x2E
    HCI_ERR_INSUFFICIENT_SECURITY                                  = 0x2F
    HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE                           = 0x30
    HCI_ERR_RESERVED2                                              = 0x31
    HCI_ERR_ROLE_SWITCH_PENDING                                    = 0x32
    HCI_ERR_RESERVED3                                              = 0x33
    HCI_ERR_RESERVED_SLOT_VIOLATION                                = 0x34
    HCI_ERR_ROLE_SWITCH_FAILED                                     = 0x35
    HCI_ERR_EXTENDED_INQUIRY_RESP_TOO_LARGE                        = 0x36
    HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST                   = 0x37
    HCI_ERR_HOST_BUSY_PAIRING                                      = 0x38
    HCI_ERR_CONN_REJ_NO_SUITABLE_CHAN_FOUND                        = 0x39
    HCI_ERR_CONTROLLER_BUSY                                        = 0x3A
    HCI_ERR_UNACCEPTABLE_CONN_INTERVAL                             = 0x3B
    HCI_ERR_DIRECTED_ADV_TIMEOUT                                   = 0x3C
    HCI_ERR_CONN_TERM_MIC_FAILURE                                  = 0x3D
    HCI_ERR_CONN_FAILED_TO_ESTABLISH                               = 0x3E
    HCI_ERR_MAC_CONN_FAILED                                        = 0x3F
   