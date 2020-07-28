#include "my_printf.h"
#include "nor_flash.h"
#include <stdlib.h>
#include <string.h>


#define NOR_FLASH_BASE 0 //NOR->cs0
#define NOR_FLASH_BASE_OFFSET_BIT 1 //CPU A1 <-> A0 NOR


/*
  Print a menu to achieve
    ID nor nor_flash_test
    erase nor flash sector
    program a address
    read an address
*/
void nor_flash_test(void)
{
  char c;
  while(1)
  {
    //Menu
    my_printf("[s] Scan nor flash\n\r");
    my_printf("[e] Erase nor flash (sector)\n\r");
    my_printf("[w] Write nor flash\n\r");
    my_printf("[r] Read nor flash\n\r");
    my_printf("[q] Exit\n\r");
    my_printf("Selection: ");
    c=get_char();
    my_printf("%c\n\r", c);

    switch(c)
    {
      case's':
      case'S':
        scan_nor_flash();
        break;
      case'e':
      case'E':
        erase_nor_flash();
        break;
      case'w':
      case'W':
        write_nor_flash();
        break;
      case'r':
      case'R':
        read_nor_flash();
        break;
      case'q':
      case'Q':
        return;
        break;
    }
  }
}

void write_one_word_nor_flash(unsigned int base, unsigned int offset, unsigned short value)
{
  volatile unsigned short *p=(volatile unsigned short*)(base+(offset<<NOR_FLASH_BASE_OFFSET_BIT));
  *p=value;
}

unsigned int read_one_word_nor_flash(unsigned int base, unsigned int offset)
{
  volatile unsigned short *p=(volatile unsigned short*)(base+(offset<<NOR_FLASH_BASE_OFFSET_BIT));
  return *p;
}

void cmd_nor_flash(unsigned int offset, unsigned int cmd) //Need to disable PWM timer interrupt
{
  write_one_word_nor_flash(NOR_FLASH_BASE, offset, cmd);
}

unsigned int data_nor_flash(unsigned int offset)
{
  return read_one_word_nor_flash(NOR_FLASH_BASE, offset);
}


/*
    Read and print
    Manufacturer ID
    Device ID
    NOR size
    Sector start addresses(erase)

    Interrutp interference:
    In CFI mode, when an interrutp requested, CPU pc points to 0x18 (IRQ) /0x02 (undefined exception) in NOR flash etc,
    pc will not get any valid data, resulting in program crash.
*/
void scan_nor_flash(void)
{
  unsigned int size;
  int device, vendor;
  //char ebr; //Erase block region
  //unsigned int ebr_BASE=0, ebr_BASE_offset, ebr_SECTOR, ebr_SECTOR_size, ebr_SECTOR_addr;
  unsigned int sector_table[50];
  /*
  A NOR flash may have multiple bank area = 4 ,
  a bank may have multiple sectors.
  CFI publication 100

  Index for Erase Bank Area 1
  [2E, 2D]+1 = # of same-size sectors in bank 1, length 2 bytes
  [30, 2F] = sector size in multiples of 256-bytes-> sector size=[30, 2F]*256, length 2 bytes

  Bank(region) ->sector ->sector size
  */
  cmd_nor_flash(0x555, 0xaa);    //Unlock
	cmd_nor_flash(0x2aa, 0x55);
	cmd_nor_flash(0x555, 0x90);    //Read id
	vendor = data_nor_flash(0);    //Manufacturer ID
	device = data_nor_flash(1);    //Devide ID
	cmd_nor_flash(0, 0xf0);        //Reset

  cmd_nor_flash(0x55, 0x98);   //Enter CFI
  size=(1<<data_nor_flash(0x27))/(1024*1024); //Arithmetic bit shift 2^data_nor_flash(0x27)
  my_printf("\r\n");
  my_printf("Devide ID = %x, Vendor ID = %x\n\r", device, vendor); //Device: 2249=MX29LV160DB
                                                                   //Vendor: C2
  my_printf("NOR Flash size = %dM \n\r", size);
  /*
  char str[4];
  str[0]=data_nor_flash(0x10); //Q
  str[1]=data_nor_flash(0x11); //R
  str[2]=data_nor_flash(0x12); //Y
  str[3]='\0';
  my_printf("str=%s\n\r",str);
  */

/*  ebr = data_nor_flash(0x2c); //Number of erase regions(bank) within device
  ebr_BASE_offset=0x2d;
  ebr_SECTOR_addr=ebr_BASE;

  unsigned int cnt=0;
  my_printf("Sector start addresses:\n\r");

  for(int i=0; i<ebr; i++) //1+2+1+31 =35 sectors
  {
    ebr_SECTOR=data_nor_flash(ebr_BASE_offset)+(data_nor_flash(ebr_BASE_offset+1)<<8)+1;
    //number of erase blocks(sectors) of identical size within region (bank)
    ebr_SECTOR_size=(data_nor_flash(ebr_BASE_offset+2)+(data_nor_flash(ebr_BASE_offset+3)<<8))*256;\
    //the erase blocks(sectors) within this region (bank) = [30, 2F]*256
    ebr_BASE_offset+=4;
    for(int j=0;j<ebr_SECTOR;j++)
    {
      my_printf("0x%08x ", ebr_SECTOR_addr);
      ebr_SECTOR_addr+=ebr_SECTOR_size; //Sector start addresses += sector_size
      cnt++;
      if(cnt%5==0) my_printf("\r\n"); //Add a new line every 5 outputs
    }
  }*/

  cmd_nor_flash(0x00, 0xf0); //Exit CFI

  sector_table_func(sector_table);
  unsigned int cnt=0;
  my_printf("Sector starting addresses: (%d sectors total)\n\r",sector_table[0]);
  for(int i=1; i<=sector_table[0]; i++)
  {
    my_printf("0x%08x ", sector_table[i]);
    cnt++;
    if (cnt % 5 == 0) my_printf("\n\r");
  }
  my_printf("\r\n");
}

/*
  Return a table of starting addresse of sectors.
  sector_table [0] total number of sectors =x-1
  sector_table [1 ...x-1] starting addresse of sectors
  sector_table [x] address of last sector
*/
unsigned int* sector_table_func(unsigned int sector_table[])
{
  //unsigned int sector_table[50];
  unsigned int ebr_BASE=0, ebr_BASE_offset, ebr_SECTOR, ebr_SECTOR_size, ebr_SECTOR_addr;
  char ebr; //Erase block region
  ebr_BASE_offset=0x2d;
  ebr_SECTOR_addr=ebr_BASE;
  unsigned int cnt=0;

  cmd_nor_flash(0x55, 0x98);   //Enter CFI
  ebr = data_nor_flash(0x2c); //Number of erase regions(bank) within device

  for(int i=0; i<ebr; i++) //1+2+1+31 =35 sectors
  {
    ebr_SECTOR=data_nor_flash(ebr_BASE_offset)+(data_nor_flash(ebr_BASE_offset+1)<<8)+1;
    //number of erase blocks(sectors) of identical size within region (bank)
    ebr_SECTOR_size=(data_nor_flash(ebr_BASE_offset+2)+(data_nor_flash(ebr_BASE_offset+3)<<8))*256;
    //the erase blocks(sectors) within this region (bank) = [30, 2F]*256
    ebr_BASE_offset+=4;
    for(int j=0;j<ebr_SECTOR;j++)
    {
      sector_table[cnt+1]=ebr_SECTOR_addr;
      ebr_SECTOR_addr+=ebr_SECTOR_size; //Sector start addresses += sector_size
      cnt++;
    }
  }
  cmd_nor_flash(0x00, 0xf0); //Exit CFI
  sector_table[0]=cnt;
  sector_table[cnt+1]=ebr_SECTOR_addr-1; //Address of end of NOR flash = 1fffff
  return sector_table;
}

/*
  Erase a sector
  If the address is the starting sector address, erase the entire sector
  If the address is between two starting sector addresses, erase the sector in between two sector addresses
*/
void erase_nor_flash(void)
{
  unsigned int addr; //Nor flash address
  char addr_str[10]; //UART0 input string buffer

  scan_nor_flash();
  my_printf("Select one sector to be erased (input addr to the end of sector):");
  get_s(addr_str); //Get string input from UART0
  addr=strtol(addr_str,NULL, 16); //Using newlib/libc.a to convert ASCII to hex


  cmd_nor_flash(0x555, 0xaa); //Unlock
	cmd_nor_flash(0x2aa, 0x55);
	cmd_nor_flash(0x555, 0x80);  //CMD erase sector

  cmd_nor_flash(0x555, 0xaa);
  cmd_nor_flash(0x2aa, 0x55);
  cmd_nor_flash(addr>>1,0x30);  //CMD erase sector, restore CPU A1<->A0 NOR

  IS_Q6_TOGGLING(addr);

  if(data_VF(addr,1,0))
   {
     my_printf("Done.\r\n");
   }else
   {
     my_printf("Error.\r\n");
   }
   cmd_nor_flash(0, 0xf0);        //Reset
}

/*
  Write some data to an address(es)(ending with x0, x2, x4, x6, x8, xA, xC, xE)
  Padding any input data in odd length by byte 1= 0x00
*/
void write_nor_flash(void)
{
  unsigned int addr, addr_dup; //Nor flash address
  char addr_str[10]; //UART0 input string buffer
  char data_str[100];
  unsigned int data_str_length;

  my_printf("Enter an address to be written (any even number address)\r\n");
  my_printf("i.e. x0, x2, x4, x6, x8, xA, xC, xE:");
  get_s(addr_str); //Get string input from UART0
  addr=strtol(addr_str,NULL, 16); //Using newlib/libc.a to convert ASCII to hex
                                  //(ascii)0xffee ->(hex)0xffee
  addr_dup=addr;

  my_printf("Enter data to be written: ");
  get_s(data_str); //Get string input from UART0
  //my_printf("strlength = %d\r\n",strlen(data_str));

  data_str_length=strlen(data_str);
  unsigned int i, val_word;
  if(data_str_length%2) //Deal with odd string length
  {
    data_str[data_str_length+1]='\0';
    data_str[data_str_length]=0; //Padding to second byte
  }
  for(i=0; i<data_str_length; i=i+2)
  {
    val_word = data_str[i] + (data_str[i+1]<<8); //A word, [byte 1, byte 0]

    cmd_nor_flash(0x555, 0xaa); //Unlock
    cmd_nor_flash(0x2aa, 0x55);
    cmd_nor_flash(0x555, 0xa0); //Program
    cmd_nor_flash(addr>>1, val_word);      //restore CPU A1<->A0 NOR
    IS_Q6_TOGGLING(addr);

    addr += 2;
  }

 if(data_VF(addr_dup,0,data_str))
  {
    my_printf("Done.\r\n");
  }else
  {
    my_printf("Error.\r\n");
  }
  cmd_nor_flash(0, 0xf0);        //Reset
}


/*
  Print NOR flash data in an array 16X5
  And display ASCII values
*/
void read_nor_flash(void)
{
  unsigned int addr; //Nor flash address
  volatile unsigned char *addr_p;
  char addr_str[10]; //UART0 input string buffer
  unsigned char data_str[16];

  my_printf("Set an address to read from:");
  get_s(addr_str); //Get string input from UART0
  addr=strtol(addr_str,NULL, 16); //Using newlib/libc.a to convert ASCII to hex
  //my_printf("0x%x \n\r", addr);
  addr_p=(volatile unsigned char *)addr; //Convert addr to a pointer

  for(int i=0;i<5;i++) //8bit x 4 x 4 = 32bits x 4 per line, 5 columns

  //49 6e 74 65 72 72 75 70 74 20 45 78 63 65 70 74  | Interrupt Except

  {
    //Print raw data values, each line has 16 values
    for(int j=0;j<16;j++)
    {
      data_str[j]=*addr_p;
      my_printf("%02x ", data_str[j]);
      *addr_p++;
    }
    my_printf(" | ");
    //Print ASCII, each line has 16 characters
    for(int j=0;j<16;j++)
    {
      if(data_str[j]<0x20||data_str[j]>0x7e) //Not a character
      {
        put_char('.');
      }else
      {
          put_char(data_str[j]); //Print ASCII
      }
    }
    my_printf("\r\n");
  }
}

/*
  This function is to check if a byte writting/erasting is still in progress.
*/
void IS_Q6_TOGGLING(unsigned int addr)
{
  //unsigned int n_1, n;
  //n_1=data_nor_flash(addr>>1);
  //n  =data_nor_flash(addr>>1);
  while(((data_nor_flash(addr>>1))^(data_nor_flash(addr>>1)))&(1<<6)); //Bit toggling true
}

/*
  This function is used to verify if a data/string/sector is successfully written/erased.
*/
bool data_VF(unsigned int addr, bool ffff, char data_str[100])
{

  volatile unsigned char *addr_p;
  bool cond=0; //Init as ERROR


  my_printf("Verifying\r\n");
  if(ffff) //Erase CMD occured
  {
    int sector_idx;
    unsigned int sector_table[50];
    sector_table_func(sector_table);

    for(sector_idx=1;sector_idx<=sector_table[0]+1;sector_idx++)
    {
      if(addr==sector_table[sector_idx]) //Addr == start of a sector
      {
        break;
      }
      if(addr<sector_table[sector_idx]) //Addr bwtween two sectors
      {
        sector_idx--; //Move index back by 1
        break;
      }
      //return cond=0;
    }
    my_printf("Erasing sector range [%x, %x] \r\n",sector_table[sector_idx], sector_table[sector_idx+1]);
    addr_p=(volatile unsigned char *)sector_table[sector_idx]; //Convert addr to a pointer, pointing to start of a sector
    for(unsigned int i=sector_table[sector_idx];i<sector_table[sector_idx+1];i++) //Sweep between two sectors
    {
      if(*addr_p!=0xff)
      {
        cond=0; //Error
        break;
      }else
      {
        *addr_p++;
        cond=1; //Done
      }

    }
  }else
  {
    addr_p=(volatile unsigned char *)addr; //Convert addr to a pointer
    for(int i=0;i<strlen(data_str);i++) //Verifying buffer against NOR flash data
    {
      if(data_str[i]==*addr_p)//Read and compare
      {
        *addr_p++; //Byte by byte
        cond=1; //Done, lazy
      }else
      {
        cond=0; //Error
        break;
      }
    }
  }
    return cond;
}
