#ifndef __UART_H__
#define __UART_H__

void uart_init();
void uart_putc(char);
char uart_getc();
void uart_puts(const char *);

static inline void putc(void *p, char c)
{
    uart_putc(c);
}

#endif // !__UART_H__