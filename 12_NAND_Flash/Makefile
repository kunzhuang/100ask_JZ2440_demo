ifeq (1,1)
LIBPATH :=
NLIBPATH :=
LIBPATH +=-lgcc -L/usr/lib/gcc/arm-none-eabi/6.3.1/ #libgcc library: it provides some basic hardware functionalities, such as the division.
NLIBPATH += /usr/lib/arm-none-eabi/newlib/libc.a #https://www.sourceware.org/newlib/libc.html
																								 #https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html
objs := start.o init_SDRAM_reloc.o nand_flash.o serial.o my_printf.o Key_IRQ_LED.o IRQ_handler_c.o excp.o main.o PWM_timer_IRQ.o


nand_flash.bin:$(objs)
	arm-none-eabi-ld -T sdram.lds $^ $(NLIBPATH) $(LIBPATH) -o nand_flash.elf
	arm-none-eabi-objcopy -O binary -S nand_flash.elf $@
	arm-none-eabi-objdump -D -m arm nand_flash.elf > nand_flash.dis

%.o:%.c
	arm-none-eabi-gcc -march=armv4 -O1 -Wall -c -o $@ $<

%.o:%.S
	arm-none-eabi-gcc -march=armv4 -O1 -Wall -c -o $@ $<

clean:
	rm -f *.bin *.elf *.dis *.o
endif


ifeq (1,2)
LIBPATH :=
LIBPATH +=-lgcc -L/usr/lib/gcc/arm-none-eabi/6.3.1/

all:
	arm-none-eabi-gcc -c -o serial.o serial.c
	arm-none-eabi-gcc -c -o my_printf.o my_printf.c
	arm-none-eabi-gcc -c -o main.o main.c
	arm-none-eabi-gcc -c -o start.o start.S
	arm-none-eabi-ld -Ttext 0 -Tdata 0xe80 start.o serial.o my_printf.o main.o $(LIBPATH) -o uart.elf
	arm-none-eabi-objcopy -O binary -S uart.elf uart.bin
	arm-none-eabi-objdump -D uart.elf > uart.dis

clean:
	rm *.bin *.o *.elf *.dis
endif
