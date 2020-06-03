//#include "s3c24xx.h"
//#include "serial.h"
#include "my_printf.h"

int main()
{
    unsigned char c;

    uart0_init();   //115200 8N1

    put_s("Hello world\n\r");

    my_printf_test();

    while(1)
    {
      c=get_char();

      if(c=='\r') // New line
      {
        put_char('\n');
      }
      if(c=='\n')
      {
        put_char('\r');
      }

      put_char(c);
    }

    return 0;
}
