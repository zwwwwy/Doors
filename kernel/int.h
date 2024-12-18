#ifndef C_INT_H
#define C_INT_H
#include "../lib/io.h"
#include "printk.h"
#include "trap.h"

#define TO_STR(X) #X
#define IRQ_HANDLER(int_num)                                                                                           \
	__attribute__((naked)) void IRQ_##int_num##_handler(void)                                                          \
	{                                                                                                                  \
		__asm__ __volatile__("pushq  $0\n\t"                                                                           \
							 "pushq  %rax\n\t"                                                                         \
							 "leaq   " TO_STR(IRQ_##int_num##_func) "(%rip),%rax\n\t"                                  \
																	"xchgq     %rax, (%rsp)\n\t"                       \
																	"jmp   save_reg\n\t");                             \
	}
#define IRQ_FUNC(int_num)                                                                                              \
	void IRQ_##int_num##_func(void)                                                                                    \
	{                                                                                                                  \
		printk("This is an interrupt from IRQ which index is %x.\n", int_num);                                         \
		io_out8(0x20, 0x20);                                                                                           \
		io_out8(0xa0, 0x20);                                                                                           \
	}

#endif
