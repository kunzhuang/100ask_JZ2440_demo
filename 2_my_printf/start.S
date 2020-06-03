.text
.global _start

_start:

	ldr r0, =0x53000000 // Disable watch dog timer
	ldr r1, =0
	str r1, [r0]


	/*
		MPLL, FCLK, HCLK, PCLK configuration
		FCLK=400M, HCLK =100M, PCLK=50M
	*/
	ldr r0, =0x4C000000  //Lock time
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	ldr r0, =0x4C000014  //CLKDIVN, tF:tH:tP=1:4:8
	ldr r1, =0x05
	str r1, [r0]	//Clock divider controller 设置完成 需要设置异步模式

	/*协处理器 asynchromous 设置*/
	// MMU_SetAsyncBusMode
	mrc p15,0,r0,c1,c0,0
	orr r0, r0, #0xC0000000 //#R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	ldr r0, =0x4C000004  //MPLLCON, m, s p
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]



	/*
		分辨 NOR/NAND启动
		NAND启动0地址对应RAM
		NOR启动0地址对应NOR，需要特殊格式才能写入NOR
		写0 到0地址再读出来
		如果得到0，它对应RAM，这就是NAND启动，否则NOR
	*/

	mov r1, #0  // r1=0
	ldr r0, [r1]   // r0 读出原来的0地址值备份
	str r1, [r1]   // r1 写0
	ldr r2, [r1]   // r2 读r1值
	cmp r1, r2   // 对比r1 和r2
	ldr sp, =0x40000000+ 4096 // r1=!r2 NOR
	moveq sp, #4096 // r1==r2 NAND， 4K内存， stack pointer 指向4K
	streq r0, [r1] // r1==r2 恢复r0的值

	bl main

halt:
b halt //死循环
