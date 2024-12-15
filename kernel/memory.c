#include "memory.h"
#include "info.h"
#include "printk.h"
#include <stddef.h>
extern memory_descriptor mmu_struct;

unsigned long init_page(page_struct* page, unsigned long attr)
{
	if (page->attr == 0)
	{
		--page->zone_struct_ptr->page_free_count;
		++page->zone_struct_ptr->page_ref_count_sum;
		++page->zone_struct_ptr->page_using_count;
		page->attr = attr;
		++page->reference_count;
		MARK_BITS_MAP(mmu_struct.bits_map_array, page->addr_phy);
	}
	else if ((page->attr & PAGE_REFERENCED) || (page->attr & PAGE_K_SHARE_TO_U) || (attr & PAGE_REFERENCED) ||
			 (attr % PAGE_K_SHARE_TO_U))
	{
		page->attr |= attr;
		++page->reference_count;
		++page->zone_struct_ptr->page_ref_count_sum;
	}
	else
	{
		page->attr |= attr;
		MARK_BITS_MAP(mmu_struct.bits_map_array, page->addr_phy);
	}
	return 0;
}

unsigned long clean_page(page_struct* page)
{
	if (page->attr == 0)
	{
		return 0;
	}
	if ((page->attr & PAGE_REFERENCED) || (page->attr & PAGE_K_SHARE_TO_U))
	{
		--page->reference_count;
		--page->zone_struct_ptr->page_ref_count_sum;
		if (page->reference_count == 0)
		{
			page->attr = 0;
			++page->zone_struct_ptr->page_free_count;
			--page->zone_struct_ptr->page_using_count;
			UNMARK_BITS_MAP_ABS(mmu_struct.bits_map_array, page->addr_phy);
		}
	}
	else
	{
		page->attr			  = 0;
		page->reference_count = 0;
		++page->zone_struct_ptr->page_free_count;
		--page->zone_struct_ptr->page_ref_count_sum;
		--page->zone_struct_ptr->page_using_count;
		UNMARK_BITS_MAP_ABS(mmu_struct.bits_map_array, page->addr_phy);
	}
	return 0;
}

page_struct* alloc_a_page(unsigned long page_attr)
{
	zone_struct*  zone;
	unsigned long idx;
	unsigned long offset;
	unsigned long bit_map;
	unsigned long k;
	for (int i = 0; i < mmu_struct.zones_size; ++i)
	{
		zone = mmu_struct.zones_array + i;
		if (zone->page_free_count < 1)
			continue;
		idx		= ((zone->pages_array->addr_phy) >> BITS_OF_OFFSET) / 64;
		offset	= ((zone->pages_array->addr_phy) >> BITS_OF_OFFSET) % 64;
		bit_map = *(mmu_struct.bits_map_array + idx);
		k		= 0; // 用于统计当前区域的绝对页号，假设页在区域的分布是线性的
		while ((1ul << offset) & bit_map)
		{
			if (offset == 63)
			{
				bit_map = *(mmu_struct.bits_map_array + (++idx));
				offset	= 0;
				++k;
			}
			else
			{
				++offset;
				++k;
			}
		}

		page_struct* page = zone->pages_array + k;
		page->addr_phy	  = (idx * 64 + offset) << BITS_OF_OFFSET;
		init_page(page, page_attr);
		return page;
	}

	return NULL;
}

page_struct* alloc_pages(unsigned int num, unsigned long page_attr)
{
	unsigned long mask = 0;
	zone_struct*  zone;
	unsigned long idx;
	unsigned long offset;
	unsigned long bit_map;
	unsigned long k;
	for (int i = 0; i < num; ++i)
	{
		mask |= 1;
		mask <<= 1;
	}
	for (int i = 0; i < mmu_struct.zones_size; ++i)
	{
		zone = mmu_struct.zones_array + i;
		if (zone->page_free_count < 1)
			continue;
		idx		= ((zone->pages_array->addr_phy) >> BITS_OF_OFFSET) / 64;
		offset	= ((zone->pages_array->addr_phy) >> BITS_OF_OFFSET) % 64;
		bit_map = *(mmu_struct.bits_map_array + idx);
		k		= 0;
		while (((mask >> (64 - offset)) | (mask << offset)) & bit_map)
		{
			if (offset == 63)
			{
				bit_map = *(mmu_struct.bits_map_array + (++idx));
				offset	= 0;
				++k;
			}
			else
			{
				++offset;
				++k;
			}
		}
		page_struct* pages = zone->pages_array + k;
		for (int i = 0; i < num; ++i)
		{
			(pages + i)->addr_phy = (idx * 64 + offset + i) << BITS_OF_OFFSET;
			init_page(pages + i, page_attr);
		}
		return pages;
	}
	return NULL;
}
