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
#include "user.h"

void kernel_process()
{
    printf("kernel process started, el = %d\r\n", get_el());

    unsigned long start = (unsigned long)&user_start;
    unsigned long end = (unsigned long)&user_end;
    unsigned long process = (unsigned long)&user_process;

    int err = move_to_user_mode(start, end - start, process - start);   // move init kernel process to user process
    if (err < 0)
        printf("failed to move kernel process to user mode\r\n");
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

    int pid = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0); // start init kernel process
    if (pid < 0) {
        println("error while starting kernel process");

        return;
    }

    for (;;)
        schedule();
}