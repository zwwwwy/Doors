#include "init.h"
#include "info.h"
#include "mmu.h"
#include "printk.h"
#include "trap.h"

display_struct	  display_info;
buffer_struck	  buffer_info;
memory_descriptor mmu_struct;
memory_info		  memory_info_struct;

void init_display() {
	unsigned short screen_width;
	unsigned short screen_height;
	unsigned char  bpp;

	__asm__ __volatile__("movw (0x90112), %0\n\t"
						 "movw (0x90114), %1\n\t"
						 "movb (0x90119), %2\n\t"
						 : "=r"(screen_width), "=r"(screen_height), "=r"(bpp)
						 :);
	display_info.screen_width  = screen_width;
	display_info.screen_height = screen_height;
	display_info.BitsPerPixel  = bpp;
	display_info.row		   = 0;
	display_info.col		   = 0;
	display_info.init_cur_pos  = (int*)0xffff800001000000;
	display_info.char_width	   = 8;
	display_info.char_height   = 16;
	display_info.max_row	   = screen_height / display_info.char_height;
	display_info.max_col	   = screen_width / display_info.char_width;
}

void init_buffer() {
	char* ptr = (char*)0xffff800000010000;

	buffer_info.init_ptr	= (void*)ptr;
	buffer_info.current_ptr = (void*)ptr;
	for (int i = 0; i < 4096; ++i) {
		*ptr = 0;
		++ptr;
	}
	buffer_info.limit = 4096;
}

void init_trap() {
	SET_INT_GATE_DPL_0(0, 0, divide_fault_handler);
	SET_INT_GATE_DPL_0(1, 0, debug_fault_handler);
	SET_INT_GATE_DPL_0(2, 0, nmi_int_handler);
	SET_INT_GATE_DPL_0(3, 0, breakPoint_trap_handler);
	SET_INT_GATE_DPL_0(4, 0, overFlow_trap_handler);
	SET_INT_GATE_DPL_0(5, 0, boundsCheck_fault_handler);
	SET_INT_GATE_DPL_0(6, 0, invalidOpcode_fault_handler);
	SET_INT_GATE_DPL_0(7, 0, deviceNotAvailable_fault_handler);
	SET_INT_GATE_DPL_0(8, 0, doubleFault_abort_handler);
	SET_INT_GATE_DPL_0(9, 0, coprocessorSegmentOverrun_fault_handler);
	SET_INT_GATE_DPL_0(10, 0, invalidTSS_fault_handler);
	SET_INT_GATE_DPL_0(11, 0, segmentNotPresent_fault_handler);
	SET_INT_GATE_DPL_0(12, 0, stackSegmentFault_fault_handler);
	SET_INT_GATE_DPL_0(13, 0, generalProtectionFault_fault_handler);
	SET_INT_GATE_DPL_0(14, 0, pageFault_fault_handler);
	// 15 Intel保留
	SET_INT_GATE_DPL_0(16, 0, floatPointError_fault_handler);
	SET_INT_GATE_DPL_0(17, 0, alignmentCheck_fault_handler);
	SET_INT_GATE_DPL_0(18, 0, machineCheck_abort_handler);
	SET_INT_GATE_DPL_0(19, 0, SIMDFloatException_fault_handler);
	// 20-31 Intel保留
	return;
}

void init_memory() {
	extern char _text;
	extern char _etext;
	extern char _edata;
	extern char _end;
	mmu_struct.start_code = (unsigned long)&_text;
	mmu_struct.end_code	  = (unsigned long)&_etext;
	mmu_struct.end_data	  = (unsigned long)&_edata;
	mmu_struct.end_brk	  = (unsigned long)&_end;
	// type=1为ram，type=2为rom
	memory_info* memory_info_ptr = (memory_info*)0xffff800000090200;
	mmu_struct.memory_info_array = &memory_info_struct;

	unsigned long mem_sum  = 0;
	unsigned long page_sum = 0;
	unsigned long mem_start;
	for (int i = 0; i < 32; ++i) {
		if (memory_info_ptr->type == 0) {
			break;
		}

		// 将0x90200处的内存信息转存至内核
		mmu_struct.memory_info_array[i].addr = memory_info_ptr->addr;
		mmu_struct.memory_info_array[i].len	 = memory_info_ptr->len;
		mmu_struct.memory_info_array[i].type = memory_info_ptr->type;
		mmu_struct.memory_info_length		 = i + 1;

		printk("address:%lx->%lx, len:%ld, type:%d\n", memory_info_ptr->addr,
			   memory_info_ptr->addr + memory_info_ptr->len, memory_info_ptr->len,
			   memory_info_ptr->type);

		// 计算RAM区域的帧总数
		if (memory_info_ptr->type == 1) {
			mem_sum += memory_info_ptr->len;
			mem_start = ALIGN_PAGE(memory_info_ptr->addr);
			page_sum +=
				(memory_info_ptr->addr + memory_info_ptr->len - mem_start) >> BITS_OF_OFFSET;
		}
	}
	printk("memory: %ld, pages: %ld\n", mem_sum, page_sum);

	printk("start_code:%lx\n", mmu_struct.start_code);
	printk("start_code:%lx\n", mmu_struct.end_code);
	printk("start_code:%lx\n", mmu_struct.end_data);
	printk("start_code:%lx\n", mmu_struct.end_brk);
}
