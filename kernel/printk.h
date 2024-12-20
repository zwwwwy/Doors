#ifndef C_PRINTK_H
#define C_PRINTK_H
#include <stdarg.h>

void putchark(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width, char char_heigth);

int printk(char* format, ...);
int printk_color(char* format, int FR_color, int BK_color, ...);
int vsprintfk(char* format, va_list args);

void clear_buffer();
void clear_screen();
void roll_up_line_clean(int n);
#endif
