#include "s3c24xx.h"
#include "PWM_timer_IRQ.h"
#include "IRQ_handler_c.h"


void PWM_timer_init(void)
{
  /*
    Timer clk = PCLK/{prescaler value+1}/{divider value}
              =50000000/(99+1)/16
              =31250 Hz
    TCFGx Timer Configuration Registerx

  */
  TCFG0 =99; //Timer 0
  TCFG1 &=~0xf;
  TCFG1 |=3; //MUX 0 ->1/16 divider

  //Count Buffer Register & Compare Buffer Register (TCNTBx TCMPBx)
  TCNTB0 =31250; //Interval 1s
  //TCMPB0[15:0] not used in this example, we are only interested in interrupt request

  //Timer controller register
  TCON |= (1<<1); //Manual update from TCNTB0 and TCMPB0

  TCON &=~(1<<1|1<<3|1<<0);// Clear
  TCON |= (1<<3); //Auto reload enable
  TCON |= (1<<0); //Timer start

  #if IRQ_func_pointer_en ==1
  IRQ_registration(10, PWM_timer_irq);
  #endif
  }

void PWM_timer_irq(char irq)
{
  static int cnt=0;
  int temp;
  //Light generator
  cnt++;
  temp=~cnt;
  temp &=7; //mask
  GPFDAT &=~(7<<4); //111<<4, all led on
  GPFDAT |= (temp<<4);

  /*
    D10 -> nLED_1 -> GPF4
    D11 -> nLED_2 -> GPF5
    D12 -> nLED_4 -> GPF6
    GPFDAT[7:0] = 0 led on, =1 led off
    1   2   3   4   5   6   7   8   9
    001 010 011 100 101 110 111 000 001
    110 101 100 011 010 001 000 111 110
    */
}
