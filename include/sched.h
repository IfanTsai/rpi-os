#ifndef __SCHED_H__
#define __SCHED_H__


#define TASK_CPU_CONTEXT_OFFSET       (offsetof(task_struct_t, cpu_context))
#define TASK_FP_SIMD_CONTEXT_OFFSET   (offsetof(task_struct_t, fp_simd_context))

#ifndef __ASSEMBLER__

#define NR_TASKS 64
#define THREAD_SIZE 4096

#define TASK_RUNNING 0
#define TASK_ZOMBIE  1

#define PF_KTHREAD  (1 << 0)

typedef struct {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;    // x29
    unsigned long lr;    // x30
    unsigned long sp; 
} cpu_context_t;

#define MAX_PROCESS_PAGES     16

typedef struct {
    unsigned long phys_addr;
    unsigned long virt_addr;
} user_page_t;

typedef struct {
    unsigned long pgd;
    int user_pages_count;
    user_page_t user_pages[MAX_PROCESS_PAGES];
    int kernel_pages_count;
    unsigned long kernel_pages[MAX_PROCESS_PAGES];
} mm_struct_t;

typedef struct {
    __uint128_t v[32];  // v0 - v31
    unsigned int fpsr;
    unsigned int fpcr;
} fp_simd_context_t;

typedef struct {
    cpu_context_t cpu_context;
    fp_simd_context_t fp_simd_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
    unsigned long flags;
    mm_struct_t mm;
#ifdef DEBUG     // debug for print_memory_layout
    long int_stack_count;
    long registers_stack_count;
    long stack_count;
#endif
} task_struct_t;

#define INIT_TASK { \
/* cpu_context */ { 0 }, \
/* fpsimd_context */ { { 0 }, 0, 0}, \
/* state etc */ 0, 0, 1, 0, PF_KTHREAD, \
/* mm */ { 0, 0, { { 0 } }, 0, { 0 } }  \
}

extern task_struct_t *current;
extern task_struct_t *tasks[NR_TASKS];
extern int nr_tasks;

static inline void preempt_disable()
{
    current->preempt_count++;
}

static inline void preempt_enable()
{
    current->preempt_count--;
}

unsigned int get_free_pid();
void ret_from_fork();
void switch_to(task_struct_t *next);
void cpu_switch_to(task_struct_t *prev, task_struct_t *next, unsigned int cpu_context_off, unsigned int fp_simd_context_off);
void schedule();
void timer_tick();
void exit_process();
void print_memory_layout();

#endif // __ASSEMBLER__

#endif // !__SCHED_H__