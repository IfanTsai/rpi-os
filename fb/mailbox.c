#include "peripherals/mailbox.h"
#include "mailbox.h"
#include "utils.h"

bool mbox_call(byte channel, volatile unsigned int *mbox)
{
    while (get32(MBOX_STATUS) & MBOX_FULL);  // wait util can write

    // 28 bit address (MSB) + 4 bit channel (LSB)
    unsigned int val = (((unsigned long)mbox) & ~0xf) | (channel & 0xf);
    put32(MBOX_WRITE, val);

    for (;;) {
        while (get32(MBOX_STATUS) & MBOX_EMPTY);  // wait to reply

        // is it a response to out message?
        if (val == get32(MBOX_READ))
            return mbox[1] == MBOX_RESPONSE;   // is it a valid sucessful resonse?
    }

    return false;
}