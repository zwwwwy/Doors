obj		= ../build/objs
other	= ../build/other
bin		= ../build/bins

obj_file= $(obj)/head.o $(obj)/main.o $(obj)/printk.o $(obj)/init_display.o
all: system.bin
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary $(bin)/system.bin $(bin)/kernel.bin

system.bin: $(obj_file)
	ld -b elf64-x86-64 -o $(bin)/system.bin $(obj_file) -T kernel.lds

$(obj)/head.o: head.S
	gcc -E head.S > $(other)/head.s
	as --64 -o $(obj)/head.o $(other)/head.s

$(obj)/main.o: main.c
	gcc -mcmodel=large -fno-builtin -m64 -c main.c -o $(obj)/main.o

$(obj)/printk.o: printk.c
	gcc -mcmodel=large -fno-builtin -m64 -c printk.c -o $(obj)/printk.o

$(obj)/init_display.o: init_display.c
	gcc -mcmodel=large -fno-builtin -m64 -c init_display.c -o $(obj)/init_display.o
