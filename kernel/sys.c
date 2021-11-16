#include "sys.h"
#include "printf.h"
#include "fork.h"
#include "mm.h"

int sys_write(char *buf)
{
    printf(buf);

    return 0;
}

long sys_malloc()
{
    unsigned long addr = get_free_page();
    
    return addr ? addr : -1;
}

int sys_exit()
{
    exit_process();

    return 0;
}

int sys_priroiry(long priority)
{
    current->priority = priority;

    return 0;
}

int sys_fork()
{
    return copy_process(0, 0, 0, 0);
}


void * const sys_call_table[] = {
    sys_write,
    sys_malloc,
    sys_exit,
    sys_priroiry,
    sys_fork,
};