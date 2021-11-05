#include "uart.h"

void start_kernel()
{
    uart_init();
    uart_puts("Hello, world!\r\n");

    for (;;)
        uart_putc(uart_getc());
}