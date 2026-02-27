ARCH_DIR = arch/x86
IMAGE = myos.img

# 最终生成的二进制路径
KERNEL_BIN = kernel
BOOT_BIN = $(ARCH_DIR)/bl.bin
PREPARE_BIN = $(ARCH_DIR)/prepare.bin
KLOADER_BIN = $(ARCH_DIR)/kloader.bin

# 编译工具
CC = gcc-7
LD = ld
CFLAGS = -m64 -std=gnu11 -ffreestanding -fno-stack-protector -nostdlib \
         -Iinclude -I$(ARCH_DIR)/include -g

.PHONY: all clean run

all: $(IMAGE)

# 编译通用代码
src/math.o: src/math.c
	$(CC) $(CFLAGS) -c $< -o $@

src/sched/sched.o: src/sched/sched.c
	$(CC) $(CFLAGS) -c $< -o $@

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

src/stdlib.o: src/stdlib.c
	$(CC) $(CFLAGS) -c $< -o $@

# 核心：最终链接。将 arch 下的增量包和根目录的对象合并
$(KERNEL_BIN): src/main.o src/stdlib.o src/sched/sched.o src/math.o $(ARCH_DIR)/pt.o
	$(LD) -z max-page-size=4096 -m elf_x86_64 -T $(ARCH_DIR)/klinker.lds -e _start $^ -o $(KERNEL_BIN)

# 确保 arch 目录先编译
$(ARCH_DIR)/pt.o $(BOOT_BIN) $(PREPARE_BIN) $(KLOADER_BIN):
	$(MAKE) -C $(ARCH_DIR)

# 制作磁盘镜像
$(IMAGE): $(BOOT_BIN) $(PREPARE_BIN) $(KLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=20480
	dd if=$(BOOT_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(PREPARE_BIN) of=$(IMAGE) seek=1 conv=notrunc
	dd if=$(KLOADER_BIN) of=$(IMAGE) seek=4 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) seek=15 conv=notrunc

run: $(IMAGE)
	bash qemu.sh

clean:
	$(MAKE) -C $(ARCH_DIR) clean
	rm -f *.o src/*.o *.elf $(KERNEL_BIN) $(IMAGE)
