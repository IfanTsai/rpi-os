#include "peripherals/gpio.h"
#include "peripherals/pm.h"
#include "mailbox.h"
#include "utils.h"

void power_off()
{
    // power off devices one by one
    for (int i = 0; i < 0xa; i++) {
        mbox[0] = 8 * 4;                // length of the message
        mbox[1] = MBOX_REQUEST;

        mbox[2] = MBOX_TAG_SETPOWER;    // set power state
        mbox[3] = 8;
        mbox[4] = 8;
        mbox[5] = (unsigned int)i;      // device ID
        mbox[6] = 0;                    // 0: off, 1: on

        mbox[7] = MBOX_TAG_LAST;

        mbox_call(MBOX_CH_PROP);
    }

    // power off gpio pins
    put32(GPFSEL0, 0);
    put32(GPFSEL1, 0);
    put32(GPFSEL2, 0);
    put32(GPFSEL3, 0);
    put32(GPFSEL4, 0);
    put32(GPFSEL5, 0);
    put32(GPPUD, 0);

    // flush gpio setup, can refer function uart_init
    delay(150);
    put32(GPPUDCLK0, 0xffffffff);
    put32(GPPUDCLK1, 0xffffffff);
    delay(150);
    put32(GPPUDCLK0, 0);
    put32(GPPUDCLK1, 0);

    // power off the SoC (GPU + CPU)
    unsigned int selector = get32(PM_RSTS);
    selector &= ~0xfffffaaa;
    selector |= 0x555;
    put32(PM_RSTS, PM_WDOG_MAGIC | selector);
    put32(PM_WDOG, PM_WDOG_MAGIC | 10);
    put32(PM_RSTC, PM_WDOG_MAGIC | PM_RSTC_FULLRST);
}

void power_reset()
{
    unsigned int selector = get32(PM_RSTS);
    selector &= ~0xfffffaaa;
    put32(PM_RSTS, PM_WDOG_MAGIC | selector);
    put32(PM_WDOG, PM_WDOG_MAGIC | 10);
    put32(PM_RSTC, PM_WDOG_MAGIC | PM_RSTC_FULLRST);
}