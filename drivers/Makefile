include $(TOP_DIR)/config.mk

MODULE_OBJS := \
	chr_device/driver.o

TARGET_OBJS := drvs.o

all: $(TARGET_OBJS)

chr_device/driver.o:
	@$(MAKE) -C chr_device

$(TARGET_OBJS): $(MODULE_OBJS)
	@$(LD) $(LDFLAGS_OBJ) -o $@ $^
	@echo "	LD" $<

clean:
	@rm -f *.o *.bin *.elf */**/*.o **/*.lds **/*.o */*.asm klinker.lds generated/*
	@find . -name "*.o" -delete

