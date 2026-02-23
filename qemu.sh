qemu-system-i386 \
    -drive format=raw,file=myos.img \
	-cpu qemu64,+cmov \
    -vga std \
    -S -s \
    -display curses