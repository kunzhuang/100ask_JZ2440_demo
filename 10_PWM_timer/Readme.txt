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
11. Enable interrupt control register, Interrupt mask register(INTMASK) via IRQ_registration
12. Create a function pointer (irq_array[bit](bit)) array to store key_eint_irq (key IRQ handler)

13. Calculate timer0 prescaler value and divider value (TCFG0=99, TCFG1 |=3) ->31250 Hz
14. Calculate Count Buffer Register value (TCNTB0 =31250) -> Interval ->1s
15. Compare buffer register is not used (TCMPB0), irrelevant to timer interrupt
15. Manual update from TCNTB0 and TCMPB0 (TCON |= (1<<1))
16. Clear manual update, auto-reload enable, timer start
17. Enable interrupt control register (timer0, bit=10), Interrupt mask register(INTMASK) via IRQ_registration
18. Create a function pointer (irq_array[bit](bit), bit=10) array to store PWM_timer_irq (timer0 IRQ handler)

19. External interrupt occurred, pc abs jumps back to 0x00000018@NAND/NOR
20. pc abs jumps (ldr pc, vector_base_IRQ) back to IRQ_handler 0x3000XXXX@SDRAM
21. Save R0 ~ R12, LR=LR-4(addr of next instruction)
22. bl relative jumps to IRQ_handler_c @SDRAM
23. IRQ_handler_c calls irq_array[bit](bit) that points to key_eint_irq or PWM_timer_irq
24. bit=INTOFFSET
25. key_eint_irq to determine which EINT is triggered, turn led on/off, clear pending bits
26. PWM_timer_irq to light up LEDs, clear pending bits
27. Recover R0 ~ R12, PC=LR, CPSR=SPSR
28. Continue PC=LR in main





Using Timer0 to generate interrupt procedures
GPIO controller
  Port F control registers(GPFCON), enable external interrupt
    GPF0(EINT0/S2), GPF2(EINT2/S3), GPG3(EINT11/S4), GPG11(EINT19/S5)
  External  interrupt control register, both edge triggered=11x
    EXTINT0 (EINT0, EINT2), EXTINT1[14:12] (EINT11), EXTINT2[14:12] (EINT19)
  External interrupt mask register, enable=0
    EINTMASK[3:0]=reserved, EINTMASK[11], EINTMASK[19]

Timer0 controller
  Timer clk = PCLK/{prescaler value+1}/{divider value}
            =50000000/(99+1)/16
            =31250 Hz
  TCFGx Timer Configuration Registerx
    TCFG0 =99;
    TCFG1 |=3 ->MUX 0 ->1/16 divider

  Count Buffer Register & Compare Buffer Register (TCNTBx TCMPBx)
  TCNTB0 =31250->Interval 1s
  //TCMPB0[15:0] not used in this example, we are only interested in interrupt request

Interrupt controller
  Interrupt mask register, 0 available, 1 masked
    INTMASK[ 0]->EINT0 ->via IRQ_registration
    INTMASK[ 1]->EINT1 ->via IRQ_registration
    INTMASK[ 2]->EINT2 ->via IRQ_registration
    INTMASK[10]->timer0->via IRQ_registration

CPSR[7]£¬I bit, IRQ disable 1/enable 0

lr=lr-4 before entering IRQ_handler_c

GPFDAT to control on/off LEDS
  GPFDAT[ 0](EINT0  /GPF0/S2)
  GPFDAT[ 2](EINT2  /GPF2/S3)
  GPGDAT[ 3](EINT11 /GPG3/S4)
  GPGDAT[11](EINT19/GPG11/S5)

Determine which EINT is triggered
  Using INTOFFSET Interrupt offset register to ID interrupt source from EINT0, EINT1, EINT2, EINT3, EINT4_7, EINT8_23, timer0

  Using EINTPEND external interrupt pending register (GPIO) to ID which EINT [EINT4 to 23] is triggered
    EINTPEND[11](EINT11), EINTPEND[19](EINT19)

Clear pending bits
		Write 1 bit back to SRCPND and INTPND£¬ bit position determined by INTOFFSET
		EINTPEND is cleared by writing 1 bit back
		Resetting INTOFFSET is not required.

		Source pending (SRCPND) register
		  SRCPND[0](EINT0), SRCPND[2](EINT2), SRCPND[5](EINT8_23), SRCPND[10](timer0)

		Interrupt pending(input) register (bit)
		  INTPND[0](EINT0), INTPND[2](EINT2), INTPND[5](EINT8_23), INTPND[10](timer0)
		INTOFFSET, Interrupt offset(intoffset) register (offset value)
			It shows which interrupt request of IRQ mode is in the INTPND register
			  EINT0=0, EINT2=2, EINT8_23=5, timer0=10
