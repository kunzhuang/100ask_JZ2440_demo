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
	str r1, [r0]	//Clock divider controller ������� ��Ҫ�����첽ģʽ

	/*Э������ asynchromous ����*/
	// MMU_SetAsyncBusMode
	mrc p15,0,r0,c1,c0,0
	orr r0, r0, #0xC0000000 //#R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	ldr r0, =0x4C000004  //MPLLCON, m, s p
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]



	/*
		�ֱ� NOR/NAND����
		NAND����0��ַ��ӦRAM
		NOR����0��ַ��ӦNOR����Ҫ�����ʽ����д��NOR
		д0 ��0��ַ�ٶ�����
		����õ�0������ӦRAM�������NAND����������NOR
	*/

	mov r1, #0  // r1=0
	ldr r0, [r1]   // r0 ����ԭ����0��ֵַ����
	str r1, [r1]   // r1 д0
	ldr r2, [r1]   // r2 ��r1ֵ
	cmp r1, r2   // �Ա�r1 ��r2
	ldr sp, =0x40000000+ 4096 // r1=!r2 NOR
	moveq sp, #4096 // r1==r2 NAND�� 4K�ڴ棬 stack pointer ָ��4K
	streq r0, [r1] // r1==r2 �ָ�r0��ֵ

	//Init SDRAM_init()
	bl SDRAM_init

	//Copy .data to SDRAM address
	ldr r1, =_data_load_addr //data section start address in BIN
	ldr r2, =_data_start     //SDRAM BASE ADD 0x30000000
	ldr r3, =_data_end				//SDRAM BASE ADD 0x30000000  + sizeof(*(.data)
cpy:
	 ldr r4, [r1] //R4=*R1m, load  4 bytes
	 str r4, [r2] //*R2=R4,  store 4 bytes
	 add r1, r1, #4 //R1+4
	 add r2, r2, #4 //R2+4
	 cmp r2, r3     //Until _data_start==_data_end
	 ble cpy				//R2<=R3 continue to cpy

//Clean/reset BSS
	ldr r1, =_bss_start
	ldr r2, =_bss_end
	mov r3, #0 //Init R3=0,*_bss_start=0
clean:
	str r3, [r1]//R3=*R1, store 4 bytes
	add r1, r1, #4 //R1+4
	cmp r1, r2
	ble clean //if _bss_start<=_bss_end

	bl main

halt:
b halt //��ѭ��