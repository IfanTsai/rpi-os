.global get_processor_id
get_processor_id:
    mrs x0, mpidr_el1      // read processor ID from mpidr_el1 (Multiprocessor Affimity Register) register
    and x0, x0, #0xff
    ret

.global get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

.globl put32
put32:
    str w1,[x0]
    ret

.globl get32
get32:
    ldr w0,[x0]
    ret

.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.globl memzero
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    bgt memzero
    ret

.globl set_pgd
set_pgd:
    msr ttbr0_el1, x0
    tlbi vmalle1is   // invalidate TLB entries
    dsb ish          // ensure completion of TLB invalidation
    isb              // fluash instruction pipeline
    ret

.globl get_pgd
get_pgd:
    mov x1, 0
    ldr x0, [x1]
    mov x0, 0x1000
    msr ttbr0_el1, x0
    ldr x0, [x1]
    ret