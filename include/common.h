#ifndef __COMMON_H__
#define __COMMON_H__

#define offsetof(TYPE, MEMBER)  \
            ( (size_t)&( (TYPE *)0 )->MEMBER )

#define ARRAY_SIZE(ARRAY)  \
            ( sizeof(ARRAY) / sizeof( (ARRAY)[0] ) )

#define NULL  ((void *)0)
typedef unsigned char byte;

#define true 1
#define false 0
typedef unsigned char bool;

typedef unsigned long size_t;

#endif