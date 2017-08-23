/*
 * my_audio.h
 *
 *  Created on: 2017-8-23
 *      Author: Administrator
 */

#ifndef MY_AUDIO_H_
#define MY_AUDIO_H_
#include "../../proj/tl_common.h"

#ifndef		TL_NOISE_SUPRESSION_ENABLE
#define		TL_NOISE_SUPRESSION_ENABLE			0
#endif

#ifndef		TL_MIC_PACKET_BUFFER_NUM
#define		TL_MIC_PACKET_BUFFER_NUM		4
#endif

#ifndef 	ADPCM_PACKET_LEN
#define 	ADPCM_PACKET_LEN		0
#endif

#ifndef 	TL_MIC_ADPCM_UNIT_SIZE
#define 	TL_MIC_ADPCM_UNIT_SIZE		0
#endif

#ifndef		TL_MIC_BUFFER_SIZE
#define		TL_MIC_BUFFER_SIZE		0
#endif

#ifndef		TL_SDM_BUFFER_SIZE
#define		TL_SDM_BUFFER_SIZE		0
#endif

#if TL_MIC_BUFFER_SIZE
s16		buffer_mic[TL_MIC_BUFFER_SIZE>>1];
#endif

#if TL_SDM_BUFFER_SIZE
s16		buffer_sdm[TL_SDM_BUFFER_SIZE>>1];
#endif

void mypcm_to_adpcm (signed short *ps, int len, signed short *pd);
void mymic_to_adpcm (int *ps, int len, signed short *pd);
void myadpcm_to_pcm (signed short *ps, signed short *pd, int len);
void myadpcm_to_sdm (signed short *ps, int len);
void mysilence_to_sdm (void);

void	myproc_mic_encoder (void);
int		mymic_encoder_data_ready (int *pd);
int	*	mymic_encoder_data_buffer ();

void 	myproc_sdm_decoder (void);
int  	mysdm_decode_data (int *ps, int nbyte);
void 	mysdm_decode_rate (int step, int adj);
int   	mysdm_bytes_in_buffer ();

#endif /* MY_AUDIO_H_ */
