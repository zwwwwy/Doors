obj		= ../build/objs
other	= ../build/other
bin		= ../build/bins

CC = gcc
CFLAGS = -mcmodel=large -fno-builtin -m64

src_c = $(wildcard *.c)
obj_file = $(addprefix $(obj)/, $(src_c:.c=.o))

all: $(obj_file)

$(obj)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
