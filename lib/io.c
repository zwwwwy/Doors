void io_out8(unsigned short port, unsigned char value)
{
	__asm__ __volatile__("outb	%0,	%%dx	\n\t"
						 "mfence			\n\t"
						 :
						 : "a"(value), "d"(port)
						 : "memory");
}
