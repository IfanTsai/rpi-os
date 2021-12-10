#include "mm.h"
#include "arm/sysregs.h"
#include "arm/mmu.h"
#include "peripherals/base.h"

.section ".text.boot"

.global _start
_start:
    bl get_processor_id
    cbz x0, master         // goto master when the processor ID is 0
    b proc_hang            // other processors are run in dead loop

proc_hang:
    wfi
    b proc_hang

master:
    //  <<ARM Architecture Reference Manual ARMv8>> D12.2.100
    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0  // set little endian, disable icache, dcache and mmu

    // <<ARM Architecture Reference Manual ARMv8>> D12.2.44
    ldr x0, =HCR_VALUE
    msr hcr_el2, x0    // set the execution state for EL1 is AArch64

    // <<ARM Architecture Reference Manual ARMv8>> D12.2.99
    ldr x0, =SCR_VALUE
    msr scr_el3, x0    // set the execution state for EL2 is AArch64

    // <<ARM Architecture Reference Manual ARMv8>> C5.2.19
    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0   //  disable all interruot, set AArch64 state is EL1h

    // <<ARM Architecture Reference Manual ARMv8>> D12.2.29
    ldr x0, =CPACR_VALUE
    msr cpacr_el1, x0  // allow EL1 use FP/SMID

    // <<ARM Architecture Reference Manual ARMv8>> C5.2.6
    adr x0, el1_entry
    msr elr_el3, x0    //  set EL3 exception returns address

    eret  // exception return, restore PSTATE from spsr_el3, ldr PC = elr_el3

el1_entry:
    adr x0, bss_start
    adr x1, bss_end
    cmp x0, x1
    beq skip_clean_bss
    bl clean_bss
skip_clean_bss:
    bl __create_page_tables

    mov x0, #VA_START
    add sp, x0, #LOW_MEMORY  // set stack virtual address

    adrp x0, pg_dir
    msr ttbr1_el1, x0       // load page dir

    ldr x0, =TCR_VALUE
    msr tcr_el1, x0         // set mmu some flags

    ldr x0, =MAIR_VALUE
    msr mair_el1, x0        // set page attributes

    // load linker address, must be loaded before MMU is enabled, because PC will use address translation after MMU is enabled
    ldr x2, =kernel_main

    mov x0, #SCTLR_MMU_ENABLED
    msr sctlr_el1, x0       // enable MMU

    br x2

clean_bss:
    str xzr, [x0], #8
    cmp x0, x1
    bne clean_bss
    ret

    .macro create_pgd_entry, tbl, virt, tmp1, tmp2, tmp3
    create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2, \tmp3
    create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2, \tmp3
    .endm   // end .macro  create_pgd_entry

    .macro create_table_entry, tbl, virt, shift, tmp1, tmp2, tmp3
    lsr \tmp1, \virt, #\shift
    and \tmp1, \tmp1, #PTRS_PER_TABLE - 1        // table index  (PGD Index or PUD Index)
    add \tmp3, \tmp1, #1                         // index + 1
    mov \tmp2, #PAGE_SIZE
    // next page table = \tbl + PAGE_SIZE * (index + 1)
    madd \tmp2, \tmp3, \tmp2, \tbl               // next page table index (if tbl is PGD table, next table is PUD table)
    orr \tmp2, \tmp2, #MM_TYPE_PAGE_TABLE        // generate page entry descriptor
    str \tmp2, [\tbl, \tmp1, lsl #3]             // * (table + index x 8) = page entry
    add \tmp1, \tmp1, #1
    mov \tmp2, #PAGE_SIZE
    // \tbl = \tbl + PAGE_SIZE x ( index + 1 )
    madd \tbl, \tmp1, \tmp2, \tbl               // next level table page
    .endm   // end .macro create_table_entry

    .macro create_block_map, tbl, phys, start, end, flags, tmp1
    lsr \start, \start, #SECTION_SHIFT
    and \start, \start, #PTRS_PER_TABLE - 1      // table start index (PMD start index)
    lsr \end, \end, #SECTION_SHIFT
    and \end, \end, #PTRS_PER_TABLE - 1          // table end index (PMD end index)
    lsr \phys, \phys, #SECTION_SHIFT
    mov \tmp1, #\flags
    orr \phys, \tmp1, \phys, lsl #SECTION_SHIFT  // table entry
9999:
    str \phys, [\tbl, \start, lsl #3]            // store the entry
    add \start, \start, #1                       // next entry
    add \phys, \phys, #SECTION_SIZE              // next block
    cmp \start, \end
    bls 9999b
    .endm   // end .macro create_block_map

/*
                          Virtual address                                                 Physical Memory
+-----------------------------------------------------------------------+              +-----------------_+
|         | PGD Index | PUD Index | PMD Index |      Section offset     |              |                  |
+-----------------------------------------------------------------------+              |                  |
63        47     |    38      |   29     |    20            |           0              |    Section N     |
                 |            |          |                  |                    +---->+------------------+
                 |            |          |                  |                    |     |                  |
          +------+            |          |                  |                    |     |                  |
          |                   |          +----------+       |                    |     |------------------|
+------+  |        PGD        |                     |       +------------------------->| Physical address |
| ttbr |---->+-------------+  |           PUD       |                            |     |------------------|
+------+  |  |             |  | +->+-------------+  |            PMD             |     |                  |
          |  +-------------+  | |  |             |  | +->+-----------------+     |     +------------------+
          +->| PUD address |----+  +-------------+  | |  |                 |     |     |                  |
             +-------------+  +--->| PMD address |----+  +-----------------+     |     |                  |
             |             |       +-------------+  +--->| Section address |-----+     |                  |
             +-------------+       |             |       +-----------------+           |                  |
                                   +-------------+       |                 |           |                  |
                                                         +-----------------+           |                  |
                                                                                       +------------------+

                           PGD, PUD Descriptor format
`+-----------------------------------------------------------------------------------------------+
 | Upper attributes | Address (bits 47:12) | Lower attributes | Block/table bit (1) | Valid bit |
 +----------------------------------------------------------------------------------------------+
 63                 47                     11                 2                    1           0

                           PMD Descriptor format (section seize: 2M = 2 ^ 20)
 `+----------------------------------------------------------------------------------------------------------+
 | Upper attributes | Address (bits 47:21) | reserved  | Lower attributes | Block/table bit (0) | Valid bit |
 +----------------------------------------------------------------------------------------------------------+
 63                 47                    20          11                 2                    1           0

*/
/**
 * in order to map 1G memory, I just use 3 page: 1 PGD, 1 PUD and 1 PMD, I put them in contiguous memory
 * and we use block memroy mapping, the block size is 2MB
 */
__create_page_tables:
    mov x29, x30                         // save return address

    adrp x0, pg_dir        // it is defined in linker script
    mov x1, #PG_DIR_SIZE
    bl memzero

    adrp x0, pg_dir
    mov x1, #VA_START
    create_pgd_entry x0, x1, x2, x3, x4  // x2, x3 x4 is temp register

    /* Mapping kernel and init stack 0x0 - 0x3f000000 */
    mov x1, xzr                         // start mapping from physical offset 0
    mov x2, #VA_START                   // first virtual address
    ldr x3, =(VA_START + DEVICE_PHY_BASE - SECTION_SIZE)  // last virtual address
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4   // x0: PMD table, x4: temp register

    /* Mapping device memory 0x3f000000 - 0x40000000 */
    mov x1, #DEVICE_PHY_BASE                         // start mapping from device address
    ldr x2, =(VA_START + DEVICE_PHY_BASE)            // first virtual address
    ldr x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)   // last virtual address
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    /* inorder to map 0x40000000 - 0x80000000 (1GB), need to create another PUD entry and PMD page */
    sub x0, x0, #PAGE_SIZE                            // PUD table address
    ldr x1, =(VA_START + PERIPHERAL_PHY_BASE)
    create_table_entry x0, x1, PUD_SHIFT, x2, x3, x4

    /* Mapping device memory 0x40000000 - 0x80000000 */
    mov x1, #PERIPHERAL_PHY_BASE                         // start mapping from device address
    ldr x2, =(VA_START + PERIPHERAL_PHY_BASE)            // first virtual address
    ldr x3, =(VA_START + PERIPHERAL_PHY_BASE + PHYS_MEMORY_SIZE - SECTION_SIZE)   // last virtual address
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    mov x30, x29                                        // restore return address
    ret
