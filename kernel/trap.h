#ifndef C_TRAP_H
#define C_TRAP_H

typedef struct {
	unsigned char x[16];
} idt_element;
extern idt_element idt_64[];

#define SET_INT_GATE_DPL_0(n, ist, func_addr) set_gate(idt_64 + n, ist, 0x8e, func_addr)

void set_gate(idt_element* idt_element, unsigned ist, unsigned short attr, void* func_addr);

void __stack_chk_fail(void);
void blue_screen();
void divide_error_entry();
#endif
