#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>

void printf_init(void *putp, void (*putf)(void *, char));

#define printf tfp_printf
#define sprintf tfp_sprintf
#define println tfp_println

void tfp_printf(char *fmt, ...);
void tfp_sprintf(char* s, char *fmt, ...);
void tfp_println(char *buf);

#endif // !__PRINTF_H__