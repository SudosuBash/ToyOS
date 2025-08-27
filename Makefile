NASM := nasm
all:
	cd ${PWD}/boot; \
	$(NASM) -f bin bl.S -o ../boot.bin; \
	cd .. \