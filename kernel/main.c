#include "common.h"
#include "uart.h"
#include "printf.h"
#include "utils.h"
#include "irq.h"
#include "timer.h"

void start_kernel()
{
    uart_init();
    printf_init(NULL, putc);
    exception_vector_table_init();
    timer_init(200000);
    enable_interrupt_controller();
    enable_irq();

    printf("\r\n\r\n");
    printf("processor id = %d\r\n", get_processor_id());
    printf("el = %d\r\n", get_el());

    for (;;)
        uart_putc(uart_getc());    // for input echo
}