#include "s3c24xx.h"
#include "serial.h"
#include "my_printf.h"
#include "init_SDRAM_reloc.h"
#include "Key_IRQ_LED.h"
#include "PWM_timer_IRQ.h"
#include "IRQ_handler_c.h"



char Global_Char_0='B';
char Global_Char_1='c';
char Global_Char_2=0;


void delay(volatile int d)
{
	while (d--);
}

int main(void)
{
    //uart0_init();   //115200 8N1
    //nor_Tacc_init(7);
		GPIO_LED_init();
		#if IRQ_func_pointer_en==0
		interruptCtrl_init();
		#endif
		key_GPIO_eint_init();
		PWM_timer_init();
    while(1)
    {
			my_printf(" Global_Char_2=0x%8x\n\r",  Global_Char_2);
			delay(1000000);
			put_char(Global_Char_0);
      Global_Char_0++;
      delay(1000000);
			put_char(Global_Char_1);
			Global_Char_1++;
			delay(1000000);
			my_printf("  0x%8x\n\r",  TCNTO0);
			delay(1000000);
    }

    return 0;
}
