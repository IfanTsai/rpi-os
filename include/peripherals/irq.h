#ifndef __P_IRQ_H__
#define __P_IRQ_H__

#include "peripherals/base.h"

#define IRQ_BASIC_PENDING   (PBASE + 0x0000B200)
#define IRQ_PENDING_1       (PBASE + 0x0000B204)
#define IRQ_PENDING_2       (PBASE + 0x0000B208)
#define FIQ_CONTROL         (PBASE + 0x0000B20C)
#define ENABLE_IRQS_1       (PBASE + 0x0000B210)
#define ENABLE_IRQS_2       (PBASE + 0x0000B214)
#define ENABLE_BASIC_IRQS   (PBASE + 0x0000B218)
#define DISABLE_IRQS_1      (PBASE + 0x0000B21C)
#define DISABLE_IRQS_2      (PBASE + 0x0000B220)
#define DISABLE_BASIC_IRQS  (PBASE + 0x0000B224)

#define CORE0_INT_SOURCE    (PERIPHERAL_BASE + 0x60)

#define SYSTEM_TIMER_IRQ_0  (1 << 0)
#define SYSTEM_TIMER_IRQ_1  (1 << 1)
#define SYSTEM_TIMER_IRQ_2  (1 << 2)
#define SYSTEM_TIMER_IRQ_3  (1 << 3)
#define GPU_IRQ             (1 << 8)
#define LOCAL_TIME_IRQ      (1 << 11)

#endif  // !__P_IRQ_H__