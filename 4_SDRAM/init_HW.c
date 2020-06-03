#include "s3c24xx.h"
#include "init_HW.h"

void nor_Tacc_init(int val)
{
  BANKCON0 &=~(0b111<<8);
  BANKCON0 |=(val<<8);
  //BANKCON0  =(val<<8);
}

void SDRAM_init(void)
{
  //Bus width and wait control register for Bank 6
  BWSCON &= ~(1<<27|1<<26|3<<24); //Clear
  BWSCON |=  (0<<27); //Determines SRAM for using UB/LB for bank 6 Not using UB/LB
  BWSCON |=  (0<<26); //Determines WAIT status for bank 6 WAIT disable
  BWSCON |=  (2<<24); //Determines data bus width for bank 6, 32bit

  //Bus width and wait control register for Bank 7
  BWSCON &= ~(1<<31|1<<30|3<<28); //Clear
  BWSCON |=  (0<<31); //Determines SRAM for using UB/LB for bank 7 Not using UB/LB
  BWSCON |=  (0<<30); //Determines WAIT status for bank 7 WAIT disable
  BWSCON |=  (2<<28); //Determines data bus width for bank 7, 32bit


  //Bank control register
  BANKCON6 &=~(3<<15|3<<2|3<<0); //Clear
  BANKCON6 |= (3<<15); //Determine the memory type= SDRAM
  BANKCON6 |= (0<<3|0<<2); //RAS Row to CAS Col delay, 2 clocks
  BANKCON6 |= (1<<0); //Column address number 9bit =01
  BANKCON7=BANKCON6;


  //Refresh controller register
  REFRESH &=~(1<<23|1<<22|3<<20|3<<18|0b11111111111<<0); //Clear
  REFRESH |= (1<<23); //SDRAM refresh enable
  REFRESH |= (0<<22); //SDRAM refresh Mode auto
  REFRESH |= (0<<20); //SDRAM RAS precharge time, 2clocks=00 20ns
  REFRESH |= (1<<18); //SDRAM semi row cycle time	SDRAM row cycle time
                      //Trc=Tsrc+Trp=70ns
	                    //Tsrc=50ns, 5clocks=01

  REFRESH |= (1269<<0); //Refresh counter
                      //RefreshPeroid=(2^11-refresh_count+1)/HCLK=64ms
                    	//Refresh_count=2^11+1-100*64ms/8192=1269


  //Banksize register
  BANKSIZE &=~(1<<7|1<<5|1<<4|7<<0); //Clear
  BANKSIZE |= (1<<7); //ARM core burst operation enable
  BANKSIZE |= (1<<5); //SDRAM power down enable control by SCKE
  BANKSIZE |= (1<<4); // SCLK is active only during the access
  BANKSIZE |= (1<<0); //BANK6/7 memory map 64MB


  //SDRAM mode register set register
  MRSRB6 &=~((1<<9)|(7<<4)); //Clear
  //MRSRB6 &=~(7<<4); //Clear
  MRSRB6 |= (0<<9); //Write burst length
  //MRSRB6 |= (00<<7); //Test mode, fixed
  MRSRB6 |= (2<<4); //CAS latency = 2clocks, Memory Mode Register
  //MRSRB6 |= (1<<3); //Burst type Sequential =0, fixed
  //MRSRB6 |= (000<<0); //Burst length=000, fixed
  MRSRB7=MRSRB6; //Lazy


}

int SDRAM_test(void)
{
  volatile unsigned char *p=(volatile unsigned char*) 0x30000000; //SDRAM base address
  int i;
  for (i=0;i<1000;i++) //Write SDRAM
  {
    p[i]=0x55;
  }

  for (i=0;i<1000;i++) //Read SDRAM
  {
    if(p[i]!=0x55) return 1;
  }
  return 0;
}
