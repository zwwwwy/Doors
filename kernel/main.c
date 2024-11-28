#include "init.h"
#include "printk.h"

#include "info.h"
extern buffer_struck  buffer_info;
extern display_struct display_info;

__attribute__((naked)) void setup_kernel() {
	__asm__ __volatile__("movq  %0, %%rsp\n\t"
						 "movw  %1, %%ax\n\t"
						 "movw  %%ax, %%ss\n\t"
						 "jmp	kernel_start\n\t"
						 :
						 : "i"(0xffff800000007e00), "i"(0x0010)
						 : "ax");
}

void kernel_start(void) {
	init_display();
	init_buffer();
	for (int i = 0; i < 10; ++i) {
		printk_color("num:%o\n", 0x00ff0000, 0x0000ff00, i);
	}

	/*printk("switch to 64 bits, kernel loaded successfully. printk built successfully.\n");*/
	while (1) {
	};
}
