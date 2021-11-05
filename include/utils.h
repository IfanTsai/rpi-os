#ifndef __UTILS_H__
#define __UTILS_H__

unsigned int get_processor_id();
unsigned int get_el();
void put32(unsigned long, unsigned int);
unsigned int get32(unsigned long);
void delay(unsigned long);

#endif // __UTILS_H__