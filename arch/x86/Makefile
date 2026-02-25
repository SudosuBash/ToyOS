# 工具链
AS = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

CINCLUDE = -I. -I./include -I../../include
CFLAGS = -ffreestanding -fno-stack-protector -nostdlib \
         $(CINCLUDE) -g -O0

# 注意：这里去掉了最终的链接脚本，因为还没到最终成型的时候
all: bl.bin prepare.bin kloader.bin pt.o pgtable_kern.o

# 1. 引导扇区
bl.bin: bl.S
	$(AS) -f bin $< -o $@

# 2. 环境准备
prepare.bin: prepare.S
	$(AS) -f bin $< -o $@

# 3. 加载器
kloader.bin: rddisk.S kloader/64bit_mode.S kloader/kloader.c kloader/pgtable.c kloader/elf_loader.c
	$(AS) -f elf32 rddisk.S -o rddisk.o
	$(CC) -m32 -E -P $(CINCLUDE) kloader/64bit_mode.S > kloader/64bit_mode.asm
	$(AS) -f elf32 kloader/64bit_mode.asm -o 64bit_mode.o
	$(CC) -m32 $(CFLAGS) -c kloader/kloader.c -o kloader.o 
	$(CC) -m32 $(CFLAGS) -c kloader/pgtable.c -o pgtable.o 
	$(CC) -m32 $(CFLAGS) -c kloader/elf_loader.c -o elf_loader.o 
	$(LD) -T linker.lds -m elf_i386 -e _start kloader.o rddisk.o 64bit_mode.o pgtable.o elf_loader.o -o kloader.elf
	$(OBJCOPY) -O binary kloader.elf kloader.bin

# 4. 重点：将 x86_64 相关的内核代码打包成一个 .o 文件
pt.o: kern_start.c put.c asm.c pgtable.c kstart.S kclock.o
	$(CC) -E -P -x c $(CINCLUDE) klinker.lds.S > klinker.lds
	$(CC) $(CFLAGS) -c kern_start.c -o kern_start.o
	$(AS) -f elf64 kstart.S -o kstart.o
	$(CC) $(CFLAGS) -c put.c -o put.o
	$(CC) $(CFLAGS) -c asm.c -o asm.o
	$(CC) $(CFLAGS) -c pgtable.c -o pgtable_kern.o
	$(CC) $(CFLAGS) -c kclock.c -o kclock.o
	$(LD) -r kern_start.o kstart.o asm.o put.o pgtable_kern.o kclock.o -o pt.o 

clean:
	rm -f *.o *.bin *.elf kloader/*.o kloader/*.asm klinker.lds