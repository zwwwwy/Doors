#ifndef C_DISPLAYINFO_H
#define C_DISPLAYINFO_H

typedef struct {
	int	 screen_width;
	int	 screen_height;
	int	 row;
	int	 col;
	int* init_cur_pos;
} display_struct;

#endif
