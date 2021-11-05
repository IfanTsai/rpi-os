#include "common.h"
#include "uart.h"
#include "printf.h"
#include "utils.h"

void start_kernel()
{
    uart_init();
    printf_init(NULL, putc);

    printf("\r\n\r\n");
    printf("processor id = %d\r\n", get_processor_id());
    printf("el = %d\r\n", get_el());

    for (;;)
        uart_putc(uart_getc());    // for input echo
}