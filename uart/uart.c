#include "uart.h"
#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"

void uart_init()
{
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(0x7 << 12);                   // clean gpio14 alternate function
    selector |= 0x2 << 12;                      // set alternate function 5(TXD1) for gpio14
    selector &= ~(0x7 << 15);                   // clean gpio15 alternate function
    selector |= 0x2 << 15;                      // set alternate function 5(RXD1) for gpio15
    put32(GPFSEL1, selector);

    put32(GPPUD, 0x0);                         // disable pull-up/down
    delay(150);                                 // wait 150 cycles  – this provides the required set-up time for the control signal
    put32(GPPUDCLK0, (0x1 << 14) | (0x1 << 15)); // write to GPPUDCLK0 to clock the control signal into the GPIO pads
    delay(150);                                 // wait 150 cycles
    put32(GPPUDCLK0, 0x0);                      // remove the clock

    put32(AUX_ENABLES, 1);                      // enable mini uart (this also enables access to its registers)
    put32(AUX_MU_CNTL_REG, 0x0);                // disable auto flow control and disable receiver and transmitter (for now)
    put32(AUX_MU_IER_REG, 0x0);                 // disable receive and transmit interrupts
    put32(AUX_MU_LCR_REG, 0x3);                 // enable 8 bit mode
    put32(AUX_MU_MCR_REG, 0);                   // set RTS line to be always high
    // baudrate = system_clock_freq / (8 * ( baudrate_reg + 1 )) , system_clock_freq = 250 MHz
    put32(AUX_MU_BAUD_REG, 270);                // set baud rate to 115200

    put32(AUX_MU_IIR_REG, 0x11 << 1);           // clear FIFO

    put32(AUX_MU_CNTL_REG, 0x3);                // finally, enable transmitter and receiver
}

void uart_putc(char c)
{
    while (!(get32(AUX_MU_LSR_REG) & 0x20));
    put32(AUX_MU_IO_REG, c);
}

char uart_getc()
{
    while (!(get32(AUX_MU_LSR_REG) & 0x01));

    return get32(AUX_MU_IO_REG) & 0xff;
}

void uart_puts(const char* str)
{
    while (*str)
        uart_putc(*str++);
}