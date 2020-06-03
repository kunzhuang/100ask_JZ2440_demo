#include "s3c24xx.h"
//#include "serial.h"
#include "my_printf.h"
#include "init_HW.h"

int main(void)
{
    uart0_init();   //115200 8N1

    nor_Tacc_init(7);
    my_printf("BANKCON0 = 0x%08x\n\r", BANKCON0);

    SDRAM_init();

    my_printf("BWSCON   = 0x%08x\n\r", BWSCON);
    my_printf("BANKCON6 = 0x%08x\n\r", BANKCON6);
    my_printf("BANKCON7 = 0x%08x\n\r", BANKCON7);
    my_printf("REFRESH  = 0x%08x\n\r", REFRESH);
    my_printf("BANKSIZE = 0x%08x\n\r", BANKSIZE);
    my_printf("MRSRB6   = 0x%08x\n\r\n\r", MRSRB6);

    if(SDRAM_test())
    {
      my_printf("SDRAM ERROR!!!\n\r");
    }else
    {
      my_printf("SDRAM GOOD!!!\n\r");
    }

    return 0;
}
