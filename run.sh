qemu-system-x86_64 \
    -enable-kvm -cpu host -smp 12 -m 8G\
    -blockdev driver=file,node-name=hdd,filename=./build/imgs/boot.img \
    -device ide-hd,drive=hdd

