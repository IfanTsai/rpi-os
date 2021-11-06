#include "mm.h"
#include "arm/sysregs.h"

.section ".text.boot"

.global _start
    bl get_processor_id
    cbz x0, master         // goto master when the processor ID is 0
    b proc_hang            // other processors are run in dead loop

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
    mov sp, #LOW_MEMORY    // set stack address
    bl start_kernel
    
clean_bss:
    str xzr, [x0], #8
    cmp x0, x1
    bne clean_bss
    ret

proc_hang:
    wfi
    b proc_hang