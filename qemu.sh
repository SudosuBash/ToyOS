qemu-system-x86_64 \
    -drive format=raw,file=myos.img \
	-cpu qemu64,+cmov \
    -S -s \
    -chardev stdio,id=char0,mux=on -serial chardev:char0 -mon chardev=char0 \
    -nographic \
    -m 4G \
    -smp 4
