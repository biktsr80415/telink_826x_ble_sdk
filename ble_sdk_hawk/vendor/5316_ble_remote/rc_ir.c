/**
 *******************************************************************************
 * @FileName:  rc_ir.c
 * @Create:    2018-06-25
 * @Author:    qiu.gao@telink-semi.com
 * @Copyright: (C) Telink semi Ltd.cn
 * @Brief:
 *******************************************************************************
 */

#include "drivers.h"
#include "tl_common.h"
#include "rc_ir.h"
#include "app_config.h"

#if (REMOTE_IR_ENABLE)

IR_DataTypeDef irData;
static volatile u8 IrState = 0;

static u16 irStartMark = 0;
static u16 irStartSpace = 0;

static u16 irLogic0Mark = 0;
static u16 irLogic0Space = 0;

static u16 irLogic1Mark = 0;
static u16 irLogic1Space = 0;

static u16 irStopMark = 0;
static u16 irStopSpace = 0;

static u16 irWaitSpace = 0;
static u16 irWaitSpaceCnt   = 0;
static u16 irWaitRemainSpace = 0;

static u16 irRepeatStartMark = 0;
static u16 irRepeatSartSpace = 0;
static u16 irRepeatMark = 0;

static u16 irRepeatWaitSpace = 0;
static u16 irRepeatWaitSpaceCnt = 0;
static u16 irRepeatWaitRemainSpace = 0;

/**
 * @Brief:  IR initialization.
 * @Param:  IR_Pin ->
 * @Return: None.
 */
void IR_Init(eIR_PinTypeDef IR_Pin)
{
	if(IR_Pin == IR_Pin_PA0)//PA0
	{
		gpio_set_func(GPIO_PA0, AS_PWM);
		GPIOA_AF->RegBits.P0_AF = GPIOA0_PWM0;
	}
	else if(IR_Pin == IR_Pin_PB3)//PB3
	{
		gpio_set_func(GPIO_PB3, AS_PWM);
		GPIOB_AF->RegBits.P3_AF = GPIOB3_PWM0;
	}
	else
	{
		return;
	}

	/* PWM Initialization */
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	pwm_set_cycle_and_duty(PWM0, IR_CARRIER_CYCLE, IR_CARRIER_CYCLE/IR_CARRIER_DUTY );//38K, duty = 1/3
	pwm_set_mode(PWM0, PWM_MODE_IR_DMA_FIFO);
	pwm_polarity_set(PWM0, 1);

	/* IR timing Initialization */
	u32 irSendTime = 0;//Unit:us

	//IR Start bit timing
	irStartMark = pwm_set_waveform_data(1, PWM_SRC_NORMAL, (IR_START_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irStartSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, (IR_START_NO_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irSendTime += (IR_START_CARRIER_TIME + IR_START_NO_CARRIER_TIME);

	//IR Logic 1 timing
	irLogic1Mark = pwm_set_waveform_data(1, PWM_SRC_NORMAL,(IR_1_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irLogic1Space = pwm_set_waveform_data(0, PWM_SRC_NORMAL,(IR_1_NO_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irSendTime += (IR_1_CARRIER_TIME + IR_1_NO_CARRIER_TIME);

	//IR Logic 0 timing
	irLogic0Mark = pwm_set_waveform_data(1, PWM_SRC_NORMAL,(IR_0_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irLogic0Space = pwm_set_waveform_data(0, PWM_SRC_NORMAL,(IR_0_NO_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irSendTime += (IR_0_CARRIER_TIME + IR_0_NO_CARRIER_TIME);

	//IR Stop bit timing
	irStopMark = pwm_set_waveform_data(1, PWM_SRC_NORMAL,(IR_STOP_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irStopSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL,(IR_STOP_NO_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irSendTime += (IR_STOP_CARRIER_TIME + IR_STOP_NO_CARRIER_TIME);

	u32 irWaitTime = IR_SPACE_TIME - irSendTime;

	//IR Wait timing
	irWaitSpace = irWaitTime * IR_CARRIER_FRE/1000000;
	irWaitSpaceCnt = irWaitSpace / 0x3fff;
	irWaitRemainSpace = irWaitSpace % 0x3fff;
	irWaitSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, 0x3fff);
	irWaitRemainSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, irWaitRemainSpace);

	//IR Repeat start bit timing
	irSendTime = 0;
	irRepeatStartMark = pwm_set_waveform_data(1,PWM_SRC_NORMAL, (IR_REPEAT_START_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irRepeatSartSpace = pwm_set_waveform_data(0,PWM_SRC_NORMAL, (IR_REPEAT_START_NO_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irSendTime += (IR_REPEAT_START_CARRIER_TIME + IR_REPEAT_START_NO_CARRIER_TIME);

	//IR Repeat timing
	irRepeatMark = pwm_set_waveform_data(1,PWM_SRC_NORMAL, (IR_REPEAT_CARRIER_TIME * IR_CARRIER_FRE/1000000));
	irSendTime += IR_REPEAT_CARRIER_TIME;

	u32 irRepeatWaitTime = IR_SPACE_TIME - irSendTime;

	irRepeatWaitSpace = irRepeatWaitTime * IR_CARRIER_FRE/1000000;
	irRepeatWaitSpaceCnt = irRepeatWaitSpace / 0x3fff;
	irRepeatWaitRemainSpace = irRepeatWaitSpace % 0x3fff;
	irRepeatWaitSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, 0x3fff);
	irRepeatWaitRemainSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, irRepeatWaitRemainSpace);

	irData.len = 0;
	IrState = IR_STATE_IDLE;
}

/**
 * @Brief:  Send IR wave of NEC format.
 * @Param:  addr1 ->
 * @Param:  addr2 ->
 * @cmd:    cmd   ->
 * @Return: None.
 */
void IR_SendNec(u8 addr1, u8 addr2, u8 cmd)
{
	u32 irCode = (((~cmd)|0x00000000)<<24) | ((cmd|0x00000000)<<16) | ((addr2|0x00000000)<<8) | (addr1|0x00000000);

	u32 cnt = 0;

	u32 irSendTime = 0;

	//IR Start timing
	irData.data[cnt++] = irStartMark;
	irData.data[cnt++] = irStartSpace;
	irSendTime += (IR_START_CARRIER_TIME + IR_START_NO_CARRIER_TIME);

	//IR data bit timing
	for(int i = 0; i< 32; i++)
	{
		if(irCode & (0x00000001<<i))//Logic "1"
		{
			irData.data[cnt++] = irLogic1Mark;
			irData.data[cnt++] = irLogic1Space;
			irSendTime += (IR_1_CARRIER_TIME + IR_1_NO_CARRIER_TIME);
		}
		else//Logic "0"
		{
			irData.data[cnt++] = irLogic0Mark;
			irData.data[cnt++] = irLogic0Space;
			irSendTime += (IR_0_CARRIER_TIME + IR_0_NO_CARRIER_TIME);
		}
	}

	//IR Stop bit timing
	irData.data[cnt++] = irStopMark;
	irData.data[cnt++] = irStopSpace;
	irSendTime += (IR_STOP_CARRIER_TIME + IR_STOP_NO_CARRIER_TIME);

	u32 irWaitTime = IR_SPACE_TIME - irSendTime;

	//IR Wait timing
	irWaitSpace = irWaitTime * IR_CARRIER_FRE/1000000;
	irWaitSpaceCnt = irWaitSpace / 0x3fff;
	irWaitRemainSpace = irWaitSpace % 0x3fff;
	irWaitSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, 0x3fff);
	irWaitRemainSpace = pwm_set_waveform_data(0, PWM_SRC_NORMAL, irWaitRemainSpace);

	for(int i = 0; i<irWaitSpaceCnt; i++)
	{
		irData.data[cnt++] = irWaitSpace;
	}
	irData.data[cnt++] = irWaitRemainSpace;

	//IR Data length
	irData.len = cnt * 2;

	//PWM Start
	pwm_dma_ir_data_config((u16*)&irData);
	pwm_dma_enable();
	pwm_clear_irq_status(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);
	pwm_interrupt_enable(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);

	IrState = IR_STATE_SENDING;
}

/**
 * @Brief:  Send IR repeat wave of NEC format.
 * @Param:  None.
 * @Return: None.
 */
void IR_SendRepeat(void)
{
	irData.len = 0;

	u32 cnt = 0;

	//IR Repeat start timing
	irData.data[cnt++] = irRepeatStartMark;
	irData.data[cnt++] = irRepeatSartSpace;

	//IR Repeat timing
	irData.data[cnt++] = irRepeatMark;

	//IR Repeat wait timing
	for(int i = 0; i< irRepeatWaitSpaceCnt; i++)
	{
		irData.data[cnt++] = irRepeatWaitSpace;
	}
	irData.data[cnt++] = irRepeatWaitRemainSpace;

	//IR Data length
	irData.len = cnt * 2;

	//PWM Start
	pwm_dma_ir_data_config((u16*)&irData);
	pwm_dma_enable();
	pwm_clear_irq_status(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);
	pwm_interrupt_enable(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);

	IrState = IR_STATE_SENDING;
}

/**
 * @Brief:  Stop sending IR.(The function can ensure the sending of complete IR waves.)
 * @Param:  None
 * @Return: None.
 */
void IR_Release(void)
{
	IrState = IR_STATE_STOP;
}

/**
 * @Brief:  Stop IR Sending directly.
 * @Param:  None.
 * @Return: None.
 */
void IR_Stop(void)
{
	//Reset PWM
	reg_rst_clk0 |= FLD_RST_PWM;
	for(int i = 0; i < CLOCK_SYS_CLOCK_1US*20; i++);
	reg_rst_clk0 &= ~FLD_RST_PWM;

	pwm_clear_irq_status(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);
	pwm_interrupt_disable(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);

	irData.len = 0;
	IrState = IR_STATE_IDLE;
}

/**
 * @Brief:  Get IR state
 * @Param:  None.
 * @Return: IR state IR_STATE_SENDING or IR_STATE_IDLE or IR_STATE_STOP.
 */
u8 IR_GetIrState(void)
{
	return IrState;
}

/**
 * @Brief:  IR interrupt routine.
 * @Param:  None.
 * @Return: None.
 */
void IR_IRQHandler(void)
{
	if(pwm_get_irq_status(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY))
	{
		//Clear interrupt Flag
		pwm_clear_irq_status(FLD_PWM_IRQ_PWM0_IR_DMA_FIFO_EMPTY);

		if(IrState == IR_STATE_SENDING)
		{
			IR_SendRepeat();
		}
		else if(IrState == IR_STATE_STOP)
		{
			IR_Stop();
		}
	}
}

#endif/* End of REMOTE_IR_ENABLE */
