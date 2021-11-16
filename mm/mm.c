#include "mm.h"
#include "common.h"
#include "arm/mmu.h"
#include "string.h"

static byte mem_map[PAGING_PAGES];

unsigned long allocate_kernel_page()
{
    unsigned long page = get_free_page();

    return !page ? 0 : page +VA_START;
}

unsigned long allocate_user_page(task_struct_t *task, unsigned long va)
{
    unsigned long page = get_free_page();
    if (!page)
        return 0;

    map_page(task, va, page);

    return page + VA_START;
}

unsigned long get_free_page()
{
    for (int i = 0; i < ARRAY_SIZE(mem_map); i++){
        if (!mem_map[i]) {
            mem_map[i] = 1;

            unsigned long page = LOW_MEMORY + i * PAGE_SIZE;
            memset((void *)__VA(page), 0, PAGE_SIZE);

            return page;
        }
    }

    return 0;
}

void free_page(unsigned long p)
{
    mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

void map_table_entry(unsigned long *pte, unsigned long va, unsigned long pa)
{
    unsigned long index = va >> PAGE_SHIFT;
    index = index & (PTRS_PER_TABLE - 1);
    unsigned long entry = pa | MMU_PTE_FLAGS;
    pte[index] = entry;
}

unsigned long map_table(unsigned long *table, unsigned long shift, unsigned long va, int *new_table)
{
    unsigned long index = va >> shift;
    index = index & (PTRS_PER_TABLE - 1);
    if (!table[index]) {
        *new_table = 1;
        unsigned long next_level_table = get_free_page();
        unsigned long entry = next_level_table | MM_TYPE_PAGE_TABLE;
        table[index] = entry;

        return next_level_table;
    } else {
        *new_table = 0;
    }

    return table[index] & PAGE_MASK;
}

/*
 * the execution process is similar to __create_page_tables in boot.S,
 * but the difference is that it only maps 1 page and it uses page mapping instead of block mapping
 */
void map_page(task_struct_t *task, unsigned long va, unsigned long page)
{
    if (!task->mm.pgd) {
        task->mm.pgd = get_free_page();
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
    }

    unsigned long pgd = task->mm.pgd;
    int new_table;
    unsigned long pud = map_table((unsigned long *)(pgd + VA_START), PGD_SHIFT, va, &new_table);
    if (new_table)
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud;

    unsigned long pmd = map_table((unsigned long *)(pud + VA_START) , PUD_SHIFT, va, &new_table);
    if (new_table)
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;

    unsigned long pte = map_table((unsigned long *)(pmd + VA_START), PMD_SHIFT, va, &new_table);
    if (new_table)
        task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;

    map_table_entry((unsigned long *)(pte + VA_START), va, page);
    user_page_t p = {page, va};
    task->mm.user_pages[task->mm.user_pages_count++] = p;
}

/**
 * @brief copy cureent task usre virtual memroy to the same virtual address of destination task
 * 
 * @param dst the destination task
 * @return retrurn 0 if succeed, -1 if an error occured
 */
int copy_virt_memory(task_struct_t *dst)
{
    task_struct_t *src = current;
    for (int i = 0; i < src->mm.user_pages_count; i++) {
        unsigned long kernel_va = allocate_user_page(dst, src->mm.user_pages[i].virt_addr);
        if (!kernel_va)
            return -1;

        memcpy((void *)kernel_va, (void *)src->mm.user_pages[i].virt_addr, PAGE_SIZE);
    }

    return 0;
}

/**
 * @brief handle page fault interrupt, allocate and map the new page
 * 
 * @param addr the memory address this triggered the page fault
 * @param esr  the value of ESR_ELx register
 * @return int retrurn 0 if succeed, -1 if an error occured
 */
int do_mem_abort(unsigned long addr, unsigned long esr)
{
    unsigned long dfs = (esr & 0b111111);
    if ((dfs & 0b111100) == 0b100) {
        unsigned long page = get_free_page();
        if (!page)
            return -1;

        map_page(current, addr & PAGE_MASK, page);

        return 0;
    }

    return -1;
}