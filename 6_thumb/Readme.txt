Readme

start.o
before _start
.code 32 //ARM instruction

before bl SDRAM_init

	adr r0, thumb_func
	add r0, r0, #1 //bit0=1, bx ->CPU state to Thumb
	bx r0 //ARMÌø×ªThumb
.code 16 //Thumb instruction
thumb_func:
	//Init SDRAM_init()
	bl SDRAM_init

	ldr r0, =main
	mov pc, r0//Point to SDRAM to start main


Makefile
	arm-none-eabi-gcc -Wall -mthumb -c -o $@ $<