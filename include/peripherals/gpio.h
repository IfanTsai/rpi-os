#ifndef __P_GPIO_H__
#define __P_GPIO_H__

#include "peripherals/base.h"

#define GPFSEL0         (PBASE + 0x00200000)
#define GPFSEL1         (PBASE + 0x00200004)
#define GPFSEL2         (PBASE + 0x00200008)
#define GPFSEL3         (PBASE + 0x0020000c)
#define GPFSEL4         (PBASE + 0x00200010)
#define GPFSEL5         (PBASE + 0x00200014)
#define GPSET0          (PBASE + 0x0020001C)
#define GPCLR0          (PBASE + 0x00200028)
#define GPPUD           (PBASE + 0x00200094)
#define GPPUDCLK0       (PBASE + 0x00200098)
#define GPPUDCLK1       (PBASE + 0x0020009c)

#endif  // !__P_GPIO_H__
