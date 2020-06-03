#ifndef _INIT_IRQ_C_H
#define _INIT_IRQ_C_H

#if IRQ_func_pointer_en ==1
typedef void(*irq_func)(char irq);
irq_func irq_array[32]; //32 possible addresses, int
#endif


void IRQ_handler_c(void);

#if IRQ_func_pointer_en ==1
extern void IRQ_registration(int irq, irq_func fp);
#else
void interruptCtrl_init(void);
#endif

#endif
