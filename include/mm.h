#ifndef __MM_H__
#define __MM_H__

#define PAGE_SHIFT              12
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE               (1 << PAGE_SHIFT)       // 4K
#define SECTION_SIZE            (1 << SECTION_SHIFT)   // 2M

#define LOW_MEMORY              (2 * SECTION_SIZE)     // 4M

#endif // !__MM_H__