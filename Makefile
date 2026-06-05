export TOP_DIR := $(shell pwd)
include $(TOP_DIR)/config.mk

.PHONY: all clean
KERNEL_BIN := kernel

KERNEL_OBJS := $(SRC_DIR)/main.o \
			   $(SRC_DIR)/log/vfprintf.o \
			   $(SRC_DIR)/log/kprintf.o \
               $(SRC_DIR)/base/math.o \
			   $(SRC_DIR)/acpi/acpi_rsdp.o \
			   $(SRC_DIR)/acpi/acpi_apic.o \
			   $(SRC_DIR)/acpi/acpi_hpet_timer.o \
               $(SRC_DIR)/stdlib.o \
			   $(SRC_DIR)/sched/sched_eevdf.o \
			   $(SRC_DIR)/sched/sched_idle.o \
			   $(SRC_DIR)/task/fork.o \
			   $(SRC_DIR)/sched/sched.o \
			   $(SRC_DIR)/task/task.o \
			   $(SRC_DIR)/task/pid.o \
			   $(SRC_DIR)/task/exec.o \
               $(SRC_DIR)/base/bitmap.o \
			   $(SRC_DIR)/base/linklist.o \
			   $(SRC_DIR)/base/hlist.o \
			   $(SRC_DIR)/base/rbtree.o \
			   $(SRC_DIR)/base/rio_queue.o \
			   $(SRC_DIR)/base/rio_broadcast.o \
               $(SRC_DIR)/mm/mm_page.o \
			   $(SRC_DIR)/mm/mm_slab.o \
			   $(SRC_DIR)/mm/mm.o \
			   $(SRC_DIR)/mm/mm_early.o \
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
			   $(SRC_DIR)/drvframe/drv.o \
			   $(SRC_DIR)/drvframe/devicebus.o \
			   $(SRC_DIR)/sched/sched_drv.o \
			   $(SRC_DIR)/version.o \
			   $(SRC_DIR)/irq/irq.o 


ARCH_DEPENDS := $(ARCH_DIR)/pt.o

DRV_DEPENDS := $(DRV_DIR)/drvs.o

ARCH_GENERATED := $(ARCH_GENERATED_DIR)/syscall_id.h \
				$(ARCH_GENERATED_DIR)/syscall_id.inc \
				${INCLUDE_DIR}/generated/version.h \
				$(INCLUDE_DIR)/generated/offset.h
				
.PHONY: all clean run

all: $(ARCH_GENERATED) $(ARCH_DEPENDS) $(KERNEL_BIN)
	@dd if=$(KERNEL_BIN) of=$(IMAGE) seek=34 conv=notrunc
	@echo "	DD" $@

$(KERNEL_BIN): $(KERNEL_OBJS) $(DRV_DEPENDS) $(ARCH_DIR)/pt.o
	@$(LD) $(LDFLAGS_KERNEL) $^ -o $@
	@echo "	LD" $<

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

$(SRC_DIR)/%/%.o: $(SRC_DIR)/%/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

.PHONY: $(ARCH_DEPENDS)
$(ARCH_DEPENDS):
	@$(MAKE) -C $(ARCH_DIR)

$(ARCH_GENERATED_DIR)/syscall_id.h: tools/gen_systable_header.py
	@$(PY) $< $(ARCH)
	@echo "	GEN" $@

$(ARCH_GENERATED_DIR)/syscall_id.inc: tools/gen_systable.py
	@$(PY) $< $(ARCH)
	@echo "	GEN" $@

$(INCLUDE_DIR)/generated/offset.h: tools/gen_offset
	@./$< > $@
	@rm -rf $<
	@echo "	GEN" $@

tools/gen_offset: tools/gen_offset.c
	@$(CC) $(CINCLUDE) $< -o $@

${INCLUDE_DIR}/generated/version.h: tools/gen_version.py
	@$(PY) $< $(ARCH)
	@echo "	GEN" $@

.PHONY: $(DRV_DEPENDS)
$(DRV_DEPENDS): 
	@$(MAKE) -C $(DRV_DIR)

run: $(IMAGE)
	bash qemu.sh

clean:
	$(MAKE) -C $(ARCH_DIR) clean
	$(MAKE) -C $(DRV_DIR) clean
	rm -f $(IMAGE) $(KERNEL_BIN) script/*.S
	rm -rf ${INCLUDE_DIR}/generated/*
	rm -rf ${ARCH_GENERATED_DIR}/generated/*
	rm -rf tools/__pycache__
	find $(SRC_DIR) -name "*.o" -delete