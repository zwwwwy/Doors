#ifndef C_PRINTK_H
#define C_PRINTK_H
#include <stdarg.h>

void putchar(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width,
			 char char_heigth);

int printk(char* format, ...);
int vsprintfk(char* format, va_list args);

void clear_buffer();
void clear_screen();

#endif
