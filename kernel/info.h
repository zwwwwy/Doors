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

typedef struct {
	void*		 init_ptr;
	void*		 current_ptr;
	unsigned int limit;
} buffer_struck;

typedef struct {
	unsigned int addr1;
	unsigned int addr2;
	unsigned int len1;
	unsigned int len2;
	unsigned int type;
} memory_info;

#endif
