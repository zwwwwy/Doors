#ifndef C_DISPLAYINFO_H
#define C_DISPLAYINFO_H

#define RED 0x00ff0000
#define GREEN 0x0000ff00
#define BLUE 0x000000ff
#define YELLOW 0x00ffff00
#define PURPLE 0x00ff00ff
#define WHITE 0x00ffffff
#define BLACK 0x00000000
#define ORANGE 0x00ea925e
#define BLUE_SCREEN 0x000078d7
#define RED_C 0x00ce4f55
#define GREEN_C 0x0031ad6f
#define BLUE_C 0x000072b5

// 用于描述显存区及一些显示信息
typedef struct {
	unsigned int screen_width;
	unsigned int screen_height;
	unsigned int row;
	unsigned int col;
	unsigned int BitsPerPixel;
	unsigned int char_width;
	unsigned int char_height;
	unsigned int max_row;
	unsigned int max_col;
	int*		 init_cur_pos;
} display_struct;

// 用于描述内核io缓冲区的情况
typedef struct {
	void*		 init_ptr;
	void*		 current_ptr;
	unsigned int limit;
} buffer_struck;

// 用于获取0x90200处存放的在loader中获取的内存信息
typedef struct __attribute__((packed)) memory_info {
	unsigned long addr;
	unsigned long len;
	unsigned int  type;
} memory_info;

// 单一页面描述符
typedef struct page_struct {
	struct zone_struct* zone_struct_ptr;
	unsigned long		addr_phy;
	unsigned long		attr;
	unsigned long		reference_count;
	unsigned long		age;
} page_struct;

// 单一区域描述符
typedef struct zone_struct {
	struct memory_descriptor* memory_descriptor_ptr;

	struct page_struct* pages_array;  // 区域页面描述符数组
	unsigned long		pages_length; // 区域页面描述符数组长度

	unsigned long start_addr;  // 区域起始地址
	unsigned long end_addr;	   // 区域终止地址
	unsigned long zone_length; // 区域长度
	unsigned long attr;

	unsigned long page_using_count;	  // 区域已使用物理页数
	unsigned long page_free_count;	  // 区域空闲物理页数
	unsigned long page_ref_count_sum; // 区域物理页被引用次数
} zone_struct;

typedef struct memory_descriptor {
	struct memory_info* memory_info_array;
	unsigned long		memory_info_length;

	unsigned long* bits_map_array;
	unsigned long  bits_length; // 字节大小
	unsigned long  bits_size;	// 数量大小

	struct page_struct* pages_array;
	unsigned long		pages_length;
	unsigned long		pages_size;

	struct zone_struct* zones_array;
	unsigned long		zones_length;
	unsigned long		zones_size;

	unsigned long start_code;	 // 内核程序起始代码段地址
	unsigned long end_code;		 // 内核程序结束代码段地址
	unsigned long end_data;		 // 内核程序结束数据段地址
	unsigned long end_brk;		 // 内核程序结束地址
	unsigned long end_of_struct; // 内存页管理结构的结束地址
} memory_descriptor;

#endif
