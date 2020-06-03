.text
.global _start

_start:
	b reset 		@ Vector 0: reset, relative jump from 0x0 still in NAND/NOR
	@b udef_excp @ Vector 4: Undefined instruction exception, relative jump from 0x2 still in NAND/NOR
	@ldr pc,= udef_excp @ Vector 4: Undefined instruction exception, absolute jump SDRAM
											@ GCC may put PC->ADDx(halt)->udef_excp, ADDx may be located nearly out-of-bounds >=4KB
	ldr pc, vector_base_udef_excp @ Vector 4: Undefined instruction exception, absolute jump to SDRAM, udef_excp=0x3000XXXX

vector_base_udef_excp: @ SDRAM optimization
	.word udef_excp			 @ Continuous memory address <=4KB

udef_excp:
	@ Hardare handling procedures
	@  1. LR_und stores addr of next instruction
	@  2. SPSR_und=CPSR
	@  3. CPSR mode bits M[4:0]=11011
	@  4. PC point to 0x00000004 ->udef_excp


	@ Save  R0 ~ R12, LR
	ldr sp, =0x34000000 @ sp_und->0x4000000=67108864/1024/1024=64MB
	stmdb sp!, {r0-r12, lr} @ Save R0 ~ R12 and lr (link register, R14) to SP=0x34000000, top of stack, decrement before
													@ LR stores addr of next instruction, after finish handling the exception

	@ Handle undefined instruction exception
	mrs r0, cpsr @ move to ARM register from special register
	ldr r1, =udef_excp_string
	bl printException @ Output-> CPSR=0x600000DB M[4:0]=11011, relative jump to SDRAM

	@ Recover R0 ~ R12, PC
	ldmia sp!, {r0-r12, pc}^ @ Copy from addr SP to R0 ~ R12 and PC (program counter,R15), increment after
													 @ PC=LR
													 @  '^' copy SPSR back to CPSR
	@ msr @ move to special register from ARM register

udef_excp_string:
	.string "Undefined Instruction Exception Occurred"
.align 4


reset:
	ldr r0, =0x53000000 @  Disable watch dog timer
	ldr r1, =0
	str r1, [r0]


	/*
		MPLL, FCLK, HCLK, PCLK configuration
		FCLK=400M, HCLK =100M, PCLK=50M
	*/
	ldr r0, =0x4C000000  @ Lock time
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	ldr r0, =0x4C000014  @ CLKDIVN, tF:tH:tP=1:4:8
	ldr r1, =0x05
	str r1, [r0]	@ Clock divider controller 设置完成 需要设置异步模式

	/*协处理器 asynchromous 设置*/
	@  MMU_SetAsyncBusMode
	mrc p15,0,r0,c1,c0,0
	orr r0, r0, #0xC0000000 @ #R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	ldr r0, =0x4C000004  @ MPLLCON, m, s p
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]



	/*
		分辨 NOR/NAND启动
		NAND启动0地址对应RAM
		NOR启动0地址对应NOR，需要特殊格式才能写入NOR
		写0 到0地址再读出来
		如果得到0，它对应RAM，这就是NAND启动，否则NOR
	*/

	mov r1, #0  @  r1=0
	ldr r0, [r1]   @  r0 读出原来的0地址值备份
	str r1, [r1]   @  r1 写0
	ldr r2, [r1]   @  r2 读r1值
	cmp r1, r2   @  对比r1 和r2
	ldr sp, =0x40000000+ 4096 @  r1=!r2 NOR
	moveq sp, #4096 @  r1==r2 NAND， 4K内存， stack pointer 指向4K
	streq r0, [r1] @  r1==r2 恢复r0的值


	@ Init SDRAM_init()
	bl SDRAM_init

	@ Copy .text .rodata .data to SDRAM address
	bl copy2sdram

	@ Clean/reset BSS
	bl clean_reset_bss

	@ UART0 init
	ldr pc,=uart0init @ absolute jump to SDRAM, load the address of the function label uart0init into PC
uart0init:
	bl uart0_init

	bl my_printf_test1

undef_code:
	.word 0xdeadc0de @ Undefined instruction exception test

	bl my_printf_test2

	@ bl main @ Program is still running in NOR/NAND启动
	ldr pc, =main @ Point to SDRAM

halt:
	b halt @ 死循环
