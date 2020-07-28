Readme
NOR flash region(bank) -> block(sector)
1. Ignore any IRQs, exceptions, the program starts from SDRAM
2. NOR flash is in read mode after reset
3. CPU to NOR is connected in 16 bite mode, ADDR CPU A1 <-> A0 NOR, writing in byte 1 then byte 0, length even number
4. Reading MX29LV800BBTC(35 sectors) is like access a memory address *pointer (only returns read 1 bytes/command)
5. Writing data/command into an address requires unlock commands
6. CFI mode provides flash size and sector starting addresses
7. Any write to or read from addresses is below 0x30000000 (SDRAM), when booting from NOR flash
8. Write sequence
    Enter an address(ending with x0, x2, x4, x6, x8, xA, xC, xE)
    Padding any input data in odd length by byte 1= 0x00
    Unlock, write words, Q6Toggling, verify data (buffer against NOR)
9. Erase sequence
    Enter an address
    Unlock, erase sector command
    If the address is the starting sector address, erase the entire sector
    If the address is between two starting sector addresses, erase the sector in between two sector addresses
    verify data (buffer against NOR)

10. When issuing command write or command read, always shift address <<1 before sent to NOR,
    ADDR CPU A1 <-> A0 NOR, NOR sees original command address
11. When issuing real addr write or addr read, always use original address, NOR sees shifted address,
    ADDR CPU A1 <-> A0 NOR, CPU wants to see the real addr
