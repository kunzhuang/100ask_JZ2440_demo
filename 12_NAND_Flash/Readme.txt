Readme
NAND(K9F2G08U0M) -> block(128KB)->page(row)->column(2KB)
One address -> one byte
1. Ignore any IRQs, exceptions, 2440 copies first 4K data from NAND to SDRAM
2. init_SDRAM_reloc inits NAND, and copies data from NAND [0x00000000, .data]to SDRAM [0x30000000, 0x30000000+.data], resets .bss
    Given an address, read data, copy to SDRAM (char, *address)
    start.s + init_SDRAM_reloc + nand_flash.c must be located <=4KB (addr 0x1000)
    Makefile obj order start.o init_SDRAM_reloc.o nand_flash.o
3. ldr pc, =main absolute jumps to SDRAM main

4. NAND init, clock, enable controller,  soft unlock
5. NAND CS, n_CS, CMD(1 byte), data(1 byte), nand flash data

6. Read NAND, print 16*5 array
    CS_EN -> CMD -> Column address X2 -> Row address X3 -> CMD -> NFDATA to buffer -> CS_DIS
7. Erase NAND block, 128KB/block
    CS_EN -> CMD -> Row address X3 -> CMD -> CS_DIS
8. Write NAND block, erase first
    CS_EN -> CMD -> Column address X2 -> Row address X3 -> CMD -> buffer to NFDATA -> CS_DIS
