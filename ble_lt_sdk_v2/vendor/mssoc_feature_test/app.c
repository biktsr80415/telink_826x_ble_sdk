#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj/drivers/battery.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/uart.h"

#if (__PROJECT_MSSOC_FEATURE_TEST__)



#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_ALL

#define 	MY_ADV_INTERVAL_MIN					ADV_INTERVAL_30MS
#define 	MY_ADV_INTERVAL_MAX					ADV_INTERVAL_35MS



MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);


u32			tick_led_config;
//////////////////////////////////////////////////////////////////////////////
//	 Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'R', 'e', 'm',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

const u8	tbl_scanRsp [] = {
		0x08, 0x09, 't', 'R', 'e', 'm', 'o', 't', 'e',
	};


u32 interval_update_tick = 0;
int device_in_connection_state;


u32		advertise_begin_tick;



//////////////////// key type ///////////////////////
#define IDLE_KEY	   			0
#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
#define IR_KEY   	   			3

u8 		key_type;
u8 		user_key_mode;

u8 		key_buf[8] = {0};

int 	key_not_released;

int 	ir_not_released;

u32 	latest_user_event_tick;

u8 		user_task_flg;
u8 		sendTerminate_before_enterDeep = 0;
u8 		ota_is_working = 0;



void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{

}

void	task_connect (u8 e, u8 *p, int n)
{



}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
#define MAX_BTN_SIZE			2
#define BTN_VALID_LEVEL			0

#define USER_BTN_1				0x01
#define USER_BTN_2				0x02
#define SW1_GPIO				0
#define SW2_GPIO				0
u32 ctrl_btn[] = {SW1_GPIO, SW2_GPIO};
u8 btn_map[MAX_BTN_SIZE] = {USER_BTN_1, USER_BTN_2};

typedef	struct{
	u8 	cnt;				//count button num
	u8 	btn_press;
	u8 	keycode[MAX_BTN_SIZE];			//6 btn
}vc_data_t;
vc_data_t vc_event;

typedef struct{
	u8  btn_history[4];		//vc history btn save
	u8  btn_filter_last;
	u8	btn_not_release;
	u8 	btn_new;					//new btn  flag
}btn_status_t;
btn_status_t 	btn_status;


u8 btn_debounce_filter(u8 *btn_v)
{
	u8 change = 0;

	for(int i=3; i>0; i--){
		btn_status.btn_history[i] = btn_status.btn_history[i-1];
	}
	btn_status.btn_history[0] = *btn_v;

	if(  btn_status.btn_history[0] == btn_status.btn_history[1] && btn_status.btn_history[1] == btn_status.btn_history[2] && \
		btn_status.btn_history[0] != btn_status.btn_filter_last ){
		change = 1;

		btn_status.btn_filter_last = btn_status.btn_history[0];
	}

	return change;
}

u8 vc_detect_button(int read_key)
{
	u8 btn_changed, i;
	memset(&vc_event,0,sizeof(vc_data_t));			//clear vc_event
	//vc_event.btn_press = 0;

	for(i=0; i<MAX_BTN_SIZE; i++){
		if(BTN_VALID_LEVEL != !gpio_read(ctrl_btn[i])){
			vc_event.btn_press |= BIT(i);
		}
	}

	btn_changed = btn_debounce_filter(&vc_event.btn_press);


	if(btn_changed && read_key){
		for(i=0; i<MAX_BTN_SIZE; i++){
			if(vc_event.btn_press & BIT(i)){
				vc_event.keycode[vc_event.cnt++] = btn_map[i];
			}
		}

		return 1;
	}

	return 0;
}

void proc_button (void)
{
	static u32 button_det_tick;
	if(clock_time_exceed(button_det_tick, 5000))
	{
		button_det_tick = clock_time();
	}
	else{
		return;
	}


//	static u32 button_history = 0;
//	static u32 last_singleKey_press_tick;

	static int button1_press_flag;
	static u32 button1_press_tick;
	static int button2_press_flag;
	static u32 button2_press_tick;

	static int consumer_report = 0;

	int det_key = vc_detect_button (1);

	if (det_key)  //key change: press or release
	{
		u8 key0 = vc_event.keycode[0];
		u8 key1 = vc_event.keycode[1];

		if(vc_event.cnt == 2)  //two key press
		{

		}
		else if(vc_event.cnt == 1) //one key press
		{
			if(key0 == USER_BTN_1)
			{
				button1_press_flag = 1;
				button1_press_tick = clock_time();

			}
			else if(key0 == USER_BTN_2)
			{
				button2_press_flag = 1;
				button2_press_tick = clock_time();

			}
		}
		else{  //release

			button1_press_flag = 0;
			button2_press_flag = 0;


		}

	}


}




//u32 AA_tick[256];
//int AA_index_tick= 0;

int first_power_on = 0;


#define TEST_LP_MODE			TEST_LP_SUSPEND_TIMER_WAKEUP

void user_init()
{





	usb_dp_pullup_en (1);  //open USB enum




#if (FEATURE_TEST_MODE == TEST_LOW_POWER_DRIVER)
	if(analog_read(0x3a) == 0x5a){  //read flag
		first_power_on = 0;
	}
	else{
		first_power_on = 1;
	}


	gpio_set_output_en(GPIO_PA1, 1);
	if(first_power_on){
		gpio_toggle(GPIO_PA1);
		sleep_us(500000);
		gpio_toggle(GPIO_PA1);
		sleep_us(500000);
		gpio_toggle(GPIO_PA1);
		sleep_us(500000);
		gpio_toggle(GPIO_PA1);
		sleep_us(500000);
//		gpio_toggle(GPIO_PA1);
//		sleep_us(500000);
	}
	else{
		gpio_toggle(GPIO_PA1);
		sleep_us(100000);
		gpio_toggle(GPIO_PA1);
		sleep_us(100000);
		gpio_toggle(GPIO_PA1);
		sleep_us(500000);
		gpio_toggle(GPIO_PA1);
		sleep_us(500000);
//		DBG_CHN1_TOGGLE;
//		sleep_us(100000);
	}

	analog_write(0x3a, 0x5a); //set flag


	usb_dp_pullup_en (0);

#if(TEST_LP_MODE == TEST_LP_SUSPEND_NO_WAKEUP)

  	gpio_set_input_en(GPIO_DM, 0);
	gpio_set_input_en(GPIO_DP, 0);
	gpio_set_input_en(GPIO_SWS, 0);

  	cpu_sleep_wakeup(SUSPEND_MODE , 0, 0);

#elif(TEST_LP_MODE == TEST_LP_SUSPEND_GPIO_CORE_WAKEUP)

	gpio_set_input_en(GPIO_DM, 0);
	gpio_set_input_en(GPIO_DP, 0);
	gpio_set_input_en(GPIO_SWS, 0);

	///////////////  gpio core wakeup  /////////////////////
	gpio_set_input_en(GPIO_PA0, 1);
	gpio_setup_up_down_resistor(GPIO_PA0, PM_PIN_PULLDOWN_100K);
	gpio_set_wakeup(GPIO_PA0, 1, 1);
	gpio_core_wakeup_enable_all(1);


	while(1){

//		PM_SuspendEnter(PM_WAKEUP_CORE, 0);
		cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_CORE, 0);  //PM_WAKEUP_CORE

		DBG_CHN1_TOGGLE;
		sleep_us(300000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
		DBG_CHN1_TOGGLE;
		sleep_us(600000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
  	}

#elif(TEST_LP_MODE == TEST_LP_SUSPEND_TIMER_WAKEUP)

	gpio_set_input_en(GPIO_DM, 0);
	gpio_set_input_en(GPIO_DP, 0);

	gpio_set_output_en(GPIO_PA2, 1);

	u32 now_tick;
	while(1){

		now_tick = clock_time();
//		AA_tick[AA_index_tick++] = now_tick;

//		PM_SuspendEnter(PM_WAKEUP_TIMER, now_tick + 2 * CLOCK_SYS_TIMER_CLOCK_1S);
		cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_TIMER, now_tick + 1*CLOCK_SYS_TIMER_CLOCK_1S);

		DBG_CHN1_TOGGLE;
		sleep_us(200000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
		DBG_CHN1_TOGGLE;
		sleep_us(600000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
  	}

#elif(TEST_LP_MODE == TEST_LP_DEEPSLEEP_NO_WAKEUP)


	gpio_set_input_en(GPIO_DM, 0);
	gpio_set_input_en(GPIO_DP, 0);
	gpio_set_input_en(GPIO_SWS, 0);

	cpu_sleep_wakeup(DEEPSLEEP_MODE , 0, 0);

#elif(TEST_LP_MODE == TEST_LP_DEEPSLEEP_GPIO_PAD_WAKEUP)

	gpio_set_input_en(GPIO_DM, 0);
	gpio_set_input_en(GPIO_DP, 0);
	gpio_set_input_en(GPIO_SWS, 0);

	gpio_setup_up_down_resistor(GPIO_PA0, PM_PIN_PULLDOWN_100K);
	cpu_set_gpio_wakeup(GPIO_PA0, 1, 1);

	while(1){
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //PM_WAKEUP_PAD
	}


#elif(TEST_LP_MODE == TEST_LP_DEEPSLEEP_TIMER_WAKEUP)

	u32 sleep_time = 0;
  	if(first_power_on){
  		sleep_time = 2*CLOCK_SYS_TIMER_CLOCK_1S;
  	}
  	else{
  		sleep_time = 1*CLOCK_SYS_TIMER_CLOCK_1S;
  	}


  	//usb_dp_pullup_en(0);
  	//shutdown_gpio();
  	gpio_set_input_en(GPIO_DM, 0);
	gpio_set_input_en(GPIO_DP, 0);
	gpio_set_input_en(GPIO_SWS, 0);

	u32 now_tick;
  	while(1){

  		now_tick = clock_time();
  		cpu_sleep_wakeup(DEEPSLEEP_MODE , PM_WAKEUP_TIMER, now_tick + sleep_time);
//  		PM_DeepSleepEnter(now_tick + sleep_time);

		DBG_CHN1_TOGGLE;
		sleep_us(700000);
		DBG_CHN1_TOGGLE;
		sleep_us(300000);
  	}

#else


#endif






#endif

}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_loop;
//unsigned short battValue[20];


void main_loop (void)
{
	tick_loop ++;



}


#endif  //end of __PROJECT_826x_HID_SAMPLE
