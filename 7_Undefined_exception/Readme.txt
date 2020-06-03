Readme

1. Program starts from 0x0, b reset
2. Copy to sdram, relocation
3. Program counter/pc abs jumps to UART0_init @SDRAM
4. Undefined instruction occurred, pc abs jumps back to 0x00000004@NAND/NOR
5. pc abs jumps back to udef_excp0x3000XXXX@SDRAM
6. Save R0 ~ R12, LR(addr of next instruction)
7. Handle undefined instruction exception
8. Recover R0 ~ R12, PC=LR, CPSR=SPSR
