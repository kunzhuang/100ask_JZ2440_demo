#ifndef _EXCEP_
#define _EXCEP_

#include "my_printf.h"
//#include "serial.h"

void printException(unsigned int cpsr, char* str );
void printSWIval(unsigned int *pSWI);
//static int out_c(int c); //No need to declear static function in header
//static int out_s (const char *s);
//static int out_num(long n, int base,char lead,int maxwidth);
//static int my_va_printf(const char *fmt, va_list ap);
//int my_printf_test(void);
#endif
