asms	= $(wildcard ./*.asm)
bins	= $(asms:./%.asm=../../build/bins/%.bin)
imgs 	= ../../build/imgs/boot.img

all : $(bins) $(imgs)

#$(bins) : $(asms)
#    @mkdir -p $(dir $@)
#    nasm -f bin $^ -o $@
#
../../build/bins/%.bin : %.asm
	@mkdir -p $(dir $@)
	nasm -f bin $^ -o $@

../../build/imgs/boot.img: ../../build/bins/boot.bin
	@mkdir -p $(dir $@)
	dd if=$^ of=$@ bs=512 count=1 conv=notrunc
#
#../../build/imgs/boot.img: ../../build/boot.img ../../build/bins/loader.bin
#    @mkdir -p ../../mnt
#    mount -o loop ../../build/boot.img ../../mnt
#    cp ../../build/bins/loader.bin ../../mnt/
#    umount ../../mnt

#../../build/imgs/boot.img: $(bins)
#    @mkdir -p $(dir $@)
#    dd if=../../build/bins/boot.bin of=../../build/imgs/boot.img bs=512 count=1 conv=notrunc
#    @mkdir -p ../../mnt
#    sudo mount -o loop -t vfat ../../build/imgs/boot.img ../../mnt
#    cp ../../build/bins/loader.bin ../../mnt/
#    sync
#    sudo umount ../../mnt


clean: 
	rm -rf $(bins)

debug:
	@echo $(bins)

.PHONY: all debug
