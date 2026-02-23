ARCH_DIR = arch/x86
KERNEL_BIN = $(ARCH_DIR)/kernel.bin
BOOT_BIN = $(ARCH_DIR)/bl.bin
PREPARE_BIN = $(ARCH_DIR)/prepare.bin
KLOADER_BIN = $(ARCH_DIR)/kloader.bin

IMAGE = myos.img

.PHONY: all clean run

all: $(IMAGE)

$(BOOT_BIN) $(PREPARE_BIN) $(KLOADER_BIN) $(KERNEL_BIN):
	$(MAKE) -C $(ARCH_DIR)

# 核心步骤：将各段二进制文件按扇区拼接
$(IMAGE): $(BOOT_BIN) $(PREPARE_BIN) $(KLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=20480
	dd if=$(BOOT_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(PREPARE_BIN) of=$(IMAGE) seek=1 conv=notrunc
	dd if=$(KLOADER_BIN) of=$(IMAGE) seek=4 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) seek=10 conv=notrunc

run: $(IMAGE)
	bash qemu.sh

clean:
	$(MAKE) -C $(ARCH_DIR) clean
	rm -f $(IMAGE)