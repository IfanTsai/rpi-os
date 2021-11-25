#include "peripherals/irq.h"
#include "peripherals/timer.h"
#include "irq.h"
#include "printf.h"
#include "utils.h"
#include "timer.h"

static const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1T",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32",

    "SYNC_ERROR",
    "SYSCALL_ERROR",
    "DATA_ABORT_ERROR",
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address, unsigned int el)
{
    printf("%s, ESR: %x, address: %x, el: %d\r\n", entry_error_messages[type], esr, address, el);
}

void enable_interrupt_controller()
{
    put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
    put32(TIMER_CTRL, get32(TIMER_CTRL) | ( 1 << 29 ));
}

void handle_irq()
{
    unsigned int irq = get32(CORE0_INT_SOURCE);
    irq &= (~GPU_IRQ);    // do not care GPU interrupt
    if (irq) {
        switch (irq) {
        case LOCAL_TIME_IRQ:
            handle_local_timer_irq();

            break;
        default:
            printf("Unknown core 0 irq: 0x%x\r\n", irq);
        }
    }

    irq = get32(IRQ_PENDING_1);
    if (irq) {

    switch (irq) {
        case (SYSTEM_TIMER_IRQ_1):
            handle_sys_timer_irq();

            break;
        default:
            printf("Unknown pending 1 irq: 0x%x\r\n", irq);
        }
    }
}
