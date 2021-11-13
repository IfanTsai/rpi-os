#include "common.h"
#include "uart.h"
#include "printf.h"
#include "utils.h"
#include "irq.h"
#include "timer.h"
#include "sched.h"
#include "fork.h"
#include "unistd.h"
#include "errno.h"

void user_process(char *array)
{
    for (;;) {
        for (int i = 0; i < 5; i++) {
            uart_putc(array[i]);
            delay(100000);
        }
    }
}

void kernel_user_process()
{
    write("user process started\r\n");

    unsigned long stack = malloc();
    if (stack < 0) {
        printf("error while allocating stack for process 1, errno = %d\r\n", errno);

        return;
    }

    int err = clone((unsigned long)&user_process, (unsigned long)"12345", stack);
    if (err < 0) {
        printf("error while clonning process 1, errno = %d\r\n", errno);

        return;
    }

    stack = malloc();
    if (stack < 0) {
        printf("error while allocating stack for process 2, errno = %d\r\n", errno);

        return;
    }

    err = clone((unsigned long)&user_process, (unsigned long)"abcde", stack);
    if (err < 0) {
        printf("error while clonning process 2, errno = %d\r\n", errno);

        return;
    }

    exit();
}

void kernel_process()
{
    printf("kernel process started, el = %d\r\n", get_el());

    int err = move_to_user_mode((unsigned long)&kernel_user_process);   // move init kernel process to user process
    if (err < 0)
        printf("error while moving process to user mode\r\n");
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

    int pid = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0, 0); // start init kernel process
    if (pid < 0) {
        println("error while starting kernel process");

        return;
    }

    for (;;)
        schedule();
}