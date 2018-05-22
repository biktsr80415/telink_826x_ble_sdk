
#ifdef MCU_STARTUP_8255

#ifndef __LOAD_RAM_SIZE__
#define	__LOAD_RAM_SIZE__		0xc
#endif

	.code	16
@********************************************************************************************************
@                                           MACROS AND DEFINIITIONS
@********************************************************************************************************
@.include "version.in"

					@ Mode, correspords to bits 0-5 in CPSR
	.equ MODE_BITS,		0x1F	@ Bit mask for mode bits in CPSR
	.equ IRQ_MODE, 		0x12	@ Interrupt Request mode
	.equ SVC_MODE, 		0x13	@ Supervisor mode 

	.equ IRQ_STK_SIZE,	0x180
	.equ __LOAD_RAM, 	__LOAD_RAM_SIZE__
	
@********************************************************************************************************
@                                            TC32 EXCEPTION VECTORS
@********************************************************************************************************

	.section	.vectors,"ax"
	.global		__reset
	.global	 	__irq
	.global 	__start
	.global		__LOAD_RAM

__start:					@ MUST,  referenced by boot.link

	.extern irq_handler

	.extern  _ramcode_size_div_16_
	.extern  _ramcode_size_div_256_
	.extern  _ramcode_size_div_16_align_256_
	.extern  _ramcode_size_align_256_
	.extern  _ictag_start_
	.extern  _ictag_end_

	.org 0x0
	tj	__reset
@	.word	(BUILD_VERSION)
	.org 0x8
	.word	(0x544c4e4b)
	.word	(0x00800000 + 0x08)    @_ramcode_size_div_16_align_256_
	@.word	(0x00880000 + _ramcode_size_div_16_align_256_)

	.org 0x10
	tj		__irq
	.org 0x18
	.word	(_bin_size_)
@********************************************************************************************************
@                                   LOW-LEVEL INITIALIZATION
@********************************************************************************************************
	.extern  main


	.org 0x20
__reset:

	@ add debug, PA4 output 1
@	tloadr     	r1, DEBUG_GPIO    @0x800582  PA4 oen
@	tmov		r0, #139      @0b 11101111 = 0xef = #139
@	tstorerb	r0, [r1, #0]

@	tmov		r0, #16			@0b00010000 = 0x10 = #16
@	tstorerb	r0, [r1, #1]	@0x800583  PA4 output

    tloadr  	r0, BOOT_CHECK              @r0 = 0x544c4e4b
    tloadr  	r1, FLL_D+12				@r1 = others or 0x544c4e4b
    tcmp    	r0, r1
    tjeq    	CHECK_RET					@r0== r1 jump to FLL_STK_END

	tloadr		r0, IC_IA             		@r0 = 0x800608
	tloadr		r1, CODE_CPY				@r1 = 0x000060  -> 0x000070
	tloadr		r2, CODE_CPY+4				@r2 = virtual_ram_code_size=vector_size+ram_code_size
	tmov        r4, #1						@r4 = 1
	tshftl      r4, r4, #24					@r4 = r4<<24

PWR_ON_CPY:
	tcmp		r1, r2						@
	tjge		FLL_STK_I					@r1>=r2 jump to FLL_STK_I

	tmov        r5, r4						@r5=r4
	tadd        r5, r1						@r5=r5+r1
	tstorer 	r5, [r0, #0]           		@*(unsigned int*)0x800608 = r5;

LOOP_WHILE:
	tloadr		r3, [r0, #0]           		@r3 = *(unsigned int*)0x800608;
	tasr        r3, r3, #24					@r3 = r3>>24
	tcmp        r3, #0
    tjne        LOOP_WHILE					@r3!=0 jump to loop_wait

    tadd    	r1, #16						@r1=r1+16
    tj          PWR_ON_CPY					@jump to PWR_ON_CPY
PWR_ON_CPY_END:

CHECK_RET:
	tloadr		r0, IC_IA+4
	tmov		r1, #1
	tstorer		r1, [r0,#0]        			@*(unsigned int*)0x800000 = 1    as flag used in C code
	tj			FLL_STK_END					@斯辉：直接跳到FLL_STK_END，_start_data_到ram最后没有赋值为0xffffffff，是何考虑？

    .balign 4
BOOT_CHECK:
    .word   (0x544c4e4b)

IC_IA:
    .word 0x00800608
    .word 0x00800000

CODE_CPY:
    .word   (0x00000080)               		@0
@    .word   (_ramcode_size_align_256_) 		@4
	.word   (0x4000)

	@ add debug, PA4 output 1
DEBUG_GPIO:
	.word	(0x80058a)                  @40  PA4 oen





	.org 0x80
FLL_STK_I:
	tloadr	r0, FLL_D
	tloadr	r1, FLL_D+4
	tloadr	r2, FLL_D+8

FLL_STK:
	tcmp	r1, r2
	tjge	FLL_STK_END
	tstorer r0, [r1, #0]
	tadd    r1, #4
	tj		FLL_STK
FLL_STK_END:

	tloadr	r0, DAT0
	tmcsr	r0			
	tloadr	r0, DAT0 + 8
	tmov	r13, r0  

	tloadr	r0, DAT0 + 4
	tmcsr	r0	
	tloadr	r0, DAT0 + 12
	tmov	r13, r0  

	tmov	r0, #0
	tloadr	r1, DAT0 + 16
	tloadr	r2, DAT0 + 20

ZERO:
	tcmp	r1, r2
	tjge	ZERO_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO
ZERO_END:

	tloadr	r1, DAT0 + 28
	tloadr	r2, DAT0 + 32

ZERO_TAG:
	tcmp	r1, r2
	tjge	ZERO_TAG_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO_TAG
ZERO_TAG_END:

SETIC:
	tloadr     	r1, DAT0 + 24
	tloadr      r0, DAT0 + 36					@ IC tag start
	tstorerb	r0, [r1, #0]
	tadd    	r0, #1							@ IC tag end
	tstorerb	r0, [r1, #1]
	@tmov		r0, #0;
	@tstorerb	r0, [r1, #2]


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

#if 0
SETSPISPEED:
	tloadr     	r1, DAT0 + 36
	tmov		r0, #0xbb		@0x0b for fast read; 0xbb for dual dat/adr
	tstorerb	r0, [r1, #0]
	tmov		r0, #3			@3 for dual dat/adr
	tstorerb	r0, [r1, #1]
#endif

	tjl	main
END:	tj	END

	.balign	4
DAT0:
	.word	0x12			    @IRQ    @0
	.word	0x13			    @SVC    @4
	.word	(irq_stk + IRQ_STK_SIZE)
	.word	(0x84c000)		    @12  stack end
	.word	(_start_bss_)               @16
	.word	(_end_bss_)                 @20
	.word	(0x80060c)                  @24
	.word	(0x844000)                  @28
	.word	(0x844100)                  @32
	.word	(0x40)                      @36
@	.word	_ictag_start_               @28		@ IC tag start
@	.word	_ictag_end_	            	@32		@ IC tag end
@	.word	_ramcode_size_div_256_		@36


@	.word	(0x808000 + __LOAD_RAM * 0x100)                  @28		@ IC tag start
@	.word	(0x808000 + (__LOAD_RAM + 1) * 0x100)            @32		@ IC tag end
@	.word	(0x80000e)    				@36
@	.word	(0x80058c)                  @36		gpio
@	.word	(0x800620)                  @40		watchdog
@	.word	(0x802c01)                  @44		watchdog
DATA_I:	
	.word	_dstored_
	.word	_start_data_
	.word	_end_data_

FLL_D:
	.word	0xffffffff                	@0
	.word	(_start_data_)             	@4
	@.word	(0x84c000)					@8
	.word	(_start_data_ + 32)
	.word	(0x544c4e4b)               	@12
	.word   (_retention_data_start_)    @16
    .word   (_retention_data_end_)      @20
    .word   (_rstored_)                 @24

	.align 4
__irq:
	tpush    	{r14}
	tpush    	{r0-r7}
	tmrss    	r0
	
	tmov		r1, r8
	tmov		r2, r9
	tmov		r3, r10
	tmov		r4, r11
	tmov		r5, r12
	tpush		{r0-r5}
	
	tjl      	irq_handler

	tpop		{r0-r5}
	tmov		r8, r1
	tmov		r9, r2
	tmov		r10,r3
	tmov		r11,r4
	tmov		r12,r5

	tmssr    	r0
	tpop		{r0-r7}
	treti    	{r15}

ASMEND:

	.section .bss
	.align 4
	.lcomm irq_stk, IRQ_STK_SIZE
	.end

#endif
