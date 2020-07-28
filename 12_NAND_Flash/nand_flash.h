#ifndef _NAND_FLASH_
#define _NAND_FLASH_

//#include <stdbool.h>
extern void nand_flash_init(void);
extern void read_nand_flash(volatile unsigned int addr, volatile unsigned char* buff, unsigned int length);
void nand_flash_chipID(void);
void nand_flash_test(void);
void nand_cs_en(void);
void nand_cs_dis(void);
void nand_CMD(unsigned char cmd);
void nand_addr_byte(unsigned char addr);
unsigned char nand_r_data_byte(void);
void nand_w_data_byte(unsigned char data);
void wait_ready(void);
void read_nand_flash_menu(void);
int erase_nand_flash_block(unsigned int addr, unsigned int length);
void erase_nand_flash_block_menu(void);
void write_nand_flash(unsigned int addr, char *buff, unsigned int length);
void write_nand_flash_menu(void);

#endif
