qemu-system-x86_64 \
    -blockdev driver=file,node-name=hdd,filename=./build/imgs/boot.img \
    -device ide-hd,drive=hdd \

