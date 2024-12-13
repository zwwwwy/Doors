#include "mmu.h"
#include "info.h"
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
