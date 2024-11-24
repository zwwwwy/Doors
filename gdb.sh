qemu-system-x86_64 \
    -blockdev driver=file,node-name=hdd,filename=./build/imgs/boot.img \
    -device ide-hd,drive=hdd \
    -s -S &
gdb -ex "target remote localhost:1234"\
    -ex "set disassembly-flavor intel"\
    -ex "c"
    #-ex 'set architecture i8086' \
    #-ex "b *0x7c00" \
