subdirs	= bootsector kernel lib
bins	= $(wildcard ./build/bins/*.bin)
imgs	= $(wildcard ./build/imgs/*.img)
objs 	= $(wildcard ./build/objs/*)
other 	= $(wildcard ./build/other/*)

bin_dir = ./build/bins
obj_dir = ./build/objs

kernel_obj = $(obj_dir)/head.o $(obj_dir)/main.o $(obj_dir)/printk.o $(obj_dir)/init.o $(obj_dir)/trap.o
lib_obj = $(obj_dir)/string.o

all: $(subdirs) ./build/imgs/boot.img

$(subdirs): building
	@$(MAKE) -C $@

./build/imgs/boot.img: $(bin_dir)/loader.bin $(bin_dir)/kernel.bin
	dd if=./build/bins/boot.bin of=./build/imgs/boot.img count=1 seek=0 conv=notrunc
	dd if=./build/bins/boot.bin of=./build/imgs/boot.img count=1 seek=5 conv=notrunc
	@mkdir -p ./mnt
	@sudo mount -o loop -t vfat ./build/imgs/boot.img ./mnt
	sudo cp ./build/bins/loader.bin ./mnt
	sudo cp ./build/bins/kernel.bin ./mnt
	@sync
	@sudo umount ./mnt

$(bin_dir)/kernel.bin: $(bin_dir)/system.bin
	@objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary $(bin_dir)/system.bin $(bin_dir)/kernel.bin

$(bin_dir)/system.bin: $(kernel_obj) $(lib_obj)
	@ld -b elf64-x86-64 -o $(bin_dir)/system.bin $(kernel_obj) $(lib_obj) -T kernel.lds

building: 
	@echo "start building..."

debug:
	echo rm  $(bins)  $(objs) $(other)

clean:
	rm  $(bins)  $(objs) $(other)

.PHONY: all $(subdirs) building clean
