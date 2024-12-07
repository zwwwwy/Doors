#ifndef C_MMU_H
#define C_MMU_H

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
#define ALIGN_PAGE(addr) (addr + PAGE_OFFSET_SIZE - 1) & PAGE_ADDR_MASK

#endif
