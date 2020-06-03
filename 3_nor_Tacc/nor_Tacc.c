#include "s3c24xx.h"

void nor_Tacc_init(int val)
{
  BANKCON0 &=~(0b111<<8);
  BANKCON0 |=(val<<8);
  //BANKCON0  =(val<<8);
}
