
//#include "weixin.h"

#include "../../proj/tl_common.h"


#ifndef			AES_SOFTWARE_MODE
	#if _WIN32
		#define			AES_SOFTWARE_MODE			1
	#else
		#define			AES_SOFTWARE_MODE			0
	#endif
#endif		// !AES_SOFTWARE_MODE

#if  AES_SOFTWARE_MODE
void aes_block_encryption(unsigned char *key, unsigned char *plaintext, unsigned char *result)
{
	unsigned char sk[16];
	int i;
	for (i=0; i<16; i++)
	{
		sk[i] = key[i]; //key[15 - i];
	}
	_rijndaelSetKey (sk);

	for (i=0; i<16; i++)
	{
		sk[i] = plaintext[i]; //plaintext[15 - i];
	}
	_rijndaelEncrypt (sk);

	 memcpy (result, sk, 16);
}

void aes_block_decryption(unsigned char *key, unsigned char *pin, unsigned char *result)
{
	unsigned char sk[16];
	int i;
	for (i=0; i<16; i++)
	{
		sk[i] = key[i]; //key[15 - i];
	}
	_rijndaelSetKey (sk);

	for (i=0; i<16; i++)
	{
		sk[i] = pin[i]; //pin[15 - i];
	}
	_rijndaelDecrypt (sk);

	 memcpy (result, sk, 16);
}

#else				// hardware implementation

void aes_block_encryption(u8 *key, u8 *plaintext, u8 *result)
{
	int i;

	reg_aes_ctrl = 0x00;

	u16 aesKeyStart = 0x550;
	for (i=0; i<16; i++) {
		REG_ADDR8(aesKeyStart + i) = key[i]; //key[15 - i];
	}

    u8 *ptr = plaintext;

    /* feed the data */
    for (int i=0; i<4; i++)
    {
		reg_aes_data = (ptr[0]) | (ptr[1]<<8) | (ptr[2]<<16) | (ptr[3]<<24);
    	ptr += 4;
    }

    /* start encrypt */

    /* wait for aes ready */
    while ( !(reg_aes_ctrl & BIT(2)) );

    /* read out the result */
    ptr = result;
    for (i=0; i<4; i++) {
        *((u32*)ptr) = reg_aes_data;
        ptr += 4;
    }
}

void aes_block_decryption(u8 *key, u8 *plaintext, u8 *result)
{
	int i;

	reg_aes_ctrl = 0x01;

	u16 aesKeyStart = 0x550;
	for (i=0; i<16; i++) {
		REG_ADDR8(aesKeyStart + i) = key[i]; //key[15 - i];
	}

    u8 *ptr = plaintext;

    /* feed the data */
    for (int i=0; i<4; i++)
    {
    	//reg_aes_data = (ptr[3]) | (ptr[2]<<8) | (ptr[1]<<16) | (ptr[0]<<24);
		reg_aes_data = (ptr[0]) | (ptr[1]<<8) | (ptr[2]<<16) | (ptr[3]<<24);
    	ptr += 4;
    }

    /* start encrypt */

    /* wait for aes ready */
    while ( !(reg_aes_ctrl & BIT(2)) );

    /* read out the result */
    ptr = result;
    for (i=0; i<4; i++) {
        *((u32*)ptr) = reg_aes_data;
        ptr += 4;
    }
}

#endif

unsigned int aes_cbc_pks7_enc(const unsigned char *pPlainText, unsigned char *pCipherText, unsigned int nDataLen, const unsigned char *key)
{
	unsigned char *pIV = (unsigned char *)key;
	unsigned int i, j;
	unsigned char p;
	unsigned int length = nDataLen;
	nDataLen = ((nDataLen>>4) + 1)<<4;
	p=16-(length&15);
	if (pPlainText != pCipherText)
	{
		memcpy(pCipherText,pPlainText,length);
	}
	
	if(length<nDataLen)
	{
		memset(pCipherText + length, p, nDataLen - length);
	}
	if(length == nDataLen)
	{
		memset(pCipherText + length, p,16);
	}

	for (i = nDataLen>>4; i > 0 ; i--, pCipherText += 16)
	{
		for (j=0; j<16; j++)
		{
			pCipherText[j] ^= pIV[j];
		}
		
		aes_block_encryption ((unsigned char *)key, pCipherText, pCipherText);
		pIV = pCipherText;
	}
	return(nDataLen);
}

int aes_cbc_pks7_dec(unsigned char *pPlainText, const unsigned char *pCipherText, unsigned int nDataLen, const unsigned char *key)
{
	unsigned int i, j;
	int n = -1;

	if (pPlainText != pCipherText)
	{
		memcpy(pPlainText, pCipherText, nDataLen);
	}

	pPlainText += nDataLen - 16;
	for (i = nDataLen>>4; i > 0 ; i--, pPlainText -= 16)
	{
		aes_block_decryption ((unsigned char *)key, pPlainText, pPlainText);

		if (i == 1)
		{// 最后一块数据
			for (j=0; j<16; j++)
			{
				pPlainText[j] ^= key[j];
			}
			if (nDataLen == 16)
			{
				n = pPlainText[15] > 16 ? 0 : 16 - pPlainText[15];
			}
		}
		else
		{
			for (j=0; j<16; j++)
			{
				pPlainText[j] ^= pPlainText[j - 16];
			}
			if (n < 0)
			{
				n = pPlainText[15] > 16 ? 0 : nDataLen - pPlainText[15];
			}
		}
	}
	return n;
}
