#include "info.h"
#include "init.h"
#include "printk.h"
#include "trap.h"

extern buffer_struck  buffer_info;
extern display_struct display_info;

__attribute__((naked)) void setup_kernel() {
	__asm__ __volatile__("movq  %0, %%rsp\n\t"
						 "movw  %1, %%ax\n\t"
						 "movw  %%ax, %%ss\n\t"
						 "jmp	kernel_start\n\t"
						 :
						 : "i"(0xffff800000010000), "i"(0x0010)
						 : "ax");
}

void kernel_start(void) {
	init_display();
	init_buffer();
	printk("idt:%lx\n", *(long*)idt_64);
	printk("idt_addr:%lx\n", idt_64);
	init_trap();
	/*int i = 1 / 0;*/
	printk("aaaaa");
	while (1) {
	};
}
