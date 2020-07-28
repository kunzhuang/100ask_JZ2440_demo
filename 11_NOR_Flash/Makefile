ifeq (1,1)
LIBPATH :=
NLIBPATH :=
LIBPATH +=-lgcc -L/usr/lib/gcc/arm-none-eabi/6.3.1/ #libgcc library: it provides some basic hardware functionalities, such as the division.
NLIBPATH += /usr/lib/arm-none-eabi/newlib/libc.a #https://www.sourceware.org/newlib/libc.html
																								 #https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html
objs := start.o main.o serial.o my_printf.o init_SDRAM_reloc.o excp.o Key_IRQ_LED.o PWM_timer_IRQ.o IRQ_handler_c.o nor_flash.o

nor_flash.bin:$(objs)
	arm-none-eabi-ld -T sdram.lds $^ $(NLIBPATH) $(LIBPATH) -o nor_flash.elf
	arm-none-eabi-objcopy -O binary -S nor_flash.elf $@
	arm-none-eabi-objdump -D -m arm nor_flash.elf > nor_flash.dis

%.o:%.c
	arm-none-eabi-gcc -march=armv4 -O -Wall -c -o $@ $<

%.o:%.S
	arm-none-eabi-gcc -march=armv4 -O -Wall -c -o $@ $<

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
