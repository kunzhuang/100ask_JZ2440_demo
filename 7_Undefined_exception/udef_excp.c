#include "udef_excp.h"

void printException(unsigned int cpsr, char* str )
{
  my_printf("CPSR=%x\n\r", cpsr);
  my_printf("%s\n\r", str);
}
