#include "init.h"
#include "printk.h"

#include "info.h"
extern buffer_struck buffer_info;

__attribute__((naked)) void setup_kernel() {
	__asm__ volatile("movq  $0xffff800000007e00, %rsp\n\t"
					 "movq  $0x0010, %rax\n\t"
					 "movq  %rax, %ss\n\t"
					 "jmp kernel_start\n\t");
}

void kernel_start(void) {
	init_display();
	init_buffer();

	printk("switch to 64 bits, kernel loaded successfully. printk built successfully.\n");
	printk("current:%x\n", buffer_info.current_ptr);
	printk("num:%x\n", 4088);
	printk("num:%u\n", 4088);
	printk("num:%d\n", -4088);
	printk("current:%x\n", buffer_info.current_ptr);
	while (1) {
	};
}
