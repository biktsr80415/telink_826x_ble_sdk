#ifdef __PROJECT_USB_HOST__
	.code	16
@********************************************************************************************************
@                                           MACROS AND DEFINIITIONS
@********************************************************************************************************

                            @Mode, ctorespords to bits 0-5 in CPSR
	.equ MODE_BITS,		0x1F	@ Bit mask for mode bits in CPSR
	.equ IRQ_MODE, 		0x12	@ Interrupt Request mode
	.equ SVC_MODE, 		0x13	@ Supervisor mode

	.equ SVC_STK_SIZE,	0x100
	.equ IRQ_STK_SIZE,	0x40
	.equ __LOAD_RAM, 	0x11
@********************************************************************************************************
@                                            ARM EXCEPTION VECTORS
@********************************************************************************************************
	.code 16

	.section .vectors,"ax"
	.global __reset
	.global	__LOAD_RAM
	.global __start
	.extern  main
	.extern  _ramcode_size_div_256_

__start:					@ MUST,  referenced by boot.link
	.org 0x0
	tjl	__reset

	.org 0x8
	.word  (0x544c4e4b)
	.word  (0x008801c0)

	.org 0x10 @interrupt
	tpush	{r14}
	tmov	r14, r1
	tnop
	tnop

	@@@@  adjust rx delay: 1/4 marginal, 2/3 safe @@@@@@@@@@@
	tnop
	tnop
	@tnop
	@tnop
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	tloadrb	r1, [r0, #1]	@add delay to skip SYNC
	tsub r1, #3
	tjne	RX_RCV_DAT
	@	1/4 marginal; 2/3 OK
	tnop
	tnop
	@tnop
	@tnop
	tj		RX_WAIT_EOP_START
	.align 4
RX_RCV_DAT:
	tloadrb	r1, [r0, #0]
	tloadrb	r1, [r0, #0]
	@ SYNC end

	tloadrb	r1, [r0, #0]
	@@tnop
    @@tnop
    @@tnop
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r4, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r5, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r6, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r7, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r8, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r9, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r10, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r11, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1

	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r11, r3
	tloadrb	r2, [r0, #0]
        tmov	r3, #0
	tshftr	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #0
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #1
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #2
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #3
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #4
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #5
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #6
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #7
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #8
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #9
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #10
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #11
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #12
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #13
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #14
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #15
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #16
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #17
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #18
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #19
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #20
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #21
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #22
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #23
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #24
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #25
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #26
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #27
        tor     r3, r1
	tloadrb	r1, [r0, #0]
        @@tnop
	tshftl	r2, r2, #28
        tor     r3, r2
	tloadrb	r2, [r0, #0]
        @@tnop
	tshftl	r1, r1, #29
        tor     r3, r1


	tloadrb	r1, [r0, #0]
	tshftl	r2, r2, #30
        tor     r3, r2
        tmov	r12, r3

	tmov	r0, r14
	tstorer	r4, [r0, #0]
	tstorer	r5, [r0, #4]
	tstorer	r6, [r0, #8]
	tstorer	r7, [r0, #12]
	tmov	r4, r8
	tstorer	r4, [r0, #16]
	tmov	r4, r9
	tstorer	r4, [r0, #20]
	tmov	r4, r10
	tstorer	r4, [r0, #24]
	tmov	r4, r11
	tstorer	r4, [r0, #28]
	tmov	r4, r12
	tstorer	r4, [r0, #32]

	tj	RX_WAIT_EOP_END

RX_WAIT_EOP_START:
	tmov	r3, #1
	tshftl	r3, #9
	.align	4
RX_WAIT_EOP_LOOP:
	tloadrb	r1,	[r0, #0]
	tsub	r3, #1
	tjeq	RX_WAIT_EOP_END
	tcmp	r1,	#0
	tloadrb	r2, [r0, #0]
	tjne	RX_WAIT_EOP_LOOP
	tnop

RX_SEND_ACK_START:
	@SEND 0xd280
	tloadr	r1, [r0, #0]
	tloadr	r1, [r0, #0]
	tloadr	r1, [r0, #0]
	tloadr	r1, [r0, #0]
	tmov	r1,	#0
	tstorerb	r1, [r0, #2]	@change to output mode
	tloadr	r1, ACKDATA
RX_SEND_ACK_LOOP:
	tstorerb	r1, [r0, #3]
	tshftr		r1, #2
	tjne		RX_SEND_ACK_LOOP

	tmov	r1, #0
	tstorerb	r1, [r0, #3]
	tstorerb	r1, [r0, #3]
	tmov	r1, #2
	tmov	r1, #2
	tmov	r1, #2
	tmov	r1, #2
	tmov	r1, #2
	tstorerb	r1, [r0, #3]
	tmov	r1, #3
	tmov	r1, #3
	tmov	r1, #3
	tstorerb	r1, [r0, #2]	@change to input mode

RX_WAIT_EOP_END:
	tloadr	r0, INTDAT0 + 0
	tloadr	r1, INTDAT0 + 4
	tstorer	r1, [r0, #0]
	treti       {r15}

	.align	4
ACKDATA:
	.word	0x569a5999
INTDAT0:
	.word	0x800648
	.word	0xffffffff

	.balign 4
	.thumb_func
__reset:
	tloadr	r0, DAT1
	tmcsr	r0			
	tloadr	r0, DAT1 + 8
	tmov	r13, r0  

	tloadr	r0, DAT1 + 4
	tmcsr	r0	
	tloadr	r0, DAT1 + 12
	tmov	r13, r0  

	tmov	r0, #0
	tloadr	r1, DAT1 + 16
	tloadr	r2, DAT1 + 20
ZERO:
	tstorer	r0, [r1, #0]
	tadd     r1, #4
	tcmp	r1, r2
	tjle	ZERO
ZERO_END:

	tloadr	r1, DAT1 + 28
	tloadr	r2, DAT1 + 32

ZERO_TAG:
	tcmp	r1, r2
	tjge	ZERO_TAG_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO_TAG
ZERO_TAG_END:

SETIC:
	tloadr     	r1, DAT1 + 24
	tloadr      r0, DAT1 + 36					@ _ramcode_size_div_256_
	tstorerb	r0, [r1, #0]
	tadd    	r0, #1							@ IC tag end
	tstorerb	r0, [r1, #1]

	tloadr		r1, DATA_I
	tloadr		r2, DATA_I+4
	tloadr		r3, DATA_I+8

COPY_DATA:
	tcmp		r2, r3
	tjge		COPY_DATA_END
	tloadr		r0, [r1, #0]
	tstorer 	r0, [r2, #0]
	tadd    	r1, #4
	tadd		r2, #4
	tj			COPY_DATA
COPY_DATA_END:

	tjl	main
END:	tj	END

	.balign	4
DAT1:
	.word	0x12			    @IRQ    @0
	.word	0x13			    @SVC    @4
	.word	(irq_stk + IRQ_STK_SIZE)  @irq-stack top, reserved 16 byte hole to easy debug mem layout
	.word	(0x80bfe0)		    @12  stack end
	.word	(_start_bss_)               @16
	.word	(_end_bss_)                 @20
	.word   (0x80060c)                  @24
	.word	_ictag_start_               @28		@ IC tag start
	.word	_ictag_end_	            	@32		@ IC tag end
	.word	_ramcode_size_div_256_		@36
	        @32		@ IC tag end
DATA_I:
	.word	_dstored_
	.word	_start_data_
	.word	_end_data_

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@		usb_cmd function:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global	usb_cmd
	.thumb_func
usb_cmd:
	tpush	{r4, r5, r6, r7, lr}
	tmov	r4, r8
	tmov	r5, r9
	tmov	r6, r10
	tmov	r7, r11
	tpush	{r4, r5, r6, r7}
	tmov	r4, r12
	tpush	{r4}
	tpush	{r2}

	tloadr	r5, [r0, #0]
	tloadr	r6, [r0, #4]
	tloadr	r7, [r0, #8]
	tloadr	r3, [r0, #12]
	tmov	r8, r3
	tloadr	r3, [r0, #16]
	tmov	r9, r3
	tloadr	r3, [r0, #20]
	tmov	r10, r3
	tloadr	r3, [r0, #24]
	tmov	r11, r3
	tloadr	r3, [r0, #28]
	tmov	r12, r3
	tloadr	r3, [r0, #32]
	tmov	r14, r3

	tloadr	r0, USBDAT0 + 0
	tmov	r3, #2
	tstorerb	r3, [r0, #3] @OUTPUT [1:0]
	tloadr	r3, USBDAT0 + 4
	tstorerb	r3, [r0, #2] @OEN [1:0]

	tloadr	r2, USBDAT0 + 8
	tloadr	r3, USBDAT0 + 12
	tloadr	r4, USBDAT0 + 16

	tj	TX_START	

	.align	4
USBDAT0:
	.word	0x00800598     @PDx Input reg.
	.word   0x000000fc     @0b11111100. PD0\PD1 OUTPUT
	.word   0xffffffff
	.word   0x00800640     @interrupt mask0
	.word   0x01040000			@GPIO interupt enable: bit24 (global enable); bit18 (gpio_irq enable)

TX_START:
	tnop
	tnop
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R0
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]

	tjeq	TX_EOP
	tmov	r5, r6
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R0:
	tjeq	TX_R2
	tmov	r5, r7
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP		@ token end: 
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_EOP
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjne	TX_TOKEN_END
TX_EOP:
	tj	TX_END
TX_TOKEN_END:
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R2
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R1:
	tjeq	TX_R3
	tmov	r5, r8
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R3
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R2:
	tjeq	TX_R4
	tmov	r5, r9
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R4
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R3:
	tjeq	TX_R5
	tmov	r5, r10
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R5
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R4:
	tjeq	TX_R6
	tmov	r5, r11
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R6
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R5:
	tjeq	TX_R7
	tmov	r5, r12
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R7
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R6:
	tjeq	TX_R8
	tmov	r5, r14
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
	tjeq	TX_R8
	tshftr     r5, r5, #2
	tsub	r1, #1
	tstorerb	r5, [r0, #3]
TX_R7:
TX_R8:
	tnop
TX_END:
	tnop
	tnop
	tmov		r1, #2
	tstorerb	r1, [r0, #3]		@drive SE0 end, drive J
	tstorerb	r2, [r0, #2]		@float DP/DM
	tpop	{r1}
	tstorer	r2, [r3, #8]			@clear all interrupt flag
	tstorer	r4, [r3, #0]			@enable gpio interrupt??
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tnop
	tmov	r5, #0	
	tloadr	r4, DATA_WI
	tstorer	r5, [r4, #0]			@disable interrupt

	tpop	{r4}
	tmov	r12, r4
	tpop	{r4, r5, r6, r7}
	tmov	r8, r4
	tmov	r9, r5
	tmov	r10, r6
	tmov	r11, r7
	tpop	{r4, r5, r6, r7, pc}
	

	.align	4
DATA_WI:
	.word   0x00800640
ASMEND:
        .align 4

	.section .bss
	.align 4
	.lcomm irq_stk, IRQ_STK_SIZE
	.lcomm svc_stk, SVC_STK_SIZE
	.end
#endif
