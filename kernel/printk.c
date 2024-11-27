#include "printk.h"
#include "ascii_array.h"
#include "info.h"
#include <stdarg.h>

extern display_struct display_info;
extern buffer_struck  buffer_info;

// for的初始化减完length还要减1是因为vsprintfk返回纯字符数量，不含结尾处的0。
#define printk_process()                                                                           \
	for (char* p = (char*)buffer_info.current_ptr - length - 1; *p; ++p) {                         \
		if (*p == '\b') {                                                                          \
			--x;                                                                                   \
			length -= 2;                                                                           \
		} else if (*p == '\t') {                                                                   \
			x += 4 - (x % 4);                                                                      \
			--length;                                                                              \
		} else if (*p == '\n') {                                                                   \
			x = 0;                                                                                 \
			++y;                                                                                   \
			--length;                                                                              \
		} else {                                                                                   \
			putchar(*p, x, y, FR_color, BK_color, char_width, char_heigth);                        \
			++x;                                                                                   \
		}                                                                                          \
	}                                                                                              \
	display_info.col = x;                                                                          \
	display_info.row = y;

void putchar(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width,
			 char char_heigth) {
	// 光标应当指向下个字符的左上角
	int	 col_offset;
	int* cur_ptr =
		display_info.init_cur_pos + y * char_heigth * display_info.screen_width + x * char_width;

	for (int i = 0; i < char_heigth; ++i) {
		col_offset = 1 << (char_width - 1);
		for (int j = 0; j < char_width; ++j) {
			if (ascii[ascii_code][i] & (col_offset >> j)) {
				*(cur_ptr + j) = FR_color;
			} else {
				*(cur_ptr + j) = BK_color;
			}
		}
		cur_ptr += display_info.screen_width;
	}
}

int vsprintfk(char* format, va_list args) {
	// 返回格式化后的纯字符数量(含\b,\n,\t等，但不含结尾的0)
	unsigned num_u;
	char	 num_char;
	// 有内存越界风险，不过缓冲区的后面应该没什么东西，暂时不考虑
	if (buffer_info.current_ptr - buffer_info.init_ptr <= 0) {
		clear_buffer();
	}
	char* buffer_ptr = (char*)buffer_info.current_ptr;
	char* start		 = buffer_ptr;
	char* fmt_s_ptr;

	for (int i = 0; format[i]; i++) {
		if (format[i] == '%') {

			char* ptr_brfore_format = buffer_ptr;
			switch (format[i + 1]) {

			case 'u':
				num_u = va_arg(args, unsigned);
				do {
					num_char		= num_u % 10;
					num_u			= num_u / 10;
					*(buffer_ptr++) = num_char + 0x30;
				} while (num_u);
				break;

			case 'i':
			case 'd':
				num_u = va_arg(args, unsigned);
				if (num_u & (1 << 31)) {
					num_u			  = (~num_u) + 1;
					*(buffer_ptr++)	  = '-';
					ptr_brfore_format = buffer_ptr; // 符号不能随数字部分倒序
				}
				do {
					num_char		= num_u % 10;
					num_u			= num_u / 10;
					*(buffer_ptr++) = num_char + 0x30;
				} while (num_u);
				break;

			case 'x':
				num_u = va_arg(args, unsigned);

				*(buffer_ptr++)	  = '0';
				*(buffer_ptr++)	  = 'x';
				ptr_brfore_format = buffer_ptr; // 符号不能随数字部分倒序
				do {
					num_char = num_u % 16;
					num_u	 = num_u / 16;
					if (num_char < 10) {
						num_char += 0x30;
					} else {
						num_char += 87;
					}
					*(buffer_ptr++) = num_char;

				} while (num_u);
				break;

			case 'b':
				num_u = va_arg(args, unsigned);

				*(buffer_ptr++)	  = '0';
				*(buffer_ptr++)	  = 'b';
				ptr_brfore_format = buffer_ptr; // 符号不能随数字部分倒序
				do {
					num_char		= num_u % 2;
					num_u			= num_u / 2;
					*(buffer_ptr++) = num_char + 0x30;

				} while (num_u);
				break;

			case 'o':
				num_u = va_arg(args, unsigned);

				*(buffer_ptr++)	  = '0';
				*(buffer_ptr++)	  = 'o';
				ptr_brfore_format = buffer_ptr; // 符号不能随数字部分倒序
				do {
					num_char		= num_u % 8;
					num_u			= num_u / 8;
					*(buffer_ptr++) = num_char + 0x30;

				} while (num_u);
				break;

			case 'c':
				num_char		= (char)va_arg(args, int);
				*(buffer_ptr++) = num_char;
				break;

			case 's':
				fmt_s_ptr = va_arg(args, char*);
				for (; *fmt_s_ptr; ++fmt_s_ptr) {
					*(buffer_ptr++) = *fmt_s_ptr;
				}
				ptr_brfore_format = buffer_ptr; // 字符串不倒序
				break;

			default:
				*(buffer_ptr++) = format[i];
				--i;
			}
			++i;
			char* ptr_end_format = buffer_ptr - 1;

			// 将数字部分倒序
			while (ptr_brfore_format < ptr_end_format) {
				char tmp			   = *ptr_end_format;
				*(ptr_end_format--)	   = *ptr_brfore_format;
				*(ptr_brfore_format++) = tmp;
			}
		} else {
			*(buffer_ptr++) = format[i];
		}
	}

	*(buffer_ptr++) = 0;
	va_end(args);
	buffer_info.current_ptr = buffer_ptr;
	return buffer_ptr - start - 1;
}

int printk(char* format, ...) {
	int	 FR_color	 = 0x00ffffff;
	int	 BK_color	 = 0x00000000;
	char char_width	 = 8;
	char char_heigth = 16;

	int x = display_info.col;
	int y = display_info.row;

	va_list args;
	va_start(args, format);
	int length = vsprintfk(format, args);

	printk_process();

	return length;
}

int printk_color(char* format, int FR_color, int BK_color, ...) {
	char char_width	 = 8;
	char char_heigth = 16;

	int x = display_info.col;
	int y = display_info.row;

	va_list args;
	va_start(args, BK_color);
	int length = vsprintfk(format, args);

	printk_process();

	return length;
}

void clear_buffer() {
	char* ptr = (char*)buffer_info.init_ptr;
	for (int i = 0; i < buffer_info.limit; ++i) {
		*(ptr++) = 0;
	}
	buffer_info.current_ptr = buffer_info.init_ptr;
}

void clear_screen() {
	int* ptr = display_info.init_cur_pos;
	for (int i = 0; i < display_info.screen_height * display_info.screen_width; ++i) {
		*(ptr++) = 0;
	}
	display_info.row = 0;
	display_info.col = 0;
}
