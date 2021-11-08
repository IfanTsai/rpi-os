#ifndef __MM_H__
#define __MM_H__

#include "peripherals/base.h"

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE               (1 << PAGE_SHIFT)       // 4K
#define SECTION_SIZE            (1 << SECTION_SHIFT)   // 2M

#define LOW_MEMORY              (2 * SECTION_SIZE)     // 4M
#define HIGH_MEMORY             PBASE                  // 0x3F000000

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY / PAGE_SIZE)

#ifndef __ASSEMBLER__

unsigned long get_free_page();
void free_page(unsigned long p);

#endif // __ASSEMBLER__

#endif // !__MM_H__