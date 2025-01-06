#ifndef C_MMU_H
#define C_MMU_H
#include "info.h"

/***************************************************
 * IA-32e下虚拟地址结构
 * |--------|--------|--------|---------|--------|
 *   47-39    38-30    29-21     20-12     11-0
 *   PML4    PAGE_1G  PAGE_2M   PAGE_4k   OFFSET
 *
 * 每级页表的项数:  2^9=512项
 * 页表项长度:      8B(64b)
 * 每级页表的大小:  512*8B=4K
 ***************************************************/

// 采用2M分页
#define BITS_OF_OFFSET 21 // offset区域的位数
#define PAGE_OFFSET_SIZE (1lu << BITS_OF_OFFSET)
#define PAGE_OFFSET_MASK (PAGE_OFFSET_SIZE - 1)
#define PAGE_ADDR_MASK (~PAGE_OFFSET_MASK)

// 将每页与2M对齐
#define ALIGN_PAGE(addr) (addr + PAGE_OFFSET_MASK) & PAGE_ADDR_MASK

// 基于info.h中的信息
#define MARK_BITS_MAP(bit_map, addr)                                                                                   \
	*(bit_map + (addr >> BITS_OF_OFFSET) / 64) |= 1ul << ((addr >> BITS_OF_OFFSET) % 64);
#define UNMARK_BITS_MAP(bit_map, addr)                                                                                 \
	*(bit_map + (addr >> BITS_OF_OFFSET) / 64) ^= 1ul << ((addr >> BITS_OF_OFFSET) % 64);
#define UNMARK_BITS_MAP_ABS(bit_map, addr)                                                                             \
	*(bit_map + (addr >> BITS_OF_OFFSET) / 64) &= ~(1ul << ((addr >> BITS_OF_OFFSET) % 64));

#define VIRT2PHY(addr) ((unsigned long)(addr) - 0xffff800000000000)
#define PHY2VIRT(addr) ((unsigned long*)((unsigned long)addr + 0xffff800000000000))

#define BITS_OF_OFFSET_4k 12
#define PAGE_OFFSET_SIZE_4k (1lu << BITS_OF_OFFSET_4k)
#define PAGE_OFFSET_MASK_4k (PAGE_OFFSET_SIZE_4k - 1)
#define PAGE_ADDR_MASK_4k (~PAGE_OFFSET_MASK_4k)
#define ALIGN_PAGE_4k(addr) (addr + PAGE_OFFSET_MASK_4k) & PAGE_ADDR_MASK_4k

// 区域属性
#define ZONE_DMA 1
#define ZONE_NORMAL (1 << 1)
#define ZONE_UNMAPED (1 << 2)

/*
 * 页属性
 * PAGE_PTABLE_MAPED:	经过页表映射过的页
 * PAGE_KERNEL_INIT:	内核初始化程序
 * PAGE_ACTIVE:			使用中的页
 * PAGE_KERNEL: 		内核层页
 */
#define PAGE_PTABLE_MAPED 1
#define PAGE_KERNEL_INIT (1 << 1)
#define PAGE_REFERENCED (1 << 2)
#define PAGE_DIRTY (1 << 3)
#define PAGE_ACTIVE (1 << 4)
#define PAGE_UP_TO_DATE (1 << 5)
#define PAGE_DEVICE (1 << 6)
#define PAGE_KERNEL (1 << 7)
#define PAGE_K_SHARE_TO_U (1 << 8)
#define PAGE_SLAB (1 << 9)

unsigned long init_page(page_struct* page, unsigned long attr);
unsigned long clean_page(page_struct* page);
page_struct*  alloc_a_page(unsigned long page_attr);
// num<=64
page_struct* alloc_pages(unsigned int num, unsigned long page_attr);

typedef struct
{
	unsigned long pml4t;
} pml4t_t;
#define mk_mpl4t(addr, attr) ((unsigned long)(addr) | (unsigned long)(attr))
#define set_mpl4t(mpl4tptr, mpl4tval) (*(mpl4tptr) = (mpl4tval))

typedef struct
{
	unsigned long pdpt;
} pdpt_t;
#define mk_pdpt(addr, attr) ((unsigned long)(addr) | (unsigned long)(attr))
#define set_pdpt(pdptptr, pdptval) (*(pdptptr) = (pdptval))

typedef struct
{
	unsigned long pdt;
} pdt_t;
#define mk_pdt(addr, attr) ((unsigned long)(addr) | (unsigned long)(attr))
#define set_pdt(pdtptr, pdtval) (*(pdtptr) = (pdtval))

typedef struct
{
	unsigned long pt;
} pt_t;
#define mk_pt(addr, attr) ((unsigned long)(addr) | (unsigned long)(attr))
#define set_pt(ptptr, ptval) (*(ptptr) = (ptval))

#endif
