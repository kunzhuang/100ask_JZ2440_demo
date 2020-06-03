#include "my_printf.h"

unsigned char hex_tab[]={'0','1','2','3','4','5','6','7',\
                    '8','9','a','b','c','d','e','f'};

static int out_c(int c) //Pring char
{
 __out_putchar(c);
 return 0;
}

static int out_s (const char *s) //Pring string
{
 while (*s != '\0')
   __out_putchar(*s++);
 return 0;
}

static int out_num(long n, int base,char lead,int maxwidth)
{
 unsigned long m=0;
 char buf[MAX_NUMBER_BYTES], *s = buf + sizeof(buf);
                                //s points to the last address of buff[]
                                //buf address + sizeof(buf)
                                //            + a number
 int count=0,i=0;

 *--s = '\0'; //Ending of a string

 if (n < 0){  //Temp negative number to positive number
   m = -n;
 }
 else{
   m = n;
 }

 do{
   *--s = hex_tab[m%base]; //Remainder
   count++;
 }while ((m /= base) != 0); //Int division 1234%10 ->hex_tab[4]

 if( maxwidth && count < maxwidth){
   for (i=maxwidth - count; i; i--)
     *--s = lead; //Fill 0 or space before a number
}

 if (n < 0)
   *--s = '-';

 return out_s(s);
}


static int my_va_printf(const char *fmt, va_list ap)
{
  //char lead =' ';
  //int max_width=0;
  for(;*fmt!='\0';fmt++) //fmt++ until end of the line
  {
    char lead =' ';
    int max_width=0; //Declear here to avoid my_printf("num=%5d %05d\n\r", 0x1, 123);

    if (*fmt!='%')
    {
      out_c(*fmt); //Print all chars until there is a %
      continue;
    }

    fmt++; //If there is a %, skip this address
    if(*fmt=='0')
    {
      lead='0';
      fmt++; //If there is a 0 after %, Space or 0 to fill before a number
    }
    while (*fmt>='0'&& *fmt<='9')
    {
      max_width*=10;
      max_width+=(*fmt-'0');
      fmt++; // To get the number after '%0'
             // %021 -> max_width= 0, max_width='2'-'0' ASCII code to get a real number
             //      -> max_width=20, max_width=20+'1'-'0'
    }
    switch(*fmt)
    {
      case 'd': out_num(va_arg(ap,int),            10, lead, max_width); break; //Print int
      case 'o': out_num(va_arg(ap, unsigned int),   8, lead, max_width); break; //Print unsigned oct
      case 'u': out_num(va_arg(ap, unsigned int),  10, lead, max_width); break; //Print unsigned int
      case 'x': out_num(va_arg(ap, unsigned int),  16, lead, max_width); break; //Print unsigned hex
      case 'c': out_c(va_arg(ap,int));   break; //Print char
      case 's': out_s(va_arg(ap,char*)); break; //Print string
      default:
      out_c(*fmt);
      break;
    }
  }
  return 0;
}

//reference :  int printf(const char *format, ...);
int my_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	my_va_printf(fmt, ap);
	va_end(ap);
	return 0;
}

/*
int my_printf_test(void)
{
 my_printf("This is www.100ask.org   my_printf test\n\r") ;
 my_printf("test char           =%c,%c\n\r", 'A','a') ;
 my_printf("test decimal number =%d\n\r",    123456) ;
 my_printf("test decimal number =%d\n\r",    -123456) ;
 my_printf("test hex     number =0x%x\n\r",  0x55aa55aa) ;
 my_printf("test string         =%s\n\r",    "www.100ask.org") ;
 my_printf("num=%08d\n\r",   12345);
 my_printf("num=%8d\n\r",    12345);
 my_printf("num=0x%08x\n\r", 0x12345);
 my_printf("num=0x%8x\n\r",  0x12345);
 my_printf("num=0x%02x\n\r", 0x1);
 my_printf("num=0x%2x\n\r",  0x1);

 my_printf("num=%05d\n\r", 0x1);
 my_printf("num=%5d\n\r",  0x1);
 my_printf("num=%5d %05d\n\r", 0x1, 123);
 return 0;
}
*/

void my_printf_test1(void)
{
  my_printf("TEST1\n\r");
}

void my_printf_test2(void)
{
  my_printf("TEST2\n\r");
}
