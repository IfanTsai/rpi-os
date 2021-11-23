#include "printf.h"

typedef void (*putcf)(void *, char);
static putcf stdout_putf;
static void *stdout_putp;

static int uli2a(unsigned long int num, unsigned int base, int uc, char *bf)
{
    int n = 0;
    unsigned int d = 1;

    while (num / d >= base)
        d *= base;

    while (d) {
        int dgt = num / d;
        num %= d;
        d /= base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10);
            ++n;
        }
    }

    *bf = 0;

    return n;
}

static inline void li2a(long num, char *bf)
{
    if (num < 0) {
        num = -num;
        *bf++ = '-';
    }

    uli2a(num, 10, 0, bf);
}

static inline void i2a(int num, char *bf)
{
    if (num < 0) {
        num = -num;
        *bf++ = '-';
    }

    uli2a(num, 10, 0, bf);
}

static inline int a2d(char ch)
{
    switch (ch) {
    case '0' ... '9':
        return ch - '0';
    case 'a' ... 'f':
        return ch - 'a' + 10;
    case 'A' ... 'F':
        return ch - 'A' + 10;
    default:
        return -1;
    }
}

static char a2i(char ch, char **src, int base, int *nump)
{
    char *p = *src;
    int num = 0;
    int digit;

    while ((digit = a2d(ch)) >= 0) {
        if (digit > base)
            break;

        num = num * base + digit;
        ch = *p++;
    }

    *src = p;
    *nump = num;

    return ch;
}

static void f2a(double num, unsigned int base, int uc, int w, int f, char *bf)
{
    int n = uli2a((unsigned long int)num, base, uc, bf);
    *(bf + n) = '.';
    n++;

    for (int i = 0; i < f; i++) {
        int t = num * 10;
        bf[n + i] = (t % 10) + '0';
        num *= 10;
    }


    *(bf + n + f) = 0;
}

static void putchw(void *putp, putcf putf, int n, char z, char *bf)
{
    char fc = z ? '0' : ' ';
    char ch;
    char *p = bf;

    while (*p && *p++ != '.' && n > 0)
        n--;

    while (n-- > 0)
        putf(putp, fc);

    while ((ch = *bf++))
        putf(putp, ch);
}

void tfp_format(void *putp, putcf putf, char *fmt, va_list va)
{
    char bf[32];
    char ch;

    while ((ch = *(fmt++))) {
        if (ch != '%')
            putf(putp, ch);
        else {
            char lz = 0, lng = 0;
            int w = 0;
            int f = 2;
            ch = *(fmt++);

            if (ch == '0') {
                ch = *(fmt++);
                lz = 1;
            }

            if (ch >= '0' && ch <= '9')
                ch = a2i(ch, &fmt, 10, &w);

            if (ch == '.') {
                ch = *(fmt++);
                ch = a2i(ch, &fmt, 10, &f);
            }

            if (ch == 'l') {
                ch = *(fmt++);
                lng = 1;
            }

            switch (ch) {
            case 0:
                goto abort;
            case 'u': {
                if (lng)
                    uli2a(va_arg(va, unsigned long int), 10, 0, bf);
                else
                    uli2a(va_arg(va, unsigned int), 10, 0, bf);

                putchw(putp, putf, w, lz, bf);

                break;
            }
            case 'd': {
                if (lng)
                    li2a(va_arg(va, unsigned long int), bf);
                else
                    i2a(va_arg(va, int), bf);

                putchw(putp, putf, w, lz, bf);

                break;
            }
            case 'x':
            case 'X':
                if (lng)
                    uli2a(va_arg(va, unsigned long int), 16, (ch == 'X'), bf);
                else
                    uli2a(va_arg(va, unsigned int), 16, (ch == 'X'), bf);

                putchw(putp, putf, w, lz, bf);

                break;
            case 'c':
                putf(putp, (char)(va_arg(va, int)));

                break;
            case 's':
                putchw(putp, putf, w, 0, va_arg(va, char *));

                break;
            case 'f':
                f2a(va_arg(va, double), 10, 0, w, f, bf);

                putchw(putp, putf, w, lz, bf);
                break;
            case '%':
                putf(putp, ch);
            default:
                break;
            }
        }
    }
abort:;
}

void printf_init(void *putp, void (*putf)(void *, char))
{
    stdout_putf = putf;
    stdout_putp = putp;
}

void tfp_printf(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    tfp_format(stdout_putp, stdout_putf, fmt, va);

    va_end(va);
}

static inline void putcp(void *p, char c)
{
    *(*((char **)p))++ = c;
}

void tfp_sprintf(char *s, char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    tfp_format(&s, putcp, fmt, va);
    putcp(&s, 0);

    va_end(va);
}

void tfp_println(char *buf)
{
    tfp_printf(buf);
    tfp_printf("\r\n");
}
