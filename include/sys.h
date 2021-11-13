#ifndef __SYS_H__
#define __SYS_H__

#define __NR_syscalls    5

#define __NR_write       0
#define __NR_malloc      1
#define __NR_clone       2
#define __NR_exit        3
#define __NR_priority    4

#ifndef __ASSEMBLER__

#include "errno.h"

#define _syscall0(type, name) \
type name() \
{ \
    long __res; \
    __asm__ __volatile__ ( \
    "   mov w8, %w1\n" \
    "   svc #0\n" \
    "   mov %x0, x0" \
    : "=&r" (__res) \
    : "r" (__NR_##name) \
    :); \
    errno = -__res; \
    if (__res >= 0) \
        return (type) __res; \
    return -1; \
}

#define _syscall1(type, name, atype) \
type name(atype a) \
{ \
    long __res; \
    __asm__ __volatile__ ( \
    "   mov w8, %w1\n" \
    "   mov x0, %x2\n" \
    "   svc #0\n" \
    "   mov %x0, x0" \
    : "=&r" (__res) \
    : "r" (__NR_##name), "r" ((long)(a)) \
    :); \
    errno = -__res; \
    if (__res >= 0) \
        return (type) __res; \
    return -1; \
}


#endif // !__ASSEMBLER__

#endif // !__SYS_H__