#include "register.h"
#include "clock.h"
#include "irq.h"
#include "analog.h"

void clock_init(void)
{
	/* External XTAL act as clock source */
	if(CLOCK_SRC == EXTERNANL_XTAL)
	{
		if(CLOCK_SYS_CLOCK_HZ == SYS_CLK_16M_PAD)
		{
			SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC_DIV = 3;
			SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC = SYS_CLK_SRC_FHS_DIV;
			SYSCLK_SEL->ClkSelBits.FHS_CLK_SRC_L = FHS_CLK_SRC_L_48M_OR_24M_PAD;
			reg_fhs_sel = FHS_CLK_SRC_H_48M_OR_24M_RC;
		}
		else if(CLOCK_SYS_CLOCK_HZ == SYS_CLK_24M_PAD)
		{
			SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC_DIV = 2;
			SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC = SYS_CLK_SRC_FHS_DIV;
			SYSCLK_SEL->ClkSelBits.FHS_CLK_SRC_L = FHS_CLK_SRC_L_48M_OR_24M_PAD;
			reg_fhs_sel = FHS_CLK_SRC_H_48M_OR_24M_RC;
		}
		else if(CLOCK_SYS_CLOCK_HZ == SYS_CLK_32M_PAD)
		{
			 SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC = SYS_CLK_SRC_FHS_2V3_DIV;
			 SYSCLK_SEL->ClkSelBits.FHS_CLK_SRC_L = FHS_CLK_SRC_L_48M_OR_24M_PAD;
			 reg_fhs_sel = FHS_CLK_SRC_H_48M_OR_24M_RC;
		}
	}
	/* Internal RC act as clock source */
	else if(CLOCK_SRC == INTERNAL_RC)
	{
		if(CLOCK_SYS_CLOCK_HZ == SYS_CLK_16M_RC)
		{
			SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC = SYS_CLK_SRC_FHS_2V3_DIV;
			SYSCLK_SEL->ClkSelBits.FHS_CLK_SRC_L = FHS_CLK_SRC_L_24M_RC;
			reg_fhs_sel = FHS_CLK_SRC_H_48M_OR_24M_RC;
		}
		else if(CLOCK_SYS_CLOCK_HZ == SYS_CLK_24M_RC)
		{
			SYSCLK_SEL->ClkSelBits.SYS_CLK_SRC = SYS_CLK_SRC_24M_RC;
		}
	}
	/* clock source error! */
	else
	{
		while(1);
	}

	/* WatchDog Configuration */
	#if(MODULE_WATCHDOG_ENABLE)
		wd_startEx(WATCHDOG_INIT_TIMEOUT);
	#endif

	/* Timer0 initialization for BLE */
	reg_tmr_ctrl8 &= ~0x01;
	reg_tmr0_tick = REG_ADDR32(0x740);//"reg_tmr0_tick" must be set as "REG_ADDR32(0x740)".
	reg_irq_mask |=	FLD_IRQ_TMR0_EN;  //Enable Timer0 Interrupt
	write_reg8(0x63c,0x01); //continuous tick mode
	reg_tmr_ctrl8 |= 0x01;  //Enable Timer0
}

_attribute_ram_code_ unsigned int clock_time(void)
{
 	#if 0
 		return reg_tmr0_tick;
 	#else
 		return reg_system_tick;
 	#endif
}

_attribute_ram_code_
unsigned int clock_time_exceed(unsigned int ref, unsigned int span_us)
{
 	return ((unsigned int)(clock_time() - ref) > span_us * sys_tick_per_us);
}

_attribute_ram_code_
void sleep_us (unsigned int us)
{
	unsigned int t = clock_time();
	while(!clock_time_exceed(t, us)){}
}

/**
 * @Brief:  24M RC Calibration.(error: 0.01%)
 * @Param:  None.
 * @Return: None.
 */
void MCU_24M_RC_ClockCalibrate(void)
{
	unsigned char temp = 0;

	temp = analog_read(0x02);
	temp |= (1<<4);
	analog_write(0x02,temp);

	/* Enable 24M RC calibration. */
	temp = analog_read(0x83);
	temp |= (1<<0);
	temp &= ~(1<<1);
	analog_write(0x83,temp);

	/* Wait Calibration completely. */
	while(!(analog_read(0x84) & 0x01));

	unsigned char CalValue = 0;
	CalValue = analog_read(0x85);
	analog_write(0x30,CalValue);

	/* Disable 24M RC calibration. */
	temp = analog_read(0x83);
	temp &= ~(1<<0);
	analog_write(0x83,temp);

	temp = analog_read(0x02);
	temp &= ~(1<<4);
	analog_write(0x02,temp);
}
/*----------------------------- End of File ----------------------------------*/
