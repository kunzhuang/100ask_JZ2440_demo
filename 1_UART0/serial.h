#ifndef _UART_0
#define _UART_0

void uart0_init(void);
void put_char(int c);
char get_char(void);
void put_s(const char *s);

#endif
