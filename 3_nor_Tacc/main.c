#include "s3c24xx.h"
//#include "serial.h"
#include "my_printf.h"
#include "nor_Tacc.h"

int main(void)
{
    unsigned char c;

    uart0_init();   //115200 8N1

    //put_s("Hello world\n\r");
    //my_printf_test();
    while(1)
    {
      put_s("Enter value between 0 and 7:");
      c=get_char();
      my_printf("%1d \r\n", c- '0');
      nor_Tacc_init(c - '0'); //ASCII conversion
      my_printf("num=0x%08x\n\r", BANKCON0);
      led_test();
    }

    return 0;
}
