ifeq (1,1)
LIBPATH :=
LIBPATH +=-lgcc -L/usr/lib/gcc/arm-none-eabi/6.3.1/
objs := start.o main.o serial.o my_printf.o init_HW.o udef_excp.o

Udef_exp.bin:$(objs)
	arm-none-eabi-ld -T sdram.lds $^ $(LIBPATH) -o Udef_exp.elf
	arm-none-eabi-objcopy -O binary -S Udef_exp.elf $@
	arm-none-eabi-objdump -D -m arm Udef_exp.elf > Udef_exp.dis

%.o:%.c
	arm-none-eabi-gcc -Wall -c -o $@ $<

%.o:%.S
	arm-none-eabi-gcc -Wall -c -o $@ $<

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
