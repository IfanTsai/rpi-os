#ifndef	_USER_H
#define	_USER_H

void user_delay(unsigned long);
void user_process();
extern unsigned long user_start;
extern unsigned long user_end;
extern unsigned long user_bss_start;
extern unsigned long user_bss_end;

#endif  /*_USER_H */