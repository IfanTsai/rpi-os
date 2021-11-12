#include "fork.h"
#include "sched.h"
#include "mm.h"
#include "string.h"
#include "printf.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg,
                    unsigned long stack, unsigned long priority)
{
    preempt_disable();

    task_struct_t *p = (task_struct_t *)get_free_page();
    if (!p)
        return -1;

    pt_regs_t *child_regs = task_pt_regs(p);
    memset(child_regs, 0, sizeof(pt_regs_t));
    memset(&p->cpu_context, 0, sizeof(cpu_context_t));

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
        child_regs->regs[0]  = 0;  // set x0  = 0
        child_regs->sp = stack + PAGE_SIZE;

        p->stack = stack;
    }

    p->flags = clone_flags;
    p->priority = priority != 0 ? priority : current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1;  // disable perempt until call schedule_tail

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

int move_to_user_mode(unsigned long pc)
{
    unsigned long stack = get_free_page();   // allocate new user stack
    if (!stack)
        return -1;

    pt_regs_t *regs = task_pt_regs(current);
    memset(regs, 0, sizeof(pt_regs_t));

    regs->pc = pc;
    regs->pstate = PSTATE_MODE_EL0t;
    regs->sp = stack + PAGE_SIZE;

    current->stack = stack;

    return 0;
}
