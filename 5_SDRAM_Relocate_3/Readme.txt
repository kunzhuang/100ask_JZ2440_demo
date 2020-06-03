Readme

Clean/Reset .bss variable to zeros
GV inits zero variable

Makefile:
arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o SDRAM.elf //Linking script

sdram.lds
SECTIONS{
  .text 0 :{*(.text)}
  .rodata :{*(.rodata)}
  .data 0x30000000 :AT(0xa00)
  {
    _data_load_addr = LOADADDR(.data);
    _data_start = . ; /* Current location=0x30000000 */
    *(.data)
    _data_end = . ; /* Current location=0x30000000 + sizeof(*(.data)) */
  }
  _bss_start=.;
  .bss :{*(.bss) *(.common)}
  _bss_end=.;
}

start.o
//Init SDRAM_init()
bl SDRAM_init

//Clean/reset BSS
	ldr r1, =_bss_start
	ldr r2, =_bss_end
	mov r3, #0 //Init R3=0,*_bss_start=0
clean:
	strb r3, [r1]//R3=*R1
	add r1, r1, #1 //R1++
	cmp r1, r2
	bne clean //if _bss_start!=_bss_end

main.c
char Global_Char_2=0;

int main(void)
{
    while(1)
    {
	my_printf("Global_Char_2=0x%8x\n\r",  Global_Char_2);
	delay(1000000);
    }
    return 0;
}
