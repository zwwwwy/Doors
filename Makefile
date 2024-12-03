subdirs	= bootsector kernel
bins	= $(wildcard ./build/bins/*.bin)
imgs	= $(wildcard ./build/imgs/*.img)
objs 	= $(wildcard ./build/objs/*)
other 	= $(wildcard ./build/other/*)

bin_dir = ./build/bins
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

building: 
	@echo "start building..."

debug:
	echo rm  $(bins)  $(objs) $(other)

clean:
	rm  $(bins)  $(objs) $(other)

.PHONY: all $(subdirs) building clean
