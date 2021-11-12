#ifndef __FORK_H__
#define __FORK_H__

#include "sched.h"

/*
 * PSTATE register bits
 */
#define PSTATE_MODE_EL0t   0x00000000
#define PSTATE_MODE_EL1t   0x00000004
#define PSTATE_MODE_EL1h   0x00000005
#define PSTATE_MODE_EL2t   0x00000008
#define PSTATE_MODE_EL2h   0x00000009
#define PSTATE_MODE_EL3t   0x0000000c
#define PSTATE_MODE_EL3h   0x0000000d

/* this structure used under save_registers and store_registers */
typedef struct {
    unsigned long regs[31];  // x0 - x30
    unsigned long sp;        // sp_or sp_el0
    unsigned long pc;
    unsigned long pstate;
} pt_regs_t;

int move_to_user_mode(unsigned long pc);
int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg,
                    unsigned long stack, unsigned long priority);

static inline pt_regs_t *task_pt_regs(task_struct_t *task)
{
    return (pt_regs_t *)((unsigned long)task + THREAD_SIZE - sizeof(pt_regs_t));
}

#endif // !__FORK_H__