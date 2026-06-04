include $(TOP_DIR)/config.mk

TOOLS_DIR := tools
KLOADER_DIR := kloader
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
				kernel/timer/timer.o \
				kernel/mm/mm_info.o \
				kernel/early_boot.o \
				kernel/fault/crash.o \
				kernel/atomic/atomic.o \
				kernel/task/fork.o \
				kernel/task/exec.o \
				kernel/task/switch.o \
				kernel/task/switch_task.o \
				kernel/syscall/sysenter.o \
				kernel/syscall/sysenter_fn.o \
				kernel/devicetree/dt.o \
				kernel/devicetree/dt_smp.o \
				kernel/devicetree/dt_pm.o \
				kernel/stdlib.o \
				kernel/acpi/acpi_apic.o

# TOOLS_OBJS := tools/libtools.o
				
.PHONY: all clean

all: boot.bin kloader/kloader.bin pt.o
	@dd if=/dev/zero of=$(IMAGE) count=20480
	@dd if=$(ARCH_DIR)/boot.bin of=$(IMAGE) conv=notrunc
	@dd if=$(ARCH_DIR)/kloader/kloader.bin of=$(IMAGE) seek=4 conv=notrunc
	@echo "	DD" $(IMAGE)

kernel/%.lds: kernel/%.lds.proc
	@$(CC) -E -P -x c $(CINCLUDE) $< -o $@
	@echo "	GEN" $@

./%.bin: ./%.S
	@$(CC) -E -P $(CINCLUDE) $< > $@.asm
	@$(AS) -f bin $@.asm -o $@
	@echo "	AS" $<

boot.bin: bspBoot.bin ap.bin
	@cat $^ > boot.bin
	@rm -rf $^
	@echo "	CAT" $@

ap.bin: ap.S
kernel/%.o: kernel/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

kernel/%.o: kernel/%.S
	@$(CC) -m64 -E -P $(CINCLUDE) $< > $@.asm
	@$(AS) $(ASFLAGS) -f elf64 $@.asm -o $@
	@echo "	AS" $<

kernel/%/%.o: kernel/%/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

kernel/%/%.o: kernel/%/%.S
	@$(CC) -m64 -E -P $(CINCLUDE) $< > syscall/sysenter_fn.asm
	@$(AS) $(ASFLAGS) -f elf64 syscall/sysenter_fn.asm -o $@
	@echo "	CC" $<

kloader/kloader.bin: 
	@$(MAKE) -C $(KLOADER_DIR)

pt.o:  $(PT_OBJS) kernel/klinker.lds 
	@$(LD) $(LDFLAGS_OBJ) -r $(PT_OBJS) -o $@
	@echo "	LD" $<

# tools/libtools.o: 
# 	@$(MAKE) -C $(TOOLS_DIR)

clean:
	rm -f *.o *.a *.bin *.elf */**/*.o **/*.lds **/*.o */*.asm klinker.lds include/generated/* *.asm
	find . -name "*.o" -delete
	find . -name "*.elf" -delete
	find . -name "*.a" -delete