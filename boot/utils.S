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