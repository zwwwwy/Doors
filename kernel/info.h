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
	int	 screen_width;
	int	 screen_height;
	int	 row;
	int	 col;
	int	 BitsPerPixel;
	int	 char_width;
	int	 char_height;
	int	 max_row;
	int	 max_col;
	int* init_cur_pos;
} display_struct;

typedef struct {
	void* init_ptr;
	void* current_ptr;
	int	  limit;
} buffer_struck;

#endif
