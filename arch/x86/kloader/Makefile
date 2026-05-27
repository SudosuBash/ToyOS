include $(TOP_DIR)/config.mk

KLOADER_OBJS := kloader.o \
                pgtable.o \
                elf_loader.o \
                rddisk.o \
                64bit_mode.o \
				acpi_initializer.o

TARGET_OBJ := kloader.bin

%.o: %.S
	@$(CC) -m32 -E -P $(CINCLUDE) $< > $@.asm
	@$(AS) -f elf32 $@.asm -o $@
	@echo "	AS" $<

%.o: %.c
	@$(CC) $(CFLAGS_M32) -c $< -o $@
	@echo "	CC" $<

%.lds: %.lds.proc
	@$(CC) -E -P -x c $(CINCLUDE) $< -o $@
	@echo "	GEN" $@

$(TARGET_OBJ): $(KLOADER_OBJS) linker.lds
	@$(LD) -T linker.lds -m elf_i386 -e _start $(KLOADER_OBJS) -o kloader.elf
	@$(OBJCOPY) -O binary kloader.elf $@
	@echo "	LD" $<

all: $(TARGET_OBJ)