#include "s3c24xx.h"
#include "Key_IRQ_LED.h"
#include "IRQ_handler_c.h"


//Push buttons as external interrupt sources
void key_GPIO_eint_init(void)
{
  //GPFCON, port F control registers, ADD=0x56000050
  //GPF0=GPFCON[1:0]=10=EINT[0]
  //GPF2=GPFCON[5:4]=10=EINT[2]
  GPFCON &=~(1<<1|1<<0|1<<5|1<<4); //Clean bits
  GPFCON |= (1<<1|0<<0|1<<5|0<<4); //Push button S2 S3

  //GPG11=GPGCON[7:6]=10=EINT[11]
  //GPG19=GPGCON[23:22]=10=EINT[19]
  GPGCON &=~(1<<7|1<<6|1<<23|1<<22); //Clean bits
  GPGCON |= (1<<7|0<<6|1<<23|0<<22); //Push button S4 S5

  //Enternal edge triggering
  //EXTINT0, external  interrupt control register 0, ADD=0x56000088
  //EXTINT1, external  interrupt control register 1, ADD=0x5600008c
  //EXTINT2, external  interrupt control register 2, ADD=0x56000090
  //EINT0 =EXTINT0[2:0]  =11x=both edge triggered
  //EINT2 =EXTINT0[10:8] =11x=both edge triggered
  EXTINT0 |= (1<<2|1<<1|1<<10|1<<9); //EINT0(S2), EINT1(S3)

  //EINT11=EXTINT1[14:12]=11x=both edge triggered
  EXTINT1 |= (1<<14|1<<13); //EINT11(S4),

  //EINT19=EXTINT2[14:12]=11x=both edge triggered
  EXTINT2 |= (1<<14|1<<13); //EINT19(S5),

  //EINTMASK, External interrupt mask register, ADD=0x560000a4
  //EINT11=EINTMASK[11]= 0 =enable interrupt
  //EINT19=EINTMASK[19]= 0 =enable interrupt
  //EINTMASK[3:0]=reserved
  EINTMASK &=~(1<<11|1<<19); //Clean bits, enable interrupt

#if IRQ_func_pointer_en ==1
  IRQ_registration(0, key_eint_irq);
  IRQ_registration(2, key_eint_irq);
  IRQ_registration(5, key_eint_irq);
#endif
}

void GPIO_LED_init(void)
{
  /*
  D10 -> nLED_1 -> GPF4
  D11 -> nLED_2 -> GPF5
  D12 -> nLED_4 -> GPF6
  */
  GPFCON &=~(1<<8|1<<9|1<<10|1<<11|1<<12|1<<13); //Clear
  GPFCON |= (1<<8|1<<10|1<<12); //GPIO port F 4 5 6 as outputs
  GPFDAT |= (1<<4|1<<5|1<<6); //GPIO port F 4 5 6 high, all leds off
}




void key_eint_irq(char irq)
{

/*
  D10 -> nLED_1 -> GPF4
  D11 -> nLED_2 -> GPF5
  D12 -> nLED_4 -> GPF6

  LED, GPFCON, control registers
  GPFCON[9:8]=GPFCON[11:10]=GPFCON[12:11]=01=output
  GPFDAT[7:0] = 0 led on, =1 led off

  External interrupt input
  EINT0/GPF0/S2
  EINT2/GPF2/S3
  EINT11/GPG3/S4
  EINT19/GPG11/S5
  GPFCON, control registers
  GPFCON[1:0]=GPFCON[5:4]=10=EINT
  GPGCON[7:6]=GPGCON[23:22]=10=EINT
*/
  unsigned int temp_GPIO_EINTPEND=EINTPEND;
  if(irq==0) //EINT0: S2 -> D12 LED -> GPF6
  {

    if(GPFDAT&(1<<0)) //GPF0 as an input at GPFDAT[0]
    {
      GPFDAT|= (1<<6); //Release, set 1, D12 off
    }else{
      GPFDAT&=~(1<<6); //Pressm, set 0,D12 on
    }

  }else if (irq==2)// EINT2: S3 -> D11 LED -> GPF5
  {

    if(GPFDAT&(1<<2)) //GPF2 as an input at GPFDAT[0]
    {
      GPFDAT|= (1<<5); //Release, set 1,D11 off
    }else{
      GPFDAT&=~(1<<5); //Press, set 0,D11 on
    }
  }
  else if (irq==5)// EINT11: S4 -> D10 LED -> GPF4, EINT19: S5 -> All LEDs
  {
    if(EINTPEND&(1<<11)) //EINT11,external interrupt pending register, GPIO
    {

      if(GPGDAT&(1<<3)) //GPG3 as an input at GPGDAT[3]
      {
        GPFDAT|= (1<<4); //Release,set 1, D10 off
      }else{
        GPFDAT&=~(1<<4); //Press,set 0, D10 on
      }
    }else if(EINTPEND&(1<<19)) //EINT19,external interrupt pending register, GPIO
      {
      if(GPGDAT&(1<<11)) //GPG11 as an input at GPGDAT[11]
      {
        GPFDAT|= ((1<<4)|(1<<5)|(1<<6)); //Release, set 1, all LEDS off
      }else{
        GPFDAT&=~((1<<4)|(1<<5)|(1<<6)); //Press, set 0, all LEDS on
      }
      }
    }

  EINTPEND=temp_GPIO_EINTPEND; //Clear EINTPEND
}
