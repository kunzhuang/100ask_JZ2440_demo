ifeq (1,1)
LIBPATH :=
LIBPATH +=-lgcc -L/usr/lib/gcc/arm-none-eabi/6.3.1/
objs := start.o main.o serial.o my_printf.o init_SDRAM_reloc.o excp.o Key_IRQ_LED.o

Key_IRQ.bin:$(objs)
	arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o Key_IRQ.elf
	arm-none-eabi-objcopy -O binary -S Key_IRQ.elf $@
	arm-none-eabi-objdump -D -m arm Key_IRQ.elf > Key_IRQ.dis

%.o:%.c
	arm-none-eabi-gcc -O -Wall -c -o $@ $<

%.o:%.S
	arm-none-eabi-gcc -O -Wall -c -o $@ $<

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
