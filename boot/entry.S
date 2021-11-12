#include "entry.h"
#include "arm/sysregs.h"
#include "sys.h"

    .macro ventry label
    .align 7   // since every exception entry offset is 0x80, align to 2 ^ 7 bit
    b \label
    .endm

    .macro save_registers, el
    sub sp, sp, #REGISTER_FRAME_SIZE
    stp x0,  x1,  [sp, #16 * 0]
    stp x2,  x3,  [sp, #16 * 1]
    stp x4,  x5,  [sp, #16 * 2]
    stp x6,  x7,  [sp, #16 * 3]
    stp x8,  x9,  [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]

    .if \el == 0
    mrs x21, sp_el0
    .else
    add x21, sp, #REGISTER_FRAME_SIZE
    .endif

    mrs x22, elr_el1
    mrs x23, spsr_el1

    stp x30, x21, [sp, #16 * 15]
    stp x22, x23, [sp, #16 * 16]
#ifdef DEBUG
    bl set_register_stack
    bl print_memory_layout
#endif
    .endm    // end .macro save_registers, el

    .macro restore_registers, el
    ldp x22, x23, [sp, #16 * 16]
    ldp x30, x21, [sp, #16 * 15]

    .if \el == 0
    msr sp_el0, x21
    .endif

    msr elr_el1, x22
    msr spsr_el1, x23

    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    add sp, sp, #REGISTER_FRAME_SIZE
#ifdef DEBUG
    bl clear_register_stack
    bl print_memory_layout
#endif
    eret
    .endm // end .macro restore_registers, el

    .macro handle_invalid_entry el, type
    save_registers \el
    mov x0, #\type
    mrs x1, esr_el1    // the reason for the exception is saved in the esr_elx
    mrs x2, elr_el1    // the return address is saved in elr_elx 
    bl show_invalid_entry_message
    b err_hang
    .endm

/**
 * exception vectors
 */
.align 11
.global exception_vector_table
exception_vector_table:
    ventry sync_invalid_el1t         // Synchronous EL1t
    ventry irq_invalid_el1t          // IRQ EL1t
    ventry fiq_invalid_el1t          // FIQ EL1t
    ventry error_invalid_el1t        // Error EL1t

    ventry sync_invalid_el1h         // Synchronous EL1h
    ventry el1_irq                   // IRQ EL1h, only realize it now !
    ventry fiq_invalid_el1h          // FIQ EL1h
    ventry error_invalid_el1h        // Error EL1h

    ventry el0_sync                  // Synchronous 64-bit EL0
    ventry el0_irq                   // IRQ 64-bit EL0
    ventry fiq_invalid_el0_64        // FIQ 64-bit EL0
    ventry error_invalid_el0_64      // Error 64-bit EL0

    ventry sync_invalid_el0_32       // Synchronous 32-bit EL0
    ventry irq_invalid_el0_32        // IRQ 32-bit EL0
    ventry fiq_invalid_el0_32        // FIQ 32-bit EL0
    ventry error_invalid_el0_32      // Error 32-bit EL0

sync_invalid_el1t:
    handle_invalid_entry 1, SYNC_INVALID_EL1t

irq_invalid_el1t:
    handle_invalid_entry 1, IRQ_INVALID_EL1t

fiq_invalid_el1t:
    handle_invalid_entry 1, FIQ_INVALID_EL1t

error_invalid_el1t:
    handle_invalid_entry 1, ERROR_INVALID_EL1t

sync_invalid_el1h:
    handle_invalid_entry 1, SYNC_INVALID_EL1h

fiq_invalid_el1h:
    handle_invalid_entry 1, FIQ_INVALID_EL1h

error_invalid_el1h:
    handle_invalid_entry 1, ERROR_INVALID_EL1h

fiq_invalid_el0_64:
    handle_invalid_entry 0, FIQ_INVALID_EL0_64

error_invalid_el0_64:
    handle_invalid_entry 0, ERROR_INVALID_EL0_64

sync_invalid_el0_32:
    handle_invalid_entry 0, SYNC_INVALID_EL0_32

irq_invalid_el0_32:
    handle_invalid_entry 0, IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
    handle_invalid_entry 0, FIQ_INVALID_EL0_32

error_invalid_el0_32:
    handle_invalid_entry 0, ERROR_INVALID_EL0_32

el1_irq:
    save_registers 1
    bl handle_irq
    restore_registers 1

el0_irq:
    save_registers 0
    bl handle_irq
    restore_registers 0

el0_sync:
    save_registers 0
    mrs x25, esr_el1
    lsr x24, x25,  #ESR_ELx_EC_SHIFT   // exception class
    cmp x24, #ESR_ELx_EC_SVC64
    beq el0_svc

sc_nr  .req x25   // the number of system calls
sc_no  .req x26   // system call number
sc_tbl .req x27   // the pointer of system call table

el0_svc:
    adr sc_tbl, sys_call_table
    uxtw sc_no, w8              // uxtx: Zero-extend  32bit to 64bit
    mov sc_nr, #__NR_syscalls
    bl enable_irq
    cmp sc_no, sc_nr
    bhs err_sys

    ldr x16, [sc_tbl, sc_no, lsl #3]  // sc_stbl + sc_no * 8
    blr x16

    bl disable_irq
    str x0, [sp, #S_X0]
    restore_registers 0

err_sys:
    handle_invalid_entry 0, SYSCALL_ERROR

.globl ret_from_fork
ret_from_fork:
    bl schedule_tail
    cbz x19, ret_to_user
    //bl print_memory_layout
    mov x0, x20   // x20: arg, x19: fn
    blr x19      // should never return

ret_to_user:
    bl disable_irq
    restore_registers 0    // call eret to switch to user mode

err_hang:
    wfi
    b err_hang