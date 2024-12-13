obj		= ../build/objs
other	= ../build/other
bin		= ../build/bins

CC = gcc
CFLAGS = -mcmodel=large -fno-builtin -m64

src_c = $(wildcard *.c)
obj_file = $(obj)/head.o $(addprefix $(obj)/, $(src_c:.c=.o))

all: $(obj_file)

$(obj)/head.o: head.S
	gcc -E head.S > $(other)/head.s
	as --64 -o $(obj)/head.o $(other)/head.s

$(obj)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
