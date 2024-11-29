#include "printk.h"
void __stack_chk_fail(void) {
	printk_color("[ERROR]Stack fail.\n", 0x00ff0000, 0);
	while (1)
		;
}

void int_not_define() {
	__asm__ __volatile__("cld\n\t"
						 "pushq	%%rax\n\t"
						 "pushq	%%rbx\n\t"
						 "pushq	%%rcx\n\t"
						 "pushq	%%rdx\n\t"
						 "pushq	%%rbp\n\t"
						 "pushq	%%rdi\n\t"
						 "pushq	%%rsi\n\t"
						 "pushq	%%r8\n\t"
						 "pushq	%%r9\n\t"
						 "pushq	%%r10\n\t"
						 "pushq	%%r11\n\t"
						 "pushq	%%r12\n\t"
						 "pushq	%%r13\n\t"
						 "pushq	%%r14\n\t"
						 "pushq	%%r15\n\t"

						 "movq	%%es, %%rax\n\t"
						 "pushq %%rax\n\t"
						 "movq	%%ds, %%rax\n\t"
						 "pushq %%rax\n\t"

						 "movq 	%0, %%rax\n\t"
						 "movq 	%%rax, %%ds\n\t"
						 "movq 	%%rax, %%es\n\t" ::"i"(0x10)
						 : "rax");

	unsigned long pc_addr;
	__asm__ __volatile__("movq 	176(%%rsp),%0\n\t" : "=a"(pc_addr)::"rsp");
	printk_color("[ERROR]An unknown interrupt or system exception occurred. current PC=%l\n",
				 0x00ff0000, 0, pc_addr);
	while (1)
		;
}
