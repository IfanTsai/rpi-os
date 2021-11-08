#include "common.h"
#include "uart.h"
#include "printf.h"
#include "utils.h"
#include "irq.h"
#include "timer.h"
#include "sched.h"
#include "fork.h"

void process(char *array)
{
    for (;;) {
        for (int i = 0; i < 5; i++){
            uart_putc(array[i]);
            delay(100000);
        }
    }
}

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

    int res = copy_process((unsigned long)&process, (unsigned long)"12345", 3);   // set prioriy is 3
    if (res) {
        printf("error while starting process 1");
        return;
    }

    res = copy_process((unsigned long)&process, (unsigned long)"abcde", 1);    // set prioriy is 1
    if (res) {
        printf("error while starting process 2");
        return;
    }

    for (;;)
        //uart_putc(uart_getc());    // for input echo
        schedule();
}