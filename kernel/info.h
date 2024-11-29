#ifndef C_DISPLAYINFO_H
#define C_DISPLAYINFO_H

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
