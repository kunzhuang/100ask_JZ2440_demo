#ifndef _INIT_HW_H
#define _INIT_HW_H

//void nor_Tacc_init(int val);
void SDRAM_init(void);
//int SDRAM_test(void);
void copy2sdram(void);
void clean_reset_bss(void);

#endif
