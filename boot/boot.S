#include "mm.h"

.section ".text.boot"

.global _start
    mrs x0, mpidr_el1      // read processor ID from mpidr_el1 (Multiprocessor Affimity Register) register
    and x0, x0, #0xff
    cbz x0, master         // goto master when the processor ID is 0
    b proc_hang            // other processors are run in dead loop

master:
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
    b proc_hang