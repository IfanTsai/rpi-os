.globl exception_vector_table_init
exception_vector_table_init:
    adr x0, exception_vector_table
    msr vbar_el1, x0               // set system exception vector table address
    ret

.globl enable_irq
enable_irq:
    msr daifclr, #2     // DAIF: clear I
    ret

.globl disable_irq
disable_irq:
    msr daifset, #2    // DAIF: set I
    ret