#pragma  once



#define IRQ_USE_AES_CCM_ENABLE			1

#define MAINLOOP_USING					BIT(0)
#define IRQ_APPLY_USE					BIT(7)
extern  volatile u8 aes_ccm_using;


#define AES_BLOCK_SIZE     16


#define		SUCCESS			0
enum {
    AES_SUCC = SUCCESS,
    AES_NO_BUF,               
    AES_FAIL,
};


struct CCM_FLAGS_TAG {
    union {
        struct {
            u8 L : 3;
            u8 M : 3;
            u8 aData :1;
            u8 reserved :1;            
        } bf;
        u8 val;
    };
};

typedef struct CCM_FLAGS_TAG ccm_flags_t;


typedef struct {
    union {
        u8 A[AES_BLOCK_SIZE];
        u8 B[AES_BLOCK_SIZE];
    } bf;
    
    u8 tmpResult[AES_BLOCK_SIZE];
    u8 newAstr[AES_BLOCK_SIZE];
} aes_enc_t;

u8 aes_ccmAuthTran(u8 micLen, u8 *key, u8 *iv, u8 *mStr, u16 mStrLen, u8 *aStr, u16 aStrLen, u8 *result);
u8 aes_ccmDecAuthTran(u8 micLen, u8 *key, u8 *iv, u8 *mStr, u16 mStrLen, u8 *aStr, u8 aStrLen, u8 *mic);
u8 aes_initKey(u8 *key);
u8 aes_encrypt(u8 *key, u8 *data, u8 *result);

u8 aes_ccm_encryption(u8 *key, u8 *iv, u8 *aStr, u8 *mic, u8 mStrLen, u8 *mStr, u8 *result);
u8 aes_ccm_decryption(u8 *key, u8 *iv, u8 *aStr, u8 *mic, u8 mStrLen, u8 *mStr, u8 *result);
u8 aes_ecb_encryption(u8 *key, u8 len, u8 *plaintext_flipped, u8 *result);
void aes_ll_encryption(u8 *key, u8 *plaintext, u8 *result);
void aes_ll_ccm_encryption_init (u8 *ltk, u8 *skdm, u8 *skds, u8 *ivm, u8 *ivs, ble_crypt_para_t *pd);
void aes_ll_ccm_encryption(u8 *pkt, int master, ble_crypt_para_t *pd);
int aes_ll_ccm_decryption(u8 *pkt, int master, ble_crypt_para_t *pd);		//OK return 0
u8 aes_att_encryption_packet(u8 *key, u8 *iv, u8 *mic, u8 mic_len, u8 *ps, u8 len);
u8 aes_att_decryption_packet(u8 *key, u8 *iv, u8 *mic, u8 mic_len, u8 *ps, u8 len);

