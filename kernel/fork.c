#include "fork.h"
#include "sched.h"
#include "mm.h"
#include "string.h"
#include "printf.h"
#include "string.h"
#include "utils.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long priority)
{
    preempt_disable();

    task_struct_t *p = (task_struct_t *)allocate_kernel_page();
    if (!p)
        return -1;

    pt_regs_t *child_regs = task_pt_regs(p);

    if (clone_flags & PF_KTHREAD) {
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;
    } else {
        pt_regs_t *cur_regs = task_pt_regs(current);
        *child_regs = *cur_regs;   // copy the cpu state of the parent process to the child process completely
        /**
         *  When the child process is scheduled, restore_registers will restore the register value from pt_regs,
         *  which will cause the x0 returned by the system call to be 0
         */
        child_regs->regs[0] = 0;  // set x0  = 0

        copy_virt_memory(p);  // allocate and copy parent memory immediately, not use COW
    }

    p->flags = clone_flags;
    p->priority = priority != 0 ? priority : current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; // disable preemtion until schedule_tail

#ifdef DEBUG
    p->stack_count = 0;
    p->int_stack_count = 0;
    p->registers_stack_count = 0;
#endif // !DEBUG

    p->cpu_context.lr = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)child_regs;

    unsigned int pid = get_free_pid();
    tasks[pid] = p;
    nr_tasks++;

    preempt_enable();

    //print_memory_layout();

    return pid;
}

int move_to_user_mode(unsigned long start, unsigned long size, unsigned long pc)
{
    pt_regs_t *regs = task_pt_regs(current);
    regs->pstate = PSTATE_MODE_EL0t;
    regs->pc = pc;
    // assume the size of user stack does not exceed the 1 page, set the stack is two page, noted the 1 page is pt_regs
    regs->sp = PAGE_SIZE * 2; 

    unsigned long code_page = allocate_user_page(current, 0);
    if (!code_page)
        return -1;

    memcpy((void *)code_page, (void *)start, size);
    set_pgd(current->mm.pgd);

    return 0;
}
