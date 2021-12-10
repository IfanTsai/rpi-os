#ifndef __TIMER_H__
#define __TIMER_H__

#include "common.h"

typedef void (*local_timer_func)();

typedef struct {
    unsigned long ms;
    local_timer_func cb;
    bool valid;
} timer_t;

void sys_timer_init(unsigned int interval);
void handle_sys_timer_irq();
void local_timer_init();
void handle_local_timer_irq();
void add_timer(unsigned int ms, local_timer_func cb);

#endif