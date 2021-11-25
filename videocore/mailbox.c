#include "peripherals/mailbox.h"
#include "mailbox.h"
#include "utils.h"
#include "printf.h"

// the buffer must be 16-byte aligned as only upper 28bits of the address can be passed via the mailbox
volatile unsigned int __attribute__((aligned(16))) mbox[36];

bool mbox_call(byte channel)
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


unsigned long get_board_serial()
{
    mbox[0] = 8 * 4;                // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message

    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (!mbox_call(MBOX_CH_PROP)) {
        printf("failed to get board number\r\n");

        return 0;
    }

    return ((unsigned long)mbox[6] << 32) + mbox[5];
}

double get_core_temperature()
{
    mbox[0] = 8 * 4;                // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message

    mbox[2] = MBOX_TAG_GETTEMP;     // get core temperature
    mbox[3] = 8;                    // buffer size
    mbox[4] = 4;
    mbox[5] = 0;
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (!mbox_call(MBOX_CH_PROP)) {
        printf("failed to get core temperature\r\n");

        return 0;
    }

    return mbox[6] / 1000.0;
}

/**
 * @brief Set the onboard led (green) status
 *
 * @param status true: on, false: off
 * @return true: on
 * @return false: off
 */
bool set_onboard_led_status(bool status)
{
    mbox[0] = 8 * 4;                // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message

    mbox[2] = MBOX_TAG_SETLED;      // set led status
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 130;                  // green led
    mbox[6] = status;

    mbox[7] = MBOX_TAG_LAST;

    if (!mbox_call(MBOX_CH_PROP)) {
        printf("failed to set led status\r\n");

        return 0;
    }

    return (bool)mbox[6];
}

/**
 * @brief Get the onboard led (green) status
 *
 * @return true: on
 * @return false: off
 */
bool get_onboard_led_status()
{
    mbox[0] = 8 * 4;                // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message

    mbox[2] = MBOX_TAG_GETLED;      // set led status
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 130;                  // green led
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (!mbox_call(MBOX_CH_PROP)) {
        printf("failed to get led status\r\n");

        return 0;
    }

    return (bool)mbox[6];
}
