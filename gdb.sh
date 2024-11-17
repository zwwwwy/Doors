qemu-system-i386 -blockdev driver=file,node-name=f0,filename=./build/imgs/boot.img -device floppy,drive=f0 -s -S &
gdb -ex "target remote localhost:1234"\
    -ex 'set architecture i8086' \
    -ex "set disassembly-flavor intel"\
    -ex "c"
    #-ex "b *0x7c00" \
