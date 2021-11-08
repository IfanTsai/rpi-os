#include "timer.h"
#include "utils.h"
#include "peripherals/timer.h"
#include "printf.h"
#include "sched.h"

static unsigned int g_cur_val;
static unsigned int g_interval;

void timer_init(unsigned int interval)
{
    g_interval = interval;

    g_cur_val = get32(TIMER_CLO); // get timer counter lower 32 bit
    g_cur_val += g_interval;
    put32(TIMER_C1, g_cur_val);  // set timer compare value
}

void handle_timer_irq()
{
    //printf("timer interrupt received, timer current value %d\r\n", g_cur_val);

    g_cur_val += g_interval;

    put32(TIMER_C1, g_cur_val);       // set timer compare value
    put32(TIMER_CS, TIMER_CS_M1);     // clear timer status
    timer_tick();
}