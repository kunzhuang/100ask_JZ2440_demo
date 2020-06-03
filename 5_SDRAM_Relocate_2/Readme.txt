Readme

Mutiple GVs, copy all .data data to SDRAM

Makefile:
arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o SDRAM.elf //Linking script

sdram.lds
SECTIONS{
  .text 0 :{*(.text)}
  .rodata :{*(.rodata)}
  .data 0x30000000 :AT(0xa00)
  {
    data_load_addr = LOADADDR(.data);
    data_start = . ; /* Current location=0x30000000 */
    *(.data)
    data_end = . ; /* Current location=0x30000000 + sizeof(*(.data)) */
  }
  .bss :{*(.bss) *(.common)}
}

start.o
//Init SDRAM_init()
bl SDRAM_init

//Copy .data to SDRAM address, 1 byte by 1 byte
ldr r1, =data_load_addr    //data section start address in BIN
ldr r2, =data_start              //SDRAM BASE ADD 0x30000000, start address
ldr r3, =data_end	   //SDRAM BASE ADD 0x30000000  + sizeof(*(.data), end address
cpy:
	 ldrb r4, [r1]                        //R4=*R1
	 strb r4, [r2]                       //*R2=R4
	 add r1, r1, #1                   //R1++
	 add r2, r2, #1                   //R2++
	 cmp r2, r3                        //Until data_start==data_end
	 bne cpy	              //Not equal continue to cpy

main.c
char Global_Char_0='B';
char Global_Char_1='c';

int main(void)
{
    while(1)
    {
      put_char(Global_Char_0);
      Global_Char_0++;
      delay(1000000);

     put_char(Global_Char_1);
     Global_Char_1++;
     delay(1000000);
    }
    return 0;
}