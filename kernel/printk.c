#include "printk.h"
#include "../lib/string.h"
#include "ascii_array.h"
#include "info.h"
#include <stdarg.h>

extern display_struct display_info;
extern buffer_struck  buffer_info;

// for的初始化减完length还要减1是因为vsprintfk返回纯字符数量，不含结尾处的0。
#define PRINTK_PROCESS()                                                                                               \
	for (char* p = (char*)buffer_info.current_ptr - length - 1; *p; ++p)                                               \
	{                                                                                                                  \
		if (*p == '\b')                                                                                                \
		{                                                                                                              \
			--x;                                                                                                       \
			length -= 2;                                                                                               \
		}                                                                                                              \
		else if (*p == '\t')                                                                                           \
		{                                                                                                              \
			x += 4 - (x % 4);                                                                                          \
			--length;                                                                                                  \
		}                                                                                                              \
		else if (*p == '\n')                                                                                           \
		{                                                                                                              \
			x = 0;                                                                                                     \
			++y;                                                                                                       \
			--length;                                                                                                  \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			if (x > display_info.max_col - 1)                                                                          \
			{                                                                                                          \
				x = 0;                                                                                                 \
				--y;                                                                                                   \
			}                                                                                                          \
			if (y > display_info.max_row - 1)                                                                          \
			{                                                                                                          \
				roll_up_line_clean(1);                                                                                 \
				--y;                                                                                                   \
			}                                                                                                          \
			putchark(*p, x, y, FR_color, BK_color, char_width, char_heigth);                                           \
			++x;                                                                                                       \
		}                                                                                                              \
	}                                                                                                                  \
	display_info.col = x;                                                                                              \
	display_info.row = y;

// 第二行因为数字部分不随符号倒序
#define SCALE_CHANGE_BELOW_DEC(scale)                                                                                  \
	ptr_brfore_format = buffer_ptr;                                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		*(buffer_ptr++) = char_lst[num_l % scale];                                                                     \
		num_l			= num_l / scale;                                                                               \
	} while (num_l);                                                                                                   \
	ptr_end_format = buffer_ptr - 1;                                                                                   \
	while (ptr_brfore_format < ptr_end_format)                                                                         \
	{                                                                                                                  \
		char tmp			   = *ptr_end_format;                                                                      \
		*(ptr_end_format--)	   = *ptr_brfore_format;                                                                   \
		*(ptr_brfore_format++) = tmp;                                                                                  \
	}

void putchark(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width, char char_heigth)
{
	// 光标应当指向下个字符的左上角
	int	 col_offset;
	int* cur_ptr = display_info.init_cur_pos + y * char_heigth * display_info.screen_width + x * char_width;

	for (int i = 0; i < char_heigth; ++i)
	{
		col_offset = 1 << (char_width - 1);
		for (int j = 0; j < char_width; ++j)
		{
			if (ascii[ascii_code][i] & (col_offset >> j))
			{
				*(cur_ptr + j) = FR_color;
			}
			else
			{
				*(cur_ptr + j) = BK_color;
			}
		}
		cur_ptr += display_info.screen_width;
	}
}

int vsprintfk(char* format, va_list args)
{
	// 返回格式化后的纯字符数量(含\b,\n,\t等，但不含结尾的0)
	unsigned long num_l;

	char* char_lst = "0123456789abcdef";
	// 有内存越界风险，不过缓冲区的后面应该没什么东西，暂时不考虑
	if (buffer_info.current_ptr - buffer_info.init_ptr <= buffer_info.limit)
	{
		clear_buffer();
	}
	char* buffer_ptr = (char*)buffer_info.current_ptr;
	char* start		 = buffer_ptr;
	char* fmt_s_ptr;

	for (int i = 0; format[i]; i++)
	{
		if (format[i] == '%')
		{

			char* ptr_brfore_format = buffer_ptr;
			char* ptr_end_format;
			switch (format[i + 1])
			{

			case 'u':
				num_l = (unsigned)va_arg(args, unsigned);
				SCALE_CHANGE_BELOW_DEC(10);
				break;

			case 'i':
			case 'd':
				num_l = (unsigned)va_arg(args, unsigned);
				if (num_l & (1 << 31))
				{
					num_l			= (~(unsigned)num_l) + 1;
					*(buffer_ptr++) = '-';
				}
				SCALE_CHANGE_BELOW_DEC(10)
				break;

			case 'x':
				num_l = (unsigned)va_arg(args, unsigned);

				*(buffer_ptr++) = '0';
				*(buffer_ptr++) = 'x';
				SCALE_CHANGE_BELOW_DEC(16);

				break;

			case 'b':
				num_l = (unsigned)va_arg(args, unsigned);

				*(buffer_ptr++) = '0';
				*(buffer_ptr++) = 'b';
				SCALE_CHANGE_BELOW_DEC(2);

				break;

			case 'o':
				num_l = (unsigned)va_arg(args, unsigned);

				*(buffer_ptr++) = '0';
				*(buffer_ptr++) = 'o';
				SCALE_CHANGE_BELOW_DEC(8);

				break;

			case 'l':
				num_l = va_arg(args, unsigned long);

				switch (format[i + 2])
				{
				case 'u':
					SCALE_CHANGE_BELOW_DEC(10);
					break;

				case 'i':
				case 'd':
					if (num_l & ((unsigned long)1 << 63))
					{
						num_l			= (~num_l) + 1;
						*(buffer_ptr++) = '-';
					}
					SCALE_CHANGE_BELOW_DEC(10);
					break;

				case 'x':
					*(buffer_ptr++) = '0';
					*(buffer_ptr++) = 'x';
					SCALE_CHANGE_BELOW_DEC(16);
					break;

				case 'b':
					*(buffer_ptr++) = '0';
					*(buffer_ptr++) = 'b';
					SCALE_CHANGE_BELOW_DEC(2);
					break;

				case 'o':
					*(buffer_ptr++) = '0';
					*(buffer_ptr++) = 'o';
					SCALE_CHANGE_BELOW_DEC(8);
					break;
				default:
					*(buffer_ptr++) = format[i];
					--i;
					break;
				}
				++i;

				break;

			case 'c':
				*(buffer_ptr++) = (char)va_arg(args, int);
				break;

			case 's':
				fmt_s_ptr = va_arg(args, char*);
				for (; *fmt_s_ptr; ++fmt_s_ptr)
				{
					*(buffer_ptr++) = *fmt_s_ptr;
				}
				break;

			default:
				*(buffer_ptr++) = format[i];
				--i;
				break;
			}
			++i;
		}
		else
		{
			*(buffer_ptr++) = format[i];
		}
	}

	*(buffer_ptr++) = 0;
	va_end(args);
	buffer_info.current_ptr = buffer_ptr;
	return buffer_ptr - start - 1;
}

int printk(char* format, ...)
{
	int	 FR_color	 = 0x00ffffff;
	int	 BK_color	 = 0x00000000;
	char char_width	 = display_info.char_width;
	char char_heigth = display_info.char_height;

	int x = display_info.col;
	int y = display_info.row;

	va_list args;
	va_start(args, format);
	int length = vsprintfk(format, args);

	PRINTK_PROCESS();

	return length;
}

int printk_color(char* format, int FR_color, int BK_color, ...)
{
	char char_width	 = display_info.char_width;
	char char_heigth = display_info.char_height;

	int x = display_info.col;
	int y = display_info.row;

	va_list args;
	va_start(args, BK_color);
	int length = vsprintfk(format, args);

	PRINTK_PROCESS();

	return length;
}

void clear_buffer()
{
	char* ptr = (char*)buffer_info.init_ptr;
	for (int i = 0; i < buffer_info.limit; ++i)
	{
		*(ptr++) = 0;
	}
	buffer_info.current_ptr = buffer_info.init_ptr;
}

void clear_screen()
{
	int* ptr = display_info.init_cur_pos;
	for (int i = 0; i < display_info.screen_height * display_info.screen_width; ++i)
	{
		*(ptr++) = 0;
	}
	display_info.row = 0;
	display_info.col = 0;
}

void roll_up_line_clean(int n)
{
	// 滚过的信息被清理，不被缓存
	int* init_ptr  = display_info.init_cur_pos;
	int* end_ptr   = init_ptr + display_info.screen_height * display_info.screen_width;
	int* start_ptr = init_ptr + n * display_info.screen_width * display_info.char_height;
	int	 count	   = end_ptr - start_ptr;

	__asm__ __volatile__("cld\n\t"
						 "rep movsl\n\t" ::"S"(start_ptr),
						 "D"(init_ptr), "c"(count));
	// 后n行清零
	memset(end_ptr - n * display_info.screen_width * display_info.char_height, 0,
		   n * display_info.screen_width * display_info.char_height * 4);
	display_info.row -= n > display_info.row ? display_info.row : n;
}
