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
		"screen. I am not going to get any infomation, take care of yourself.\n\nDetails:\n",
		WHITE, BLUE_SCREEN);
}

void set_gate(idt_element* idt_element_addr, unsigned char ist, unsigned short attr,
			  void* func_addr) {
	__asm__ __volatile__(
		"movq 	%%rax, %%rdi\n\t" // idt地址->rdi
		"xorq	%%rax, %%rax\n\t"
		"movl 	$0x80000,%%eax\n\t" // 段选择子
		"movw 	%%dx, %%ax\n\t"
		"andq 	$0xffffffffffff0000, %%rdx\n\t"
		"movw 	%0, %%dx\n\t" // ist复位，使用保护模式的栈切换模式，若开启需要和dx的后三位或一下
		"orw 	%1, %%dx\n\t"
		"movq 	%%rdx, %%rcx\n\t"
		"shl 	$32, %%rcx\n\t"
		"xorq 	%%rcx, %%rax\n\t"
		"shr 	$32, %%rdx\n\t"
		"movq 	%%rax, (%%rdi)\n\t"
		"movq 	%%rdx, 8(%%rdi)\n\t"

		::"r"((short)(attr << 8)),
		"r"((short)ist), "a"(idt_element_addr), "d"(func_addr));
}

__attribute__((naked)) void save_reg() {
	__asm__ __volatile__("pushq		%rax\n\t"
						 "movq 		%es, %rax\n\t"
						 "pushq		%rax\n\t"
						 "movq 		%ds, %rax\n\t"
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
						 "movq 	$0x10, %rax\n\t"
						 "movq  %rax, %ds\n\t"
						 "movq  %rax, %es\n\t"
						 "callq *%rdx\n\t"
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

// 0 #DE
__attribute__((naked)) void divide_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	divide_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void divide_fault() {
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

// 1 #DB
__attribute__((naked)) void debug_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	debug_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void debug_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[INT]", WHITE, BLACK);
	printk_color("Debug interrupt. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK, error_code,
				 *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 2 不可屏蔽中断
__attribute__((naked)) void nmi_int_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	nmi_int(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void nmi_int() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[INT]", WHITE, BLACK);
	printk_color("Non-maskable interrupt occurred. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 3 #BP
__attribute__((naked)) void breakPoint_trap_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	breakPoint_trap(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void breakPoint_trap() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[INT]", WHITE, BLACK);
	printk_color("Step into a break point. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 4 #OF
__attribute__((naked)) void overFlow_trap_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	overFlow_trap(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void overFlow_trap() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[WARNING]", ORANGE, BLACK);
	printk_color("Arithmetic overflow. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK, error_code,
				 *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 5 #BR
__attribute__((naked)) void boundsCheck_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	boundsCheck_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void boundsCheck_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[WARNING]", ORANGE, BLACK);
	printk_color("Array bounds or index out of range. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 6 #UD
__attribute__((naked)) void invalidOpcode_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	invalidOpcode_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void invalidOpcode_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Invlid or unknown instruction. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 7 #DF
__attribute__((naked)) void deviceNotAvailable_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	deviceNotAvailable_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void deviceNotAvailable_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Co-processor or device unavailable. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 8 #DF
__attribute__((naked)) void doubleFault_abort_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	doubleFault_abort(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void doubleFault_abort() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color(
		"Exception triggered while handling another exception. error_code=%ld, IP=%lx, SP=%lx",
		WHITE, BLACK, error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 9 协处理器段超越，386以后的cpu不产生该异常
__attribute__((naked)) void coprocessorSegmentOverrun_fault_handler() {
	__asm__ __volatile__("pushq $0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	coprocessorSegmentOverrun_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void coprocessorSegmentOverrun_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[WARNING]", ORANGE, BLACK);
	printk_color("Access beyond coprocess's segment limit. error_code=%ld, IP=%lx, SP=%lx", WHITE,
				 BLACK, error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 10 #TS
__attribute__((naked)) void invalidTSS_fault_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	invalidTSS_fault(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void invalidTSS_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Invlid or corrupted TSS. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 11 #NP
__attribute__((naked)) void segmentNotPresent_fault_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	segmentNotPresent_fault(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void segmentNotPresent_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Access to a non-existent segment. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 12 #SS
__attribute__((naked)) void stackSegmentFault_fault_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	stackSegmentFault_fault(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void stackSegmentFault_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Error in stack segment access. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 13 #GP
__attribute__((naked)) void generalProtectionFault_fault_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	generalProtectionFault_fault(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void generalProtectionFault_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("General protection fault. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 14 #PF
__attribute__((naked)) void pageFault_fault_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	pageFault_fault(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void pageFault_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Page fault. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK, error_code,
				 *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 15 Inter保留
// 16 #MF
__attribute__((naked)) void floatPointError_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	floatPointError_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void floatPointError_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[WARNING]", ORANGE, BLACK);
	printk_color("Float point error. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK, error_code,
				 *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 17 #AC
__attribute__((naked)) void alignmentCheck_fault_handler() {
	__asm__ __volatile__(
		// 应压入错误码-error_code
		"pushq 	%rax\n\t"
		"leaq 	alignmentCheck_fault(%rip), %rax\n\t"
		"xchgq 	%rax, (%rsp)\n\t"
		"jmp 	save_reg\n\t");
}

void alignmentCheck_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[WARNING]", ORANGE, BLACK);
	printk_color("Misaligned memory access. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 18 #MC
__attribute__((naked)) void machineCheck_abort_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	machineCheck_abort(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void machineCheck_abort() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("Hardware error or malfunction. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK,
				 error_code, *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}

// 19 #XF
__attribute__((naked)) void SIMDFloatException_fault_handler() {
	__asm__ __volatile__("pushq 	$0\n\t"
						 "pushq 	%rax\n\t"
						 "leaq 	SIMDFloatException_fault(%rip), %rax\n\t"
						 "xchgq 	%rax, (%rsp)\n\t"
						 "jmp 	save_reg\n\t");
}

void SIMDFloatException_fault() {
	long rsp;
	long error_code;
	__asm__ __volatile__("movq 	%%rcx, %0\n\t"
						 "movq 	%%rsi, %1\n\t"
						 : "=r"(rsp), "=r"(error_code));

	printk_color("[ERROR]", RED, BLACK);
	printk_color("SIMD float exception. error_code=%ld, IP=%lx, SP=%lx", WHITE, BLACK, error_code,
				 *(long*)(rsp + 0x98), rsp);
	while (1)
		;
}
