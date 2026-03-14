# --- 工具链配置 ---
AS      := nasm
CC      := gcc-7
LD      := ld
OBJCOPY := objcopy

# --- 编译选项 ---
CINCLUDE := -I. -Iinclude -I../../include
CFLAGS   := -ffreestanding -fno-stack-protector -nostdlib $(CINCLUDE) -g 
CFLAGS_M32 := -m32 $(CFLAGS) -O0
CFLAGS_M64 := -m64 -O2 -mcmodel=kernel -fno-pic -fno-omit-frame-pointer $(CFLAGS)  -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow

# --- 目标文件列表 ---

# kloader 需要的文件 (32位)
KLOADER_OBJS := kloader/kloader.o \
                kloader/pgtable.o \
                kloader/elf_loader.o \
                rddisk.o \
                64bit_mode.o 

# pt.o 需要的文件 (64位)
PT_OBJS      := kern_start.o \
                kstart.o \
                put.o \
                asm.o \
                pgtable_kern.o \
				cpu/cpu.o \
				cpu/smp_percpu.o \
				cpu/gdt.o \
				irq/irq_handler.o \
				irq/irq.o \
				irq/pic/apic.o \
				irq/pic/pic.o \
				mm/mm_info.o \
				early_boot.o \
				fault/crash.o \
				atomic/atomic.o

.PHONY: all clean

all: boot.bin kloader.bin pt.o

# --- 1 & 2. 引导与准备 (纯汇编) ---
%.bin: %.S
	$(AS) -f bin $< -o $@

64bit_mode.o: kloader/64bit_mode.S
	$(CC) -m32 -E -P $(CINCLUDE) $< > kloader/64bit_mode.asm
	$(AS) -f elf32 kloader/64bit_mode.asm -o $@

kloader/%.o: kloader/%.c
	$(CC) $(CFLAGS_M32) -c $< -o $@

rddisk.o: rddisk.S
	$(AS) -f elf32 $< -o $@

kloader.bin: $(KLOADER_OBJS)
	$(LD) -T linker.lds -m elf_i386 -e _start $^ -o kloader.elf
	$(OBJCOPY) -O binary kloader.elf $@

# --- 4. 内核部分 pt.o (64位) ---
klinker.lds: klinker.lds.S
	$(CC) -E -P -x c $(CINCLUDE) $< -o $@

pgtable_kern.o: pgtable.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

cpu/%.o: cpu/%.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

irq/%.o: irq/%.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

irq/*/%.o: irq/*/%.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

irq/%.o: irq/%.S
	$(AS) -f elf64 $< -o $@

mm/%.o: mm/%.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

atomic/%.o: atomic/%.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

fault/%.o: fault/%.c
	$(CC) $(CFLAGS_M64) -c $< -o $@

kstart.o: kstart.S
	$(AS) -f elf64 $< -o $@

pt.o: klinker.lds $(PT_OBJS)
	$(LD) -r $(PT_OBJS) -o $@

# --- 清理 ---
clean:
	rm -f *.o *.bin *.elf kloader/*.o kloader/*.asm klinker.lds */*.o