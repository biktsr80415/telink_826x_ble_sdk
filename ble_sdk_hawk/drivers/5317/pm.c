
#include "bsp.h"

#include "compiler.h"
#include "register.h"
#include "analog.h"
#include "clock.h"
#include "irq.h"
#include "rf_drv.h"
#include "pm.h"

#include "stack/ble/ble.h"
#include "stack/ble/service/ble_ll_ota.h"

cpu_pm_handler_t  cpu_sleep_wakeup;
suspend_handler_t func_before_suspend = 0;

/**
 * @Brief: PM initialization.(Low Speed Clock(LSC)
 * @Param: LSC_32KSrc -> can be LSC_32kSrc_XTAL/LSC_32kSrc_RC.
 * @ReVal: None.
 */
void pm_init(LSC_32kSrcTypeDef LSC_32KSrc)
{
	if(LSC_32KSrc == LSC_32kSrc_XTAL){
		//32K clock source select:32K XTAL
		MCU_32kClockSourceSelect(LSC_32kSrc_XTAL);

		cpu_sleep_wakeup = cpu_sleep_wakeup_32kpad;
		blt_miscParam.pad32k_en = 1;

	}else if(LSC_32KSrc == LSC_32kSrc_RC){
		//32K clock source select:32K RC
		MCU_32kClockSourceSelect(LSC_32kSrc_RC);

		cpu_sleep_wakeup = cpu_sleep_wakeup_32krc;
		blt_miscParam.pm_enter_en = 1;
	}
}

/**
 * @Brief:  Let MCU enter sleep state.
 * @Param:  None.
 * @Return: None.
 */
_attribute_ram_code_
_attribute_no_inline_
void sleep_start(void){
#if 0  //8267 send flash wakeup cmd when reboot
	REG_ADDR8 (0x0d) = 0;
	REG_ADDR8 (0x0c) = 0xb9;
	for(volatile int i=0; i<2; i++); //1440ns when 32M clk
	REG_ADDR8 (0x0d) = 1;
#endif

	REG_ADDR8(0x5a1) &= 0x00; //MSPI ie disable
	write_reg8(0x80006f,0x81);
	//for(volatile int i=0; i<0x30; i++);//28.5uS(24MHz)
	for(volatile int i = 0; i<6; i++);//4.25us(24MHz)
	REG_ADDR8(0x5a1) |= 0xff; //MSPI ie enable

#if 0 //8267 send flash wakeup cmd when reboot
	REG_ADDR8 (0x0d) = 0;
	REG_ADDR8 (0x0c) = 0xab;
	for(volatile int i=0; i<2; i++); //1440ns when 32M clk
	REG_ADDR8 (0x0d) = 1;
#endif
}

const TBLCMDSET  tbl_cpu_wakeup_init[] = {
	{0x60, 0x00,   TCMD_UNDER_BOTH | TCMD_WRITE},//open all the clk,disable all the rst
	{0x61, 0x00,   TCMD_UNDER_BOTH | TCMD_WRITE},//open all the clk,disable all the rst
	{0x62, 0x00,   TCMD_UNDER_BOTH | TCMD_WRITE},//open all the clk,disable all the rst
	{0x63, 0xff,   TCMD_UNDER_BOTH | TCMD_WRITE},//open all the clk,disable all the rst
	{0x64, 0xff,   TCMD_UNDER_BOTH | TCMD_WRITE},//open all the clk,disable all the rst
	{0x65, 0xff,   TCMD_UNDER_BOTH | TCMD_WRITE},

	{0x5b5, 0x0c,  TCMD_UNDER_BOTH | TCMD_WRITE},//Enable gpio(core) irq and wakeup for keyboard

	{0x03, 0x4b,   TCMD_UNDER_BOTH | TCMD_WAREG},//Increase Flash current
	{0x06, 0x00,   TCMD_UNDER_BOTH | TCMD_WAREG},

	{0x620, 0x01,  TCMD_UNDER_BOTH | TCMD_WRITE},//Timer0 Enable.

	{0x20, 0x00,   TCMD_UNDER_BOTH | TCMD_WAREG},//wakeup reset time: (0xff - 0xc1)*32 = 2000 us
	{0x2d, 0x48,   TCMD_UNDER_BOTH | TCMD_WAREG},//quick settle: 200 us
};

void cpu_wakeup_init(void)
{
	load_tbl_cmd_set(tbl_cpu_wakeup_init, sizeof (tbl_cpu_wakeup_init)/sizeof (TBLCMDSET));

	WriteAnalogReg(0x81, 0xf8); //increase XTAL current

	/* Open 24M XTAL. */
	WriteAnalogReg(0x05, 0xca);
	for(volatile unsigned int i =0; i<10*24; i++);
	WriteAnalogReg(0x05, 0xc2);
	for(volatile unsigned int i =0; i<1000*24; i++);

	reg_dma_chn_irq_msk = 0;

	/* Set 24M XTAL buffer and doubler. */
	WriteAnalogReg(0x80, 0x61); //Enable 24M clk buf
	WriteAnalogReg(0x81, 0xcf); //Enable 24M clk buf -> 0x4f
	WriteAnalogReg(0x82, 0x5f); //Enable 48M doubler

	/* 24M RC calibrate. */
	MCU_24M_RC_ClockCalibrate();

	/* System Timer enable. */
	reg_sys_timer_ctrl |= FLD_SYS_TIMER_EN;

	/* 32K RC calibration. */
	//mcu_32k_rc_clock_calibration();

	/* Must */
	write_reg8(0x74a,0x29);//Enable calibration and close system timer 0x29
	write_reg8(0x74a,0x28);//End calibration,calibration value is to be written to register 0x749|0x748.0x28
	write_reg8(0x74a,0xa8);//Enable system timer and disable calibration

	//等待32个32K cycle以保证32K校准值能够采集完成。
	for(volatile int i = 0; i < CLOCK_SYS_CLOCK_1US*1000; i++);

#if EXTERNAL_32K_XTAL_EN
	pm_init(LSC_32kSrc_XTAL);
#else
	pm_init(LSC_32kSrc_RC);
#endif
}

/**
 * @Brief: Select 32K clock source
 * @Param: None.
 * @ReVal: None.
 */
void MCU_32kClockSourceSelect(LSC_32kSrcTypeDef LSC_32kSrc)
{
	unsigned char temp = 0;

	/* Power on 32K RC/32K XTAL. */
	temp = analog_read(0x05);
	temp |= 0x03;
	temp &= ~LSC_32kSrc;
	analog_write(0x05,temp);

//	temp = analog_read(0x2c);
//	temp |= 0x03;
//	temp &= ~LSC_32kSource;
//	analog_write(0x2c,temp);

	/* Set 32k Clock source as 32K RC. */
	if(LSC_32kSrc == LSC_32kSrc_RC)
	{
		temp = analog_read(0x2d);
		temp &= ~(1<<7);
		analog_write(0x2d,temp);
	}
	else if(LSC_32kSrc == LSC_32kSrc_XTAL)/* Set 32k Clock source as 32K XTAL. */
	{
		//Disable 50K pull-up resistor of 32K XTAL pin(PB6 PB7).
		//gpio_set_50k_pullup(0);

		temp = analog_read(0x2d);
		temp |= (1<<7);
		analog_write(0x2d,temp|0x68);

		//Set 32k XTAL duty :50%
		temp = analog_read(0x07);
		temp &= ~(0x03);
		analog_write(0x07,temp);
	}
}

/**
 * @Brief: MCU internal 32K RC calibrate.Calibration accuracy is 1.6%
 * @Param: None.
 * @ReVal: None.
 */
void mcu_32k_rc_clock_calibration(void){
	unsigned char temp = 0;

	temp = analog_read(0x02);
	temp |= (1<<2);
	analog_write(0x02, temp);

	/* Enable 32K RC calibration. */
	temp = analog_read(0x83);
	temp |= (1<<0);//Enable
	temp |= (1<<1);//Select calibrate 32k RC
	analog_write(0x83,temp);

	/* Wait Calibration completely. */
	while(!(analog_read(0x84) & 0x01));

	unsigned char CalValue = 0;
	CalValue = analog_read(0x85);
	analog_write(0x2f,CalValue);

	/* Disable 32K RC calibration. */
	temp = analog_read(0x83);
	temp &= ~(1<<0);
	analog_write(0x83,temp);

	temp = analog_read(0x02);
	temp &= ~(1<<2);
	analog_write(0x02, temp);
}

#if 0
/**
 * @Brief: MCU initialization.
 * @Param: None.
 * @ReVal: None.
 */
void cpu_wakeup_init(void)
{
	LoadTblCmdSet (tbl_cpu_wakeup_init, sizeof (tbl_cpu_wakeup_init)/sizeof (TBLCMDSET));

	WriteAnalogReg(0x81, 0xf8); //increase XTAL current

	/* Open 24M XTAL. */
	WriteAnalogReg(0x05, 0xca);
	for(volatile unsigned int i =0; i<10*24; i++);
	WriteAnalogReg(0x05, 0xc2);
	for(volatile unsigned int i =0; i<1000*24; i++);

	reg_dma_chn_irq_msk = 0;

	/* Set 24M XTAL buffer and doubler. */
	WriteAnalogReg(0x80, 0x61); //Enable 24M clk buf
	WriteAnalogReg(0x81, 0xcf); //Enable 24M clk buf -> 0x4f
	WriteAnalogReg(0x82, 0x5f); //Enable 48M doubler

	/* 24M RC calibrate. */
	MCU_24M_RC_ClockCalibrate();

	/* System Timer enable. */
	reg_sys_timer_ctrl |= FLD_SYS_TIMER_EN;

	/* 32K RC calibration. */
	//mcu_32k_rc_clock_calibration();

	/* Must */
	write_reg8(0x74a,0x29);//Enable calibration and close system timer 0x29
	write_reg8(0x74a,0x28);//End calibration,calibration value is to be written to register 0x749|0x748.0x28
	write_reg8(0x74a,0xa8);//Enable system timer and disable calibration

	//等待32个32K cycle以保证32K校准值能够采集完成。
	for(volatile int i =0;i<sys_tick_per_us*1000;i++);

	/* Select 32k pad as 32k clock source */
	if(blt_miscParam.pad32k_en)
	{
		MCU_32kClockSourceSelect(LSC_32kSrc_XTAL);
	}

	/**
	 * Set system clock source and system clock source divider factor.
	 * The code to set the system clock must be before the timer initializes the code.
	 * 保证timer0的时钟和system timer时钟相同以及tick值的同步，避免在无PM时导致BLE_STATE_BRX_S中断无法进入，从而导致BLE断连
	 */
	//16MHz sys clock
	//write_reg8(0x66, 0x43);
	//write_reg8(0x70,0x00);//must

	//enable timer0 for system tick irq mode
	//todo: avoid cpu stall wakeup by timer0
	//reg_tmr_ctrl8 |= 0x00;
	//reg_tmr0_tick = REG_ADDR32(0x740);//"reg_tmr0_tick" must be set as "REG_ADDR32(0x740)".
	//reg_irq_mask |=	FLD_IRQ_TMR0_EN;  //Enable Timer0 Interrupt
	//write_reg8(0x63c,0x01); //continuous tick mode
	//reg_tmr_ctrl8 |= 0x01;  //Enable Timer0

	/* Determine the flash address of the new firmware. */
#if 0//(BLE_REMOTE_OTA_ENABLE)
	if (REG_ADDR8(0x63e)){
		REG_ADDR8(0x63e) = (REG_ADDR8(0x63e) & 0x3) | ((ota_firmware_size_k>>2)<<2);
		ota_program_offset = 0;
	}else{
		ota_program_offset = 0x20000;
	}

	unsigned int mark0, mark1;
	flash_read_page(0x8,     4, (unsigned char *)&mark0);
	flash_read_page(0x20008, 4, (u8 *)&mark1);
	if( mark0 == 0x544c4e4b && mark1 == 0x544c4e4b){
		blt_miscParam.conn_mark = 1;
	}
#endif

#if 0
	unsigned char flashFlag = 0;

	flash_read_page(0x8, 1, (u8 *)&flashFlag);
	if(flashFlag == 0x4b){
		ota_program_offset = 0x20000;
	}else{
		ota_program_offset = 0;
	}

	unsigned int mark0, mark1;
	flash_read_page(0x8,     4, (unsigned char *)&mark0);
	flash_read_page(0x20008, 4, (unsigned char)&mark1);
	if(mark0 == 0x544c4e4b && mark1 == 0x544c4e4b){
		blt_miscParam.conn_mark = 1;
	}
#endif
}
#endif

/**
 * @Brief:  Set Pad(analog) as wake-up source.
 * @Param:  pin ->
 * @Param:  pol ->
 * @Param:  en  ->
 * @Return: None.
 */
#define AREG_PAD_WAKEUP_EN(i)		((i>>8) + 0x27)
#define AREG_PAD_WAKEUP_POL(i)		((i>>8) + 0x21)
void cpu_set_gpio_wakeup (GPIO_PinTypeDef pin, int pol, int en) {
	///////////////////////////////////////////////////////////
	// 		  PA[7:0]	    PB[7:0]		PC[7:0]
	// pol: ana_21<7:0>	 ana_22<7:0>  ana_23<7:0>
	// en:	ana_27<7:0>	 ana_28<7:0>  ana_29<7:0>

    unsigned char mask = pin & 0xff;
	unsigned char areg;
	unsigned char val;

	////////////////////////// polarity ////////////////////////
	areg = AREG_PAD_WAKEUP_POL(pin);
	val = ReadAnalogReg(areg);
	if (pol) {
		val &= ~mask;
		//gpio_setup_up_down_resistor(pin,GPIO_PULL_DOWN_100K);
	}
	else {
		val |= mask;
		//gpio_setup_up_down_resistor(pin,GPIO_PULL_UP_10K);
	}
	WriteAnalogReg (areg, val);

	/////////////////////////// enable /////////////////////
	areg = AREG_PAD_WAKEUP_EN(pin);
	val = ReadAnalogReg(areg);
	if (en) {
		val |= mask;
	}
	else {
		val &= ~mask;
	}
	WriteAnalogReg (areg, val);
}

/**
 * @Brief:  Set pad filter.
 * @Param:
 * @Return: None.
 */
void pm_set_filter(unsigned char en)
{
	unsigned char tmp;
	tmp = ReadAnalogReg(0x26);
	if(en)
	{
		tmp |= BIT(3);
	}
	else
	{
		tmp &= ~BIT(3);
	}
	WriteAnalogReg (0x26, tmp);
}

/**
 * @Brief:  Get 32k timer tick value.
 * @Param:  None.
 * @Return: 32k timer tick value.
 */
_attribute_ram_code_ unsigned int pm_get_32k_tick(void)
{
	reg_sys_timer_ctrl = 0x20;//0xa8;//Enable 32k tick read,close calibrate.
//	reg_sys_timer_cmd_state = 0x00;//Clear 32k read latch update flag.
	reg_sys_timer_cmd_state |= BIT(5);//Clear 32k read latch update flag.

	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");

	while(!(reg_sys_timer_cmd_state & BIT(5)));//Wait 32k latch register update.

	return reg_32k_timer_counter_latch;
}

/**
 * @Brief:  Set 32k timer tick value.
 * @Param:  None.
 * @Return: None.
 */
_attribute_ram_code_ void pm_set_32k_tick(unsigned int tick)
{
	reg_sys_timer_ctrl = 0x21;//Enable 32k tick write.
	while(reg_sys_timer_cmd_state & 0x40);

	reg_32k_timer_tick = tick;
	reg_sys_timer_cmd_state |= BIT(3);//Start 32k tick write.

//	while((reg_sys_timer_cmd_state & BIT(3)) == 0);//Wait

	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");

	while(reg_sys_timer_cmd_state & BIT(3));//Wait done.
}

/**
 * @Brief:  Register callback of PM.
 * @Param:  func -> pointer point to function
 * @Return: None.
 */
void bls_pm_registerFuncBeforeSuspend (suspend_handler_t func )
{
	func_before_suspend = func;
}

/**
 * @Brief:
 * @Param:
 * @Return:
 */
_attribute_ram_code_ int cpu_sleep_wakeup_32krc (int deepsleep, int wakeup_src,
		                                         unsigned int wakeup_tick)
{
#if (BLS_PROC_LONG_SUSPEND_ENABLE)
	unsigned int span = (unsigned int)(wakeup_tick - clock_time ());

	if(span > (sys_tick_per_us<<21) && span < 0xc0000000){  //2,097,152 us
		DBG_CHN3_HIGH;
		cpu_stall_wakeup_by_timer0(	5000 * sys_tick_per_us);
		DBG_CHN3_LOW;
	}

	span = (unsigned int)(wakeup_tick - clock_time ());
	unsigned short tick_32k_calib = REG_ADDR16(0x748);
	unsigned short tick_32k_halfCalib = tick_32k_calib>>1;

	unsigned char long_suspend = 0;

#else
	unsigned short tick_32k_calib = REG_ADDR16(0x748);
	unsigned short tick_32k_halfCalib = tick_32k_calib >> 1;

	unsigned char long_suspend = 0;
	unsigned int span = (unsigned int)(wakeup_tick - clock_time ());
#endif

	if(wakeup_src & PM_WAKEUP_TIMER){
		if (span > 0xc0000000)  //BIT(31)+BIT(30)   3/4 cylce of 32bit
		{
			return  analog_read (0x44) & 0x0f;
		}
		else if (span < EMPTYRUN_TIME_US * sys_tick_per_us) // 0 us base
		{
			unsigned int t = clock_time ();
			analog_write (0x44, 0x0f);			//clear all status
#if 1
			unsigned char st;
			do {
				st = analog_read (0x44) & 0x0f;
			} while ( ((unsigned int)clock_time () - t < span) && !st);
			return st;
#else  //save power
			if(span > 150){
				cpu_stall_wakeup_by_timer0(span - 100);
			}
			return (analog_read (0x44) & 0x0f);
#endif
		}
		else
		{
			if( span > 0x0ff00000 ){  //BIT(28) = 0x10000000   16M:16S; 32M:8S  48M: 5.5S
				long_suspend = 1;
			}
		}
	}

	/* Execute the callback function. */
	if(func_before_suspend){
		if (!func_before_suspend())
		{
			return PM_WAKEUP_CORE;
		}
	}

	/* Enter critical region. */
	unsigned char r = irq_disable ();

//	for(volatile int i=0; i<10; i++);//9.6us (16MHz)

#if 0
	unsigned int tick_cur = clock_time ();
	unsigned int tick_32k_cur = pm_get_32k_tick ();//3.75us
#else
	reg_sys_timer_ctrl = 0xa0;//0xa8;//Enable 32k tick read,close calibrate.
	reg_sys_timer_cmd_state = BIT(5);//Clear 32k read latch update flag.

	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");
	asm("tnop");asm("tnop");asm("tnop");asm("tnop");

	while(!(reg_sys_timer_cmd_state & BIT(5)));//Wait 32k latch register update.

	unsigned int tick_32k_cur = reg_32k_timer_counter_latch;

	unsigned int tick_cur = clock_time ();

	reg_sys_timer_ctrl = 0x20;
#endif

	unsigned int tick_wakeup_reset = wakeup_tick - EARLYWAKEUP_TIME_US * sys_tick_per_us;

	/* Set digital/analog wake-up source. */
	analog_write(0x26, wakeup_src|(PM_PAD_FILTER_EN ? BIT(3) : 0x00));

	//if digital wake-up is enabled,Select digital wake-up.
	write_reg8(0x6e, (wakeup_src & PM_WAKEUP_CORE) ? 0x08 : 0x00);

	analog_write (0x44, 0x0f);//Clear wake-up flag.

    /* Power down the corresponding module. */
	unsigned char rc32k_power_down = 0;
	if (wakeup_src & PM_WAKEUP_TIMER ) {
		rc32k_power_down = 0x00;//32K RC need be enabled
	}
	else {
		write_reg8(0x74a,0x20);
		rc32k_power_down = 0x00 | (PM_PAD_FILTER_EN ? 0:1);//32K RC power
	}

	// Switch system clock to 24M RC.
	unsigned char reg66 = read_reg8(0x66);
	write_reg8 (0x66, 0x00);

	//32K RC power-down.
	analog_write(0x2c, (deepsleep ? 0xfe : 0x7e) | rc32k_power_down);
	analog_write(0x2d, 0x68);//default:0x48 -> 0x68

	/* Set power-on delay time when MCU is waked up. */
	span = (RESET_TIME_US * sys_tick_per_us * 16 + tick_32k_halfCalib) / tick_32k_calib;

	/* Set 32k wake-up tick. */
	unsigned int tick_32k_wakeup;

	if(long_suspend){
		tick_32k_wakeup = tick_32k_cur + (unsigned int)(tick_wakeup_reset - tick_cur)/ tick_32k_calib * 16;
	}
	else{
		tick_32k_wakeup = tick_32k_cur + ((unsigned int)(tick_wakeup_reset - tick_cur) * 16 + tick_32k_halfCalib) / tick_32k_calib;
	}

	unsigned char rst_cycle = 0xff - span;
	analog_write (0x20, rst_cycle);
	pm_set_32k_tick(tick_32k_wakeup);

	analog_write(0x44, 0x0f);//Clear wake-up flag.

	//Native LDO power down.(Must)
	if(deepsleep)
	{
		//analog_write(0x01,0xf7);

		analog_write(0x26, analog_read(0x26)|BIT(2));
		analog_write(0x01, analog_read(0x01)&0x7f);
	}
	else
	{
		//Digital power down.(save 10uA)
		//analog_write(0x01, 0x7f);

		//digital LDO pull down disable
		analog_write(0x01, 0x2a);
		analog_write(0x05,analog_read(0x05)&0x7f);
	}

	analog_write(0x44, 0x0f);//Clear wake-up flag.

//	analog_write(0x81,0xcf);//Increase XTAL current.(user for PM driver test.)

	/* Enter low power mode. */
	DBG_CHN0_LOW;   //GPIO debug
	if(analog_read(0x44)&0x0f){

	}
	else
	{
		sleep_start();
	}
	DBG_CHN0_HIGH;  //GPIO debug

	if(deepsleep){
		write_reg8(0x6f, 0x20);  //reboot
	}

	/* suspend recover setting. */
	analog_write (0x2c, 0x00);

//	analog_write(0x01, 0x77);//4.2us
	analog_write(0x01, analog_read(0x01)&0xf7);
	analog_write(0x05,analog_read(0x05)|BIT(7));

	unsigned int tick_32k = pm_get_32k_tick();//3us - 40us

	if(long_suspend){
		tick_cur += (unsigned int)(tick_32k - tick_32k_cur) / 16 * tick_32k_calib;
		//tick_cur += (u32)((tick_32k<<1) - (tick_32k_cur<<1) - 3) / 32 * tick_32k_calib;
	}
	else{
		tick_cur += (unsigned int)(tick_32k - tick_32k_cur) * tick_32k_calib / 16 ;		// current clock
		//tick_cur += (u32)((tick_32k<<1) - (tick_32k_cur<<1) - 3) * tick_32k_calib / 32;
	}

	REG_ADDR8(0x66) = reg66;//restore system clock

	//if (tick_cur >= wakeup_tick)
	//if((wakeup_src & PM_WAKEUP_TIMER) && (u32)((wakeup_tick - 16) - tick_cur ) > BIT(30))
	if((unsigned int)((wakeup_tick - 16) - tick_cur ) > BIT(30))
	{
		tick_cur = wakeup_tick - 32;
	}
	reg_tmr0_tick = tick_cur;
	reg_system_tick = tick_cur;
	REG_ADDR8(0x74a) = 0x00;//clear
	REG_ADDR8(0x74a) = 0xa8;//recover system timer and enable calibrate

	unsigned char anareg44 = analog_read(0x44) & 0x0f;

	if(!anareg44){ //GPIO 错误的无法进入suspend
		anareg44 = STATUS_GPIO_ERR_NO_ENTER_PM;
	}
	else if ( (anareg44 & WAKEUP_STATUS_TIMER) && (wakeup_src & PM_WAKEUP_TIMER) )	//wakeup from timer only
	{
		while ((unsigned int)(clock_time() -  wakeup_tick) > BIT(30));
	}

	/* Exit critical region. */
	irq_restore(r);

	return anareg44;
}

_attribute_ram_code_ int cpu_sleep_wakeup_32kpad (int deepsleep, int wakeup_src,
		                                          unsigned int wakeup_tick)
{
	return 0;
}

/**
 * @Brief:
 * @Param:
 * @Return:
 */
void blc_pm_select_internal_32k_crystal(void)
{
	cpu_sleep_wakeup = cpu_sleep_wakeup_32krc;
	blt_miscParam.pm_enter_en = 1;
}

/**
 * @Brief:
 * @Param:
 * @Return:
 */
void blc_pm_select_external_32k_crystal(void)
{
	cpu_sleep_wakeup = cpu_sleep_wakeup_32kpad;
	blt_miscParam.pad32k_en = 1;
}

/**
 * @Brief: Called in blt_sdk_main_loop()
 * @Param:
 * @Return:
 */
void blt_pm_ext32k_crystal_init(void)
{
	if(blt_miscParam.pm_enter_en){
		return;
	}
	else if(clock_time_exceed(0, 5000000)){
		blt_miscParam.pm_enter_en = 1;
		analog_write(SYS_DEEP_ANA_REG, blt_miscParam.pm_enter_en);
	}
}

void cpu_stall_wakeup_by_timer0(unsigned int tick_stall)
{
   /*Write 0x00 here may cause problem, it is removed to blt_sleep_wakeup*/
   //write_reg8(0x6f,0x00);//clear bit[0] suspend enable

    reg_tmr0_tick = 0;

    reg_tmr0_capt = tick_stall;
    reg_tmr_ctrl8 |= FLD_TMR0_EN;//enable TIMER1,mode:using sclk
    reg_mcu_wakeup_mask |= FLD_IRQ_TMR0_EN;//timer1 mask enable
    reg_tmr_sta = FLD_TMR_STA_TMR0; // clean interrupt

    write_reg8(0x6f,0x80);//stall mcu
    asm("tnop");
    asm("tnop");

    reg_tmr_sta = FLD_TMR_STA_TMR0; // clean interrupt
    reg_tmr_ctrl8 &= ~FLD_TMR0_EN;//disable TIMER1
}

void cpu_stall_wakeup_by_timer1(unsigned int tick_stall)
{
   /*Write 0x00 here may cause problem, it is removed to blt_sleep_wakeup*/
   //write_reg8(0x6f,0x00);//clear bit[0] suspend enable

    reg_tmr1_tick = 0;

    reg_tmr_ctrl8 |= FLD_TMR1_EN;//enable TIMER1,mode:using sclk
    reg_mcu_wakeup_mask |= FLD_IRQ_TMR1_EN;//timer1 mask enable
    reg_tmr_sta = FLD_TMR_STA_TMR1; // clean interrupt

    write_reg8(0x6f,0x80);//stall mcu
    asm("tnop");
    asm("tnop");

    reg_tmr_sta = FLD_TMR_STA_TMR1; // clean interrupt
    reg_tmr_ctrl8 &= ~FLD_TMR1_EN;//disable TIMER1
}

void cpu_stall_wakeup_by_timer2(unsigned int tick_stall)
{
   /*Write 0x00 here may cause problem, it is removed to blt_sleep_wakeup*/
   //write_reg8(0x6f,0x00);//clear bit[0] suspend enable

    reg_tmr2_tick = 0;

    reg_tmr2_capt = tick_stall;
    reg_tmr_ctrl8 |= FLD_TMR2_EN;//enable TIMER1,mode:using sclk
    reg_mcu_wakeup_mask |= FLD_IRQ_TMR2_EN;//timer1 mask enable
    reg_tmr_sta = FLD_TMR_STA_TMR2; // clean interrupt

    write_reg8(0x6f,0x80);//stall mcu
    asm("tnop");
    asm("tnop");

    reg_tmr_sta = FLD_TMR_STA_TMR2; // clean interrupt
    reg_tmr_ctrl8 &= ~FLD_TMR2_EN;//disable TIMER1
}









//just for pm test
void shutdown_gpio(void)
{
	//output disable
	(*(volatile unsigned char*) 0x800582) = 0xff;
	(*(volatile unsigned char*) 0x80058a) = 0xff;
	(*(volatile unsigned char*) 0x800592) = 0xff;
	(*(volatile unsigned char*) 0x80059a) = 0xff;
	(*(volatile unsigned char*) 0x8005a2) = 0xff;
	(*(volatile unsigned char*) 0x8005aa) = 0x03;

	//dataO = 0
	(*(volatile unsigned char*) 0x800583) = 0x00;
	(*(volatile unsigned char*) 0x80058b) = 0x00;
	(*(volatile unsigned char*) 0x800593) = 0x00;
	(*(volatile unsigned char*) 0x80059b) = 0x00;
	(*(volatile unsigned char*) 0x8005a3) = 0x00;
	(*(volatile unsigned char*) 0x8005ab) = 0x00;

	//ie = 0
	//SWS   589<0>
	//MSPI  5a1<7:4>
	(*(volatile unsigned char*) 0x800581) = 0x00;
	(*(volatile unsigned char*) 0x800589) = 0x01;
	(*(volatile unsigned char*) 0x800591) = 0x00;
	(*(volatile unsigned char*) 0x800599) = 0x00;
	(*(volatile unsigned char*) 0x8005a1) = 0xf0;
	(*(volatile unsigned char*) 0x8005a9) = 0x00;

}

