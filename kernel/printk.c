#include "printk.h"
#include "ascii_array.h"
#include "display_info.h"

extern display_struct display_info;

void putchar(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width,
			 char char_heigth) {
	putchar_ascii(ascii_code, x, y, FR_color, BK_color, char_width, char_heigth);
	return;
}

void printk(char* format) {
	printk_ascii(format);
	return;
}

void putchar_ascii(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width,
				   char char_heigth) {
	// 光标应当指向下个字符的左上角
	int	 col_offset;
	int* cur_ptr =
		display_info.init_cur_pos + y * char_heigth * display_info.screen_width + x * char_width;

	for (int i = 0; i < char_heigth; i++) {
		col_offset = 1 << (char_width - 1);
		for (int j = 0; j < char_width; j++) {
			if (ascii[ascii_code][i] & (col_offset >> j)) {
				*(cur_ptr + j) = FR_color;
			} else {
				*(cur_ptr + j) = BK_color;
			}
		}
		cur_ptr += display_info.screen_width;
	}
}

void printk_ascii(char* format) {
	int	 FR_color	 = 0x00ffffff;
	int	 BK_color	 = 0x00000000;
	char char_width	 = 8;
	char char_heigth = 16;

	int x = display_info.col;
	int y = display_info.row;

	for (char* p = format; *p; p++) {
		if (*p == 0x08) { // 退格
			x--;
		} else if (*p == 0x09) { // 横向指标
			x += 4 - (x % 4);
		} else if (*p == 0x0a) { // 换行
			x = 0;
			y++;
		} else if (*p == 0x25) {	// %
			if (*(p + 1) == 0x64) { // d

			} else {
				putchar_ascii(*p, x, y, FR_color, BK_color, char_width, char_heigth);
				x++;
			}
		} else {
			putchar_ascii(*p, x, y, FR_color, BK_color, char_width, char_heigth);
			x++;
		}
	}
	display_info.col = x;
	display_info.row = y;
}
