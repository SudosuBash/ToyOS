../qemu-7.2.19/build/qemu-system-i386 \
    -drive format=raw,file=disk.img \
	-cpu qemu64,+cmov \
    -L ../qemu-7.2.19/pc-bios/ \
    -vga std -vnc :1 \
    -S -s