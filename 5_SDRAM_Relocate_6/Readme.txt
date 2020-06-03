Readme

Copy .text .rodata .data to SDRAM address
Bin starts from 0x30000000
Clean/reset BSS using C
  declear __code_star in lds
  call it using
  Ex:
	  extern int __code_start;
	  volatile unsigned int* SDRAM_start=(volatile unsigned int*) &__code_start;
	
ldr pc, =main  To Point to SDRAM
Do not use absolute address/static/GV/array before relocation

Makefile:
arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o SDRAM.elf //Linking script

sdram.lds
SECTIONS
{
	. = 0x30000000;

	__code_start = .;

	. = ALIGN(4);
	.text      :
	{
	  *(.text)
	}

	. = ALIGN(4);
	.rodata : { *(.rodata) }

	. = ALIGN(4);
	.data : { *(.data) }

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) *(.COMMON) }
	_end = .;
}

start.o
//Init SDRAM_init()
bl SDRAM_init

//Copy .text .rodata .data to SDRAM address
bl copy2sdram

//Clean/reset BSS
bl clean_reset_bss

//bl main //Program is still running in NOR/NANDÆô¶¯
ldr pc, =main //Point to SDRAM

main.c
char Global_Char_0='B';
char Global_Char_1='c';
char Global_Char_2=0;

int main(void)
{
    uart0_init();   //115200 8N1

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


init_HW.c
//Copy .text .rodata .data to SDRAM address
void copy2sdram(void)
{
  extern int __code_start, __bss_start; //decleared in lds
  volatile unsigned int* SDRAM_start=(volatile unsigned int*) &__code_start;
  volatile unsigned int* SDRAM_end=(volatile unsigned int*) &__bss_start;
  volatile unsigned int* NAND_NOR_src=(volatile unsigned int*) 0;

  while(SDRAM_start<SDRAM_end) //Right before __bss_start
  {
    *SDRAM_start++=*NAND_NOR_src++; //Copy data
  }
}


//Clean/reset BSS
void clean_reset_bss(void)
{
  extern int __bss_start, _end; //decleared in lds
  volatile unsigned int* SDRAM_start=(volatile unsigned int*) &__bss_start ;
  volatile unsigned int* SDRAM_end=(volatile unsigned int*) &_end;

  while(SDRAM_start<=SDRAM_end) //Right before __bss_start
  {
    *SDRAM_start++=0; //Write 0 to SDRAM __bss_start to _end
  }
}

