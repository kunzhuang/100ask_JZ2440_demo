Readme

ldr/std operations -> 4 bytes -> 32bits
sdram.lds -> 4 byte alignment ->.=ALIGN(4);



Makefile:
arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o SDRAM.elf //Linking script

sdram.lds
SECTIONS{
  .text 0 :{*(.text)}
  .rodata :{*(.rodata)}
  .data 0x30000000 :AT(0xa00)
  {
    _data_load_addr = LOADADDR(.data);
    . = ALIGN(4);
    _data_start = .; /* Current location=0x30000000 */
    *(.data)
    _data_end = .; /* Current location=0x30000000 + sizeof(*(.data)) */
  }
  . = ALIGN(4);
  _bss_start = .;
  .bss :{*(.bss) *(.common)}
  _bss_end = .;
}
}

start.o
//Init SDRAM_init()
//Init SDRAM_init()
bl SDRAM_init

//Copy .data to SDRAM address
ldr r1, =_data_load_addr //data section start address in BIN
ldr r2, =_data_start          //SDRAM BASE ADD 0x30000000
ldr r3, =_data_end           //SDRAM BASE ADD 0x30000000  + sizeof(*(.data)
cpy:
 ldr r4, [r1]           //R4=*R1m, load  4 bytes
 str r4, [r2]           //*R2=R4,  store 4 bytes
 add r1, r1, #4     //R1+4
 add r2, r2, #4    //R2+4
 cmp r2, r3         //Until _data_start==_data_end
 ble cpy	             //R2<=R3 continue to cpy

//Clean/reset BSS
ldr r1, =_bss_start
ldr r2, =_bss_end
mov r3, #0     //Init R3=0,*_bss_start=0
clean:
str r3, [r1]     //R3=*R1, store 4 bytes
add r1, r1, #4 //R1+4
cmp r1, r2
ble clean      //if _bss_start<=_bss_end

bl main

main.c
char Global_Char_0='B';
char Global_Char_1='c';
char Global_Char_2=0;

int main(void)
{
    uart0_init();   //115200 8N1
    //nor_Tacc_init(7);

    while(1)
    {
      put_char(Global_Char_0);
      Global_Char_0++;
      delay(1000000);
	
      put_char(Global_Char_1);
      Global_Char_1++;
      delay(1000000);

      my_printf(" Global_Char_2=0x%8x\n\r",  Global_Char_2);
      delay(1000000);
    }

    return 0;
}
