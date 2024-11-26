#include "init.h"
#include "printk.h"

__attribute__((naked)) void setup_kernel() {
	__asm__ volatile("movq  $0xffff800000007e00, %rsp\n\t"
					 "movq  $0x0010, %rax\n\t"
					 "movq  %rax, %ss\n\t"
					 "jmp kernel_start\n\t");
}

void kernel_start(void) {
	init_display();

	printk("switch to 64 bits, kernel loaded successfully. printk built successfully.\n");
	while (1) {
	};
}
