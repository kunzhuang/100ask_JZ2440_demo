#ifndef _MY_PRINTF_
#define _MY_PRINTF_

#include "serial.h"

#define  __out_putchar  put_char //in serial0
#define  MAX_NUMBER_BYTES  64

//==============================================================================
//Reference #include <stdarg.h>
typedef char*  va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
//（表达式1， 表达式2） ->最终返回表达式2值（取值） 表达式1移动指针
//#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_arg(ap,t)    ( *(t *)( ap=ap + _INTSIZEOF(t), ap- _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )

//==============================================================================


//static int out_c(int c); //No need to declear static function in header
//static int out_s (const char *s);
//static int out_num(long n, int base,char lead,int maxwidth);
//static int my_va_printf(const char *fmt, va_list ap);
int my_printf(const char *fmt, ...);
//int my_printf_test(void);
void my_printf_test1(void);
void my_printf_test2(void);
#endif
