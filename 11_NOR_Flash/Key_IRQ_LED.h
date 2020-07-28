#ifndef _INIT_KEY_IRQ_LED_H
#define _INIT_KEY_IRQ_LED_H

void key_GPIO_eint_init(void);
void GPIO_LED_init(void);
extern void key_eint_irq(char irq);
#endif
