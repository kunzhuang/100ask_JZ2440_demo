#include "my_printf.h"
#include "s3c24xx.h"
#include "nand_flash.h"
#include <stdlib.h>
#include <string.h>

/*
  Init NAND Flash
  Timing and configs
*/
void nand_flash_init(void)
{
  /*
    HCLK=100MHz=10ns
    Nand flash configuration register
    NFCONF ADDR 0x4E000000

    TACLS = CLE/ALE duration setting value, duration = HCLK*TACLS
    Delay between CLE/ALE and WE, Twp=Tcls=Tals=12ns, duration is set to 0
    NFCONF[13:12] =0

    TWRPH0 = TWRPH0 duration setting value, duration = HCLK*(TWRPH0+1)
    WE pulse width =12ms, HCLK(TWRPH0+1)>=12ns, TWRPH0=1
    NFCONF[10: 8] =1

    TWRPH1 = TWRPH1 duration setting value, duration = HCLK*(TWRPH1+1)
    Duration between WE=1 and CLE/ALE=0, Tclh=Talh=5ns, HCLK(TWRPH1+1)>=12ns, TWRPH1=0
    NFCONF[ 6: 4] =0
  */
  NFCONF &=~(1<<13|1<<12);
  NFCONF &=~(1<<10|1<<9|1<<8);
  NFCONF |= (1<<8);
  NFCONF &=~(1<<6|1<<5|1<<4);

  /*
    Control register
    NFCONT ADDR 0x4E000004
    NFCONT[0]=1, NAND flash controller enable
    NFCONT[1]=1, NAND flash memory nFCE signal control(CS) disable
    NFCONT[4]=1, NAND flash ECC decoder/encoder initialization
    NFCONT[12]=0, Disable soft lock
  */
  NFCONT &=~(1<<12);
  NFCONT |= (1<<4|1<<1|1<<0);

}

/*
  Print NAND function menu
*/
void nand_flash_test(void)
{
  char c;
  nand_flash_init();
  while(1)
  {
    //Menu
    my_printf("[s] Scan NAND flash\n\r");
    my_printf("[e] Erase NAND flash (block)\n\r");
    my_printf("[w] Write NAND flash\n\r");
    my_printf("[r] Read NAND flash\n\r");
    my_printf("[q] Exit\n\r");
    my_printf("Selection: ");
    c=get_char();
    my_printf("%c\n\r", c);

    switch(c)
    {
      case's':
      case'S':
        nand_flash_chipID();
        break;
      case'e':
      case'E':
        erase_nand_flash_block_menu();
        break;
      case'w':
      case'W':
        write_nand_flash_menu();
        break;
      case'r':
      case'R':
        read_nand_flash_menu();
        break;
      case'q':
      case'Q':
        return;
        break;
    }
  }
}

/*
  Enable chip select(nFCE)
*/
void nand_cs_en(void)
{
  NFCONT &=~(1<<1); //Enable NAND CS
}

/*
  Disable chip select(nFCE)
*/
void nand_cs_dis(void)
{
  NFCONT |= (1<<1); //Disable NAND CS
}

/*
  Send a CMD/one byte
*/
void nand_CMD(unsigned char cmd)
{
  NFCMD=cmd;
  for(volatile int i=1;i<10;i++);
}

/*
  Send an address/one byte
*/
void nand_addr_byte(unsigned char addr)
{
  NFADDR=addr;
  for(volatile int i=1;i<10;i++);
}

/*
  Read one byte from NFDATA
*/
unsigned char nand_r_data_byte(void)
{
  return NFDATA;
}

/*
  Write one byte to NFDATA
*/
void nand_w_data_byte(unsigned char data)
{
  NFDATA=data;
}

/*
  NGCON status register
  NFSTAT[0]
  The status of RnB input pin
  0: NAND Flash memory busy
  1: NAND Flash memory ready to operate
*/
void wait_ready(void)
{
  while(!(NFSTAT&0x1)); //Device busy
}

/*
 Print Chip ID or other information
*/
void nand_flash_chipID(void)
{
  unsigned char buff[5]={0};
  nand_cs_en();
  nand_CMD(0x90); //Read ID
  nand_addr_byte(0x00); //Addr 0x00

  for(unsigned char i=0;i<5;i++)
  buff[i]=nand_r_data_byte();

  nand_cs_dis();

  my_printf("Maker code = 0x%x \n\r", buff[0]);
  my_printf("Device ID = 0x%x \n\r", buff[1]);
  my_printf("Internal chip number = %d \n\r", 1<<(buff[2]&(0x3)));//1*2^0
  my_printf("Cell type = %d Level cell\n\r", 2<<((buff[2]&(0xC))>>2)); //2*2^0
  my_printf("Number of Simultaneously Programmed Pages = %d\n\r", 1<<((buff[2]&(0x30))>>4)); //1*2^1
  my_printf("Pages size= %d KB\n\r", 1<<(buff[3]&(0x3))); //1*2^1
  my_printf("Block size = %d KB\n\r", 64<<((buff[3]&(0x30))>>4)); //64*2^2
  my_printf("Redundant Area Size = %d byte\n\r", 512*(8<<((buff[3]&(0x5))>>2))); //8*2^2
  my_printf("Organization = x%d\n\r", 8<<((buff[3]&(0x40))>>6));
  my_printf("Serial Access Minimum = %x\n\r", (buff[3]&(0x80))>>7);
}

/*
  Nand Page array
  Page n,   0...2047, 2048...2111
  Page n-1, 0...2047, 2048...2111
  ...       0...2047, 2048...2111
  Page 1,   0...2047, 2048...2111
  Page 0,   0...2047, 2048...2111

  Read NAND to buffer using a specified length
*/
void read_nand_flash(volatile unsigned int addr, volatile unsigned char* buff, unsigned int length)
{
  //Only reads one byte/CMD, using char pointer here
  //Length = address range
  unsigned int i=0;
  unsigned int row= addr/2048; //Row/Page addr
  unsigned int col= addr%2048; //Col =[0:2047]

  nand_cs_en();

  while(i<length)
  {
    nand_CMD(0x00); //Send read cmd

    nand_addr_byte(col&0xff); //Lower 8 bits, column address
    nand_addr_byte((col>>8)&0xff); //Higher 8 bits, column address

    nand_addr_byte(row&0xff); //Lower 8 bits, row/page address
    nand_addr_byte((row>>8)&0xff); //Middle 8 bits, row/page address
    nand_addr_byte((row>>16)&0xff); //Higher 8 bits, row/page address

    nand_CMD(0x30); //Send read cmd

    wait_ready(); //If device is busy, wait

    for(; (col<2048)&&(i<length);col++)
    {
      buff[i++]=nand_r_data_byte(); //Read one byte/char
    }
    col=0; //Reset column
    row++; //Next row or page
  }
  nand_cs_dis();
}

/*
  Print NAND flash data in an array 16X5
  And display ASCII values
*/
void read_nand_flash_menu(void)
{
  volatile unsigned int addr=0;
  char addr_str[10]; //UART0 input string buffer
  unsigned char nand_data[16]={0}; //Only reads one byes/CMD, using char

  my_printf("Set an address to read from:");
  get_s(addr_str); //Get string input from UART0
  addr=strtol(addr_str,NULL, 16); //Using newlib/libc.a to convert ASCII to hex
  //my_printf("0x%x \n\r", addr);

  for(int i=0;i<5;i++) //8bit x 4 x 4 = 32bits x 4 per line, 5 columns

  //49 6e 74 65 72 72 75 70 74 20 45 78 63 65 70 74  | Interrupt Except

  {
    read_nand_flash(addr,nand_data,16);
    //Print raw data values, each line has 16 values
    for(int j=0;j<16;j++)
    {
      my_printf("%02x ", nand_data[j]); //One byte 00 to FF
    }
    my_printf(" | ");
    //Print ASCII, each line has 16 characters
    for(int j=0;j<16;j++)
    {
      if(nand_data[j]<0x20||nand_data[j]>0x7e) //Not a character
      {
        put_char('.');
      }else
      {
        put_char(nand_data[j]); //Print ASCII
      }
    }
    my_printf("\r\n");
    addr+=16; //Next 16 addresses
  }

}

/*
  Erase 128KB+4KB= 64 pages (rows)= (2K+64)KB*64 of data
  2048 blocks = 2112 Mbits
  CMD(60) -> addr_3 -> CMD(D0)
  Address length/range or block number, which one is better?
*/
int erase_nand_flash_block(unsigned int addr, unsigned int length)
{
  unsigned int row;

  if(addr%(2048*64)||(length%2048*64)) //If not block address or 2KB*64 pages length
  {
    my_printf("Please enter an aligned block address, or a correct block length. \r\n");
    addr=2048*64*(addr/(2048*64)); //Normalized address
    my_printf("But anything within this block will be erased.[0x%08x, 0x%08x] \r\n",addr,2048*64+addr-1);
  }

  nand_cs_en();

  while(length!=0)
  {
    row=addr/2048; //Or page

    nand_CMD(0x60);                 //Block erase CMD 1st cycle

    nand_addr_byte(row&0xff);       //Lower 8 bits, row/page address
    nand_addr_byte((row>>8)&0xff);  //Middle 8 bits, row/page address
    nand_addr_byte((row>>16)&0xff); //Higher 8 bits, row/page address

    nand_CMD(0xD0);                 //Block erase CMD 2nd cycle
    wait_ready();
    length-=2048*64;
    addr+= 2048*64; //Next block address
  }

  nand_cs_dis();
  return 0;
}

/*
  Print a menu to get address of nand block to be erased
*/
void erase_nand_flash_block_menu(void)
{

  unsigned int addr; //NAND flash address
  char addr_str[10]; //UART0 input string buffer

  my_printf("Select one block to be erased (beginning address of a block):");
  get_s(addr_str); //Get string input from UART0
  addr=strtol(addr_str,NULL, 16); //Using newlib/libc.a to convert ASCII to hex
  if(!erase_nand_flash_block(addr,2048*64)) //Default erase a block/ 64 pages/rows
  {
    my_printf("Done. \r\n");
  }

}

/*
  Write buff to addr with length
  CMD(80) -> addr_5(2 col + 3 row) -> Data (up to 2112 bytes, a page/row) -> CMD(10)
*/
void write_nand_flash(unsigned int addr, char *buff, unsigned int length) //Page
{

  unsigned int row=addr/2048;
  unsigned int col=addr%2048;
  unsigned int i=0;
  nand_cs_en();

  while(i!=length) //End of data, to break while loop
  {
    nand_CMD(0x80); //Send write CMD

    nand_addr_byte(col&0xff);       //Lower 8 bits, column address
    nand_addr_byte((col>>8)&0xff);  //Higher 8 bits, column address

    nand_addr_byte(row&0xff);       //Lower 8 bits, row/page address
    nand_addr_byte((row>>8)&0xff);  //Middle 8 bits, row/page address
    nand_addr_byte((row>>16)&0xff); //Higher 8 bits, row/page address

    for(; (col<2048)&&(i<length);col++) //Address may not be page aligned, col may not =0
    {
      nand_w_data_byte(buff[i++]); //Write one byte/char
    }

    nand_CMD(0x10);
    wait_ready();

    col=0; //Reset column
    row++; //Next row or page
  }
  nand_cs_dis();
}

/*
  Print a menu to get address and data to be written into nand
*/
void write_nand_flash_menu(void)
{
  unsigned int addr; //Nor flash address
  char addr_str[10]; //UART0 input string buffer
  char data_str[100];
  unsigned int data_str_length;

  my_printf("Enter an address to be written:");
  get_s(addr_str); //Get string input from UART0
  addr=strtol(addr_str,NULL, 16); //Using newlib/libc.a to convert ASCII to hex
                                  //(ascii)0xffee ->(hex)0xffee

  my_printf("Enter data to be written: ");
  get_s(data_str); //Get string input from UART0
  //my_printf("strlength = %d\r\n",strlen(data_str));

  data_str_length=strlen(data_str);
  write_nand_flash(addr, data_str, data_str_length+1); //+1, 00 ="\0"
}
