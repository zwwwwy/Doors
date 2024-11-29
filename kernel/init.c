#include "init.h"
#include "info.h"
display_struct display_info;
buffer_struck  buffer_info;

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
