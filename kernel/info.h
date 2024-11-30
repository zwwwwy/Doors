#ifndef C_DISPLAYINFO_H
#define C_DISPLAYINFO_H

#define RED 0x00ce4f55
#define GREEN 0x0031ad6f
#define BLUE 0x000072b5
#define WHITE 0x00ffffff
#define BLACK 0x00000000
#define ORANGE 0x00ea925e
#define BLUE_SCREEN 0x000078d7
#define PURE_RED 0x00ff0000
#define PURE_GREEN 0x0000ff00
#define PURE_BLUE 0x000000ff

typedef struct {
	int	 screen_width;
	int	 screen_height;
	int	 row;
	int	 col;
	int	 BitsPerPixel;
	int	 char_width;
	int	 char_height;
	int* init_cur_pos;
} display_struct;

typedef struct {
	void* init_ptr;
	void* current_ptr;
	int	  limit;
} buffer_struck;

#endif
