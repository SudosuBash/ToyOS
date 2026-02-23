# 工具链
AS = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

# 编译选项：-m32 开启32位，-I 指定头文件路径
CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib \
         -I./include -O0
LDFLAGS = -m elf_i386 -T link.ld

# 定义目标
all: bl.bin prepare.bin kloader.bin kernel.bin

# 编译 MBR (直接输出纯二进制)
bl.bin: bl.S
	$(AS) -f bin $< -o $@

# 编译环境准备段 (直接输出纯二进制)
prepare.bin: prepare.S
	$(AS) -f bin $< -o $@

# 编译内核：包含 kstart.c, pgtable.c 以及根目录下的 main.c
kloader.bin: kloader.c pgtable.c elf_loader.c io32.S
	$(AS) -f elf32 io32.S -o io32.o
	$(AS) -f elf32 rddisk.S -o rddisk.o
	$(CC) $(CFLAGS) -c kloader.c -o kloader.o -g
	$(CC) $(CFLAGS) -c pgtable.c -o pgtable.o -g
	$(CC) $(CFLAGS) -c elf_loader.c -o elf_loader.o -g
	$(LD) -T linker.lds -m elf_i386 -e _start  kloader.o rddisk.o pgtable.o elf_loader.o -o kloader.elf

	$(OBJCOPY) -O binary kloader.elf kloader.bin

kernel.bin:
	$(CC) $(CFLAGS) -c ../../src/main.c -o main.o
	$(CC) $(CFLAGS) -c kstart.c -o kstart.o
	$(LD) -T klinker.lds -m elf_i386 -e _start main.o kstart.o -o $@
	 
clean:
	rm -f *.o *.bin *.elf