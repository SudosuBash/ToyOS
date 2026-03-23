# --- 工具链配置 ---
CC      := gcc-7
LD      := ld
OBJCOPY := objcopy
MAKE    := make

# --- 路径配置 ---
ARCH_DIR := arch/x86
SRC_DIR  := src

# --- 编译选项 ---
CINCLUDE := -Iinclude -I$(ARCH_DIR)/include
CFLAGS   := -m64 -O0 -std=gnu11 -ffreestanding -fno-stack-protector -nostdlib \
            $(CINCLUDE) -g  -mcmodel=kernel -fno-pic -fno-omit-frame-pointer -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone

# --- 目标文件列表 ---

# 最终镜像和内核名称
IMAGE      := myos.img
KERNEL_BIN := kernel

# 内核通用的对象文件 (根据你的目录结构提取)
KERNEL_OBJS := $(SRC_DIR)/main.o \
               $(SRC_DIR)/math.o \
               $(SRC_DIR)/stdlib.o \
			   $(SRC_DIR)/sched/sched_mlfq.o \
			   $(SRC_DIR)/sched/sched_rr.o \
			   $(SRC_DIR)/task/fork.o \
			   $(SRC_DIR)/task/task_manager.o \
			   $(SRC_DIR)/task/switch_task.o \
			   $(SRC_DIR)/task/task.o \
			   $(SRC_DIR)/task/pid.o \
			   $(SRC_DIR)/task/exec.o \
               $(SRC_DIR)/data_struct/bitmap.o \
			   $(SRC_DIR)/data_struct/linklist.o \
			   $(SRC_DIR)/data_struct/hlist.o \
               $(SRC_DIR)/mm/mm_page.o \
			   $(SRC_DIR)/mm/mm_slab.o \
			   $(SRC_DIR)/mm/mm.o \
			   $(SRC_DIR)/mm/mm_alloc.o \
			   $(SRC_DIR)/atomic/lock.o \
			   $(SRC_DIR)/cpu/smp.o


# 来自 arch 目录的依赖项
ARCH_DEPENDS := $(ARCH_DIR)/boot.bin \
                $(ARCH_DIR)/kloader.bin \
                $(ARCH_DIR)/pt.o

.PHONY: all clean run

all: $(IMAGE)

# --- 1. 内核链接 ---

$(KERNEL_BIN): $(KERNEL_OBJS) $(ARCH_DIR)/pt.o
	$(LD) -z max-page-size=4096 -m elf_x86_64 -T $(ARCH_DIR)/klinker.lds -e _start $^ -o $@

# --- 2. 通用 C 文件编译规则 ---

# 根目录下的 src 源文件
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 子目录下的源文件 (sched, mm, data_struct 等)
$(SRC_DIR)/sched/%.o: $(SRC_DIR)/sched/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/mm/%.o: $(SRC_DIR)/mm/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/data_struct/%.o: $(SRC_DIR)/data_struct/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- 3. 递归调用 Arch 编译 ---

$(ARCH_DEPENDS):
	$(MAKE) -C $(ARCH_DIR)

# --- 4. 磁盘镜像制作 ---

$(IMAGE): $(ARCH_DEPENDS) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=20480
	dd if=$(ARCH_DIR)/boot.bin of=$(IMAGE) conv=notrunc
	dd if=$(ARCH_DIR)/kloader.bin of=$(IMAGE) seek=4 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) seek=34 conv=notrunc

# --- 5. 辅助指令 ---

run: $(IMAGE)
	bash qemu.sh

clean:
	$(MAKE) -C $(ARCH_DIR) clean
	rm -f $(IMAGE) $(KERNEL_BIN)
	find $(SRC_DIR) -name "*.o" -delete