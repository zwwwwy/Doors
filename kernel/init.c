#include "init.h"
#include "../lib/string.h"
#include "info.h"
#include "mmu.h"
#include "printk.h"
#include "trap.h"

display_struct	  display_info;
buffer_struck	  buffer_info;
memory_descriptor mmu_struct;
memory_info		  memory_info_struct;
char			  print_buffer[4096];

void init_display()
{
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

void init_buffer()
{
	char* ptr = print_buffer;

	buffer_info.init_ptr	= (void*)ptr;
	buffer_info.current_ptr = (void*)ptr;
	for (int i = 0; i < 4096; ++i)
	{
		*ptr = 0;
		++ptr;
	}
	buffer_info.limit = 4096;
}

void init_trap()
{
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

void init_memory()
{
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

	unsigned long mem_sum		 = 0;
	unsigned long page_sum		 = 0;
	unsigned long ram_zone_count = 0;
	unsigned long mem_start;
	for (int i = 0; i < 32; ++i)
	{
		if (memory_info_ptr->type == 1)
		{
			++ram_zone_count;
		}
		if (memory_info_ptr->type == 0)
		{
			break;
		}

		// 将0x90200处的内存信息转存至内核
		mmu_struct.memory_info_array[i].addr = memory_info_ptr->addr;
		mmu_struct.memory_info_array[i].len	 = memory_info_ptr->len;
		mmu_struct.memory_info_array[i].type = memory_info_ptr->type;
		mmu_struct.memory_info_length		 = i + 1;

		printk("address:%lx->%lx, len:%ld, type:%d\n", memory_info_ptr->addr,
			   memory_info_ptr->addr + memory_info_ptr->len, memory_info_ptr->len, memory_info_ptr->type);

		mem_sum += memory_info_ptr->len;
		mem_start = ALIGN_PAGE(memory_info_ptr->addr);
		page_sum += (memory_info_ptr->addr + memory_info_ptr->len - mem_start) >> BITS_OF_OFFSET;
		++memory_info_ptr;
	}

	// 分配位图空间
	mmu_struct.bits_map_array = (unsigned long*)(ALIGN_PAGE_4k(mmu_struct.end_brk));
	mmu_struct.bits_size	  = mem_sum >> BITS_OF_OFFSET;
	mmu_struct.bits_length =
		(((unsigned long)(mem_sum >> BITS_OF_OFFSET) + sizeof(long) * 8 - 1) / 8) & (~(sizeof(long) - 1));
	memset(mmu_struct.bits_map_array, 0xff, mmu_struct.bits_length);

	// 分配页表空间
	mmu_struct.pages_array =
		(page_struct*)(ALIGN_PAGE_4k((unsigned long)mmu_struct.bits_map_array + mmu_struct.bits_length));
	mmu_struct.pages_size = mem_sum >> BITS_OF_OFFSET;
	mmu_struct.pages_length =
		((mem_sum >> BITS_OF_OFFSET) * sizeof(page_struct) + sizeof(long) - 1) & (~(sizeof(long) - 1));
	memset(mmu_struct.pages_array, 0, mmu_struct.pages_length);

	// 分配区域空间
	mmu_struct.zones_array =
		(zone_struct*)(ALIGN_PAGE_4k((unsigned long)mmu_struct.pages_array + mmu_struct.pages_length));
	mmu_struct.zones_size	= 0; // 同时用作下方初始化zone时的数组下标
	mmu_struct.zones_length = (ram_zone_count * sizeof(zone_struct) + sizeof(long) - 1) & (~(sizeof(long) - 1));
	memset(mmu_struct.zones_array, 0, mmu_struct.zones_length);

	for (int i = 0; i < mmu_struct.memory_info_length; ++i)
	{
		if (mmu_struct.memory_info_array[i].type != 1)
		{
			continue;
		}

		unsigned long start = ALIGN_PAGE(mmu_struct.memory_info_array[i].addr);
		unsigned long end	= (start + mmu_struct.memory_info_array[i].len) & PAGE_ADDR_MASK;
		// 实际来看第一个区域大小不够分一个大页
		if (start >= end)
		{
			continue;
		}

		// 初始化zone
		zone_struct* zone			= mmu_struct.zones_array + (mmu_struct.zones_size++);
		zone->memory_descriptor_ptr = &mmu_struct;
		zone->start_addr			= start;
		zone->end_addr				= end;
		zone->zone_length			= end - start;
		zone->attr					= 0;
		zone->page_using_count		= 0;
		zone->page_free_count		= zone->zone_length >> BITS_OF_OFFSET;
		zone->page_ref_count_sum	= 0;
		zone->pages_array			= (page_struct*)(mmu_struct.pages_array + (start >> BITS_OF_OFFSET));
		zone->pages_length			= zone->page_free_count;

		// 初始化page并复位其在位图中的位置
		page_struct* page = zone->pages_array;
		for (int j = 0; j < zone->pages_length; ++j, ++page)
		{
			page->zone_struct_ptr = zone;
			page->addr_phy		  = start + PAGE_OFFSET_SIZE * j;
			page->attr			  = 0;
			page->reference_count = 0;
			page->age			  = 0;
			UNMARK_BITS_MAP(mmu_struct.bits_map_array, page->addr_phy);
		}
	}

	// 初始化前2兆
	mmu_struct.pages_array[0].zone_struct_ptr = &mmu_struct.zones_array[0];
	mmu_struct.pages_array[0].addr_phy		  = 0;
	mmu_struct.pages_array[0].attr			  = 0;
	mmu_struct.pages_array[0].reference_count = 0;
	mmu_struct.pages_array[0].age			  = 0;

	// printk("\nbit map:%lx, bit length=%ld, bit size=%ld\n", mmu_struct.bits_map_array, mmu_struct.bits_length,
	//        mmu_struct.bits_size);
	// printk("page array:%lx, page length:%ld, page size:%ld\n", mmu_struct.pages_array, mmu_struct.pages_length,
	//        mmu_struct.pages_size);
	// printk("zone array:%lx, zone length:%ld, zone size:%ld\n", mmu_struct.zones_array, mmu_struct.zones_length,
	//        mmu_struct.zones_size);

	printk("\n");
	for (int i = 0; i < mmu_struct.zones_size; ++i)
	{
		zone_struct zone = mmu_struct.zones_array[i];
		for (int j = 0; j < zone.pages_length; ++j)
		{
			page_struct page = zone.pages_array[j];
			printk("page addr:%lx\n", page.addr_phy);
		}
	}
}
