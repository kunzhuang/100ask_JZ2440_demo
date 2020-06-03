Readme

1. Program starts from 0x0, b reset
2. Copy to sdram, relocation
3. 2240 switches from supervisor mode to user mode via CPSR to emulate linux apps
4. Program counter/pc abs jumps to UART0_init @SDRAM
5. Software interrupt occurred, pc abs jumps back to 0x00000008@NAND/NOR
6. pc abs jumps back to SWI_excp 0x3000XXXX@SDRAM
7. Save R0 ~ R12, LR(addr of next instruction)
8. Handle software interrupt exception
    sub r0, r4, #4, r4=lr
    SWI address and mask with 0xff000000
9. Recover R0 ~ R12, PC=LR, CPSR=SPSR
10. Abs jump to main @SDRAM
