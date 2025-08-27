NASM := nasm
all:
	cd ${PWD}/boot; \
	$(NASM) -f bin bl.S -o boot.bin; \
	$(NASM) -f bin prepare.S -o prepare.bin; \
	dd if=boot.bin of=../disk.img bs=512 seek=0 conv=notrunc; \
	dd if=prepare.bin of=../disk.img bs=512 seek=1 conv=notrunc; \
	cd .. \