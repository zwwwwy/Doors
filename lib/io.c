#include "io.h"

void		   io_out8(unsigned short port, unsigned char value);
unsigned char  io_in8(unsigned short port);
void		   io_out16(unsigned short port, unsigned short value);
unsigned short io_in16(unsigned short port);
void		   io_out32(unsigned short port, unsigned int value);
unsigned int   io_in32(unsigned short port);
