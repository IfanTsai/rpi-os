#include "sys.h"

_syscall0(long, malloc)
_syscall0(int, exit)
_syscall0(int, fork)
_syscall1(int, write, char *)
_syscall1(int, priority, long)
