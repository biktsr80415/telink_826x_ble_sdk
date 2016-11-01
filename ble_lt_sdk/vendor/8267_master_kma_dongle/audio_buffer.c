#include "../../proj/tl_common.h"
#include "../common/tl_audio.h"
#include "trace.h"

#define			USB_ISO_IN_SIZE			(MIC_SAMPLE_RATE / 1000)

#define			PACK_POINTER				(abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)
/////////////////////////////////////////////////////////
//	abuf_mic:
//		abuf_mic_wptr: step by 2
//		abuf_dec_wptr: step by 8	(abuf_mic_rptr)
/////////////////////////////////////////////////////////
u8		abuf_mic_wptr, abuf_dec_wptr;
u16		abuf_dec_rptr;

#define DEC_BUFFER_SIZE		(MIC_SHORT_DEC_SIZE<<2)

u8		abuf_mic[MIC_ADPCM_FRAME_SIZE * 4];
s16		abuf_dec[DEC_BUFFER_SIZE];

int		abuf_reset = 0;


void abuf_init ()
{
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;
	log_event (TR_T_abuf_overflow);
}

void abuf_mic_add (u32 *p)
{
	log_event (TR_T_abuf_add);
	u32 *pd = (u32 *) (abuf_mic + (abuf_mic_wptr & 3) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE>>2); i++)
	{
		*pd ++ = *p++;
	}
	abuf_mic_wptr ++;
}
#if VIN_TEST_MODE
#define SIN_MODE 0
#define COS_MODE 1
#define PERCENT_HIHG 80
#define AUDIO_SAMPLE_TIMES 5
//int sin_64_tbl[17]={0,100,200,297,392,482,569,650,724,791,851,903,946,980,1004,1019,1024};

int sin_64_tbl[64]={0,100,200,297,392,482,569,650,724,791,851,903,946,980,1004,1019,
					1024,1019,1004,980,946,903,851,791,724,650,569,482,392,297,200,100,
					0,-100,-200,-297,-392,-482,-569,-650,-724,-791,-851,-903,-946,-980,-1004,-1019,
					-1024,-1019,-1004,-980,-946,-903,-851,-791,-724,-650,-569,-482,-392,-297,-200,-100};
int cos_64_tbl[64]={1024,1019,1004,980,946,903,851,791,724,650,569,482,392,297,200,100,
					0,-100,-200,-297,-392,-482,-569,-650,-724,-791,-851,-903,-946,-980,-1004,-1019,
					-1024,-1019,-1004,-980,-946,-903,-851,-791,-724,-650,-569,-482,-392,-297,-200,-100,
					0,100,200,297,392,482,569,650,724,791,851,903,946,980,1004,1019,
					};
					

u32 vin_sample_tick=0;
u8  vin_sample_OK_cnt =0;
u8  vin_sample_Fail_cnt =0;
//reserve for further use 
/*
_attribute_ram_code_ int calc_sin(int n,u8 sin_cos)
{
	//sin_cos =0;means sin //sin_cos =1 means cos 
	int calc_result;
	n = n%64;
	if(sin_cos==0){
		if(n<=16){
			calc_result = sin_64_tbl[n];
		}else if(n<=32){
			calc_result = sin_64_tbl[32-n];
		}else if(n<=48){
			calc_result =-sin_64_tbl[n-32];
		}else if(n<=64){
			calc_result =-sin_64_tbl[64-n];   
		}
	}
	if(sin_cos==1){
		if(n<=16){
			calc_result = sin_64_tbl[16-n];
		}else if(n<=32){
			calc_result = -sin_64_tbl[n-16];
		}else if (n<=48){
			calc_result = -sin_64_tbl[48-n];
		}else if (n<=64){
			calc_result = sin_64_tbl[n-48];
		}
	}
	return calc_result;
	
}
*/
u32 fourier_trans (short int *pSample ,u16 sample_cnt,u8 DFT_Frequence ,u8 source_frequence)
{
	//use the DFT_Frequence to get the function 
	static u8 k;
	u8 i;
	u8 max_sample=0;
	k= (sample_cnt/source_frequence)*DFT_Frequence;
	//k=DFT_Frequence;
	//DFT dispatch 
	int result_dft=0;
	int dft_result_tmp_real=0;
	int dft_result_tmp_vir =0;
	u32 fourier_power =0;
	//for the peroid cyc
	for(i=0;i<sample_cnt;i++){
		dft_result_tmp_real += (int)pSample[i]*cos_64_tbl[(i*k)&0x3f];
		dft_result_tmp_vir  += (int)pSample[i]*sin_64_tbl[(i*k)&0x3f];
	}
	dft_result_tmp_real =abs(dft_result_tmp_real)>>14;
	dft_result_tmp_vir =abs(dft_result_tmp_vir)>>14;
	fourier_power = dft_result_tmp_real*dft_result_tmp_real+dft_result_tmp_vir*dft_result_tmp_vir;
	fourier_power = fourier_power>>5;//the fourier power is mutiple about 64 ,and the -1khz is the same with 1k 
	return fourier_power; 
}
u32 cal_avareage_power(short int *pSample,u16 sample_cnt)
{
	int i;
	int total =0 ;
	u32 power_tmp;
	u32 avg_power =0;
	for(i=0;i<sample_cnt;i++){
		total += (int)(pSample[i]);
	}
	total =total/sample_cnt;
	power_tmp = abs(total)>>4;
	avg_power = power_tmp*power_tmp*sample_cnt;
	return avg_power;
}
u32 cal_total_power(short int *pSample,u16 sample_cnt)
{
	int i=0;
	u32 total_power=0;
	u32 temp_power =0;
	for(i=0;i<sample_cnt;i++){
		temp_power = abs(pSample[i]);
		temp_power = temp_power*temp_power;
		temp_power = temp_power>>8;
		total_power+=temp_power;
	}
	return total_power;
}
//use the src data to demo the data ,asume the max data is 10000;
#if 1
s16 sample_tab[64]={
	0,3827,7071,9239,10000,9239,7071,3827,
		0,-3827,-7071,-9239,-10000,-9239,-7071,-3827,
	0,3827,7071,9239,10000,9239,7071,3827,
		0,-3827,-7071,-9239,-10000,-9239,-7071,-3827,
	0,3827,7071,9239,10000,9239,7071,3827,
		0,-3827,-7071,-9239,-10000,-9239,-7071,-3827,
	0,3827,7071,9239,10000,9239,7071,3827,
		0,-3827,-7071,-9239,-10000,-9239,-7071,-3827,
};

#else 
s16 sample_tab[64]={
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
		0,7071,10000,7071,
		0,-7071,-10000,-7071,
	};
#endif 


u8 dispatch_voice_qua(short int *pSample ,u16 sample_cnt)
{
	static u32 A_start_tick=0;
	static u32 A_end_tick =0;
	static u32 A_total_power ;
	static u32 A_avarage_power ;
	//static u32	A_four_tran_power[32];
	static u32 A_four_tran_power;
	static u16 A_per_cent_power;

	A_start_tick  =clock_time();
	A_total_power = cal_total_power(pSample,sample_cnt);
	A_avarage_power = cal_avareage_power(pSample,sample_cnt);
	#if FREQUENCE_1K_3K_5K_ENABLE
	static u32 a_four_trans_part[3];
	a_four_trans_part[0]= fourier_trans(pSample,sample_cnt,1,16);//get 1k DFT frequence
	a_four_trans_part[1]= fourier_trans(pSample,sample_cnt,3,16);//get 3k DFT frequence
	a_four_trans_part[2]= fourier_trans(pSample,sample_cnt,5,16);//get 5k DFT frequence
	A_four_tran_power = a_four_trans_part[0]+a_four_trans_part[1]+a_four_trans_part[2];
	#else
	A_four_tran_power = fourier_trans(pSample,sample_cnt,1,16);//get 1k DFT frequence
	#endif 
	/*
	int i;
	for(i=0;i<32;i++){
		A_four_tran_power[i]=fourier_trans(sample_tab,sample_cnt,i,16);
	}
	*/
	// in the normal state ,the avarage power is equal to the total power 
	A_per_cent_power = ((A_four_tran_power)*100)/A_total_power;
	A_end_tick =clock_time();
	if(A_per_cent_power>PERCENT_HIHG){
		return 1;
	}else{
		return 0;
	}
} 
#endif 

void abuf_mic_dec ()
{
	static int start = 1;
	static int abuf_reset_no;
	if (abuf_reset)
	{
		abuf_dec_wptr = abuf_mic_wptr;
	}
	else
	{
		u8 num_mic = abuf_mic_wptr - abuf_dec_wptr;
		u8 num_dec = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);

		if (num_mic > 4) 			// in case of overflow
		{
			log_event (TR_T_abuf_overflow_mic);
			log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
			abuf_dec_wptr ++;
		}

		if (num_dec > 4)
		{
			//log_event (TR_T_abuf_overflow_dec);
			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));

			log_event (TR_T_abuf_overflow_dec);
			abuf_reset = 16;
			start = 1;
			abuf_reset_no++;
		}
		else if ( ((!start && num_mic>=1) || (start && num_mic>=2)) && (num_dec <= 3) )
		{
			log_event (TR_T_abuf_dec);
			log_data (TR_24_abuf_dec_wptr, PACK_POINTER);
			adpcm_to_pcm (
					(s16 *) (abuf_mic + (abuf_dec_wptr & 3) * MIC_ADPCM_FRAME_SIZE),
					abuf_dec + (abuf_dec_wptr & 3) * MIC_SHORT_DEC_SIZE,
					MIC_SHORT_DEC_SIZE );
			#if VIN_TEST_MODE
			extern u8 audio_run_flag;
				//sample the data in about 1s
				#if TEST_END_SEND_SLEEP
				extern u8 	test_OK_flag;
				extern u8 	test_send_sleep_flag;
				extern u32 	test_send_tick ;
				#endif
				if(clock_time_exceed(vin_sample_tick,1000*1000)&&audio_run_flag){
						vin_sample_tick =clock_time();
						//test ok dispatch 
						if(dispatch_voice_qua(abuf_dec+64,64)){
							vin_sample_OK_cnt++;
							//if test ok 4 times ,it will send the signal to the pc
							if(vin_sample_OK_cnt>AUDIO_SAMPLE_TIMES){
								u8 *store_info;
								store_info = (volatile u8*) (0x808004);	
								store_info[2] =0;
								store_info[3]|=BIT(7);//set the OK bit 
								store_info[5]=0x4c;
								store_info[11]&=~(3<<AUIDO_TEST_BIT_POS);
								store_info[11]|=TEST_OK<<AUIDO_TEST_BIT_POS;
								store_info[0]++;//add a singal show the audio test stauts 
								vin_sample_OK_cnt =0;
								vin_sample_Fail_cnt =0;
								#if TEST_END_SEND_SLEEP
								test_OK_flag|=BIT(1);
								if(test_OK_flag==(BIT(0)|BIT(1))){
									test_send_sleep_flag =1;
									test_send_tick =clock_time();
								}
								#endif 
								
							}
						}else{
							vin_sample_Fail_cnt++;
							if(vin_sample_Fail_cnt>AUDIO_SAMPLE_TIMES){
								u8 *store_info;
								store_info = (volatile u8*) (0x808004);	
								store_info[2] =0;
								store_info[3]&=~(BIT(7));//set the fail bit 
								store_info[5]=0x4c;
								store_info[11]&=~(3<<AUIDO_TEST_BIT_POS);
								store_info[11]|=TEST_FAIL<<AUIDO_TEST_BIT_POS;
								store_info[0]++;//add a singal show the audio test stauts 
								vin_sample_OK_cnt =0;
								vin_sample_Fail_cnt =0;
								#if TEST_END_SEND_SLEEP
								test_OK_flag&=~(BIT(1));
								#endif 
							}
						}
					}
			#endif 
			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}
	}
}

_attribute_ram_code_ void abuf_dec_usb ()
{
	log_event (TR_T_abuf_usb);
	log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
	log_data (TR_24_abuf_reset, abuf_reset);
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if (buffer_empty && num >= 3 || !buffer_empty && (num >= 1 || (n_usb_iso & 3)))
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;


			if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}


			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else
		{

		}
	}
	else
	{
		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
		{
			reg_usb_ep7_dat = 0;
		}
		buffer_empty = 1;
		log_event (TR_T_abuf_zero);
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}
