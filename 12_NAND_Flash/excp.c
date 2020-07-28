#include "excp.h"

void printException(unsigned int cpsr, char* str )
{
  my_printf("CPSR=%x\n\r", cpsr);
  my_printf("%s\n\r", str);
}


void printSWIval(unsigned int *pSWI)
{
  unsigned int val=*pSWI & ~0xff000000; //Mask []
  my_printf("SWI=0x%x\n\r", val);

}
