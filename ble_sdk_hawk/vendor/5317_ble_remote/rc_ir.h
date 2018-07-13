/**
 *******************************************************************************
 * @FileName:  rc_ir.h
 * @Create:    2018-06-25
 * @Author:    qiu.gao@telink-semi.com
 * @Copyright: (C) Telink semi Ltd.cn
 * @Brief:
 *******************************************************************************
 */

#ifndef _RC_IR_H_
#define _RC_IR_H_

#include <tl_common.h>
#include <drivers.h>

/* Macro define --------------------------------------------------------------*/
#define IR_CFG_DATA_BUF_SIZE      (100)
#define IR_PROTOCOL_TYPE          IR_PROTOCOL_NEC
#define IR_PROTOCOL_NEC           (1)

/*----------------------------------------------------------------------------*/
/*----------------- NEC Protocol                        ----------------------*/
/*----------------------------------------------------------------------------*/
#if(IR_PROTOCOL_TYPE == IR_PROTOCOL_NEC)
	#define IR_CARRIER_FRE                  (38000)//38KHz
	#define IR_CARRIER_CYCLE                (CLOCK_SYS_CLOCK_HZ/IR_CARRIER_FRE)//Unit: PWM clock tick number
	#define IR_CARRIER_DUTY                 (3)//duty rate :1/3

	#define IR_START_CARRIER_TIME           (9000)
	#define IR_START_NO_CARRIER_TIME        (4500)

	#define IR_1_CARRIER_TIME               (560)
	#define IR_1_NO_CARRIER_TIME            (1690-5)
	#define IR_0_CARRIER_TIME               (560)
	#define IR_0_NO_CARRIER_TIME            (560-5)

	#define IR_STOP_CARRIER_TIME            (560)
	#define IR_STOP_NO_CARRIER_TIME         (500)

	#define IR_REPEAT_START_CARRIER_TIME    (9000)
	#define IR_REPEAT_START_NO_CARRIER_TIME (2250)
	#define IR_REPEAT_CARRIER_TIME          (560 )

	#define IR_SPACE_TIME                   (110000)//110ms
#else

#endif

/* typedef define ------------------------------------------------------------*/
/* IR state enum define */
typedef enum{
	IR_STATE_IDLE    = 0x00,
	IR_STATE_SENDING = 0x01,
	IR_STATE_STOP    = 0x02,
}eIR_StateTypeDef;

/* IR pin enum define */
typedef enum{
	IR_Pin_PA0,
	IR_Pin_PB3,
}eIR_PinTypeDef;

/* IR data structure define */
typedef struct{
	u32 len;
	u16 data[IR_CFG_DATA_BUF_SIZE];
}IR_DataTypeDef;

/* external variables --------------------------------------------------------*/
/* external functions --------------------------------------------------------*/
void IR_Init(eIR_PinTypeDef IR_Pin);
void IR_SendNec(u8 addr1, u8 addr2, u8 cmd);
void IR_SendRepeat(void);
void IR_Release(void);
void IR_Stop(void);

u8 IR_GetIrState(void);

void IR_IRQHandler(void);

#endif /* RC_IR_H_ */
