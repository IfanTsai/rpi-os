#include "timer.h"
#include "utils.h"
#include "peripherals/timer.h"
#include "printf.h"
#include "sched.h"

static unsigned int g_cur_val;
static unsigned int g_interval;

void sys_timer_init(unsigned int interval)
{
    g_interval = interval;

    g_cur_val = get32(TIMER_CLO); // get timer counter lower 32 bit
    g_cur_val += g_interval;
    put32(TIMER_C1, g_cur_val);  // set timer compare value
}

void handle_sys_timer_irq()
{
    //printf("timer interrupt received, timer current value %d\r\n", g_cur_val);

    g_cur_val += g_interval;

    put32(TIMER_C1, g_cur_val);       // set timer compare value
    put32(TIMER_CS, TIMER_CS_M1);     // clear timer status
    timer_tick();
}

#define NR_TIMERS 32
static timer_t timers[32];

void local_timer_init()
{
    /**
     * The local timer ALWAYS gets its timing pulses from the Crystal clock.
     * You get a 'timing pulse' every clock EDGE. Thus a 19.2 MHz crystal gives 38.4 M pulses/second.
     */
    unsigned int interval = 38400000 / 1000; // ms
    //_local_timer_cb = local_timer_cb;
    // [0:27] Reload value
    // [28] Timer enable (1 == enable)
    put32(TIMER_CTRL, (interval | (1 << 28)));
}

void handle_local_timer_irq()
{
    // [31] Interrupt flag clear when written as 1 (write-Only)
    // [30] Local timer-reloaded when written as 1 (write only)
    put32(TIMER_FLAG, 0x3 << 30);

    for (int i = 0; i < NR_TIMERS; i++) {
        if (timers[i].valid) {
            timers[i].ms--;
            if (!timers[i].ms) {
                timers[i].valid = false;
                timers[i].cb();
            }
        }
    }
}

void add_timer(unsigned int ms, local_timer_func cb)
{
    for (int i = 0; i < NR_TIMERS; i++) {
        if (!timers[i].valid) {
            timers[i].ms = ms;
            timers[i].cb = cb;
            timers[i].valid = true;

            break;
        }
    }
}
