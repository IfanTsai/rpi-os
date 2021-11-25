#ifndef __P_BASE_H__
#define __P_BASE_H__

#include "mm.h"

#define DEVICE_PHY_BASE      0x3f000000
#define PERIPHERAL_PHY_BASE  0x40000000
#define PBASE                (VA_START + DEVICE_PHY_BASE)
#define PERIPHERAL_BASE      (VA_START + PERIPHERAL_PHY_BASE)

#endif  // !__P_BASE_H__
