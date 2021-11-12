#include "sys.h"

.global call_sys_clone
call_sys_clone:
    mov x10, x0           // fn
    mov x11, x1           // arg
    mov x12, x2           // stack

    /* do the system call */
    mov x0, x2            // stack
    mov x8, #__NR_clone
    svc 0x0

    cmp x0, #0
    beq thread_start 
    ret

thread_start:
    mov x29, #0

    mov x0, x11          // arg
    blr x10              // fn

    mov x8, #__NR_exit
    svc 0x0