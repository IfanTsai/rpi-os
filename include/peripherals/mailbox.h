#ifndef __P_MAILBOX_H__
#define __P_MAILBOX_H__

#include "peripherals/base.h"

#define VIDEOCORE_MBOX  (PBASE + 0x0000b880)
#define MBOX_READ       (VIDEOCORE_MBOX + 0x00)
#define MBOX_POLL       (VIDEOCORE_MBOX + 0x10)
#define MBOX_SENDER     (VIDEOCORE_MBOX + 0x14)
#define MBOX_STATUS     (VIDEOCORE_MBOX + 0x18)
#define MBOX_CONFIG     (VIDEOCORE_MBOX + 0x1c)
#define MBOX_WRITE      (VIDEOCORE_MBOX + 0x20)

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

#endif // !__P_MAILBOX_H__