subdirs	= kernel
bins	= $(wildcard ./build/bins/*.bin)
imgs	= $(wildcard ./build/imgs/*.img)

all: $(subdirs) ./build/imgs/boot.img

$(subdirs): building
	@$(MAKE) -C $@

./build/imgs/boot.img: ./build/bins/loader.bin
	@mkdir -p ./mnt
	@sudo mount -o loop -t vfat ./build/imgs/boot.img ./mnt
	@sudo cp ./build/bins/loader.bin ./mnt
	@sync
	@sudo umount ./mnt

building: 
	@echo "start building..."

debug:
	@echo $(bins)

clean:
	rm -rf $(bins)
