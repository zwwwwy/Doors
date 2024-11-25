obj=../build/objs
other=../build/other
bin=../build/bins
all: system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary system $(bin)/kernel.bin

$(obj)/head.o: head.S
	gcc -E head.S > $(other)/head.s
	as --64 -o $(obj)/head.o $(other)/head.s

$(obj)/main.o: main.c
	gcc -mcmodel=large -fno-builtin -m64 -c main.c -o $(obj)/main.o

system: $(obj)/head.o $(obj)/main.o
	ld -b elf64-x86-64 -o system $(obj)/head.o $(obj)/main.o -T kernel.lds

