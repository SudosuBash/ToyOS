CC      := gcc-7
LD      := ld
OBJCOPY := objcopy
MAKE    := make

DRV_DIR  := .

ARCH := x86
ARCH_DIR := ../arch/$(ARCH)
# --- 编译选项 ---
CINCLUDE := -I../include -I$(ARCH_DIR)/include
CFLAGS   := -m64 -O2 -std=gnu11 -ffreestanding -fno-stack-protector -nostdlib \
            $(CINCLUDE) -g  -mcmodel=kernel -fno-pic -fno-omit-frame-pointer -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone -Wall

DRIVER_OBJS := chr_device/chr_dev.o

$(DRV_DIR)/%.o: $(DRV_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

drvs.o: $(DRIVER_OBJS)
	@$(LD) -r $(DRIVER_OBJS) -o $@
	@echo "	LD" $<
clean:
	@rm -f *.o *.bin *.elf */**/*.o **/*.lds **/*.o */*.asm klinker.lds generated/*
	@find . -name "*.o" -delete