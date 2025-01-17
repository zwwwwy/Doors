#include "disk.h"
#include "info.h"
#include "init.h"
#include "memory.h"
#include "pcb.h"
#include "printk.h"

extern buffer_struck	 buffer_info;
extern display_struct	 display_info;
extern memory_descriptor mmu_struct;

extern thread_struct init_thread;

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
	init_irq();
	init_disk_controller();
	init_pcb();
	init_tss_array();

	// for (int i = 0; i < 1000; ++i)
	// {
	//     printk("%d\n", i);
	// }

	int i = *(int*)0xffffffffffffffff;

	printk("zone addr:%lx\n", mmu_struct.zones_array);
	printk("zone length:%lx\n", mmu_struct.zones_length);
	printk("zone size:%lx\n", mmu_struct.zones_size);
	// read_sectors(0, 1, (unsigned short*)0x66666, 0);
	//
	printk("init_thread.fs:%d", init_thread.fs);

	while (1)
		;
}
