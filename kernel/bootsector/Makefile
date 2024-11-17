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

clean: 
	rm -rf $(bins)

debug:
	@echo $(bins)

.PHONY: all debug
