# --- 工具链配置 ---
CC      := gcc-7
LD      := ld
MAKE    := make
PY      := python3

# --- 路径配置 ---
ARCH = x86
ARCH_DIR := arch/$(ARCH)
SRC_DIR  := src
DRV_DIR	 := drivers
# --- 编译选项 ---
CINCLUDE := -Iinclude -I$(ARCH_DIR)/include
CFLAGS   := -m64 -O2 -std=gnu11 -ffreestanding -fno-stack-protector -nostdlib \
            $(CINCLUDE) -g  -mcmodel=kernel -fno-pic -fno-omit-frame-pointer -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone -Wall

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
			   $(SRC_DIR)/data_struct/rbtree.o \
               $(SRC_DIR)/mm/mm_page.o \
			   $(SRC_DIR)/mm/mm_slab.o \
			   $(SRC_DIR)/mm/mm.o \
			   $(SRC_DIR)/mm/mmap.o \
			   $(SRC_DIR)/mm/mm_alloc.o \
			   $(SRC_DIR)/mm/mm_user_vma.o \
			   $(SRC_DIR)/atomic/lock.o \
			   $(SRC_DIR)/atomic/rwlock.o \
			   $(SRC_DIR)/atomic/sem.o \
			   $(SRC_DIR)/cpu/smp.o \
			   $(SRC_DIR)/crypto/caesar.o \
			   $(SRC_DIR)/syscall/syscall.o \
			   $(SRC_DIR)/file/dir.o \
			   $(SRC_DIR)/file/file.o \
			   $(SRC_DIR)/fs/devicefs.o \
			   $(SRC_DIR)/fs/fs.o \
			   $(SRC_DIR)/drvframe/frame.o

ARCH_DEPENDS := $(ARCH_DIR)/boot.bin \
                $(ARCH_DIR)/kloader.bin \
                $(ARCH_DIR)/pt.o

DRV_DEPENDS := $(DRV_DIR)/drvs.o

ARCH_GENERATED := $(ARCH_DIR)/generated/syscall_id.h \
				$(ARCH_DIR)/generated/syscall_id.inc \

.PHONY: all clean run

all: $(IMAGE)

$(KERNEL_BIN): $(KERNEL_OBJS) $(DRV_DEPENDS) $(ARCH_DIR)/pt.o
	@$(LD) -z max-page-size=4096 -m elf_x86_64 -T $(ARCH_DIR)/kernel/klinker.lds -e _start $^ -o $@
	@echo "	LD" $<

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

$(SRC_DIR)/%/%.o: $(SRC_DIR)/%/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

$(ARCH_DEPENDS):
	@$(MAKE) -C $(ARCH_DIR)

$(ARCH_GENERATED): tools/gen_systable.py
	@$(PY) $< $(ARCH)
	@echo "	GEN" $@

$(DRV_DEPENDS): 
	@$(MAKE) -C $(DRV_DIR)

$(IMAGE): $(ARCH_GENERATED) $(ARCH_DEPENDS) $(KERNEL_BIN)
	@dd if=/dev/zero of=$(IMAGE) bs=512 count=20480
	@dd if=$(ARCH_DIR)/boot.bin of=$(IMAGE) conv=notrunc
	@dd if=$(ARCH_DIR)/kloader.bin of=$(IMAGE) seek=4 conv=notrunc
	@dd if=$(KERNEL_BIN) of=$(IMAGE) seek=34 conv=notrunc
	@echo "	DD" $@

run: $(IMAGE)
	bash qemu.sh

clean:
	$(MAKE) -C $(ARCH_DIR) clean
	$(MAKE) -C $(DRV_DIR) clean
	rm -f $(IMAGE) $(KERNEL_BIN) script/*.S
	find $(SRC_DIR) -name "*.o" -delete