#ifndef C_PRINTK_H
#define C_PRINTK_H

void putchar(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width,
			 char char_heigth);

void printk(char* format);
void putchar_ascii(char ascii_code, int x, int y, int FR_color, int BK_color, char char_width,
				   char char_heigth);

void printk_ascii(char* format);

#endif
