#include "unistd.h"
#include "sys.h"

/* it is defined in sys.S */
int call_sys_clone(unsigned long fn, unsigned long arg, unsigned long stack);

_syscall0(long, malloc)
_syscall0(int, exit)
_syscall1(int, write, char *)

int clone(unsigned long fn, unsigned long arg, unsigned long stack)
{
    return call_sys_clone(fn, arg, stack);
}