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
						 "movq 	%%rax, %%es\n\t" ::"i"(0x10));

	unsigned long ip_addr;
	__asm__ __volatile__("movq 	176(%%rsp),%0\n\t" : "=r"(ip_addr) :);
	blue_screen();
	printk_color("[WARNING]", ORANGE, BLUE_SCREEN);
	printk_color("An unknown interrupt or system exception occurred. current IP=%lx\n", WHITE,
				 BLUE_SCREEN, ip_addr);
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

void set_gate(idt_element* idt_element_addr, unsigned ist, unsigned short attr, void* func_addr) {
	__asm__ __volatile__("movq 	%%rax, %%rdi\n\t" // idt地址->rdi
						 "xorq	%%rax, %%rax\n\t"
						 "movl 	$0x80000,%%eax\n\t" // 段选择子
						 "movw 	%%dx, %%ax\n\t"
						 "andq 	$0xffffffffffff0000, %%rdx\n\t"
						 "movw 	%0, %%dx\n\t"
						 "movq 	%%rdx, %%rcx\n\t"
						 "shl 	$32, %%rcx\n\t"
						 "xorq 	%%rcx, %%rax\n\t"
						 "shr 	$32, %%rdx\n\t"
						 "movq 	%%rax, (%%rdi)\n\t"
						 "movq 	%%rdx, 8(%%rdi)\n\t"

						 ::"r"((short)(attr << 8)),
						 "a"(idt_element_addr), "d"(func_addr));
}

__attribute__((naked)) void save_reg() {
	__asm__ __volatile__("pushq	%rax\n\t"
						 "movq 	%es, %rax\n\t"
						 "pushq	%rax\n\t"
						 "movq 	%ds, %rax\n\t"
						 "pushq 	%rax\n\t"
						 "pushq 	%rbp\n\t"
						 "pushq 	%rdi\n\t"
						 "pushq 	%rsi\n\t"
						 "pushq 	%rdx\n\t"
						 "pushq 	%rcx\n\t"
						 "pushq 	%rbx\n\t"
						 "pushq 	%r8\n\t"
						 "pushq 	%r9\n\t"
						 "pushq 	%r10\n\t"
						 "pushq 	%r11\n\t"
						 "pushq 	%r12\n\t"
						 "pushq 	%r13\n\t"
						 "pushq 	%r14\n\t"
						 "pushq 	%r15\n\t"

						 "cld\n\t"
						 "movq 	0x90(%rsp), %rsi\n\t" // 错误码
						 "movq 	0x88(%rsp), %rdx\n\t" // 函数入口
						 "movq 	%rsp, %rcx\n\t"		  // rsp
						 "movq 	$0x10, %rdi\n\t"
						 "movq   %rdi, %ds\n\t"
						 "movq   %rdi, %es\n\t"
						 "callq 	*%rdx\n\t"
						 "jmp 	restore_reg");
}

__attribute__((naked)) void restore_reg() {
	__asm__ __volatile__(
		/*"add 	 $62,%rsp\n\t"*/
		"popq 	%r15\n\t"
		"popq 	%r14\n\t"
		"popq 	%r13\n\t"
		"popq 	%r12\n\t"
		"popq 	%r11\n\t"
		"popq 	%r10\n\t"
		"popq 	%r9\n\t"
		"popq 	%r8\n\t"
		"popq 	%rbx\n\t"
		"popq 	%rcx\n\t"
		"popq 	%rdx\n\t"
		"popq 	%rsi\n\t"
		"popq 	%rdi\n\t"
		"popq 	%rbp\n\t"
		"popq 	%rax\n\t"
		"movq 	%rax, %ds\n\t"
		"popq 	%rax\n\t"
		"movq 	%rax, %es\n\t"
		"popq 	%rax\n\t"
		"addq 	$0x10, %rsp\n\t"
		"iretq\n\t");
}

__attribute__((naked)) void divide_error_entry() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	divide_error(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void divide_error() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));
	blue_screen();
	printk_color("[ERROR]", PURE_RED, BLUE_SCREEN);
	printk_color("Division by zero. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLUE_SCREEN, error_code,
				 *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}
