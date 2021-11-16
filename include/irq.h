#ifndef __IRQ_H__
#define __IRQ_H__

void exception_vector_table_init();
void enable_irq();
void disable_irq();

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address, unsigned int el);
void enable_interrupt_controller();
void handle_irq();

#endif
