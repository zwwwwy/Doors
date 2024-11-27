#include "init.h"
#include "info.h"
display_struct display_info;
buffer_struck  buffer_info;

void init_display() {
	display_info.screen_width  = 1440;
	display_info.screen_height = 900;
	display_info.row		   = 0;
	display_info.col		   = 0;
	display_info.init_cur_pos  = (int*)0xa00000;
}

void init_buffer() {
	char* ptr = (char*)0x10000;

	buffer_info.init_ptr	= (void*)ptr;
	buffer_info.current_ptr = (void*)ptr;
	for (int i = 0; i < 4096; ++i) {
		*ptr = 0;
		++ptr;
	}
	buffer_info.limit = 4096;
}
