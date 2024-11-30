#include "trap.h"
#include "info.h"
#include "init.h"
#include "printk.h"

extern display_struct display_info;

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
	__asm__ __volatile__("movq 	192(%%rsp),%0\n\t" : "=r"(pc_addr) :);
	blue_screen();
	printk_color("[WARNING]", ORANGE, BLUE_SCREEN);
	printk_color("An unknown interrupt or system exception occurred. current PC=%lx\n", WHITE,
				 BLUE_SCREEN, pc_addr);
	while (1)
		;
}

void blue_screen() {
	int* display_ptr = display_info.init_cur_pos;
	for (int i = 0; i < display_info.screen_width * display_info.screen_height; ++i) {
		*display_ptr = BLUE_SCREEN;
		++display_ptr;
	}
	init_display();
	printk_color(
		":-) Opps, your pc ran into a problem and needs to restart.\n\nHAHA, it's blue "
		"screen. I will not going to get any infomation, take care of yourself.\n\nDetails:\n",
		WHITE, BLUE_SCREEN);
}
