#include "s3c24xx.h"
#include "serial.h"
//#include "my_printf.h"
#include "init_HW.h"

char Global_Char='B';

void delay(volatile int d)
{
	while (d--);
}

int main(void)
{
    uart0_init();   //115200 8N1
    //nor_Tacc_init(7);

    while(1)
    {
      put_char(Global_Char);
      Global_Char++;
      delay(1000000);
    }

    return 0;
}
