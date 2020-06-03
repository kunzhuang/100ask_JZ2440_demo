Readme

Single GV

Makefile:
arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o SDRAM.elf //Linking script

sdram.lds
SECTIONS{
  .text 0 :{*(.text)}
  .rodata :{*(.rodata)}
  .data 0x30000000 :AT(0xa00) {*(.data)} //pointing .data(GV) 0xa00 to 0x30000000
  .bss :{*(.bss) *(.common)}
}

start.o
//Init SDRAM_init()
bl SDRAM_init

//把Global_Char 初始值挪去0x30000000
mov r1, #0xa00
ldr r0, [r1]
mov r1, #0x30000000
str r0, [r1]

main.c
char Global_Char='B';

int main(void)
{
    while(1)
    {
      put_char(Global_Char);
      Global_Char++;
      delay(1000000);
    }

    return 0;
}


