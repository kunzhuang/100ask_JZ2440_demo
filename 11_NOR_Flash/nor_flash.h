#ifndef _NOR_FLASH_
#define _NOR_FLASH_

#include <stdbool.h>

void nor_flash_test(void);
void write_one_word_nor_flash(unsigned int base, unsigned int offset, unsigned short value);
unsigned int read_one_word_nor_flash(unsigned int base, unsigned int offset);
void cmd_nor_flash(unsigned int offset, unsigned int cmd);
unsigned int data_nor_flash(unsigned int offset);
void scan_nor_flash(void);
unsigned int* sector_table_func(unsigned int sector_table[]);
void erase_nor_flash(void);
void write_nor_flash(void);
void read_nor_flash(void);
void IS_Q6_TOGGLING(unsigned int addr);
bool data_VF(unsigned int addr, bool ffff, char data_str[100]);
#endif
