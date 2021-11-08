#include "fork.h"
#include "sched.h"
#include "mm.h"

int copy_process(unsigned long fn, unsigned long arg, unsigned long priority)
{
    preempt_disable();

    task_struct_t *p = (task_struct_t *)get_free_page();
    if (!p)
        return -1;
    
    p->state = TASK_RUNNING;
    p->priority = priority != 0 ? priority : current->priority;
    p->counter = p->priority;
    p->preempt_count = 1;

#ifdef DEBUG
    p->stack = 0;
    p->int_stack = 0;
    p->registers_stack = 0;
#endif // !DEBUG

    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
    p->cpu_context.lr = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

    unsigned int pid = get_free_pid();
    tasks[pid] = p;
    nr_tasks++;
    
    preempt_enable();

    //print_memory_layout();

    return 0;
}