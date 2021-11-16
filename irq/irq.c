#include "peripherals/irq.h"
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
}

void handle_irq()
{
    unsigned int irq = get32(IRQ_PENDING_1);

    switch (irq) {
    case (SYSTEM_TIMER_IRQ_1):
        handle_timer_irq();

        break;
    default:
        printf("Unknown pending irq: %x\r\n", irq);
    }
}
