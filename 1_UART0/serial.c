#include "s3c24xx.h"
#include "serial.h"


 void uart0_init(void)
 {

 	/*
 		GPHCON ADD=0x56000070
 		GPH2 [5:4]= 10 TXD[0]
 		GPH3 [7:6]= 10 RXD[0]

 		GPHUP  ADD=0x56000078
 		GPH[3:2] = 0 enable pin 2 3 pull up

 		GPHDAT ADD=0x56000074

 	*/
 	GPHCON &= ~((3<<4)|(3<<6)); //Clear
 	GPHCON |=  ((2<<4)|(2<<6)); //Enable TX/RX
 	GPHUP  |= ~((1<<2)|(1<<3)); //Enable internal pull-up
 	/*
 		Baud rate
 		UBRDIVn = (int)(UART clock/(buad rate*16))-1
    PCLK=50MHz
    UBRDIV0 = (int)(50000000/(115200*16))-1 = 26
    UBRDIV0 ADD= 0x50000028
    UART control register UCON0 to UCON2/ UART0 to UART2
    ADD=0x50000004, 0x50004004,0x50008004
    Clock select  [11:10] = 00 or 10 =PCLK
    Transmit mode [3:2]   = 01 Interrupt request or polling mode
    Recrive mode  [1:0]   = 01 Interrupt request or polling mode
 	*/
  UBRDIV0  = 26;
  //UCON0  |= ~((1<<10)|(1<<11)); //Clock select=PCLK
  //UCON0  |=   (1<< 2); //Tx polling mode
  //UCON0  |=   (1<< 0); //Rx polling mode
  UCON0 = 0x00000005;
  /*
    UART Line control registers
    ULCON0 to ULCON2 ADD= 0x50000000m 0x50004000, 0x50008000
    Word length [1:0] = 11 8bit
  */
  //ULCON0 = ~(3<<0);
 	//ULCON0 =  (3<<0);
  ULCON0 = 0x00000003;
 	/*
    FIFO
  */


 }

 void put_char(int c)
 {
   /*
     TX/RX status register
     UTRSTAT0 ADD=0x50000010
     Transmitter empty [2] =1 empty

     #define UTXH0               (*(volatile unsigned char *)0x50000020)
     #define URXH0               (*(volatile unsigned char *)0x50000024)
     char not long int
   */

 	/*
    UART transmit buffer registers
    UTXH0 L=0x50000020 H=0x50000023
  */
  while(!(UTRSTAT0 & (1<<2))); //Wait until transmitter is empty
  UTXH0 = (unsigned char) c;

 	/**/

 }

 char get_char(void)
 {
 	/*
    UART receive buffer registers
    URXH0 L=0x50000024 H=0x50000027
  */

 	/*
    TX/RX status register
    UTRSTAT0 ADD=0x50000010
    Receive buffer data ready [0] = 1 has a received data
  */
  while(!(UTRSTAT0 & (1<<0))); //Wait until receiver buffer is ready
  return URXH0;

 	/**/
 }

	/* Output strings */
 void put_s(const char *s)
 {
   while(*s)
   {
     put_char (*s);
     s++;
   }
 }
