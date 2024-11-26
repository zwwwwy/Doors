#include "display_info.h"
display_struct display_info;

void init_display() {
	display_info.screen_width  = 1440;
	display_info.screen_height = 900;
	display_info.row		   = 0;
	display_info.col		   = 0;
	display_info.init_cur_pos  = (int*)0xa00000;
}
