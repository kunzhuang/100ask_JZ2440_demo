#include "s3c24xx.h"
#include "IRQ_handler_c.h"
#include "Key_IRQ_LED.h"
#include "PWM_timer_IRQ.h"




//Init interrupt
#if IRQ_func_pointer_en ==1
void IRQ_registration(int irq, irq_func fp)
{
	irq_array[irq]=fp; //Passing fp address to irq_array
	INTMSK &=~(1<<irq);
}
#else
void interruptCtrl_init(void)
{
  //INTMASK, Interrupt mask register, ADD=0x4A000008
  //EINT0    =INTMASK[0], 0 available, 1 masked
  //EINT2    =INTMASK[2], 0 available, 1 masked
  //EINT8_23 =INTMASK[5], 0 available, 1 masked
  INTMSK &=~(1<<5|1<<2|1<<0);

  //Timer 0, interrupt enable
  INTMSK &=~(1<<10);
}
#endif


//ID external interrupt source
//Clear/reset SRCPND and INTPND
void IRQ_handler_c(void)
{
  //INTPND, Interrupt pending(input) register, ADD=0x4A000010
  //Read bits
  //EINT0    =INTPND[0], 0 not requested, 1 requested
  //EINT2    =INTPND[2], 0 not requested, 1 requested
  //EINT8_23 =INTPND[5], 0 not requested, 1 requested

  //INTOFFSET, Interrupt offset(intoffset) register, ADD=0x4A000014
  //It shows which interrupt request of IRQ mode is in the INTPND register
  //Read offset value directly
  //EINT0    =0
  //EINT2    =2
  //EINT8_23 =5

  //ID external interrupt source
  int bit=INTOFFSET;

#if IRQ_func_pointer_en ==1
	 irq_array[bit](bit); //Call irq_handler function address
#else
  if(bit==0||bit==2||bit==5) //Push button
  {
    key_eint_irq(bit);
  }else if (bit==10) //Time 0, PWM timer interrupt requested
  {
      PWM_timer_irq();
  }
#endif

  //Clear
  SRCPND = (1<<bit);
  INTPND  = (1<<bit);
}
