#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#include "common.h"

#define MBOX_REQUEST  0

#define MBOX_TAG_SETPOWER     0x28001
#define MBOX_TAG_SETCLKRATE   0x38002
#define MBOX_TAG_SETPHYWH     0x48003
#define MBOX_TAG_SETVIRTWH    0x48004
#define MBOX_TAG_SETVIRTOFF   0x48009
#define MBOX_TAG_SETDEPTH     0x48005
#define MBOX_TAG_SETPXLORDR   0x48006
#define MBOX_TAG_GETFB        0x40001
#define MBOX_TAG_GETPITCH     0x40008
#define MBOX_TAG_GETSERIAL    0x10004
#define MBOX_TAG_GETTEMP      0x30006
#define MBOX_TAG_GETVOLTAGE   0x30003
#define MBOX_TAG_GETLED       0x30041
#define MBOX_TAG_SETLED       0x38041

#define MBOX_TAG_LAST         0x00000

enum {
    MBOX_CH_POWER = 0,
    MBOX_CH_FB    = 1,
    MBOX_CH_VUART = 2,
    MBOX_CH_VCHIQ = 3,
    MBOX_CH_LEDS  = 4,
    MBOX_CH_BTNS  = 5,
    MBOX_CH_TOUCH = 6,
    MBOX_CH_COUNT = 7,
    MBOX_CH_PROP  = 8,  // request from ARM for response by VideoCore
};

extern volatile unsigned int mbox[36];;

bool mbox_call(byte channel);
unsigned long get_board_serial();
double get_core_temperature();
bool set_onboard_led_status(bool status);
bool get_onboard_led_status();

#endif // !__MAILBOX_H__