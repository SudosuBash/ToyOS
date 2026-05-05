# --- 工具链配置 ---
AS      := nasm
CC      := gcc-7
LD      := ld
OBJCOPY := objcopy
PY      := python3
# --- 编译选项 ---
CINCLUDE := -I. -Iinclude -I../../include -Itools/include
CFLAGS   := -ffreestanding -fno-stack-protector -nostdlib $(CINCLUDE) -g 
CFLAGS_M32 := -m32 $(CFLAGS) -O0 -Wall -Werror
CFLAGS_M64 := -m64 -O2 -mcmodel=kernel -fno-pic -fno-omit-frame-pointer $(CFLAGS)  -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone -Wall

# --- 目标文件列表 ---

# kloader 需要的文件 (32位)
KLOADER_OBJS := kloader/kloader.o \
                kloader/pgtable.o \
                kloader/elf_loader.o \
                kloader/rddisk.o \
                kloader/64bit_mode.o
				

# pt.o 需要的文件 (64位)
PT_OBJS      := kernel/kern_start.o \
                kernel/kstart.o \
                kernel/put.o \
                kernel/asm.o \
                kernel/pgtable.o \
				kernel/cpu/cpu.o \
				kernel/cpu/gdt.o \
				kernel/irq/irq_handler.o \
				kernel/irq/irq.o \
				kernel/irq/pic/apic.o \
				kernel/irq/pic/pic.o \
				kernel/mm/mm_info.o \
				kernel/early_boot.o \
				kernel/fault/crash.o \
				kernel/atomic/atomic.o \
				kernel/task/fork.o \
				kernel/task/exec.o \
				kernel/task/switch.o \
				kernel/syscall/sysenter.o \
				kernel/syscall/sysenter_fn.o \

.PHONY: all clean

all: boot.bin kloader.bin pt.o

kloader/%.lds: kloader/%.lds.proc
	@$(CC) -E -P -x c $(CINCLUDE) $< -o $@
	@echo "	GEN" $@

kernel/%.lds: kernel/%.lds.proc
	@$(CC) -E -P -x c $(CINCLUDE) $< -o $@
	@echo "	GEN" $@

%.bin: %.S
	@$(AS) -f bin $< -o $@
	@echo "	AS" $<

kloader/%.o: kloader/%.S
	@$(CC) -m32 -E -P $(CINCLUDE) $< > $@.asm
	@$(AS) -f elf32 $@.asm -o $@
	@echo "	AS" $<

kloader/%.o: kloader/%.c
	@$(CC) $(CFLAGS_M32) -c $< -o $@
	@echo "	CC" $<

kernel/%.o: kernel/%.c
	@$(CC) $(CFLAGS_M64) -c $< -o $@
	@echo "	CC" $<

kernel/%.o: kernel/%.S
	@$(CC) -m64 -E -P $(CINCLUDE) $< > $@.asm
	@$(AS) -f elf64 $@.asm -o $@
	@echo "	AS" $<

kernel/%/%.o: kernel/%/%.c
	@$(CC) $(CFLAGS_M64) -c $< -o $@
	@echo "	CC" $<

kernel/%/%.o: kernel/%/%.S
	@$(CC) -m64 -E -P $(CINCLUDE) $< > syscall/sysenter_fn.asm
	@$(AS) -f elf64 syscall/sysenter_fn.asm -o $@
	@echo "	CC" $<

kloader.bin: $(KLOADER_OBJS) kloader/linker.lds
	@$(LD) -T kloader/linker.lds -m elf_i386 -e _start $(KLOADER_OBJS) -o kloader.elf
	@$(OBJCOPY) -O binary kloader.elf $@
	@echo "	LD" $<

pt.o: $(PT_OBJS) kernel/klinker.lds
	@$(LD) -r $(PT_OBJS) -o $@
	@echo "	LD" $<


# --- 清理 ---
clean:
	rm -f *.o *.bin *.elf */**/*.o **/*.lds **/*.o */*.asm klinker.lds generated/*
	find . -name "*.o" -delete