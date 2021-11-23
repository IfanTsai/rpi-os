#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "common.h"
#include "utils.h"

static task_struct_t init_task = INIT_TASK;
task_struct_t *current = &init_task;
task_struct_t *tasks[NR_TASKS] = { &init_task, };
int nr_tasks = 1;

unsigned int get_free_pid()
{
    for (int i = 0; i < NR_TASKS; i++) {
        if (!tasks[i])
            return i;
    }

    return -1;
}

void _schedule()
{
    preempt_disable();

    int next, c;
    task_struct_t *p;

    for (;;) {
        c = -1;
        next = 0;
        for (int i = 0; i < NR_TASKS; i++) {
            p = tasks[i];
            if (p && p->state == TASK_RUNNING && p->counter > c) {
                c = p->counter;
                next = i;
            }
        }

        if (c)
            break;

        for (int i = 0; i < NR_TASKS; i++) {
            p = tasks[i];
            if (p)
                p->counter = (p->counter >> 1) + p->priority;
        }
    }

    switch_to(tasks[next]);

    preempt_enable();
}

void schedule()
{
    current->counter = 0;
    _schedule();
}

void switch_to(task_struct_t *next)
{
    if (current == next)
        return;
    
    task_struct_t *prev = current;
    current = next;
#ifdef DEBUG
    if (next->stack_count == 0) {
        next->stack_count = 1;
        print_memory_layout();
    }
#endif
    set_pgd(next->mm.pgd);

    cpu_switch_to(prev, next, TASK_CPU_CONTEXT_OFFSET, TASK_FP_SIMD_CONTEXT_OFFSET);
}

void schedule_tail()
{
    preempt_enable();
}

void timer_tick()
{
    current->counter--;

    if (current->counter > 0 || current->preempt_count > 0)
        return;

    current->counter = 0;
    enable_irq();

#ifdef DEBUG
    current->int_stack_count++;
    print_memory_layout();
#endif

    _schedule();

#ifdef DEBUG
    current->int_stack_count--;
    print_memory_layout();
#endif

    disable_irq();
}

void exit_process()
{
    preempt_disable();

    current->state = TASK_ZOMBIE;

    preempt_enable();

    schedule();
}

void print_memory_layout()
{
#ifdef DEBUG
    printf("\r\n\r\n");
    printf("         0 +------------------------+\r\n");
    printf("           | kernel image           |\r\n");
    printf("           |                        |\r\n");
    printf("           |------------------------|\r\n");
    printf("           | init task stack        |\r\n");
    for (int i = 1; i < NR_TASKS; i++) {
        task_struct_t *p = tasks[i];
        if (p) {
            printf("0x%08x +------------------------+\r\n", p);
            printf("           | task_struct %d          |\r\n", i);
            printf("           |------------------------|\r\n");
            printf("           |                        |\r\n");
            for (int j = 0; j < p->int_stack_count; j++) {
                printf("           |------------------------|\r\n");
                printf("           | task %d stack (int)     |\r\n", i);
            }

            if (p->registers_stack_count > 0) {
                printf("           |------------------------|\r\n");
                printf("           | task %d saved registers |\r\n", i);
            }

            if (p->stack_count > 0) {
                printf("           |------------------------|\r\n");
                printf("           | task %d stack          |\r\n", i);
            }
        }
    }
    printf("0x3f000000 +------------------------+\r\n");
    printf("           | device registers       |\r\n");
    printf("0x40000000 +------------------------+\r\n");
    printf("\r\n\r\n");
#endif
}

void set_register_stack()
{
#ifdef DEBUG
    current->registers_stack_count = 1;
#endif
}

void clear_register_stack()
{
#ifdef DEBUG
    current->registers_stack_count = 0;
#endif
}
