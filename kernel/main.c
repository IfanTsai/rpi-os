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
#include "fb.h"
#include "mailbox.h"
#include "power.h"

static void kernel_process()
{
    printf("kernel process started, el = %d\r\n", get_el());

    unsigned long start = (unsigned long)&user_start;
    unsigned long end = (unsigned long)&user_end;
    unsigned long process = (unsigned long)&user_process;

    int err = move_to_user_mode(start, end - start, process - start);   // move init kernel process to user process
    if (err < 0)
        printf("failed to move kernel process to user mode\r\n");
}

static void draw_logo()
{
    draw_rect(880, 520, 1026, 566, 0x04, 0);
    draw_str(900, 540, "Hello RPI OS!", 0x05);
}

static void local_timer_cb()
{
    set_onboard_led_status(!get_onboard_led_status());
    printf("\r\n core temperature: %.3fC\r\n", get_core_temperature());
    add_timer(1000 * 5, local_timer_cb);
}

void kernel_main()
{
    uart_init();
    printf_init(NULL, putc);
    exception_vector_table_init();
    fb_init();
    draw_logo();
    sys_timer_init(20000);
    local_timer_init();
    add_timer(1000 * 5, local_timer_cb);
    enable_interrupt_controller();
    enable_irq();

    printf("\r\n\r\n");
    printf("board serial = 0x%016x\r\n", get_board_serial());
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