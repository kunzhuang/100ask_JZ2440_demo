#ifndef _INIT_KEY_IRQ_LED_H
#define _INIT_KEY_IRQ_LED_H

void interruptCtrl_init(void);
void key_GPIO_eint_init(void);
void GPIO_LED_init(void);
void IRQ_handler_c(void);
void key_eint_irq(char irq);
#endif
