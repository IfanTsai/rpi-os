#ifndef __P_PM_H__
#define __P_PM_H__

#include "peripherals/base.h"

#define PM_RSTC         (PBASE + 0x0010001c)
#define PM_RSTS         (PBASE + 0x00100020)
#define PM_WDOG         (PBASE + 0x00100024)
#define PM_WDOG_MAGIC    0x5a000000
#define PM_RSTC_FULLRST  0x00000020

#endif // !__P_PM_H__