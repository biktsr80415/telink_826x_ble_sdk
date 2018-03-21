

#include "../config/user_config.h"
#include "../mcu/config.h"

//#if (1)

#if(__TL_LIB_MSSOC__ || (MCU_CORE_TYPE == MCU_CORE_MSSOC))

#include "../common/types.h"
#include "../common/compatibility.h"
#include "../common/bit.h"
#include "../common/utility.h"
#include "../common/static_assert.h"
#include "../mcu/compiler.h"
#include "../mcu/register.h"
#include "../mcu/anareg.h"
#include "../mcu/analog.h"

#include "../mcu/gpio.h"

#include "../../proj_lib/pm_mssoc.h"






/************
 *
 * gpio:         indicate the pin
 * up_down:      1 need pull up, 0 need pull down
 */

/************
 *
 * gpio:         indicate the pin
 * up_down:      0	   : float
 * 				 1    : 1M   pullup
 * 				 2    : 100K pulldown
 * 				 3    : 10K  pullup

 *
 *     BIT(7.6)   BIT(5.4)   BIT(3.2)   BIT(1.0)
mask_not 0x3f       0xcf	  0xf3       0xfc

 0e		 PA3		 PA2	  PA1        PA0
 0f		 PA7         PA6      PA5        PA4		0
 10		 PB3         PB2      PA1        PB0
 11		 PB7         PB6      PB5        PB4		1
 12		 PC3         PC2      PC1        PC0
 13		 PC7         PC6      PC5        PC4		2
 14		 PD3         PD2      PD1        PD0
 15		 PD7         PD6      PD5        PD4		3

 */
 
//if GPIO_DP,please check usb_dp_pullup_en() valid or not first.
void gpio_setup_up_down_resistor(u32 gpio, u32 up_down)
{
	u8 r_val = up_down & 0x03;

    //u8 pin = gpio & 0xff;

    u8 base_ana_reg = 0x0e + ((gpio >> 8) << 1) + ( (gpio & 0xf0) ? 1 : 0 );  //group = gpio>>8;
    u8 shift_num, mask_not;

	if(gpio & 0x11){
		shift_num = 0;
		mask_not = 0xfc;
	}
	else if(gpio & 0x22){
		shift_num = 2;
		mask_not = 0xf3;
	}
	else if(gpio & 0x44){
		shift_num = 4;
		mask_not = 0xcf;
	}
	else if(gpio & 0x88){
		shift_num = 6;
		mask_not = 0x3f;
	}
	else{
		return;
	}


    if(GPIO_DP == gpio){
        //usb_dp_pullup_en (0);
    }

	analog_write(base_ana_reg, (analog_read(base_ana_reg) & mask_not) | (r_val << shift_num));
}


void gpio_set_wakeup(u32 pin, u32 level, int en)
{
    u8 bit = pin & 0xff;
    if (en) {
        BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
    }
    else {
        BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
    }
    if(level){
        BM_CLR(reg_gpio_pol(pin), bit);
    }else{
        BM_SET(reg_gpio_pol(pin), bit);
    }
}


void gpio_set_input_en(u32 pin, u32 value)
{
	u8	bit = pin & 0xff;
	u16 group = pin & 0xf00;

	if(group == GPIO_GROUPA || group == GPIO_GROUPD || group == GPIO_GROUPE)
	{
		if(value){
			BM_SET(reg_gpio_ie(pin), bit);
		}else{
			BM_CLR(reg_gpio_ie(pin), bit);
		}
	}
    else if(group == GPIO_GROUPB)
    {
    	if(value){
    		analog_write(anaReg_gpioPB_ie, analog_read(anaReg_gpioPB_ie)|bit);
    	}
    	else
    	{
    		analog_write(anaReg_gpioPB_ie, analog_read(anaReg_gpioPB_ie)&(~bit));
    	}
    }
    else if(group == GPIO_GROUPC)
    {
    	if(value){
    		analog_write(anaReg_gpioPC_ie, analog_read(anaReg_gpioPC_ie)|bit);
    	}
    	else
    	{
    		analog_write(anaReg_gpioPC_ie, analog_read(anaReg_gpioPC_ie)&(~bit));
    	}
    }

}



void gpio_set_data_strength(u32 pin, u32 value)
{
    u8 bit = pin & 0xff;
    u16 group = pin & 0xf00;
    if( (group == GPIO_GROUPA) || (group==GPIO_GROUPD) || (group == GPIO_GROUPE))
    {
    	if(value){
    		BM_SET(reg_gpio_ds(pin), bit);
    	}else{
    		BM_CLR(reg_gpio_ds(pin), bit);
    	}
    }
    else if(group == GPIO_GROUPB)
    {
    	if(value){
    		analog_write(anaReg_gpioPB_ds, analog_read(anaReg_gpioPB_ds)|bit);
    	}
    	else
    	{
    		analog_write(anaReg_gpioPB_ds, analog_read(anaReg_gpioPB_ds)&(~bit));
    	}
    }
    else if(group == GPIO_GROUPC)
    {
    	if(value){
    		analog_write(anaReg_gpioPC_ds, analog_read(anaReg_gpioPC_ds)|bit);
    	}
    	else
    	{
    		analog_write(anaReg_gpioPC_ds, analog_read(anaReg_gpioPC_ds)&(~bit));
    	}
    }
}




void gpio_init_up_down_resistor(void)
{

	analog_write (0x0e,  PULL_WAKEUP_SRC_PA0 |
						(PULL_WAKEUP_SRC_PA1<<2) |
						(PULL_WAKEUP_SRC_PA2<<4) |
						(PULL_WAKEUP_SRC_PA3<<6));

	analog_write (0x0f,  PULL_WAKEUP_SRC_PA4 |
						(PULL_WAKEUP_SRC_PA5<<2) |
						(PULL_WAKEUP_SRC_PA6<<4) |
						(PULL_WAKEUP_SRC_PA7<<6));


	analog_write (0x10,  PULL_WAKEUP_SRC_PB0 |
						(PULL_WAKEUP_SRC_PB1<<2) |
						(PULL_WAKEUP_SRC_PB2<<4) |
						(PULL_WAKEUP_SRC_PB3<<6));

	analog_write (0x11,  PULL_WAKEUP_SRC_PB4 |
						(PULL_WAKEUP_SRC_PB5<<2) |
						(PULL_WAKEUP_SRC_PB6<<4) |
						(PULL_WAKEUP_SRC_PB7<<6));


	analog_write (0x12,  PULL_WAKEUP_SRC_PC0 |
						(PULL_WAKEUP_SRC_PC1<<2) |
						(PULL_WAKEUP_SRC_PC2<<4) |
						(PULL_WAKEUP_SRC_PC3<<6));

	analog_write (0x13,  PULL_WAKEUP_SRC_PC4 |
						(PULL_WAKEUP_SRC_PC5<<2) |
						(PULL_WAKEUP_SRC_PC6<<4) |
						(PULL_WAKEUP_SRC_PC7<<6));


	analog_write (0x14,  PULL_WAKEUP_SRC_PD0 |
						(PULL_WAKEUP_SRC_PD1<<2) |
						(PULL_WAKEUP_SRC_PD2<<4) |
						(PULL_WAKEUP_SRC_PD3<<6));

	analog_write (0x15,  PULL_WAKEUP_SRC_PD4 |
						(PULL_WAKEUP_SRC_PD5<<2) |
						(PULL_WAKEUP_SRC_PD6<<4) |
						(PULL_WAKEUP_SRC_PD7<<6));
}

_attribute_ram_code_ void gpio_init(void)
{

	reg_gpio_pa_setting1 =
		(PA0_INPUT_ENABLE<<8) 	| (PA1_INPUT_ENABLE<<9)	| (PA2_INPUT_ENABLE<<10)	| (PA3_INPUT_ENABLE<<11) |
		(PA4_INPUT_ENABLE<<12)	| (PA5_INPUT_ENABLE<<13)	| (PA6_INPUT_ENABLE<<14)	| (PA7_INPUT_ENABLE<<15) |
		((PA0_OUTPUT_ENABLE?0:1)<<16)	| ((PA1_OUTPUT_ENABLE?0:1)<<17) | ((PA2_OUTPUT_ENABLE?0:1)<<18)	| ((PA3_OUTPUT_ENABLE?0:1)<<19) |
		((PA4_OUTPUT_ENABLE?0:1)<<20)	| ((PA5_OUTPUT_ENABLE?0:1)<<21) | ((PA6_OUTPUT_ENABLE?0:1)<<22)	| ((PA7_OUTPUT_ENABLE?0:1)<<23) |
		(PA0_DATA_OUT<<24)	| (PA1_DATA_OUT<<25)	| (PA2_DATA_OUT<<26)	| (PA3_DATA_OUT<<27) |
		(PA4_DATA_OUT<<28)	| (PA5_DATA_OUT<<29)	| (PA6_DATA_OUT<<30)	| (PA7_DATA_OUT<<31) ;
	reg_gpio_pa_setting2 =
		(PA0_DATA_STRENGTH<<8)		| (PA1_DATA_STRENGTH<<9)| (PA2_DATA_STRENGTH<<10)	| (PA3_DATA_STRENGTH<<11) |
		(PA4_DATA_STRENGTH<<12)	| (PA5_DATA_STRENGTH<<13)	| (PA6_DATA_STRENGTH<<14)	| (PA7_DATA_STRENGTH<<15) |
		(PA0_FUNC==AS_GPIO ? BIT(16):0)	| (PA1_FUNC==AS_GPIO ? BIT(17):0)| (PA2_FUNC==AS_GPIO ? BIT(18):0)| (PA3_FUNC==AS_GPIO ? BIT(19):0) |
		(PA4_FUNC==AS_GPIO ? BIT(20):0)	| (PA5_FUNC==AS_GPIO ? BIT(21):0)| (PA6_FUNC==AS_GPIO ? BIT(22):0)| (PA7_FUNC==AS_GPIO ? BIT(23):0);




	//PB group
	//ie
	analog_write(anaReg_gpioPB_ie, 	(PB0_INPUT_ENABLE<<0) 	| (PB1_INPUT_ENABLE<<1)	| (PB2_INPUT_ENABLE<<2)	| (PB3_INPUT_ENABLE<<3) |
									(PB4_INPUT_ENABLE<<4)	| (PB5_INPUT_ENABLE<<5) | (PB6_INPUT_ENABLE<<6)	| (PB7_INPUT_ENABLE<<7) );
	//oen
	reg_gpio_pb_oen =
		((PB0_OUTPUT_ENABLE?0:1)<<0)	| ((PB1_OUTPUT_ENABLE?0:1)<<1) | ((PB2_OUTPUT_ENABLE?0:1)<<2)	| ((PB3_OUTPUT_ENABLE?0:1)<<3) |
		((PB4_OUTPUT_ENABLE?0:1)<<4)	| ((PB5_OUTPUT_ENABLE?0:1)<<5) | ((PB6_OUTPUT_ENABLE?0:1)<<6)	| ((PB7_OUTPUT_ENABLE?0:1)<<7);
	//dataO
	reg_gpio_pb_out =
		(PB0_DATA_OUT<<0)	| (PB1_DATA_OUT<<1)	| (PB2_DATA_OUT<<2)	| (PB3_DATA_OUT<<3) |
		(PB4_DATA_OUT<<4)	| (PB5_DATA_OUT<<5)	| (PB6_DATA_OUT<<6)	| (PB7_DATA_OUT<<7) ;

	//ds
	analog_write(anaReg_gpioPB_ds, 	(PB0_DATA_STRENGTH<<0) 	| (PB1_DATA_STRENGTH<<1)  | (PB2_DATA_STRENGTH<<2)	| (PB3_DATA_STRENGTH<<3) |
									(PB4_DATA_STRENGTH<<4)	| (PB5_DATA_STRENGTH<<5)  | (PB6_DATA_STRENGTH<<6)	| (PB7_DATA_STRENGTH<<7) );
	//func
	reg_gpio_pb_gpio =
		(PB0_FUNC==AS_GPIO ? BIT(0):0)	| (PB1_FUNC==AS_GPIO ? BIT(1):0)| (PB2_FUNC==AS_GPIO ? BIT(2):0)| (PB3_FUNC==AS_GPIO ? BIT(3):0) |
		(PB4_FUNC==AS_GPIO ? BIT(4):0)	| (PB5_FUNC==AS_GPIO ? BIT(5):0)| (PB6_FUNC==AS_GPIO ? BIT(6):0)| (PB7_FUNC==AS_GPIO ? BIT(7):0);




	//PC group
	//ie
	analog_write(anaReg_gpioPC_ie, 	(PC0_INPUT_ENABLE<<0) 	| (PC1_INPUT_ENABLE<<1)	| (PC2_INPUT_ENABLE<<2)	| (PC3_INPUT_ENABLE<<3) |
									(PC4_INPUT_ENABLE<<4)	| (PC5_INPUT_ENABLE<<5) | (PC6_INPUT_ENABLE<<6)	| (PC7_INPUT_ENABLE<<7) );

	//oen
	reg_gpio_pc_oen =
		((PC0_OUTPUT_ENABLE?0:1)<<0)	| ((PC1_OUTPUT_ENABLE?0:1)<<1) | ((PC2_OUTPUT_ENABLE?0:1)<<2)	| ((PC3_OUTPUT_ENABLE?0:1)<<3) |
		((PC4_OUTPUT_ENABLE?0:1)<<4)	| ((PC5_OUTPUT_ENABLE?0:1)<<5) | ((PC6_OUTPUT_ENABLE?0:1)<<6)	| ((PC7_OUTPUT_ENABLE?0:1)<<7);
	//dataO
	reg_gpio_pc_out =
		(PC0_DATA_OUT<<0)	| (PC1_DATA_OUT<<1)	| (PC2_DATA_OUT<<2)	| (PC3_DATA_OUT<<3) |
		(PC4_DATA_OUT<<4)	| (PC5_DATA_OUT<<5)	| (PC6_DATA_OUT<<6)	| (PC7_DATA_OUT<<7) ;

	//ds
	analog_write(anaReg_gpioPC_ds, 	(PC0_DATA_STRENGTH<<0) 	| (PC1_DATA_STRENGTH<<1)  | (PC2_DATA_STRENGTH<<2)	| (PC3_DATA_STRENGTH<<3) |
									(PC4_DATA_STRENGTH<<4)	| (PC5_DATA_STRENGTH<<5)  | (PC6_DATA_STRENGTH<<6)	| (PC7_DATA_STRENGTH<<7) );

	reg_gpio_pc_gpio =
		(PC0_FUNC==AS_GPIO ? BIT(0):0)	| (PC1_FUNC==AS_GPIO ? BIT(1):0)| (PC2_FUNC==AS_GPIO ? BIT(2):0)| (PC3_FUNC==AS_GPIO ? BIT(3):0) |
		(PC4_FUNC==AS_GPIO ? BIT(4):0)	| (PC5_FUNC==AS_GPIO ? BIT(5):0)| (PC6_FUNC==AS_GPIO ? BIT(6):0)| (PC7_FUNC==AS_GPIO ? BIT(7):0);





	reg_gpio_pd_setting1 =
		(PD0_INPUT_ENABLE<<8) 	| (PD1_INPUT_ENABLE<<9)	| (PD2_INPUT_ENABLE<<10)	| (PD3_INPUT_ENABLE<<11) |
		(PD4_INPUT_ENABLE<<12)	| (PD5_INPUT_ENABLE<<13)| (PD6_INPUT_ENABLE<<14)	| (PD7_INPUT_ENABLE<<15) |
		((PD0_OUTPUT_ENABLE?0:1)<<16)	| ((PD1_OUTPUT_ENABLE?0:1)<<17) | ((PD2_OUTPUT_ENABLE?0:1)<<18)	| ((PD3_OUTPUT_ENABLE?0:1)<<19) |
		((PD4_OUTPUT_ENABLE?0:1)<<20)	| ((PD5_OUTPUT_ENABLE?0:1)<<21) | ((PD6_OUTPUT_ENABLE?0:1)<<22)	| ((PD7_OUTPUT_ENABLE?0:1)<<23) |
		(PD0_DATA_OUT<<24)	| (PD1_DATA_OUT<<25)	| (PD2_DATA_OUT<<26)	| (PD3_DATA_OUT<<27) |
		(PD4_DATA_OUT<<28)	| (PD5_DATA_OUT<<29)	| (PD6_DATA_OUT<<30)	| (PD7_DATA_OUT<<31) ;
	reg_gpio_pd_setting2 =
		(PD0_DATA_STRENGTH<<8)	| (PD1_DATA_STRENGTH<<9)	| (PD2_DATA_STRENGTH<<10)	| (PD3_DATA_STRENGTH<<11) |
		(PD4_DATA_STRENGTH<<12)	| (PD5_DATA_STRENGTH<<13)	| (PD6_DATA_STRENGTH<<14)	| (PD7_DATA_STRENGTH<<15) |
		(PD0_FUNC==AS_GPIO ? BIT(16):0)	| (PD1_FUNC==AS_GPIO ? BIT(17):0)| (PD2_FUNC==AS_GPIO ? BIT(18):0)| (PD3_FUNC==AS_GPIO ? BIT(19):0) |
		(PD4_FUNC==AS_GPIO ? BIT(20):0)	| (PD5_FUNC==AS_GPIO ? BIT(21):0)| (PD6_FUNC==AS_GPIO ? BIT(22):0)| (PD7_FUNC==AS_GPIO ? BIT(23):0);


#if 0
	reg_gpio_pe_setting1 =
		(PE4_INPUT_ENABLE<<12)	| (PE5_INPUT_ENABLE<<13)| (PE6_INPUT_ENABLE<<14)	| (PE7_INPUT_ENABLE<<15) |
		((PE4_OUTPUT_ENABLE?0:1)<<20)	| ((PE5_OUTPUT_ENABLE?0:1)<<21) | ((PE6_OUTPUT_ENABLE?0:1)<<22)	| ((PE7_OUTPUT_ENABLE?0:1)<<23) |
		(PE4_DATA_OUT<<28)	| (PE5_DATA_OUT<<29)	| (PE6_DATA_OUT<<30)	| (PE7_DATA_OUT<<31);
#endif
	reg_gpio_pe_ie = 0xff; //5a1[0:3] 必须为1,否则程序无法运行,原因未知
	reg_gpio_pe_oen = ((PE4_OUTPUT_ENABLE?0:1)<<4)	| ((PE5_OUTPUT_ENABLE?0:1)<<5) | ((PE6_OUTPUT_ENABLE?0:1)<<6)	| ((PE7_OUTPUT_ENABLE?0:1)<<7);
	reg_gpio_pe_out = (PE4_DATA_OUT<<4)	| (PE5_DATA_OUT<<5)	| (PE6_DATA_OUT<<6)	| (PE7_DATA_OUT<<7);


	reg_gpio_pe_setting2 =
		(PE4_DATA_STRENGTH<<12)	| (PE5_DATA_STRENGTH<<13)	| (PE6_DATA_STRENGTH<<14)	| (PE7_DATA_STRENGTH<<15) |
		(PE4_FUNC==AS_GPIO ? BIT(20):0)	| (PE5_FUNC==AS_GPIO ? BIT(21):0)| (PE6_FUNC==AS_GPIO ? BIT(22):0)| (PE7_FUNC==AS_GPIO ? BIT(23):0);



#if 0
	if(!pm_is_MCU_deepRetentionWakeup()){
		gpio_init_up_down_resistor();
	}
#else
	analog_write (0x0e,  PULL_WAKEUP_SRC_PA0 |
						(PULL_WAKEUP_SRC_PA1<<2) |
						(PULL_WAKEUP_SRC_PA2<<4) |
						(PULL_WAKEUP_SRC_PA3<<6));

	analog_write (0x0f,  PULL_WAKEUP_SRC_PA4 |
						(PULL_WAKEUP_SRC_PA5<<2) |
						(PULL_WAKEUP_SRC_PA6<<4) |
						(PULL_WAKEUP_SRC_PA7<<6));


	analog_write (0x10,  PULL_WAKEUP_SRC_PB0 |
						(PULL_WAKEUP_SRC_PB1<<2) |
						(PULL_WAKEUP_SRC_PB2<<4) |
						(PULL_WAKEUP_SRC_PB3<<6));

	analog_write (0x11,  PULL_WAKEUP_SRC_PB4 |
						(PULL_WAKEUP_SRC_PB5<<2) |
						(PULL_WAKEUP_SRC_PB6<<4) |
						(PULL_WAKEUP_SRC_PB7<<6));


	analog_write (0x12,  PULL_WAKEUP_SRC_PC0 |
						(PULL_WAKEUP_SRC_PC1<<2) |
						(PULL_WAKEUP_SRC_PC2<<4) |
						(PULL_WAKEUP_SRC_PC3<<6));

	analog_write (0x13,  PULL_WAKEUP_SRC_PC4 |
						(PULL_WAKEUP_SRC_PC5<<2) |
						(PULL_WAKEUP_SRC_PC6<<4) |
						(PULL_WAKEUP_SRC_PC7<<6));


	analog_write (0x14,  PULL_WAKEUP_SRC_PD0 |
						(PULL_WAKEUP_SRC_PD1<<2) |
						(PULL_WAKEUP_SRC_PD2<<4) |
						(PULL_WAKEUP_SRC_PD3<<6));

	analog_write (0x15,  PULL_WAKEUP_SRC_PD4 |
						(PULL_WAKEUP_SRC_PD5<<2) |
						(PULL_WAKEUP_SRC_PD6<<4) |
						(PULL_WAKEUP_SRC_PD7<<6));
#endif

}



#endif

