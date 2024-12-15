#include "info.h"
#include "init.h"
#include "memory.h"
#include "printk.h"

extern buffer_struck	 buffer_info;
extern display_struct	 display_info;
extern memory_descriptor mmu_struct;

__attribute__((naked)) void setup_kernel()
{
	__asm__ __volatile__("movq  %0, %%rsp\n\t"
						 "movw  %1, %%ax\n\t"
						 "movw  %%ax, %%ss\n\t"
						 "jmp	kernel_start\n\t"
						 :
						 : "i"(0xffff800000010000), "i"(0x0010)
						 : "ax");
}

void kernel_start(void)
{
	init_display();
	init_buffer();
	init_trap();
	init_memory();

	page_struct* p = alloc_pages(10, PAGE_KERNEL_INIT);
	clean_page(p + 5);
	page_struct* p1 = alloc_a_page(PAGE_KERNEL_INIT);
	page_struct* p2 = alloc_a_page(PAGE_KERNEL_INIT);

	printk("p1.addr:%lx\n", p->addr_phy);
	printk("p2.addr:%lx\n", p1->addr_phy);
	printk("p3.addr:%lx\n", p2->addr_phy);

	while (1)
		;
}
