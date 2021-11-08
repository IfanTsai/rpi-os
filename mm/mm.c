#include "mm.h"
#include "common.h"

static byte mem_map[PAGING_PAGES];

unsigned long get_free_page()
{
    for (int i = 0; i < ARRAY_SIZE(mem_map); i++) {
        if (!mem_map[i]) {
            mem_map[i] = 1;

            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }

    return 0;
}

void free_page(unsigned long p)
{
    mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}