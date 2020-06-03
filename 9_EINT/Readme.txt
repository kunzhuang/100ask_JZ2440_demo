Readme

1.  Program starts from 0x0, b reset
2.  Copy to sdram, relocation
3.  2240 switches from supervisor mode to user mode via CPSR to emulate linux apps
4.  CPSR[7]£¬I bit, IRQ disable 1/enable 0
5.  Program counter/pc abs jumps to UART0_init @SDRAM
6.  my_printf_test1/my_printf_test2
7.  Abs jump to main @SDRAM
8.  Enable I/O port control register LED (GPFCON)
9.  Enable I/O port external interrupt control register, both edge triggered (EXTINTx)
10. Enable I/O port external interrupt mask register (EINTMASK)
11. Enable interrupt control register, Interrupt mask register(INTMASK)
12. External interrupt occurred, pc abs jumps back to 0x00000018@NAND/NOR
13. pc abs jumps back to IRQ_handler 0x3000XXXX@SDRAM
14. Save R0 ~ R12, LR=LR-4(addr of next instruction)
15. bl relative jumps to IRQ_handler_c @NAND/NOR
16. IRQ_handler_c to determine which EINT is triggered,turn led on/off, clear pending bits
17. Recover R0 ~ R12, PC=LR, CPSR=SPSR
19. Continue PC=LR in main



Push button to control LED on/off using external interrupt procedures
GPIO controller
  Port F control registers(GPFCON), enable external interrupt
    GPF0(EINT0/S2), GPF2(EINT2/S3), GPG3(EINT11/S4), GPG11(EINT19/S5)
  External  interrupt control register, both edge triggered=11x
    EXTINT0 (EINT0, EINT2), EXTINT1[14:12] (EINT11), EXTINT2[14:12] (EINT19)
  External interrupt mask register, enable=0
    EINTMASK[3:0]=reserved, EINTMASK[11], EINTMASK[19]

Interrupt controller
  Interrupt mask register, 0 available, 1 masked
       INTMASK[0](EINT0), INTMASK[2](EINT2), INTMASK[5](EINT8_23)

CPSR[7]£¬I bit, IRQ disable 1/enable 0

lr=lr-4 before entering IRQ_handler_c

GPFDAT to control on/off LEDS
  GPFDAT[0](EINT0/GPF0/S2), GPFDAT[2](EINT2/GPF2/S3), GPGDAT[3](EINT11/GPG3/S4), GPGDAT[11](EINT19/GPG11/S5)

Determine which EINT is triggered
  Using INTOFFSET Interrupt offset register  to ID interrupt source from EINT0, EINT1, EINT2, EINT3, EINT4_7, EINT8_23

  Using EINTPEND external interrupt pending register (GPIO) to ID which EINT [EINT4 to 23] is triggered
    EINTPEND[11](EINT11), EINTPEND[19](EINT19)

Clear pending bits
		Write 1 bit back to SRCPND and INTPND£¬ bit position determined by INTOFFSET
		EINTPEND is cleared by writing 1 bit back
		Resetting INTOFFSET is not required.

		Source pending (SRCPND) register
		  SRCPND[0](EINT0), SRCPND[2](EINT2), SRCPND[5](EINT8_23)

		Interrupt pending(input) register (bit)
		  INTPND[0](EINT0), INTPND[2](EINT2), INTPND[5](EINT8_23)
		INTOFFSET, Interrupt offset(intoffset) register (offset value)
			It shows which interrupt request of IRQ mode is in the INTPND register
			  EINT0=0, EINT2=2, EINT8_23 =5
